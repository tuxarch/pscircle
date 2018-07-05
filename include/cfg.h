#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "types.h"
#include "color.h"
#include "point.h"

typedef struct {
	const char *font_face;
	real_t font_size;
	color_t font_color;

	real_t radius_inc;
	real_t sector;
	real_t rotation;
	bool rotate;
	const char *anchor_proc_name;
	real_t anchor_proc_angle;

	point_t center;

} tree_t;

typedef struct {
	real_t radius;
	real_t border;
	color_t bg_min;
	color_t bg_max;
	color_t fg_min;
	color_t fg_max;
} dot_t;

typedef struct {
	real_t width;
	real_t convexity;
	color_t color_min;
	color_t color_max;
} link_t;

typedef struct {
	bool show;
	bool show_header;
	point_t center;
	const char *name;
	const char *label;
	const char *value_format;
	real_t value;
} toplist_t;

typedef struct {
	real_t width;
	real_t height;
	color_t background;
	color_t color;
} bar_t;

typedef struct {
	real_t row_height;
	real_t font_size;
	color_t font_color;
	color_t pid_font_color;
	const char *font_face;
	real_t column_padding;

	bar_t bar;
	toplist_t cpulist;
	toplist_t memlist;
} toplists_t;

typedef struct {
	bool read_stdin;
	real_t interval;

	const char *output;
	const char *output_display;
	size_t output_width;
	size_t output_height;

	pid_t root_pid;
	nnodes_t max_children;
	memunit_t memory_unit;

	size_t max_mem;
	size_t min_mem;
	real_t max_cpu;
	real_t min_cpu;

	color_t background;
	const char *background_image;

	tree_t tree;
	dot_t dot;
	link_t link;
	toplists_t toplists;

} cfg_t;

extern cfg_t config;


void
parse_cmdline(int argc, char const * argv[]);

