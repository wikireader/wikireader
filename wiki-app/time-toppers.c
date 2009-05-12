#include <t_services.h>
#include <kernel.h>
#include <wl-time.h>

unsigned int get_timer(void)
{
	SYSTIM time;
	get_tim(&time);
	return (unsigned int) time;
}

