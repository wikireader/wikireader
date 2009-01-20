#ifndef __LSEARCHER_H__
#define __LSEARCHER_H__

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "debug.h"
#include "safe.h"

#define	NBG 128
#define MAXSTR 1024

#define	OFFSET 14
#define	PARITY 128
#define	SWITCH 30
#define UMLAUT 31

#define LDC_MIN 0
#define LDC_MAX 28

#define BIGRAM_MIN (UCHAR_MAX - CHAR_MAX) 
#define BIGRAM_MAX UCHAR_MAX

#define ASCII_MIN 32
#define ASCII_MAX CHAR_MAX

#define TO7BIT(x) ((x) = (x) & 127)

#define TOLOWER(c) ('A' <= (c) && (c) <= 'Z' ? (c) | 0x20 : (c))

typedef bool resultf(uchar_t *);
typedef void donef();
typedef unsigned char *ucaddr_t;

typedef struct {
  uchar_t bigram1[NBG], bigram2[NBG];
  uchar_t *addr;
  uint32_t *prefixdb;
  size_t size;
} lindex;

void kill_search();
int search(lindex *, uchar_t *, resultf, donef, bool, bool);

#endif
