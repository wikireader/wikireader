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
void debug_printf(const char* fmt, ...);

// DP = Debug Print
#define DP(on, varformat) (on) ? debug_printf varformat : (void) 0

// DX = Debug Fail
#define DX() DP(1, ("X %s line %d (%s())\n", __BASE_FILE__, __LINE__, __FUNCTION__))

#define min(a, b)	((a) < (b) ? (a) : (b))
#define max(a, b)	((a) > (b) ? (a) : (b))

#endif /* WL_MSG_H */
