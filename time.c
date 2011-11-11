#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "types.h"

gint64 g_get_time (void)
{
	struct timespec ts;
	clock_gettime (CLOCK_MONOTONIC, &ts);
	assert (-315569520000000000L < ts.tv_sec && ts.tv_sec < 315569520000000000L);
	return (((gint64) ts.tv_sec) * 1000000) + (ts.tv_nsec / 1000);
}

