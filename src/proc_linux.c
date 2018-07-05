#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>

#include "proc_linux.h"

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

typedef struct {
	int pid;
	int ppid;
	char *comm;
	ticks_t stime;
	ticks_t utime;
	ctime_t starttime;
	rss_t rss;
} proc_t;

void
read_cputime(ctime_t *cputime, ctime_t *idletime);

ctime_t
read_uptime();

proc_t *
read_proc(pid_t pid, proc_t *proc);

pnode_t *
proc_to_pnode(linux_procs_t *ctx, pnode_t *pnode, proc_t *proc);

void
linux_init(linux_procs_t *ctx)
{
#ifndef NDEBUG
	uint8_t zeros[sizeof(linux_procs_t)];
	memset(zeros, 0, sizeof(zeros));
	assert(memcmp(zeros, ctx, sizeof(linux_procs_t)) == 0);
#endif

	ctx->procdir = opendir("/proc");
	CHECK(ctx->procdir);

	ctx->hertz = sysconf(_SC_CLK_TCK);

	ctx->pagesize = getpagesize();

	read_cputime(&ctx->cputime_st, &ctx->idletime_st);

	ctx->uptime = read_uptime();
}

void
linux_dinit(linux_procs_t *ctx)
{
	assert(ctx);
	assert(ctx->procdir);

	closedir(ctx->procdir);
}

pnode_t *
linux_get_next_proc(linux_procs_t *ctx, pnode_t *pnode)
{
	assert(ctx);
	assert(pnode);
	assert(ctx->procdir);

	struct dirent *de;
	proc_t p = {0};
	while ((de = readdir(ctx->procdir)) != NULL) {
		if (de->d_type != DT_DIR)
			continue;

		char *e = NULL;
		pid_t pid = strtol(de->d_name, &e, 10);
		if (*e != '\0')
			continue;

		p.comm = pnode->name;

		if (!read_proc(pid, &p))
			continue;

		if (!proc_to_pnode(ctx, pnode, &p))
			continue;

		return pnode;
	}

	return NULL;
}

pnode_t *
proc_to_pnode(linux_procs_t *ctx, pnode_t *pnode, proc_t *proc)
{
	assert(ctx);
	assert(pnode);
	assert(proc);
	assert(pnode->name == proc->comm);

	pnode->pid = proc->pid;

	pnode->ppid = proc->ppid;

	pnode->mem = proc->rss * ctx->pagesize;

	double t = proc->utime + proc->stime;
	double dt = (double) ctx->uptime * ctx->hertz - proc->starttime;
	pnode->cpu = 100. * t / dt;

	pnode->cputime = t;

	return pnode;
}

void
linux_wait(linux_procs_t *ctx, real_t delay)
{
	assert(ctx);
	assert(delay > 0);

	unsigned sec = delay;
	unsigned usec = (delay - sec) * 1e6;

	if (sec > 0)
		sleep(sec);
	if (usec < 1e6)
		usleep(usec);

	read_cputime(&ctx->cputime_en, &ctx->idletime_en);
}

void
linux_update_proc(linux_procs_t *ctx, pnode_t *pnode)
{
	assert(ctx);
	assert(pnode);

	proc_t p = {0};

	if (!read_proc(pnode->pid, &p)) {
		pnode->cpu = 0;
		return;
	}

	double dt = (double)ctx->cputime_en - ctx->cputime_st;
	if (dt == 0) {
		pnode->cpu = 0;
		return;
	}

	double t = (double)(p.stime + p.utime) - pnode->cputime;
	pnode->cpu = 100. * t / dt;
}

real_t
linux_cpu_utilization(linux_procs_t *ctx)
{
	assert(ctx);

	double dt = ctx->cputime_en - ctx->cputime_st;
	if (dt < 0)
		dt = ctx->cputime_st;

	assert(dt > 0);

	double t = (double) ctx->idletime_en - ctx->idletime_st;

	return 1. - t /  dt;
}

