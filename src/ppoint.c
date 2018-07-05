#define _GNU_SOURCE

#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdio.h>

#include "ppoint.h"

bool ppoint_is_valid(ppoint_t p)
{
	real_t d = R(sqrt)(p.nx*p.nx + p.ny*p.ny);

	if (p.r < -PSC_EPS)
		goto invalid;

	if (R(fabs)(p.r) < PSC_EPS) {
		if (R(fabs)(p.nx) < PSC_EPS && R(fabs)(p.ny) < PSC_EPS)
			return true;
		else
			goto invalid;
	}

	if (R(fabs)(d - 1) < PSC_EPS)
		return true;

invalid:
	fprintf(stderr, "Invalid ppoint: nx:%lf ny:%lf r:%lf d:%lf e:%lf\n",
			p.nx, p.ny, p.r, d, PSC_EPS);
	return false;
}

ppoint_t
ppoint_from_radial(real_t phi, real_t rho)
{
	ppoint_t p = {0};
	p.r = rho;

	if (rho != 0) {
#ifdef HAVE_SINCOS
		R(sincos)(phi, &p.ny, &p.nx);
#else
		p.nx = R(cos)(phi);
		p.ny = R(sin)(phi);
#endif
	}

	assert(ppoint_is_valid(p));

	return p;
}

bool
ppoint_codirectinal(ppoint_t a, ppoint_t b)
{
	assert(ppoint_is_valid(a));
	assert(ppoint_is_valid(b));

	if (R(fabs)(a.r) < PSC_EPS || R(fabs)(b.r) < PSC_EPS)
		return true;

	return (R(fabs)(a.nx - b.nx) < PSC_EPS) && (R(fabs)(a.ny - b.ny) < PSC_EPS);
}

ppoint_t
ppoint_normal(ppoint_t a, bool up)
{
	assert(ppoint_is_valid(a));

	ppoint_t p = {0};
	if (R(fabs)(a.r) < PSC_EPS)
		return p;

	p.r = 1;

	if (up) {
		p.nx = -a.ny;
		p.ny = a.nx;
	} else {
		p.nx = a.ny;
		p.ny = -a.nx;
	}

	assert(ppoint_is_valid(p));

	return p;
}

ppoint_t
ppoint_add(ppoint_t a, ppoint_t b)
{
	assert(ppoint_is_valid(a));
	assert(ppoint_is_valid(b));

	real_t x = a.r*a.nx + b.r*b.nx;
	real_t y = a.r*a.ny + b.r*b.ny;
	real_t r = R(sqrt)(x*x + y*y);

	ppoint_t p = {0};

	if (R(fabs)(r) > PSC_EPS) {
		p.r = r;
		p.nx = x / r;
		p.ny = y / r;
	} else {
		p.r = 0;
		p.nx = 0;
		p.ny = 0;
	}

	assert(ppoint_is_valid(p));
	
	return p;
}

point_t
ppoint_to_point(ppoint_t a)
{
	assert(ppoint_is_valid(a));

	point_t p = {
		.x = a.r * a.nx,
		.y = a.r * a.ny
	};

	return p;
}
