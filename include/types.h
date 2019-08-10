#include "config.h"
#include <stdint.h>

#pragma once

#ifdef PSC_USE_FLOAT
#define R(x) x##f
typedef float real_t;
#define PSC_EPS 1e-5
#else
#define R(x) x
typedef double real_t;
#define PSC_EPS 1e-8
#endif

typedef PSC_NODE_COUNT_TYPE nnodes_t;

typedef PSC_MEMORY_UNIT_TYPE memunit_t;