const char *
linux_loadavg()
{
	static char buf[PSC_LABEL_BUFSIZE + 1] = {0};

	FILE *f = fopen("/proc/loadavg", "r");
	CHECK(f);

	fgets(buf, PSC_LABEL_BUFSIZE, f);

	char *end = buf;
	for (size_t i = 0; i < 4; ++i) {
		end++;
		while (*end != ' ')
			end++;
	}

	*end = '\0';

	fclose(f);

	return buf;
}

char *starts_with(char *str, const char *prefix)
{
	assert(str);
	assert(prefix);

	while (*prefix && *str) {
		if (*prefix++ != *str++)
			return NULL;
	}

	return str;
}

void
linux_meminfo(unsigned long *mtotal, unsigned long *mused, unsigned long *mfree)
{
	assert(mtotal);
	assert(mused);
	assert(mfree);

	FILE *f = fopen("/proc/meminfo", "r");
	CHECK(f);

	static char *line = NULL;
	static size_t n = 0;
	ssize_t nread = 0;

	unsigned long mbuffers = 0;
	unsigned long mcached = 0;

	unsigned long fields = 0;

	while ((nread = getline(&line, &n, f) != -1)) {
		char *s;
		if ((s = starts_with(line, "MemTotal:"))) {
			sscanf(s, "%lu", mtotal);
			fields |= 1;
		}

		if ((s = starts_with(line, "MemFree:"))) {
			sscanf(s, "%lu", mfree);
			fields |= 1 << 1;
		}

		if ((s = starts_with(line, "Buffers:"))) {
			sscanf(s, "%lu", &mbuffers);
			fields |= 1 << 2;
		}

		if ((s = starts_with(line, "Cached:"))) {
			sscanf(s, "%lu", &mcached);
			fields |= 1 << 3;
		}

		if (fields == (1 << 4) - 1)
			break;
	}

	assert(fields == (1 << 4) - 1);

	fclose(f);

	*mused = *mtotal - *mfree - mbuffers - mcached;

	*mtotal *= 1024;
	*mused *= 1024;
	*mfree *= 1024;
}

void
read_cputime(ctime_t *cputime, ctime_t *idletime)
{
	assert(cputime);
	assert(idletime);

	FILE *f = fopen("/proc/stat", "r");
	CHECK(f);

	*cputime = 0;

	fscanf(f, "%*s");
	for (size_t i = 1; i < 11; ++i) {
		unsigned long tmp = 0;
		fscanf(f, "%lu", &tmp);
		*cputime += tmp;

		if (i == 4)
			*idletime = tmp;
	}

	fclose(f);
}

ctime_t
read_uptime()
{
	FILE *f = fopen("/proc/uptime", "r");
	CHECK(f);

	uint64_t uptime = 0;

	fscanf(f, "%lu", &uptime);

	fclose(f);

	return uptime;
}

proc_t *
read_proc(pid_t pid, proc_t *proc)
{
	static char path[30];
	snprintf(path, sizeof(path), "/proc/%d/stat", pid);

	FILE *f = fopen(path, "r");
	if (!f)
		return NULL;

	static char *buf = NULL;
	static size_t n = 0;
	ssize_t l = getline(&buf, &n, f);
	fclose(f);

	if (l <= 0)
		return NULL;

	char *end = buf + l;

	sscanf(buf, "%d", &proc->pid);

	for (size_t i = 52; i != 2; --i) {
		*end = '\0';
		while (*end != ' ')
			end--;
		char *s = end + 1;
		if (i == 24)
			sscanf(s, "%ld", &proc->rss);
		if (i == 22)
			sscanf(s, "%llu", &proc->starttime);
		if (i == 15)
			sscanf(s, "%lu", &proc->utime);
		if (i == 14)
			sscanf(s, "%lu", &proc->stime);
		if (i == 4)
			sscanf(s, "%d", &proc->ppid);
	}

	if (!proc->comm)
		return proc;

	*end = '\0';

	char *s = buf;
	while (*s != ' ')
		s++;

	if (*(s + 1) == '(') {
		do {
			s++;
		} while (*s == '(');
	}

	if (*(end - 1) == ')') {
		do {
			end--;
		} while (*end == ')');

		*(end + 1) = '\0';
	}

	strncpy(proc->comm, s, PSC_MAX_NAME_LENGHT);

	return proc;
}

