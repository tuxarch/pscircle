#include <cmath>

#include "gtest/gtest.h"

extern "C" {
#include "types.h"
#include "point.h"
#include "color.h"
#include "argparser.h"
}

#define EPS 1e-5

using namespace std;
using namespace ::testing;

class args_test: public Test
{
public:
	args_test() {  }

	virtual ~args_test() {  }

	argparser_t *argparser;

	void SetUp() {
		argparser = new argparser_t();
		argparser_init(argparser);
	}

	void parse(vector<string> args) {

		int argc = args.size() +  1;
		auto argv = new const char *[argc];

		argv[0] = "argv_0";
		for (int i = 1; i < argc; ++i)
			argv[i] = args[i - 1].c_str();

		argparser_parse(argparser, argc, argv);
	}

	void TearDown() {
		delete argparser;
	}
};

TEST_F(args_test, help_screen_long) {
	EXPECT_EXIT(parse({"--help"}), ExitedWithCode(0), ".*");
}

TEST_F(args_test, help_screen_short) {
	EXPECT_EXIT(parse({"-h"}), ExitedWithCode(0), ".*");
}

TEST_F(args_test, a_few_args) {
	long arg1;
	point_t arg2;

	ARG(argparser, "--arg-1", arg1, parser_long, "d", "d");
	ARG(argparser, "--arg-2", arg2, parser_point, "d", "d");

	parse({"--arg-1=10", "--arg-2=2:3"});

	EXPECT_EQ(arg1, 10u);
	EXPECT_NEAR(arg2.x, 2, EPS);
	EXPECT_NEAR(arg2.y, 3, EPS);
}

TEST_F(args_test, unkown_valid_arg) {
	long arg1;
	point_t arg2;

	ARG(argparser, "--arg-1", arg1, parser_long, "d", "d");
	ARG(argparser, "--arg-2", arg2, parser_point, "d", "d");

	EXPECT_EXIT(parse({"--arg-3=30"}), ExitedWithCode(1), ".*");
}

TEST_F(args_test, invalid_arg_no_key) {
	long arg1;
	ARG(argparser, "--arg-1", arg1, parser_long, "d", "d");

	EXPECT_EXIT(parse({"=10"}), ExitedWithCode(1), ".*");
}

TEST_F(args_test, invalid_arg_no_value) {
	long arg1;
	ARG(argparser, "--arg-1", arg1, parser_long, "d", "d");

	EXPECT_EXIT(parse({"--arg-1"}), ExitedWithCode(1), ".*");
}

TEST_F(args_test, parse_error) {
	long arg1;

	ARG(argparser, "--arg-1", arg1, parser_long, "d", "d");

	EXPECT_EXIT(parse({"--arg-1=aaa"}), ExitedWithCode(1), ".*");
}

TEST_F(args_test, empry_string) {
	char *arg1 = nullptr;

	ARG(argparser, "--arg-1", arg1, parser_string, "d", "d");

	parse({"--arg-1="});

	ASSERT_NE(arg1, nullptr);
	EXPECT_STREQ(arg1, "");
}

TEST_F(args_test, long_key) {
	char *arg1 = nullptr;

	stringstream ss;
	ss << setw(PSC_ARG_KEY_BUFSIZE + 10) << setfill('0') << "a";
	string s = ss.str();

	ARG(argparser, "--a", arg1, parser_string, "d", "d");

	EXPECT_EXIT(parse({s + "=value"}), ExitedWithCode(1), ".*");
}

TEST_F(args_test, multiple_args) {
	char *arg1 = nullptr;
	char *arg2 = nullptr;
	char *arg3 = nullptr;
	char *arg4 = nullptr;

	ARG(argparser, "--a", arg1, parser_string, "d", "d");
	ARG(argparser, "--bb", arg2, parser_string, "d", "d");
	ARG(argparser, "--c", arg3, parser_string, "d", "d");
	ARG(argparser, "--ddd", arg4, parser_string, "d", "d");

	parse({"--a=1", "--bb=2", "--c=3", "--ddd=4"});

	ASSERT_NE(arg1, nullptr);
	ASSERT_NE(arg2, nullptr);
	ASSERT_NE(arg3, nullptr);
	ASSERT_NE(arg4, nullptr);

	EXPECT_STREQ(arg1, "1");
	EXPECT_STREQ(arg2, "2");
	EXPECT_STREQ(arg3, "3");
	EXPECT_STREQ(arg4, "4");
}

TEST(parser_memory_unit, unknown) {
	memunit_t u;
	EXPECT_FALSE(parser_memory_unit("S", &u));
}

TEST(parser_memory_unit, K) {
	memunit_t u;
	EXPECT_TRUE(parser_memory_unit("K", &u));
	EXPECT_EQ(u, 1u);
}

