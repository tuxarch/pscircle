#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "argparser.h"
#include "types.h"
#include "color.h"
#include "point.h"

void
print_help_and_exit(argparser_t *argparser);

void
print_unknown_and_exit(const char *arg);

void
print_parse_error_and_exit(const char *arg);

arg_t *
find_by_key(argparser_t *argparser, const char *key);

void
argparser_init(argparser_t *argparser)
{
#ifndef NDEBUG
	uint8_t zeros[sizeof(argparser_t)];
	memset(zeros, 0, sizeof(zeros));
	assert(memcmp(zeros, argparser, sizeof(argparser_t)) == 0);
#endif
}

void
argparser_add(argparser_t *argparser, arg_t arg)
{
	assert(arg.name);
	assert(arg.output);
	assert(arg.parser);
	assert(argparser);
	assert(argparser->nargs < PSC_MAX_ARGS);
	
	argparser->args[argparser->nargs++] = arg;
}

void
argparser_parse(argparser_t *argparser, int argc, char const * argv[])
{
	assert(argparser);
	assert(argc > 0);
	assert(argv);

	char key[PSC_ARG_KEY_BUFSIZE + 1] = {0};

	for (int i = 1; i < argc; ++i) {
		const char *a = argv[i];
		if (strcmp(a, "-h") == 0 || strcmp(a, "--help") == 0)
			print_help_and_exit(argparser);

		char *eq = strchr(a, '=');
		if (!eq || eq == a)
			print_unknown_and_exit(a);

		assert(eq > a);
		size_t l = eq - a;
		if (l > PSC_ARG_KEY_BUFSIZE)
			l = PSC_ARG_KEY_BUFSIZE;

		strncpy(key, a, l);
		key[l] = '\0';
		char *val = eq + 1;

		/* fprintf(stderr, "%s = %s\n", key, val); */

		arg_t *arg = find_by_key(argparser, key);

		if (!arg)
			print_unknown_and_exit(a);

		if (!arg->parser(val, arg->output))
			print_parse_error_and_exit(a);
	}
}

void
wrap(char *str, size_t width)
{
	assert(str);

	size_t line = 0;

	char *p = NULL;
	char *q = str;

	while (*q) {
		if (*q == ' ') {
			if (p) {
				assert(q > p);
				size_t w = q - p;
				if (line + w >= width) {
					*p = '\n';
					line = 0;
				}

				line += w;
			}
			p = q;
		}

		if (*q == '\n') {
			line = 0;
			p = NULL;
		}

		q++;
	}

	if (p) {
		assert(q > p);
		if (line + (q - p) >= width)
			*p = '\n';
	}
}

void
print_help_and_exit(argparser_t *argparser)
{
	puts(
		"pscircle -- visualizes Linux processes in a radial tree\n"
		"\n"
		"Usage:\n"
		"   pscircle [OPTION]...\n"
		"\n"
		"Options:\n"
	);

	char buf[PSC_ARG_DESCRIPTION_BUFSIZE + 1] = {0};

	for (size_t i = 0; i < argparser->nargs; ++i) {
		arg_t *a = argparser->args + i;
		printf("%*s%s=%s\n", PSC_HELPLIST_OFFSET, " ", a->name, a->defaults);
		strncpy(buf, a->description, PSC_ARG_DESCRIPTION_BUFSIZE);
		wrap(buf, PSC_HELPLIST_WIDTH);
		
		char *line = strtok(buf, "\n");
		while (line) {
			printf("%*s%s\n", 3*PSC_HELPLIST_OFFSET, " ", line);
			line = strtok(NULL, "\n");
		}
		printf("\n");
	}

	puts(
		"Author:\n"
		"    Ruslan Kuchumov, 2018\n"
		"\n"
		"For more info visit\n"
		"    <https://gitlab.com/mildlyparallel/pscircle>\n"
	);

	exit(EXIT_SUCCESS);
}

