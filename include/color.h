#pragma once

#include "types.h"

typedef struct {
	real_t r;
	real_t g;
	real_t b;
	real_t a;
} color_t;

color_t
color_from_hex(const char *hstr);

color_t
color_between(color_t a, color_t b, real_t k);

char *
color_to_hex(color_t col);