TEST(parser_memory_unit, B) {
	memunit_t u;
	EXPECT_TRUE(parser_memory_unit("B", &u));
	EXPECT_EQ(u, 0u);
}

TEST(parser_memory_unit, M) {
	memunit_t u;
	EXPECT_TRUE(parser_memory_unit("M", &u));
	EXPECT_EQ(u, 2u);
}

TEST(parser_memory, invalid_value) {
	size_t m;
	EXPECT_FALSE(parser_memory("aaK", &m));
}

TEST(parser_memory, invalid_SUFFIX) {
	size_t m;
	EXPECT_FALSE(parser_memory("10S", &m));
}

TEST(parser_memory, no_suffix) {
	size_t m;
	EXPECT_TRUE(parser_memory("10", &m));
	EXPECT_EQ(m, 10u);
}

TEST(parser_memory, B) {
	size_t m;
	EXPECT_TRUE(parser_memory("10B", &m));
	EXPECT_EQ(m, 10u);
}

TEST(parser_memory, M) {
	size_t m;
	EXPECT_TRUE(parser_memory("10M", &m));
	EXPECT_EQ(m, 10u*1024*1024);
}

TEST(parse_ulong, invalid) {
	unsigned long ul;
	EXPECT_FALSE(parser_ulong("10a", &ul));
}

TEST(parse_ulong, valid) {
	unsigned long ul;
	EXPECT_TRUE(parser_ulong("10", &ul));
	EXPECT_EQ(ul, 10u);
}

TEST(parse_long, invalid) {
	long val;
	EXPECT_FALSE(parser_long("10a", &val));
}

TEST(parse_long, valid) {
	long val;
	EXPECT_TRUE(parser_long("10", &val));
	EXPECT_EQ(val, 10l);
}

TEST(parse_string, valid) {
	char *val = NULL;
	EXPECT_TRUE(parser_string("10", &val));
	EXPECT_STREQ(val, "10");
	free(val);
}

TEST(parse_point, x_invalid) {
	point_t val;
	EXPECT_FALSE(parser_point("1a0:1", &val));
}

TEST(parse_point, y_invalid) {
	point_t val;
	EXPECT_FALSE(parser_point("10:1a", &val));
}

TEST(parse_point, invalid_no_col) {
	point_t val;
	EXPECT_FALSE(parser_point("10", &val));
}

TEST(parse_point, invalid_no_x) {
	point_t val;
	EXPECT_FALSE(parser_point(":10", &val));
}

TEST(parse_point, invalid_no_y) {
	point_t val;
	EXPECT_FALSE(parser_point("10:", &val));
}

TEST(parse_point, valid) {
	point_t val;
	EXPECT_TRUE(parser_point("10.2:3.01", &val));
	EXPECT_NEAR(val.x, 10.2, EPS);
	EXPECT_NEAR(val.y, 3.01, EPS);
}

TEST(parse_color, invalid) {
	color_t val;
	const char *output = "Can not parse HEX color: zzz";
	EXPECT_EXIT(parser_color("zzz", &val), ExitedWithCode(1), output);
}

TEST(parse_color, valid) {
	color_t val;
	EXPECT_TRUE(parser_color("EBC", &val));
	EXPECT_NEAR(val.r, 238./255, EPS);
	EXPECT_NEAR(val.g, 187./255, EPS);
	EXPECT_NEAR(val.b, 204./255, EPS);
}

TEST(parse_real, invalid) {
	real_t val;

	EXPECT_FALSE(parser_real("10a", &val));
}

TEST(parse_real, valid) {
	real_t val;
	EXPECT_TRUE(parser_real("10.2", &val));
	EXPECT_NEAR(val, 10.2, EPS);
}

TEST(parse_bool, invalid) {
	int val;
	EXPECT_FALSE(parser_bool("10.2", &val));
}

TEST(parse_bool, valid_true_uppercase) {
	int val = 0;
	EXPECT_TRUE(parser_bool("TRUE", &val));
	EXPECT_TRUE(val);
}

TEST(parse_bool, valid_true) {
	int val = 0;
	EXPECT_TRUE(parser_bool("true", &val));
	EXPECT_TRUE(val);
}

TEST(parse_bool, valid_false) {
	int val = 0;
	EXPECT_TRUE(parser_bool("false", &val));
	EXPECT_FALSE(val);
}

TEST(parse_bool, valid_1) {
	int val = 0;
	EXPECT_TRUE(parser_bool("1", &val));
	EXPECT_TRUE(val);
}

TEST(parse_bool, valid_0) {
	int val = 0;
	EXPECT_TRUE(parser_bool("0", &val));
	EXPECT_FALSE(val);
}
