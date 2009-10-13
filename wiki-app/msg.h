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

#ifndef WL_MSG_H
#define WL_MSG_H

#include <stdio.h>
#include <stdarg.h>
#include "misc.h"

enum {
	MSG_ERROR = 0,
	MSG_WARNING,
	MSG_INFO,
	MSG_DEBUG,
	MSG_LEVEL_MAX
};

void msg(int level, const char *format, ...);
void set_loglevel(int level);
void hexdump(const char *p, unsigned int len);
static inline void debug_printf(const char* fmt, ...)
{
	va_list arg_list;
	va_start(arg_list, fmt);
	msg(MSG_INFO, fmt);
	va_end(arg_list);
}

// a simplistic ASSERT based on MSG_ERROR
#define ASSERT(cond) ASSERT_((cond), __FILE__, __LINE__)
#define ASSERT_(cond, file, line) ASSERT__(cond, file, line)
#define ASSERT__(cond, file, line)                                      \
	do {                                                            \
		if (!(cond)) {                                          \
			msg(MSG_ERROR, "assert failure in: "            \
			    #file " line:" #line                        \
			    " for: " #cond "\n");                       \
			for (;;) {                                      \
			}                                               \
		}                                                       \
	} while (0)

// DP = Debug Print
#define DP(on, varformat) (on) ? debug_printf varformat : (void) 0

// DX = Debug Fail
#define DX() DP(1, ("X %s line %d (%s())\n", __BASE_FILE__, __LINE__, __FUNCTION__))

#endif /* WL_MSG_H */
