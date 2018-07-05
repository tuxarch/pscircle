#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <config.h>

#define QUOTE(name) #name

#define ARGQ(argparser, name, output, parser, defaults, description) do { \
	arg_t arg = {name, description, QUOTE(defaults), (void *) &(output), parser}; \
	argparser_add(argparser, arg); \
} while (0);

#define ARG(argparser, name, output, parser, defaults, description) do { \
	arg_t arg = {name, description, defaults, (void *) &(output), parser}; \
	argparser_add(argparser, arg); \
} while (0);

typedef struct {
	const char *name;
	const char *description;
	const char *defaults;
	void *output;
	bool (*parser)(const char *value, void *output);
} arg_t;

typedef struct {
	arg_t args[PSC_MAX_ARGS];
	size_t nargs;
} argparser_t;

void
argparser_init(argparser_t *argparser);

void
argparser_add(argparser_t *argparser, arg_t opt);

void
argparser_parse(argparser_t *argparser, int argc, char const * argv[]);

bool
parser_bool(const char *value, void *output);

bool
parser_real(const char *value, void *output);

bool
parser_color(const char *value, void *output);

bool
parser_point(const char *value, void *output);

bool
parser_string(const char *value, void *output);

bool
parser_long(const char *value, void *output);

bool
parser_ulong(const char *value, void *output);

bool
parser_memory(const char *value, void *output);

bool
parser_memory_unit(const char *value, void *output);
