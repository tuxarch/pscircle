#include <stdlib.h>
#include <stdio.h>

#include "point.h"

char *
point_to_str(point_t p)
{
	char *buf = calloc(PSC_POINT_BUFSIZE, sizeof(char));
	snprintf(buf, PSC_POINT_BUFSIZE, "%.1f:%.1f", p.x, p.y);
	return buf;
}