void
print_unknown_and_exit(const char *arg)
{
	fprintf(stderr, "Unknown argument: %s\n\n", arg);
	exit(EXIT_FAILURE);
}

void
print_parse_error_and_exit(const char *arg)
{
	fprintf(stderr, "Can not parse value from this argument: %s\n\n", arg);
	exit(EXIT_FAILURE);
}

bool
parser_bool(const char *value, void *output)
{
	assert(value);
	assert(output);

	bool *out = (bool *) output;

	if (strcmp(value, "1") == 0) {
		*out = true;
		return true;
	}

	if (strcmp(value, "0") == 0) {
		*out = false;
		return true;
	}

	if (strcmp(value, "true") == 0 || strcmp(value, "TRUE") == 0) {
		*out = true;
		return true;
	}

	if (strcmp(value, "false") == 0 || strcmp(value, "FALSE") == 0) {
		*out = false;
		return true;
	}

	return false;
}

bool
parser_real(const char *value, void *output)
{
	assert(value);
	assert(output);

	real_t *out = (real_t *) output;

	char *e = NULL;

#ifdef PSC_USE_FLOAT
	*out = strtof(value, &e);
#else
	*out = strtod(value, &e);
#endif

	return *e == '\0';
}

bool
parser_color(const char *value, void *output)
{
	assert(value);
	assert(output);

	color_t *out = (color_t *) output;
	*out = color_from_hex(value);
	return true;
}

bool
parser_point(const char *value, void *output)
{
	assert(value);
	assert(output);

	point_t *out = (point_t *) output;

	if (*value == ':')
		return false;

	char *e = NULL;

#ifdef PSC_USE_FLOAT
	out->x = strtof(value, &e);
#else
	out->x = strtod(value, &e);
#endif

	if (*e != ':')
		return false;

	value = e + 1;

	if (*value == '\0')
		return false;

#ifdef PSC_USE_FLOAT
	out->y = strtof(value, &e);
#else
	out->y = strtod(value, &e);
#endif

	return *e == '\0';
}

bool
parser_string(const char *value, void *output)
{
	assert(value);
	assert(output);

	char **out = (char **) output;

	char *tmp = calloc(strlen(value) + 1, sizeof(char));
	if (!tmp) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	strcpy(tmp, value);

	*out = tmp;

	return true;
}

bool
parser_long(const char *value, void *output)
{
	assert(value);
	assert(output);

	long *out = (long *) output;

	char *e = NULL;

	*out = strtol(value, &e, 10);

	return *e == '\0';
}

bool
parser_ulong(const char *value, void *output)
{
	assert(value);
	assert(output);

	unsigned long *out = (unsigned long *) output;

	char *e = NULL;

	*out = strtoul(value, &e, 10);

	return *e == '\0';
}

bool
parser_memory(const char *value, void *output)
{
	assert(value);
	assert(output);

	unsigned long *out = (unsigned long *) output;

	char *e = NULL;

	*out = strtoul(value, &e, 10);

	if (*e == '\0')
		return true;

	const static char units[] = {
		'B', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'
	};

	for (size_t i = 0; i < sizeof(units)/sizeof(*units); ++i) {
		if (units[i] == *e)
			return true;

		*out *= 1024;
	}

	return false;
}

bool
parser_memory_unit(const char *value, void *output)
{
	assert(value);
	assert(output);

	memunit_t *out = (memunit_t *) output;

	const static char units[] = {
		'B', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'
	};

	for (size_t i = 0; i < sizeof(units)/sizeof(*units); ++i) {
		if (units[i] != value[0])
			continue;

		*out = i;
		return true;
	}

	return false;
}

arg_t *
find_by_key(argparser_t *argparser, const char *key)
{
	for (size_t i = 0; i < argparser->nargs; ++i) {
		arg_t *a = argparser->args + i;
		if (strcmp(a->name, key) == 0)
			return a;
	}
	return NULL;
}

