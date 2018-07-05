#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "painter.h"
#include "cfg.h"

#ifndef M_PI
#define M_PI R(3.14159265358979323846)
#endif

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

void
create_image_surface(painter_t *painter);

void
create_image_surface_from_png(painter_t *painter);

void
destroy_image_surface(painter_t *painter);

void
write_image_surface(painter_t *painter);

#ifdef HAVE_X11
void
create_xlib_surface(painter_t *painter);

void
destroy_xlib_surface(painter_t *painter);

void
write_xlib_surface(painter_t *painter);
#endif

void
painter_init(painter_t *painter)
{
#ifndef NDEBUG
	uint8_t zeros[sizeof(painter_t)];
	memset(zeros, 0, sizeof(zeros));
	assert(memcmp(zeros, painter, sizeof(painter_t)) == 0);
#endif

	assert(config.output_width > 0);
	assert(config.output_height > 0);

#ifdef HAVE_X11
	if (config.output == NULL) {
		create_xlib_surface(painter);
	} else
#endif
	{
		create_image_surface(painter);
	}

	painter->_cr = cairo_create(painter->_surface);
	CHECK(painter->_cr);

	painter_fill_backgound_color(painter, config.background);

	if (config.background_image)
		painter_fill_backgound_image(painter, config.background_image);

	painter_center(painter);
}

void
painter_dinit(painter_t *painter)
{
	assert(painter);
	assert(painter->_cr);
	assert(painter->_surface);

#ifdef HAVE_X11
	if (config.output == NULL) {
		destroy_xlib_surface(painter);
	} else
#endif
	{
		destroy_image_surface(painter);
	}

	cairo_destroy(painter->_cr);
}

void
create_image_surface_from_png(painter_t *painter)
{
	assert(painter);
	assert(config.background_image);

	cairo_surface_t *sf = cairo_image_surface_create_from_png(config.background_image); 

	if (!sf) {
		fprintf(stderr,
				"Can not create surface from %s. (Only PNG is supported)\n", 
				config.background_image);
		exit(EXIT_FAILURE);
	}

	painter->_surface = sf;

	config.output_width = cairo_image_surface_get_width(sf);
	config.output_height = cairo_image_surface_get_height(sf);
}

void
create_image_surface(painter_t *painter)
{
	painter->_surface = cairo_image_surface_create(
			CAIRO_FORMAT_ARGB32, config.output_width, config.output_height); 
	CHECK(painter->_surface);
}

void
destroy_image_surface(painter_t *painter)
{
	cairo_surface_destroy(painter->_surface);
}

void
write_image_surface(painter_t *painter)
{
	const char *output = config.output;
	if (!output)
		output = "pscircle.png";

	cairo_surface_write_to_png(painter->_surface, output);
}

#ifdef HAVE_X11

void
xlib_pixmap_destroy(painter_t *painter)
{
	assert(painter);
	assert(painter->_display);
	assert(painter->_pixmap);

	if (!painter)
		return;
}

void
create_xlib_surface(painter_t *painter)
{
	painter->_display = XOpenDisplay(config.output_display);
	if (!painter->_display) {
		fprintf(stderr, "Unable to open display: '%s'\n", XDisplayName(config.output_display));
		exit(EXIT_FAILURE);
	}

	int screen = DefaultScreen(painter->_display);
	painter->_window = RootWindow(painter->_display, screen);

	int depth = DefaultDepth(painter->_display, screen);
	painter->_pixmap = XCreatePixmap(painter->_display, painter->_window,
			config.output_width, config.output_height, depth);

	Visual *visual = DefaultVisual(painter->_display, screen);
	painter->_surface = cairo_xlib_surface_create(painter->_display,
			painter->_pixmap, visual, config.output_width,
			config.output_height);

	cairo_surface_set_user_data(painter->_surface, NULL, NULL, NULL);

	cairo_xlib_surface_set_size(painter->_surface,
			config.output_width, config.output_height);
}

void
destroy_xlib_surface(painter_t *painter)
{
	assert(painter);
	assert(painter->_display);
	assert(painter->_window);
	assert(painter->_surface);

	XFreePixmap(painter->_display, painter->_pixmap);

	XClearWindow(painter->_display, painter->_window);

	cairo_surface_destroy(painter->_surface);
	XCloseDisplay(painter->_display);
}

void
write_xlib_surface(painter_t *painter)
{
	assert(painter);
	assert(painter->_display);
	assert(painter->_window);
	assert(painter->_pixmap);

	XSetWindowBackgroundPixmap(painter->_display, painter->_window, painter->_pixmap);
}
#endif


void
painter_save(painter_t *painter)
{
	assert(painter);

	cairo_save(painter->_cr);
}

void
painter_restore(painter_t *painter)
{
	assert(painter);

	cairo_restore(painter->_cr);
}

