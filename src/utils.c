#include <stdio.h>
#include <assert.h>

#include "utils.h"

void
bytes_to_human(double *n, const char **u)
{
	assert(n);
	assert(u);

	static const char *units[] = {
		"B", "K", "M", "G", "T", "P", "E", "Z", "Y"
	};

	*u = units[0];

	for (size_t i = 0; i < sizeof(units)/sizeof(*units); ++i) {
		if (*n < 1024) {
			*u = units[i];
			break;
		}
		*n /= 1024;
	}
}
