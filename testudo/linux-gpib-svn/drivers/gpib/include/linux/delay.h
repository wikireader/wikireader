
#ifndef __COMPAT_LINUX_DELAY_H
#define __COMPAT_LINUX_DELAY_H

#include <linux/version.h>
#include <linux/time.h>

#include_next <linux/delay.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9)
#include <linux/sched.h>
#include <linux/time.h>
static inline unsigned long msleep_interruptible(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs);

	while (timeout && !signal_pending(current)) {
		set_current_state(TASK_INTERRUPTIBLE);
		timeout = schedule_timeout(timeout);
	}
	return jiffies_to_msecs(timeout);
}
#endif

#endif

