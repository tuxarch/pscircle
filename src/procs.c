#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdio.h>

#include "procs.h"
#include "cfg.h"
#include "utils.h"

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

void
read_procs_stream(procs_t *procs, FILE *fp);

void
read_procs_linux(procs_t *procs);

void
link_process(procs_t *procs);

pnode_t *
get_new_process(procs_t *procs);

void
reserve_root_memory(procs_t *procs);

void
count_as_stub(pnode_t *parent, pnode_t *child);

void
update_mem_toplist(procs_t *procs, pnode_t *p);

void
update_cpu_toplist(procs_t *procs, pnode_t *p);

void
sort_top_lists(procs_t *procs);

void
add_stubs(procs_t *procs);

pnode_t *
find_by_pid(procs_t *procs, pid_t pid);

void
procs_update_stats(procs_t *procs);

void
procs_init(procs_t *procs, FILE *fp)
{
	assert(procs);

#ifndef NDEBUG
	uint8_t zeros[sizeof(procs_t)];
	memset(zeros, 0, sizeof(zeros));
	assert(memcmp(zeros, procs, sizeof(procs_t)) == 0);
#endif

	reserve_root_memory(procs);

	if (fp)
		read_procs_stream(procs, fp);
	else
		read_procs_linux(procs);

	link_process(procs);

	sort_top_lists(procs);
}

void
procs_dinit(procs_t *procs)
{
	assert(procs);
}

void
procs_update_mem_stats()
{
	unsigned long mtotal;
	unsigned long mused;
	unsigned long mfree;

	linux_meminfo(&mtotal, &mused, &mfree);

	if (config.toplists.memlist.value < 0)
		config.toplists.memlist.value = (real_t) mused / mtotal;

	if (config.toplists.memlist.label)
		return;

	double m1 = mused;
	const char *u1 = NULL;
	bytes_to_human(&m1, &u1);

	double m2 = mtotal;
	const char *u2 = NULL;
	bytes_to_human(&m2, &u2);

	static char buf[PSC_LABEL_BUFSIZE + 1] = {0};
	snprintf(buf, PSC_LABEL_BUFSIZE, "%1.1lf%s / %1.1lf%s", m1, u1, m2, u2);

	config.toplists.memlist.label = buf;
}

void
read_procs_stream(procs_t *procs, FILE *fp)
{
	assert(procs);

	while (true) {
		pnode_t *p = get_new_process(procs);
		if (!p)
			break;

		if (!stream_get_next_proc(fp, p))
			break;

		for (size_t u = 0; u < config.memory_unit; ++u)
			p->mem *= 1024;
	}
}

void
read_procs_linux(procs_t *procs)
{
	assert(procs);

	linux_procs_t lprocs = {0};

	linux_init(&lprocs);

	while (true) {
		pnode_t *p = get_new_process(procs);
		if (!p)
			break;

		if (!linux_get_next_proc(&lprocs, p))
			break;
	}

	if (config.interval > 0) {
		linux_wait(&lprocs, config.interval);

		for (size_t i = 0; i < procs->nprocesses; ++i)
			linux_update_proc(&lprocs, procs->processes + i);

		if (config.toplists.cpulist.value < 0)
			config.toplists.cpulist.value = linux_cpu_utilization(&lprocs);
	}

	if (config.toplists.cpulist.value < 0)
		config.toplists.cpulist.value = 0;

	if (!config.toplists.cpulist.label)
		config.toplists.cpulist.label = linux_loadavg();

	if (config.toplists.memlist.value < 0 || !config.toplists.memlist.label)
		procs_update_mem_stats();

	linux_dinit(&lprocs);
}

pnode_t *
get_new_process(procs_t *procs)
{
	assert(procs);

	if (procs->nprocesses == PSC_MAX_PROCS_COUNT) {
		fprintf(stderr,
				"Maximum number of processes (%d) is reached, skipping the rest.\n",
				PSC_MAX_PROCS_COUNT);
		fprintf(stderr, "To increase this value change PSC_MAX_PROCS_COUNT and recompile\n");
		return NULL;
	}

	assert(procs->processes);
	return procs->processes + procs->nprocesses++;
}


pnode_t *
find_by_pid(procs_t *procs, pid_t pid)
{
	for (size_t i = 0; i < procs->nprocesses; ++i) {
		if (procs->processes[i].pid == pid)
			return procs->processes + i;
	}

	return NULL;
}

void
link_process(procs_t *procs)
{
	assert(procs);
	assert(!procs->root);

	pnode_t *found = find_by_pid(procs, config.root_pid);
	if (!found) {
		procs->root = procs->processes;
		procs->root->pid = config.root_pid;
	} else {
		procs->root = found;
	}

	// starts from 1 to skip reserved root
	for (size_t i = 1; i < procs->nprocesses; ++i) {
		pnode_t *p = procs->processes + i;

		update_cpu_toplist(procs, p);

		update_mem_toplist(procs, p);

		if (p == procs->root)
			continue;

		pnode_t *parent = find_by_pid(procs, p->ppid);
		if (!parent)
			continue;

		if (node_nchildren(&parent->node) < config.max_children) {
			node_add((node_t *)parent, (node_t *)p);
		} else {
			count_as_stub(parent, p);
		}
	}

	add_stubs(procs);
}

