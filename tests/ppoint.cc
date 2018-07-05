#include <cmath>

#include "gtest/gtest.h"

extern "C" {
#include "ppoint.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(ppoint_from_radial, non_zero) {
	ppoint_t p = ppoint_from_radial(M_PI / 6, 2.);
	EXPECT_NEAR(p.nx, sqrt(3) / 2, EPS);
	EXPECT_NEAR(p.ny, 0.5, EPS);
	EXPECT_NEAR(p.r, 2, EPS);
}

TEST(ppoint_from_radial, zero_width) {
	ppoint_t p = ppoint_from_radial(M_PI / 6, 0.);
	EXPECT_NEAR(p.nx, 0., EPS);
	EXPECT_NEAR(p.ny, 0., EPS);
	EXPECT_NEAR(p.r, 0, EPS);
}

TEST(ppoint_codirectinal, codirectinal) {
	ppoint_t a = {sqrt(3)/2, 0.5, 2.};
	ppoint_t b = {sqrt(3)/2, 0.5, 1.};
	EXPECT_TRUE(ppoint_codirectinal(a, b));
}

TEST(ppoint_codirectinal, not_codirectinal) {
	ppoint_t a = {sqrt(3)/2, 0.5, 2.};
	ppoint_t b = {sqrt(2)/2, sqrt(2)/2, 1.};
	EXPECT_FALSE(ppoint_codirectinal(a, b));
}

TEST(ppoint_codirectinal, ortogonal) {
	ppoint_t a = {sqrt(3)/2, 0.5, 2.};
	ppoint_t b = {-0.5, sqrt(3)/2, 2.};
	EXPECT_FALSE(ppoint_codirectinal(a, b));
}

TEST(ppoint_codirectinal, opposite_direction) {
	ppoint_t a = {sqrt(3)/2, 0.5, 2.};
	ppoint_t b = {-sqrt(3)/2, -0.5, 2.};
	EXPECT_FALSE(ppoint_codirectinal(a, b));
}

TEST(ppoint_codirectinal, one_zero_vector) {
	ppoint_t a = {0., 0., 0.};
	ppoint_t b = {sqrt(3)/2, 0.5, 2.};
	EXPECT_TRUE(ppoint_codirectinal(a, b));
}

TEST(ppoint_codirectinal, two_zero_vector) {
	ppoint_t a = {0., 0., 0.};
	ppoint_t b = {0., 0., 0.};
	EXPECT_TRUE(ppoint_codirectinal(a, b));
}

TEST(ppoint_normal, up_normal) {
	ppoint_t p = {sqrt(3)/2, 0.5, 2.};
	ppoint_t n = ppoint_normal(p, true);
	EXPECT_NEAR(n.r, 1, EPS);
	EXPECT_NEAR(n.nx, -0.5, EPS);
	EXPECT_NEAR(n.ny, sqrt(3) / 2, EPS);
}

TEST(ppoint_normal, down_normal) {
	ppoint_t p = {sqrt(3)/2, 0.5, 2};
	ppoint_t n = ppoint_normal(p, false);

	EXPECT_NEAR(n.r, 1, EPS);
	EXPECT_NEAR(n.nx, 0.5, EPS);
	EXPECT_NEAR(n.ny, -sqrt(3) / 2, EPS);
}

TEST(ppoint_normal, up_normal_of_zero_vector) {
	ppoint_t p = {0., 0., 0.};
	ppoint_t n = ppoint_normal(p, false);
	EXPECT_NEAR(n.r, 0, EPS);
	EXPECT_NEAR(n.nx, 0, EPS);
	EXPECT_NEAR(n.ny, 0, EPS);
}

TEST(ppoint_normal, down_normal_of_zero_vector) {
	ppoint_t p = {0., 0., 0.};
	ppoint_t n = ppoint_normal(p, true);
	EXPECT_NEAR(n.r, 0, EPS);
	EXPECT_NEAR(n.nx, 0, EPS);
	EXPECT_NEAR(n.ny, 0, EPS);
}

TEST(ppoint_add, two_orthogonal) {
	ppoint_t a = {1, 0, 3};
	ppoint_t b = {0, -1, 4};

	ppoint_t c = ppoint_add(a, b);

	EXPECT_NEAR(c.r, 5, EPS);
	EXPECT_NEAR(c.nx, 3./5, EPS);
	EXPECT_NEAR(c.ny, -4./5, EPS);
}

TEST(ppoint_add, two_opposite) {
	ppoint_t a = {1, 0, 3};
	ppoint_t b = {-1, 0, 3};

	ppoint_t c = ppoint_add(a, b);

	EXPECT_NEAR(c.r, 0, EPS);
	EXPECT_NEAR(c.nx, 0, EPS);
	EXPECT_NEAR(c.ny, 0, EPS);
}


TEST(ppoint_to_point, valid_ppoint) {
	ppoint_t a = {sqrt(3)/2, 0.5, 2.};

	point_t c = ppoint_to_point(a);

	EXPECT_NEAR(c.x, sqrt(3), EPS);
	EXPECT_NEAR(c.y, 1, EPS);
}
