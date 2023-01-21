#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

#include "cfg.h"
#include "procs.h"
#include "timing.h"
#include "painter.h"
#include "tree_visualizer.h"
#include "toplist_visualizer.h"

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

void gen_image() {
	timing_t tm = {0};
	tm_start(&tm);
	procs_t *procs = calloc(1, sizeof(procs_t));
	CHECK(procs);

	FILE *fp = NULL;
	if (config.read_stdin)
		fp = stdin;

	procs_init(procs, fp);
	tm_tick(&tm, "init");
	node_reorder_by_leaves((node_t *)procs->root);
	node_arrange((node_t *)procs->root);
	tm_tick(&tm, "arrange");
	painter_t *painter = calloc(1, sizeof(painter_t));
	CHECK(painter);
	painter_init(painter);
	draw_tree(painter, procs);
	tm_tick(&tm, "draw tree");
	if (config.toplists.cpulist.show || config.toplists.memlist.show) {
		draw_toplists(painter, procs);
		tm_tick(&tm, "draw lists");
	}
	painter_write(painter);
	tm_tick(&tm, "write");
	painter_dinit(painter);
	tm_total(&tm);
}


void daemonize() {
	int pid = fork();
	if (pid == 0) {
		do {
			gen_image();
			sleep(config.loop_delay);
		} while (1);
	} else {
		exit(0);
	}
}

int main(int argc, const char *argv[])
{
	parse_cmdline(argc, argv);
	if (config.daemonize) {
		daemonize();
	} else {
		gen_image();
	}
	return 0;
}
