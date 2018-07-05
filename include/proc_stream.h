#pragma once

#include <stdio.h>

#include "pnode.h"

typedef struct {
	FILE *procs;
} in_stream_t;

pnode_t *
stream_get_next_proc(FILE *procs, pnode_t *pnode_t);
