#ifndef __LSEARCHER_H__
#define __LSEARCHER_H__

#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>


#include "debug.h"

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

typedef unsigned char *ucaddr_t;
typedef unsigned char uchar_t;
typedef bool resultf(uchar_t *);
typedef void donef();

typedef struct {
  uchar_t bigram1[NBG], bigram2[NBG];
  FILE *db_file;
  fpos_t db_start;
  uint32_t prefixdb[CHAR_MAX];
} lindex;

void kill_search();
int search(lindex *, char *, resultf, donef, bool, bool);

#endif
