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
 * $Id: string.h,v 1.4 2006/11/15 14:34:57 coppice Exp $
 */

#if !defined(__STRING_H_)
#define	__STRING_H_ 

#include <stddef.h>
#include <sys/types.h>

extern void *memccpy(void *, const void *, int, size_t);
extern void *memchr(const void *, int, size_t);
extern int memcmp(const void *, const void *, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
extern char *strcat(char *, const char *);
extern char *strchr(const char *, int);
extern int strcmp(const char *, const char *);
extern char *strcpy(char *, const char *);
extern int strcasecmp(const char *, const char *);
extern size_t strlen(const char *);
extern char *strlwr(char *);
extern char *strncat(char *, const char *, size_t);
extern int strncmp(const char *, const char *, size_t);
extern char *strncpy(char *, const char *, size_t) ;
extern int strncasecmp(const char *, const char *, size_t);
extern size_t strnlen(const char *, size_t);
extern char *strrchr(const char *, int);
extern char *strrev(char *);
extern char *strstr(const char *, const char *);
extern char *strupr(char *);
extern int  bcmp(const void *b1, const void *b2, size_t length);
extern char *strsep(char **, const char *);
extern void bcopy(const void *, void *, size_t);
extern size_t strlcat(char *, const char *, size_t);
extern void * memset(void *, int, size_t);
extern void bzero(void *, size_t);
extern size_t strlcpy(char *, const char *, size_t);
extern char * strdup(const char *);
extern void swab(const void *, void *, size_t);
extern char * strtok_r(char *s, const char *delim, char **last);
extern char * strtok(char *s, const char *delim);

#endif /* _STRING_H_ */
