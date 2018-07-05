#pragma once

#include <stdio.h> 

#include "node.h"

#include "pnode.h"

#include "proc_linux.h"
#include "proc_stream.h"

typedef struct {
	pnode_t *root;

	size_t nprocesses;
	pnode_t processes[PSC_MAX_PROCS_COUNT];

	pnode_t *cpu_toplist[PSC_TOPLIST_MAX_ROWS];
	pnode_t *mem_toplist[PSC_TOPLIST_MAX_ROWS];
} procs_t;

void
procs_init(procs_t *procs, FILE *fp);

void
procs_dinit(procs_t *procs);

pnode_t *
procs_child_by_pid(procs_t *procs, pid_t pid);

pnode_t *
procs_child_by_name(procs_t *procs, const char *name);
