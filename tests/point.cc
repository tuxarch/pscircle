#include <cmath>

#include "gtest/gtest.h"

extern "C" {
#include "point.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(point_to_str, valid) {
	point_t p = {123.123456, 789.123456};
	char *s = point_to_str(p);
	EXPECT_STREQ(s, "123.1:789.1");
	free(s);
}

