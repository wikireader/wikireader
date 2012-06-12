/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
