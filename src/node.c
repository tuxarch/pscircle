#include <assert.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <math.h>

#include <stdio.h>

#include "node.h"

#define FOR_CHILDREN(node) \
	for (node_t *n = node->first; n != NULL; n = n->next)
#define FOR_CHILDREN_REV(node) \
	for (node_t *n = node->last; n != NULL; n = n->prev)

node_t *
right(node_t *node);

node_t *
left(node_t *node);

node_t *
first_brother(node_t *node);

node_t *
ancesstor(node_t *node, node_t *v);

node_t *
apportion(node_t *node, node_t *default_ancestor);

void
renumber(node_t *node);

void
shift(node_t *node);

void
normilize(node_t *node, real_t minx, real_t range);

void
arrange(node_t *node);

void
move_and_findminmax(node_t *node, real_t *minx, real_t *maxx, real_t m);

void
move(node_t *wr, node_t *wl, real_t shift);

void
_find_widest(node_t *node, real_t *max, node_t **argmax, real_t depth);

void
node_add(node_t *parent, node_t *child)
{
	assert(parent);
	assert(child);

	child->_parent = parent;

	if (parent->first == NULL) {
		child->_id = 1;
		parent->first = child;
		parent->last = child;
		return;
	}

	assert(parent->last);

	child->_id = parent->last->_id + 1;

	parent->last->next = child;
	child->prev = parent->last;
	parent->last = child;
}

void
node_arrange(node_t *root)
{
	arrange(root);

	real_t minx = FLT_MAX;
	real_t maxx = -FLT_MAX;

	move_and_findminmax(root, &minx, &maxx, 0);

	normilize(root, minx, maxx);
}

node_t *
left(node_t *node)
{
	if (node->_link)
		return node->_link;

	return node->first;
}

node_t *
right(node_t *node)
{
	if (node->_link)
		return node->_link;

	return node->last;
}

node_t *
first_brother(node_t *node)
{
	if (!node->_parent)
		return NULL;

	if (node->_parent->first == node)
		return NULL;

	return node->_parent->first;
}

node_t *
ancesstor(node_t *node, node_t *v)
{
	if (!node->_parent)
		return NULL;

	FOR_CHILDREN(node->_parent) {
		if (n == v->_ancesstor)
			return n;
	}

	return NULL;
}

nnodes_t
node_nchildren(node_t *node)
{
	if (!node->first)
		return 0;
	if (node->last == node->first)
		return 1;
	return node->last->_id - node->first->_id + 1;
}

void
renumber(node_t *node)
{
	unsigned id = 1;
	FOR_CHILDREN(node)
		n->_id = id++;
}

typedef struct {
	nnodes_t nleaves;
	node_t *node;
} leaves_and_nodes_t;

int leafs_comp(const void *a, const void *b) {
	const leaves_and_nodes_t *na = (leaves_and_nodes_t *) a;
	const leaves_and_nodes_t *nb = (leaves_and_nodes_t *) b;
	if (na->nleaves < nb->nleaves)
		return -1;
	return 1;
}

nnodes_t
node_reorder_by_leaves(node_t *node)
{
	if (!node->first)
		return 1;

	nnodes_t nleaves = 0;
	nnodes_t nchildren = node_nchildren(node);

	leaves_and_nodes_t children[nchildren];

	nnodes_t i = 0;
	FOR_CHILDREN(node) {
		nnodes_t nl = node_reorder_by_leaves(n);
		nleaves += nl;
		children[i].node = n;
		children[i].nleaves = nl;
		i++;
	}

	qsort(children, nchildren, sizeof(leaves_and_nodes_t), leafs_comp);

	node->first = NULL;
	node->last = NULL;
	node_t *ff = NULL;
	node_t *ll = NULL;
	bool last = false;

	for (nnodes_t i = 0; i < nchildren; ++i) {
		node_t *n = children[i].node;
		n->next = NULL;
		n->prev = NULL;

		if (last) {
			if (!ll) {
				n->prev = NULL;
				n->next = NULL;
				node->last = n;
				ll = n;
			} else {
				ll->prev = n;
				n->next = ll;
				ll = n;
			}
		} else {
			if (!ff) {
				n->prev = NULL;
				n->next = NULL;
				node->first = n;
				ff = n;
			} else {
				ff->next = n;
				n->prev = ff;
				ff = n;
			}
		}

		last = !last;
	}

	if (!node->last)
		node->last = node->first;

	if (ll && ff) {
		ff->next = ll;
		ll->prev = ff;
	}

	renumber(node);

	return nleaves;
}

