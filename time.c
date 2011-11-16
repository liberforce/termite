#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "types.h"

gint64 g_get_time (void)
{
	struct timeval ts;
	gettimeofday (&ts, NULL);
	return (((gint64) ts.tv_sec) * 1000000) + ts.tv_usec;
}

