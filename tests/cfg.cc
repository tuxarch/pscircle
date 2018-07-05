#include <cmath>

#include "gtest/gtest.h"

extern "C" {
#include "cfg.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

template<typename T>
void parse(const char *arg, T &src, T expected) {
	int argc = 2;
	const char *argv[] = {"argv_0", arg};
	parse_cmdline(argc, argv);
	EXPECT_EQ(src, expected);
}

void parse(const char *arg, const char *&src, const char *expected) {
	int argc = 2;
	const char *argv[] = {"argv_0", arg};
	parse_cmdline(argc, argv);
	EXPECT_STREQ(src, expected);
}


void parse(const char *arg, color_t &src, const char *colhex) {
	int argc = 2;
	const char *argv[] = {"argv_0", arg};
	parse_cmdline(argc, argv);
	color_t s = color_from_hex(colhex);
	EXPECT_NEAR(src.r, s.r, EPS);
	EXPECT_NEAR(src.g, s.g, EPS);
	EXPECT_NEAR(src.b, s.b, EPS);
	EXPECT_NEAR(src.a, s.a, EPS);
}

void parse(const char *arg, point_t &src, real_t x, real_t y) {
	int argc = 2;
	const char *argv[] = {"argv_0", arg};
	parse_cmdline(argc, argv);
	EXPECT_NEAR(src.x, x, EPS);
	EXPECT_NEAR(src.y, y, EPS);
}

TEST(parse_cmdline, read_stdin) {
	parse<bool>("--stdin=true", config.read_stdin, true);
}

TEST(parse_cmdline, interval) {
	parse<real_t>("--interval=31", config.interval, 31);
}

TEST(parse_cmdline, output) {
	parse("--output=aaa.png", config.output, "aaa.png");
}

#ifdef HAVE_X11
TEST(parse_cmdline, output_display) {
	parse("--output-display=bbb", config.output_display, "bbb");
}
#endif

TEST(parse_cmdline, output_width) {
	parse<size_t>("--output-width=123", config.output_width, 123);
}

TEST(parse_cmdline, root_pid) {
	parse("--root-pid=22", config.root_pid, 22);
}

TEST(parse_cmdline, max_children) {
	parse<nnodes_t>("--max-children=22", config.max_children, 22);
}

TEST(parse_cmdline, memory_unit) {
	parse<memunit_t>("--memory-unit=M", config.memory_unit, 2);
}

TEST(parse_cmdline, memory_min_value) {
	parse<size_t>("--memory-min-value=20K", config.min_mem, 20*1024);
}

TEST(parse_cmdline, memory_max_value) {
	parse<size_t>("--memory-max-value=60M", config.max_mem, 60*1024*1024);
}

TEST(parse_cmdline, cpu_min_value) {
	parse<real_t>("--cpu-min-value=0.2", config.min_cpu, 0.2);
}

TEST(parse_cmdline, cpu_max_value) {
	parse<real_t>("--cpu-max-value=0.8", config.max_cpu, 0.8);
}

TEST(parse_cmdline, background_color) {
	parse("--background-color=DEADBEEF", config.background, "DEADBEEF");
}

TEST(parse_cmdline, background_image) {
	parse("--background-image=file.png", config.background_image, "file.png");
}

TEST(parse_cmdline, tree_center) {
	parse("--tree-center=1:4.3", config.tree.center, 1, 4.3);
}

TEST(parse_cmdline, tree_radius_increment) {
	parse<real_t>("--tree-radius-increment=30", config.tree.radius_inc, 30);
}

TEST(parse_cmdline, tree_sector_angle) {
	parse<real_t>("--tree-sector-angle=12", config.tree.sector, 12);
}

TEST(parse_cmdline, tree_rotate) {
	parse<bool>("--tree-rotate=true", config.tree.rotate, true);
}

TEST(parse_cmdline, tree_rotation_angle) {
	parse<real_t>("--tree-rotation-angle=13.4", config.tree.rotation, 13.4);
}

TEST(parse_cmdline, anchor_proc_name) {
	parse("--tree-anchor-proc-name=aaxx", config.tree.anchor_proc_name, "aaxx");
}

TEST(parse_cmdline, anchor_proc_angle) {
	parse<real_t>("--tree-anchor-proc-angle=0.145", config.tree.anchor_proc_angle, 0.145);
}

TEST(parse_cmdline, tree_font_size) {
	parse<real_t>("--tree-font-size=3.2", config.tree.font_size, 3.2);
}

TEST(parse_cmdline, tree_font_face) {
	parse("--tree-font-face=cx", config.tree.font_face, "cx");
}

TEST(parse_cmdline, tree_font_color) {
	parse("--tree-font-color=12345678", config.tree.font_color, "12345678");
}

TEST(parse_cmdline, dot_radius) {
	parse<real_t>("--dot-radius=45", config.dot.radius, 45);
}

TEST(parse_cmdline, dot_border_width) {
	parse<real_t>("--dot-border-width=78", config.dot.border, 78);
}

TEST(parse_cmdline, dot_color_min) {
	parse("--dot-color-min=444", config.dot.bg_min, "444");
}

TEST(parse_cmdline, dot_color_max) {
	parse("--dot-color-max=888", config.dot.bg_max, "888");
}

TEST(parse_cmdline, dot_border_color_min) {
	parse("--dot-border-color-min=777", config.dot.fg_min, "777");
}

TEST(parse_cmdline, dot_border_color_max) {
	parse("--dot-border-color-max=AB9", config.dot.fg_max, "AB9");
}

TEST(parse_cmdline, link_width) {
	parse<real_t>("--link-width=1.45", config.link.width, 1.45);
}

TEST(parse_cmdline, link_convexity) {
	parse<real_t>("--link-convexity=22.4", config.link.convexity, 22.4);
}

TEST(parse_cmdline, link_color_min) {
	parse("--link-color-min=DFDFDF", config.link.color_min, "DFDFDF");
}

TEST(parse_cmdline, link_color_max) {
	parse("--link-color-max=DADADA", config.link.color_max, "DADADA");
}

TEST(parse_cmdline, toplists_row_height) {
	parse<real_t>("--toplists-row-height=420", config.toplists.row_height, 420);
}

TEST(parse_cmdline, toplists_font_size) {
	parse<real_t>("--toplists-font-size=43", config.toplists.font_size, 43);
}

TEST(parse_cmdline, toplists_font_color) {
	parse("--toplists-font-color=993", config.toplists.font_color, "993");
}

TEST(parse_cmdline, toplists_pid_font_color) {
	parse("--toplists-pid-font-color=430", config.toplists.pid_font_color, "430");
}

TEST(parse_cmdline, toplists_font_face) {
	parse("--toplists-font-face=ab", config.toplists.font_face, "ab");
}

TEST(parse_cmdline, toplists_column_padding) {
	parse<real_t>("--toplists-column-padding=199", config.toplists.column_padding, 199);
}

TEST(parse_cmdline, toplists_bar_width) {
	parse<real_t>("--toplists-bar-width=4245", config.toplists.bar.width, 4245);
}

TEST(parse_cmdline, toplists_bar_height) {
	parse<real_t>("--toplists-bar-height=666", config.toplists.bar.height, 666);
}

TEST(parse_cmdline, toplists_bar_background) {
	parse("--toplists-bar-background=5AD", config.toplists.bar.background, "5AD");
}

TEST(parse_cmdline, toplists_bar_color) {
	parse("--toplists-bar-color=7AB", config.toplists.bar.color, "7AB");
}

TEST(parse_cmdline, cpulist_show) {
	parse<bool>("--cpulist-show=false", config.toplists.cpulist.show, false);
}

TEST(parse_cmdline, cpulist_show_header) {
	parse<bool>("--cpulist-show-header=true", config.toplists.cpulist.show_header, true);
}

TEST(parse_cmdline, cpulist_center) {
	parse("--cpulist-center=5:0.9", config.toplists.cpulist.center, 5, 0.9);
}

TEST(parse_cmdline, cpulist_name) {
	parse("--cpulist-name=p", config.toplists.cpulist.name, "p");
}

TEST(parse_cmdline, cpulist_label) {
	parse("--cpulist-label=aaah", config.toplists.cpulist.label, "aaah");
}

TEST(parse_cmdline, cpulist_bar_value) {
	parse<real_t>("--cpulist-bar-value=0.44", config.toplists.cpulist.value, 0.44);
}

TEST(parse_cmdline, memlist_show) {
	parse("--memlist-show=false", config.toplists.memlist.show, false);
}

TEST(parse_cmdline, memlist_center) {
	parse("--memlist-center=3:1.9", config.toplists.memlist.center, 3, 1.9);
}

TEST(parse_cmdline, memlist_show_header) {
	parse<bool>("--memlist-show-header=true", config.toplists.memlist.show_header, "true");
}

TEST(parse_cmdline, memlist_name) {
	parse("--memlist-name=nn", config.toplists.memlist.name, "nn");
}

TEST(parse_cmdline, memlist_label) {
	parse("--memlist-label=i", config.toplists.memlist.label, "i");
}

TEST(parse_cmdline, memlist_bar_value) {
	parse<real_t>("--memlist-bar-value=442.1", config.toplists.memlist.value, 442.1);
}

