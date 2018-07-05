#pragma once

#include <stdbool.h>

#include "types.h"
#include "point.h"

typedef struct {
	real_t nx;
	real_t ny;
	real_t r;
} ppoint_t;

ppoint_t
ppoint_from_radial(real_t phi, real_t rho);

bool
ppoint_codirectinal(ppoint_t a, ppoint_t b);

ppoint_t
ppoint_normal(ppoint_t a, bool up);

ppoint_t
ppoint_add(ppoint_t a, ppoint_t b);

point_t
ppoint_to_point(ppoint_t a);
