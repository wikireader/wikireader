#ifndef __SAFE_H__
#define __SAFE_H__

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "debug.h"

typedef unsigned char  uchar_t;

void *xalloc(size_t size);
FILE *xfopen(const char *path, const char *mode);
int32_t xgetc(FILE *fp);
int32_t xputc(uchar_t c, FILE *fp);
void xfflush(FILE *fp);
void xfclose(FILE *fp);
void *xmmap(void *addr, size_t len, int prot, int flags, int fildes, off_t offset);
void *xmmapf(char *path, size_t *size);

#endif