void
painter_write(painter_t *painter)
{
	assert(painter);

#ifdef HAVE_X11
	if (config.output == NULL) {
		write_xlib_surface(painter);
	} else
#endif

	{
		write_image_surface(painter);
	}
}

point_t
painter_text_size(painter_t *painter, const char *str)
{
	assert(painter);
	assert(str);

	cairo_text_extents_t extents;
	cairo_text_extents(painter->_cr, str, &extents);

	point_t p = {
		.x = extents.width + extents.x_bearing,
		.y = -extents.y_bearing
	};

	return p;
}

point_t
painter_text_int_size(painter_t *painter, int n)
{
	assert(painter);

	static char buf[PSC_LABEL_BUFSIZE] = {0};
	snprintf(buf, PSC_LABEL_BUFSIZE, "%d", n);

	return painter_text_size(painter, buf);
}

void
painter_center(painter_t *painter)
{
	assert(painter);

	cairo_translate(painter->_cr,
			config.output_width/2, config.output_height/2);
}

void
painter_translate(painter_t *painter, point_t position)
{
	assert(painter);

	cairo_translate(painter->_cr, position.x, position.y);
}

void
painter_fill_backgound_color(painter_t *painter, color_t background)
{
	cairo_rectangle(painter->_cr, 0, 0, config.output_width, config.output_height);

	cairo_set_source_rgba(painter->_cr,
		background.r,
		background.g,
		background.b,
		background.a
	);

	cairo_fill(painter->_cr);
}

void
painter_fill_backgound_image(painter_t *painter, const char *imgpah)
{
	assert(painter);
	assert(imgpah);

	cairo_surface_t *img = cairo_image_surface_create_from_png(imgpah); 
	if (!img) {
		fprintf(stderr, "Can not open image %s. (Only PNG is supported)\n", imgpah);
		exit(EXIT_FAILURE);
	}

	// XXX: Can not draw at 0:0 on Xlib surfaces for some reasons
	cairo_set_source_surface(painter->_cr, img, 0.05, 0.05);
	cairo_paint(painter->_cr);

	cairo_surface_destroy(img);
}

void
painter_set_font_face(painter_t *painter, const char *fontface)
{
	cairo_select_font_face(
		painter->_cr, 
		fontface,
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL
	);
}

void
painter_set_font_size(painter_t *painter, real_t fontsize)
{
	cairo_set_font_size(painter->_cr, fontsize);
}

void
painter_draw_circle(painter_t *painter, circle_t circle)
{
	cairo_arc(
		painter->_cr,
		circle.center.x,
		circle.center.y,
		circle.radius,
		0, 2 * M_PI
	);

	if (circle.border > 0) {
		cairo_set_line_width(painter->_cr, circle.border);  

		cairo_set_source_rgba(
			painter->_cr,
			circle.foreground.r,
			circle.foreground.g,
			circle.foreground.b,
			circle.foreground.a
		);

		cairo_stroke_preserve(painter->_cr);
	}

	cairo_set_source_rgba(
		painter->_cr,
		circle.background.r,
		circle.background.g,
		circle.background.b,
		circle.background.a
	);

	cairo_fill(painter->_cr);

	cairo_stroke(painter->_cr);

}

void
painter_draw_line(painter_t *painter, line_t line)
{
	cairo_move_to(painter->_cr, line.a.x, line.a.y);
	cairo_line_to(painter->_cr, line.b.x, line.b.y);

	cairo_set_source_rgba(
		painter->_cr,
		line.color.r,
		line.color.g,
		line.color.b,
		line.color.a
	);

	if (line.width > 0)
		cairo_set_line_width(painter->_cr, line.width);

	cairo_stroke(painter->_cr);
}

void
painter_draw_curve(painter_t *painter, curve_t curve)
{
	cairo_move_to(painter->_cr, curve.a.x, curve.a.y);

	cairo_curve_to(
		painter->_cr,
		curve.ac.x, curve.ac.y,
		curve.bc.x, curve.bc.y,
		curve.b.x, curve.b.y
	);

	cairo_set_source_rgba(
		painter->_cr,
		curve.color.r,
		curve.color.g,
		curve.color.b,
		curve.color.a
	);

	if (curve.width > 0)
		cairo_set_line_width(painter->_cr, curve.width);

	cairo_stroke(painter->_cr);
}

void
painter_draw_text(painter_t *painter, text_t text)
{
	cairo_save(painter->_cr);

	cairo_move_to(painter->_cr, text.refpoint.x, text.refpoint.y);

	cairo_rotate(painter->_cr, text.angle);
	
	cairo_set_source_rgba(
		painter->_cr, 
		text.foreground.r,
		text.foreground.g,
		text.foreground.b,
		text.foreground.a
	);

	cairo_show_text(painter->_cr, text.str);

	cairo_stroke(painter->_cr);

	cairo_restore(painter->_cr);
}
