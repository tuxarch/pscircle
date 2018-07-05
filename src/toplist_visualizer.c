#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <ppoint.h>

#include "cfg.h"
#include "node.h"
#include "toplist_visualizer.h"
#include "utils.h"

typedef struct {
	bool offset_headers;
	painter_t *painter;
	real_t pad;
	real_t barw;
} visualizer_t ;

real_t
calc_max_pid_width(painter_t *painter, pnode_t **list);

void
draw_toplist(visualizer_t *vis, toplist_t *cfg, pnode_t **list, point_t pos);

void
draw_toplists_header(visualizer_t *vis, toplist_t *cfg, point_t pos);

void
draw_toplists_row(visualizer_t *vis, toplist_t *cfg, pnode_t *node, point_t pos, real_t pid_width);

void
draw_text(visualizer_t *vis, const char *text, point_t pos);

void
draw_bar(visualizer_t *vis, real_t valur, point_t pos);

void
draw_pid(visualizer_t *vis, pid_t pid, point_t pos);

void
draw_pdot(visualizer_t *vis, pnode_t *node, point_t pos);

void
draw_toplists(painter_t *painter, procs_t *procs)
{
	visualizer_t vis = {
		.painter = painter,
		.pad     = config.toplists.column_padding,
		.barw    = config.toplists.bar.width,
		.offset_headers =
			config.toplists.cpulist.show_header || config.toplists.memlist.show_header,
	};

	painter_set_font_face(painter, config.toplists.font_face);
	painter_set_font_size(painter, config.toplists.font_size);

	size_t nrows = PSC_TOPLIST_MAX_ROWS;
	if (vis.offset_headers)
		nrows++;

	real_t rh = config.toplists.row_height;
	real_t h = rh * (nrows - 1);

	point_t pos_cpu = {
		.x = config.toplists.cpulist.center.x,
		.y = config.toplists.cpulist.center.y + rh/2 - h/2
	};

	draw_toplist(&vis, &config.toplists.cpulist, procs->cpu_toplist, pos_cpu);

	point_t pos_mem = {
		.x = config.toplists.memlist.center.x,
		.y = config.toplists.memlist.center.y + rh/2 - h/2,
	};

	draw_toplist(&vis, &config.toplists.memlist, procs->mem_toplist, pos_mem);
}

void
draw_toplist(visualizer_t *vis, toplist_t *cfg, pnode_t **list, point_t pos)
{
	if (!cfg->show)
		return;

	if (cfg->show_header)
		draw_toplists_header(vis, cfg, pos);

	if (vis->offset_headers)
		pos.y += config.toplists.row_height;

	real_t pid_width = calc_max_pid_width(vis->painter, list);

	for (size_t i = 0; i < PSC_TOPLIST_MAX_ROWS; ++i) {
		if (!list[i])
			break;

		draw_toplists_row(vis, cfg, list[i], pos, pid_width);

		pos.y += config.toplists.row_height;
	}
}

void
draw_toplists_header(visualizer_t *vis, toplist_t *cfg, point_t pos)
{
	assert(vis);
	assert(cfg);

	point_t p = {
		.x = pos.x - vis->barw/2 - vis->pad,
		.y = pos.y
	};

	point_t ndim = painter_text_size(vis->painter, cfg->name);

	p.x -= ndim.x;

	draw_text(vis, cfg->name, p);

	p.x += ndim.x + vis->pad;
	p.y -= ndim.y/2;

	draw_bar(vis, cfg->value, p);

	p.x += vis->barw + vis->pad;
	p.y += ndim.y/2;

	draw_text(vis, cfg->label, p);
}
	
char *
cpu_string(real_t n)
{
	static char buf[PSC_LABEL_BUFSIZE] = {0};
	snprintf(buf, PSC_LABEL_BUFSIZE, "%.1f%%", n);
	return buf;
}

void
draw_toplists_row(visualizer_t *vis, toplist_t *cfg, pnode_t *node, point_t pos, real_t pid_width) 
{
	static char value[PSC_LABEL_BUFSIZE] = {0};
	double m = node->mem;
	const char *u;
	bytes_to_human(&m, &u);

	snprintf(value, PSC_LABEL_BUFSIZE, cfg->value_format,
			cpu_string(node->cpu), m, u);

	point_t vdim = painter_text_size(vis->painter, value);

	pos.x -= vis->barw/2 + vis->pad + vdim.x;

	draw_text(vis, value, pos);

	pos.x += vdim.x + vis->pad;
	pos.y -= vdim.y/2;

	draw_pdot(vis, node, pos);

	pos.x += vis->barw + vis->pad;
	pos.y += vdim.y/2;

	draw_pid(vis, node->pid, pos);

	pos.x += pid_width + vis->pad;

	draw_text(vis, node->name, pos);
}

void
draw_pdot(visualizer_t *vis, pnode_t *node, point_t pos)
{
	real_t mem = pnode_mem_percentage(node);
	real_t cpu = pnode_cpu_percentage(node);

	line_t line = {
		.a = pos,
		.b = {
			.x = pos.x + vis->barw,
			.y = pos.y
		},
		.width = config.link.width,
		.color = color_between(config.link.color_min, config.link.color_max, mem)
	};

	painter_draw_line(vis->painter, line);

	circle_t dot = {
		.center = {
			.x = pos.x + vis->barw/2,
			.y = pos.y
		},
		.radius = config.dot.radius,
		.border = config.dot.border,
		.background = color_between(config.dot.bg_min, config.dot.bg_max, cpu),
		.foreground = color_between(config.dot.fg_min, config.dot.fg_max, mem)
	};

	painter_draw_circle(vis->painter, dot);
}

void
draw_pid(visualizer_t *vis, pid_t pid, point_t pos)
{
	static char buf[PSC_LABEL_BUFSIZE] = {0};
	snprintf(buf, PSC_LABEL_BUFSIZE, "%d", pid);

	text_t t = {
		.refpoint = pos,
		.angle    = 0,
		.str      = buf,
		.foreground = config.toplists.pid_font_color
	};

	painter_draw_text(vis->painter, t);
}

void
draw_text(visualizer_t *vis, const char *text, point_t pos)
{
	text_t t = {
		.refpoint = pos,
		.angle    = 0,
		.str      = text,
		.foreground = config.toplists.font_color
	};

	painter_draw_text(vis->painter, t);
}

void
draw_bar(visualizer_t *vis, real_t value, point_t pos)
{
	assert(vis);

	if (value < 0)
		value = 0;
	if (value > 1)
		value = 1;

	line_t bg = {
		.a = pos,
		.b = {
			.x = pos.x + config.toplists.bar.width,
			.y = pos.y
		},
		.width = config.toplists.bar.height,
		.color = config.toplists.bar.background
	};

	painter_draw_line(vis->painter, bg);

	line_t fg = {
		.a = pos,
		.b = {
			.x = pos.x + value * config.toplists.bar.width,
			.y = pos.y
		},
		.width = config.toplists.bar.height,
		.color = config.toplists.bar.color
	};

	painter_draw_line(vis->painter, fg);
}

real_t
calc_max_pid_width(painter_t *painter, pnode_t **list)
{
	assert(painter);
	assert(list);

	real_t max_width = 0;
	for (size_t i = 0; i < PSC_TOPLIST_MAX_ROWS; ++i) {
		if (!list[i])
			break;
		point_t dim = painter_text_int_size(painter, list[i]->pid);
		if (dim.x > max_width)
			max_width = dim.x;
	}
	return max_width;
}

