#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "color.h"

bool color_is_valid(color_t c) {
	if (c.r < 0 || c.r > 1) 
		return false;
	if (c.g < 0 || c.g > 1) 
		return false;
	if (c.b < 0 || c.b > 1) 
		return false;
	if (c.a < 0 || c.a > 1) 
		return false;
	return true;
}

color_t color_from_hex(const char *hstr)
{
	uint32_t r = 0;
	uint32_t g = 0;
	uint32_t b = 0;
	uint32_t a = 255;

	switch (strlen(hstr)) {
		case 3:
			if (sscanf(hstr, "%01x%01x%01x", &r, &g, &b) != 3)
				goto error;

			r *= 0x11;
			g *= 0x11;
			b *= 0x11;

			break;

		case 6:
			if (sscanf(hstr, "%02x%02x%02x", &r, &g, &b) != 3)
				goto error;
			break;
		case 8:
			if (sscanf(hstr, "%02x%02x%02x%02x", &r, &g, &b, &a) != 4)
				goto error;
			break;
		default:
			goto error;
	}

	color_t col = {
		.r = (real_t) r / 255,
		.g = (real_t) g / 255,
		.b = (real_t) b / 255,
		.a = (real_t) a / 255
	};

	assert(color_is_valid(col));
	return col;

error:
	fprintf(stderr, "Can not parse HEX color: %s\n", hstr);
	exit(EXIT_FAILURE);
}

color_t
color_between(color_t a, color_t b, real_t k)
{
	assert(color_is_valid(a));
	assert(color_is_valid(b));
	assert(k >= 0 && k <= 1);

	color_t col = {
		.r = a.r + k * (b.r - a.r),
		.g = a.g + k * (b.g - a.g),
		.b = a.b + k * (b.b - a.b),
		.a = a.a + k * (b.a - a.a)
	};

	return col;
}

char *
color_to_hex(color_t col)
{
	assert(color_is_valid(col));

	uint_fast8_t r = col.r * 255;
	uint_fast8_t g = col.g * 255;
	uint_fast8_t b = col.b * 255;
	uint_fast8_t a = col.a * 255;

	char *buf = calloc(PSC_COLOR_BUFSIZE, sizeof(char));

	snprintf(buf, PSC_COLOR_BUFSIZE, "%02X%01X%02X%02X", r, g, b, a);

	return buf;
}
