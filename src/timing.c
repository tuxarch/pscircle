#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "timing.h"
#include "config.h"

#define TIME_FMT "%13s: %.4lf / %.4lf seconds\n"

double
tm_cpudiff(clock_t a, clock_t b)
{
	double dt = (double) (a - b) / CLOCKS_PER_SEC;
	if (dt < 0)
		dt = 0;
	return dt;
}

double
tm_walldiff(struct timespec *a, struct timespec *b)
{
	double tn = (double) (a->tv_nsec - b->tv_nsec) * 1e-9;
	double ts = difftime(a->tv_sec, b->tv_sec);
	double dt = ts + tn;
	if (dt < 0)
		return 0;
	return dt;
}

void
tm_start(timing_t *tm)
{
#if PSC_PRINT_TIME
	tm->cputime = clock();
	clock_gettime(CLOCK_REALTIME, &tm->walltime);
	tm->ctotal = 0;
	tm->wtotal = 0;

	fprintf(stderr, "%13s  %6s / %6s \n", " ", "cpu", "wall");
#endif
}

void
tm_tick(timing_t *tm, const char *message)
{
#if PSC_PRINT_TIME
	assert(tm);
	assert(message);

	clock_t ct2 = clock();
	struct timespec wt2 = {0};
	clock_gettime(CLOCK_REALTIME, &wt2);

	double cdt = tm_cpudiff(ct2, tm->cputime);
	double wdt = tm_walldiff(&wt2, &tm->walltime);

	fprintf(stderr, TIME_FMT, message, cdt, wdt);

	tm->ctotal += cdt;
	tm->wtotal += wdt;

	tm->cputime = clock();
	clock_gettime(CLOCK_REALTIME, &tm->walltime);
#endif
}

void
tm_total(timing_t *tm)
{
#if PSC_PRINT_TIME
	fprintf(stderr, TIME_FMT, "total", tm->ctotal, tm->wtotal);
#endif
}
