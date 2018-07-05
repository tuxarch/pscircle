#include <assert.h>

#include "proc_stream.h"

void
read_word_and_skip_to_lf(FILE *procs, char *buf, size_t len);

pnode_t *
stream_get_next_proc(FILE *procs, pnode_t *pnode)
{
	assert(procs);
	assert(pnode);

	pnode_t p;
	while (!feof(procs)) {

#ifdef PSC_USE_FLOAT
		int rc = fscanf(procs, "%d %d %f %lu ", &p.pid, &p.ppid, &p.cpu, &p.mem);
#else
		int rc = fscanf(procs, "%d %d %lf %lu ", &p.pid, &p.ppid, &p.cpu, &p.mem);
#endif
		if (rc == EOF)
			return NULL;

		read_word_and_skip_to_lf(procs, pnode->name, PSC_MAX_NAME_LENGHT);

		pnode->pid = p.pid;
		pnode->ppid = p.ppid;
		pnode->cpu = p.cpu;
		pnode->mem = p.mem;

		return pnode;
	}

	return NULL;
}

void
read_word_and_skip_to_lf(FILE *procs, char *buf, size_t len)
{
	assert(procs);
	assert(buf);

	int c = 0;
	for (size_t i = 0; i < len - 1; ++i) {
		c = fgetc(procs);
		if (c == ' ' || c == '\n' || c == EOF) {
			buf[i] = '\0';
			if (c == ' ')
				break;
			return;
		}
		buf[i] = c;
	}

	buf[len - 1] = '\0';

	do {
		c = fgetc(procs);
	} while (c != '\n' && c != EOF);

	return;
}
