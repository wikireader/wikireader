#ifndef __LSEARCHER_H__
#define __LSEARCHER_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


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
  unsigned int db_start;
  uint32_t prefixdb[MAX_UPPER_PREFIX_SIZE];
  uint32_t bigram[MAX_UPPER_PREFIX_SIZE * MAX_UPPER_PREFIX_SIZE];
} lindex;

struct search_state {
    uchar_t path[MAXSTR];
    int offset;
    int count;
    bool skip;
    int pattern_len;
    int last_c;
};

static inline int char_to_index(char c) {
    if (c == 32)
        return 0;
    else if (c == 46)
        return 1;
    else if (c >= 48 && c <= 57)
        return c - 48 + 2;
    else if (c >= 65 && c <= 90)
        return c - 65 + 2 + 10;
    else if (c >= 97 && c <= 122)
        return c - 97 + 2 + 10 + 26;
    
    return -1;
}

static inline int create_index(int lindex, int rindex) {
    return (MAX_UPPER_PREFIX_SIZE * lindex) + rindex;
}

int search_slow(lindex *l, char *pathpart, struct search_state *state, resultf f, donef df);
void prepare_search(lindex *, char *pathpart, struct search_state *state);
char *search_fast(lindex *l, char *pathpart, struct search_state *state);
int load_index(lindex *l, char *path, char *prefix_path);

#endif
