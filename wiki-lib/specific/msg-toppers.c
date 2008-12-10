#include "msg.h"
#include <t_services.h>
#include <stdarg.h>

static int loglevel = MSG_LEVEL_MAX;

void msg(int level, const char *format, ...)
{
	va_list ap;
	int prio = LOG_INFO;

	if (level > loglevel)
		return;

	switch (level) {
	case MSG_ERROR:
		prio = LOG_ERROR;
		break;
	case MSG_WARNING:
		prio = LOG_WARNING;
		break;
	case MSG_INFO:
		prio = LOG_INFO;
		break;
	case MSG_DEBUG:
		prio = LOG_DEBUG;
		break;
	}

	va_start(ap, format);
	syslog_va(prio, format, ap);
	va_end(ap);
}

void set_loglevel(int level)
{
	loglevel = level;
}

