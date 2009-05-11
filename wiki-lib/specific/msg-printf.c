#include <stdio.h>
#include <stdarg.h>
#include "msg.h"

static int loglevel = MSG_LEVEL_MAX;

void msg(int level, const char *format, ...)
{
	va_list ap;
	FILE *stream = stdout;
	int clear_color = 0;

	if (level > loglevel)
		return;

	if (level == MSG_DEBUG) {
		fprintf(stream, "\033[33m");	/* yellow */
		clear_color = 1;
	}

	if (level == MSG_ERROR) {
		stream = stderr;
		fprintf(stream, "\033[31m");	/* red */
		clear_color = 1;
	}

	va_start(ap, format);
	vfprintf(stream, format, ap);
	va_end(ap);

	if (clear_color)
		fprintf(stream, "\033[0;m");
}

void set_loglevel(int level)
{
	loglevel = level;
}
