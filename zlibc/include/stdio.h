/*
 * Copyright (c) 2001 Dmitry Dicky diwil@eis.ru
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: stdio.h,v 1.9 2004/01/28 22:25:29 cliechti Exp $
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>
#include <sys/types.h>

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#ifndef EOF
# define EOF (-1)
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

int __attribute__((format (printf, 2, 3))) uprintf(int (*func)(int c), const char *fmt, ...);
int __attribute__((format (printf, 3, 4))) snprintf (char *buf, size_t size, const char *fmt, ...);
int __attribute__((format (printf, 2, 3))) sprintf (char *buf, const char *fmt, ...);
int __attribute__((format (printf, 1, 2))) printf(const char *string, ...);
int vuprintf(int (*func)(int c), const char *fmt0, va_list ap);
int vsnprintf(char *dest, size_t maxlen, const char *string, va_list ap);
int vsprintf(char *dest, const char *string, va_list ap);
int vprintf(const char *string, va_list ap);

int puts(const char *s);
int putchar(int c);      //has to be supplied by the user, but avoids useless warnings

#ifdef __cplusplus
}
#endif //__cplusplus

#endif
