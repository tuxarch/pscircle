#pragma once

#include <stdint.h>
#include <stddef.h>

#include "types.h"

typedef struct node_t node_t;

struct node_t {
	node_t *first;
	node_t *next;
	node_t *last;
	node_t *prev;
	node_t *_parent;
	node_t *_ancesstor;
	node_t *_link;

	nnodes_t _id;

	real_t x;
	real_t _mod;
	real_t _shift;
	real_t _change;
};

void
node_arrange(node_t *root);

void
node_add(node_t *parent, node_t *child);

nnodes_t
node_nchildren(node_t *node);

nnodes_t
node_reorder_by_leaves(node_t *node);

node_t *
node_widest_child(node_t *node);