void
reserve_root_memory(procs_t *procs)
{
	pnode_t *r = get_new_process(procs);
	assert(r);
	r->pid = -1;
}

void
count_as_stub(pnode_t *parent, pnode_t *p)
{
	assert(parent);
	assert(p);

	if (!parent->stub) {
		assert(parent->nstubs == 0);
		parent->stub = p;
		parent->nstubs = 1;
		return;
	}

	assert(parent->nstubs > 0);

	if (p->mem > parent->stub->mem)
		parent->stub->mem = p->mem;
	if (p->cpu > parent->stub->cpu)
		parent->stub->cpu = p->cpu;

	parent->nstubs++;
}

void
update_cpu_toplist(procs_t *procs, pnode_t *p)
{
	assert(procs);
	assert(p);

	pnode_t **found = NULL;

	for (size_t i = 0; i < PSC_TOPLIST_MAX_ROWS; ++i) {
		pnode_t **pp = &(procs->cpu_toplist[i]);
		if (*pp == NULL) {
			found = pp;
			break;
		}

		if ((*pp)->cpu < p->cpu)
			found = pp;
	}

	if (found)
		*found = p;
}

void
update_mem_toplist(procs_t *procs, pnode_t *p)
{
	assert(procs);
	assert(p);

	pnode_t **found = NULL;

	for (size_t i = 0; i < PSC_TOPLIST_MAX_ROWS; ++i) {
		pnode_t **pp = &(procs->mem_toplist[i]);
		if (*pp == NULL) {
			found = pp;
			break;
		}

		if ((*pp)->mem < p->mem)
			found = pp;
	}

	if (found)
		*found = p;
}

int cpu_comp(const void *a, const void *b) {
	const pnode_t *pa = *(const pnode_t **) a;
	const pnode_t *pb = *(const pnode_t **) b;

	if (!pa && !pb)
		return 0;
	if (!pa && pb)
		return 1;
	if (pa && !pb)
		return -1;

	if (pa->cpu < pb->cpu)
		return 1;

	return -1;
}

int mem_comp(const void *a, const void *b) {
	const pnode_t *pa = *(const pnode_t **) a;
	const pnode_t *pb = *(const pnode_t **) b;

	if (!pa && !pb)
		return 0;
	if (!pa && pb)
		return 1;
	if (pa && !pb)
		return -1;

	if (pa->mem < pb->mem)
		return 1;

	return -1;
}

void
sort_top_lists(procs_t *procs)
{
	assert(procs);

	qsort(procs->cpu_toplist, PSC_TOPLIST_MAX_ROWS,
			sizeof(pnode_t *), cpu_comp);

	qsort(procs->mem_toplist, PSC_TOPLIST_MAX_ROWS,
			sizeof(pnode_t *), mem_comp);
}

void
add_stubs(procs_t *procs)
{
	for (size_t i = 0; i < procs->nprocesses; ++i) {
		pnode_t *p = procs->processes + i;
		if (!p->stub)
			continue;
		assert(p->nstubs > 0);

		snprintf(p->stub->name, PSC_MAX_NAME_LENGHT,
				"<%zd omitted>", p->nstubs);

		node_add((node_t *)p, (node_t *)p->stub);
	}
}

void
procs_child_by_pid_recurcive(pnode_t **found, pnode_t *p, pid_t pid)
{
	assert(found);
	assert(p);

	if (*found)
		return;

	if (p->pid == pid) {
		*found = p;
		return;
	}

	for (node_t *n = p->node.first; n != NULL; n = n->next)
		procs_child_by_pid_recurcive(found, (pnode_t *) n, pid);
}

pnode_t *
procs_child_by_pid(procs_t *procs, pid_t pid)
{
	assert(procs);

	pnode_t *found = NULL;

	procs_child_by_pid_recurcive(&found, procs->root, pid);

	return found;
}

void
procs_child_by_name_recurcive(pnode_t **found, pnode_t *p, const char *name)
{
	assert(found);
	assert(p);
	assert(name);

	if (*found)
		return;

	if (strcmp(p->name, name) == 0) {
		*found = p;
		return;
	}

	for (node_t *n = p->node.first; n != NULL; n = n->next)
		procs_child_by_name_recurcive(found, (pnode_t *) n, name);
}

pnode_t *
procs_child_by_name(procs_t *procs, const char *name)
{
	assert(procs);
	assert(name);

	pnode_t *found = NULL;

	procs_child_by_name_recurcive(&found, procs->root, name);

	return found;
}
