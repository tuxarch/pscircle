#pragma once

#include <dirent.h>

#include "pnode.h"

typedef int pid_t;
typedef unsigned long ticks_t;
typedef unsigned long long ctime_t;
typedef long rss_t;

typedef struct {
	double r;
	DIR *procdir;
	ctime_t cputime_st;
	ctime_t cputime_en;
	ctime_t idletime_st;
	ctime_t idletime_en;
	long hertz;
	long uptime;
	int pagesize;
} linux_procs_t;

void
linux_init(linux_procs_t *linux_procs);

void
linux_dinit(linux_procs_t *linux_procs);

pnode_t *
linux_get_next_proc(linux_procs_t *linux_procs, pnode_t *pnode);

void
linux_wait(linux_procs_t *linux_procs, real_t delay);

void
linux_update_proc(linux_procs_t *linux_procs, pnode_t *pnode);

real_t
linux_cpu_utilization(linux_procs_t *linux_procs);

const char *
linux_loadavg();

void
linux_meminfo(unsigned long *mtotal, unsigned long *mused, unsigned long *mfree);
