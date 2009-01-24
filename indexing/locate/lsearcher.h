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
#include "locate.h"

#define MAXSTR 1024
#define TOLOWER(c) ('A' <= (c) && (c) <= 'Z' ? (c) | 0x20 : (c))


typedef unsigned char *ucaddr_t;
typedef unsigned char uchar_t;
typedef bool resultf(uchar_t *);
typedef void donef();

#define MAX_UPPER_PREFIX_SIZE   38
#define MAX_ALL_PREFIX_SIZE     (38+26)

typedef struct {
  uchar_t bigram1[NBG], bigram2[NBG];
  int db_file;
  off_t db_start;
  uint32_t prefixdb[MAX_UPPER_PREFIX_SIZE];
  uint32_t bigram[MAX_UPPER_PREFIX_SIZE * MAX_ALL_PREFIX_SIZE];
} lindex;

struct search_state {
    uchar_t path[MAXSTR];
    int offset;
    int count;
    bool skip;
    int pattern_len;
};

void kill_search();

#endif
