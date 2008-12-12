#include <sys/time.h>
#include <time.h>
#include <wl-time.h>

unsigned int get_timer(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

