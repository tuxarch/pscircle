#pragma once

#include "types.h"

typedef struct {
	real_t x;
	real_t y;
} point_t;

char *
point_to_str(point_t p);