void
shift(node_t *node)
{
	real_t shift = 0;
	real_t change = 0;

	FOR_CHILDREN_REV(node) {
		n->x += shift;
		n->_mod += shift;
		change += n->_change;
		shift += n->_shift + change;
	}
}

void
move(node_t *wr, node_t *wl, real_t shift)
{
	nnodes_t subtrees = wr->_id - wl->_id;

	wl->_change += shift / subtrees;
	wr->_change -= shift / subtrees;

	wr->x += shift;
	wr->_shift += shift;
	wr->_mod += shift;
}

void
normilize(node_t *node, real_t minx, real_t maxx)
{
	node->x -= minx;
	if (minx != maxx)
		node->x /= maxx - minx;

	FOR_CHILDREN(node)
		normilize(n, minx, maxx);
}

void 
move_and_findminmax(node_t *node,
		real_t *minx, real_t *maxx, real_t mod)
{
	node->x += mod;

	if (node->x < *minx)
		*minx = node->x;

	if (node->x > *maxx)
		*maxx = node->x;

	FOR_CHILDREN(node)
		move_and_findminmax(n, minx, maxx, mod + node->_mod);
}

void
arrange(node_t *node)
{
	if (node->first == NULL) {
		node->x = 0;
		if (first_brother(node))
			node->x = node->prev->x + 1;
		return;
	}

	node_t *default_ancestor = node->first;
	FOR_CHILDREN(node) {
		arrange(n);
		default_ancestor = apportion(n, default_ancestor);
	}

	shift(node);

	real_t midpoint = (node->first->x + node->last->x) / 2;

	if (node->prev) {
		node->x = node->prev->x + 1;
		node->_mod = node->x - midpoint;
	} else {
		node->x = midpoint;
	}
}

node_t *
apportion(node_t *node, node_t *default_ancestor)
{
	node_t *w = node->prev;
	if (!w)
		return default_ancestor;

	node_t *vir = node;
	node_t *vor = node;
	node_t *vil = w;
	node_t *vol = first_brother(node);
	real_t sir = node->_mod;
	real_t sor = node->_mod;
	real_t sil = vil->_mod;
	real_t sol = vol->_mod;

	while (right(vil) && left(vir)) {
		vil = right(vil);
		vir = left(vir);
		vol = left(vol);
		vor = right(vor);
		vor->_ancesstor = node;

		real_t shift = (vil->x + sil) - (vir->x + sir) + 1;
		if(shift > 0) {
			node_t *an = ancesstor(node, vil);
			if (!an)
				an = default_ancestor;

			move(node, an, shift);
			sir = sir + shift;
			sor = sor + shift;
		}

		sil += vil->_mod;
		sir += vir->_mod;
		sol += vol->_mod;
		sor += vor->_mod;
	}

	if (right(vil) && !right(vor)) {
		vor->_link = right(vil);
		vor->_mod += sil - sor;
	} else {
		if (left(vir) && !left(vol)) {
			vol->_link = left(vir);
			vol->_mod += sir - sol;
		}
		default_ancestor = node;
	}

	return default_ancestor;
}

void
find_widest_rec(node_t *node, real_t *max, node_t **argmax, real_t depth)
{
	real_t w = 0;
	if (node->first)
		w = R(fabs)(node->first->x - node->last->x) * depth;

	if (w > *max) {
		*max = w;
		*argmax = node;
	}

	FOR_CHILDREN(node)
		find_widest_rec(n, max, argmax, depth + 1);
}

node_t *
node_widest_child(node_t *node)
{
	real_t max = 0;
	node_t *argmax = node->first;

	find_widest_rec(node, &max, &argmax, 1);
	return argmax;
}

