#include "gtest/gtest.h"

extern "C" {
#include "color.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(color_from_hex, hex_8_digits) {
	color_t c = color_from_hex("0A141E28");
	EXPECT_NEAR(c.r, 10./255, EPS);
	EXPECT_NEAR(c.g, 20./255, EPS);
	EXPECT_NEAR(c.b, 30./255, EPS);
	EXPECT_NEAR(c.a, 40./255, EPS);
}

TEST(color_from_hex, hex_6_digits) {
	color_t c = color_from_hex("0A141E");
	EXPECT_NEAR(c.r, 10./255, EPS);
	EXPECT_NEAR(c.g, 20./255, EPS);
	EXPECT_NEAR(c.b, 30./255, EPS);
	EXPECT_NEAR(c.a, 1, EPS);
}

TEST(color_from_hex, hex_3_digits) {
	color_t c = color_from_hex("ABC");
	EXPECT_NEAR(c.r, 170./255, EPS);
	EXPECT_NEAR(c.g, 187./255, EPS);
	EXPECT_NEAR(c.b, 204./255, EPS);
	EXPECT_NEAR(c.a, 1, EPS);
}

TEST(color_between, at_0) {
	color_t a = {0.8, 0.1, 0.5, 0.2};
	color_t b = {0.4, 0.9, 0.8, 0.9};
	color_t c = color_between(a, b, 0);

	EXPECT_NEAR(c.r, 0.8, EPS);
	EXPECT_NEAR(c.g, 0.1, EPS);
	EXPECT_NEAR(c.b, 0.5, EPS);
	EXPECT_NEAR(c.a, 0.2, EPS);
}

TEST(color_between, at_1) {
	color_t a = {0.8, 0.1, 0.5, 0.2};
	color_t b = {0.4, 0.9, 0.8, 0.9};
	color_t c = color_between(a, b, 1);

	EXPECT_NEAR(c.r, 0.4, EPS);
	EXPECT_NEAR(c.g, 0.9, EPS);
	EXPECT_NEAR(c.b, 0.8, EPS);
	EXPECT_NEAR(c.a, 0.9, EPS);
}

TEST(color_between, between_0_and_1) {
	color_t a = {0.8, 0.1, 0.5, 0.2};
	color_t b = {0.4, 0.9, 0.8, 0.9};

	for (double k = 0.1; k <= 0.9; k += 0.1) {
		color_t c = color_between(a, b, k);
		EXPECT_NEAR(c.r, a.r + k * (b.r - a.r), EPS);
		EXPECT_NEAR(c.g, a.g + k * (b.g - a.g), EPS);
		EXPECT_NEAR(c.b, a.b + k * (b.b - a.b), EPS);
		EXPECT_NEAR(c.a, a.a + k * (b.a - a.a), EPS);
	}
}

TEST(color_to_hex, valid_color) {
	color_t a = {10./255, 20./255, 30./255, 40./255};
	char *c = color_to_hex(a);
	EXPECT_STREQ(c, "0A141E28");
	free(c);
}
