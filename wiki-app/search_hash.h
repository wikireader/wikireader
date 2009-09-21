#ifndef SEARCH_HASH_H
#define SEARCH_HASH_H
#include "bigram.h"

#define MAX_SEARCH_HASH_TABLE_ENTRIES (4 * 256 * 1024)
#define MAX_SEARCH_HASH_KEY (256 * 1024)
#define MAX_SEARCH_STRING_HASHED_LEN 15
#define MAX_SEARCH_STRING_ALL_HASHED_LEN 5
#define SEARCH_HASH_SEQUENTIAL_SEARCH_THRESHOLD 64

typedef struct _search_hash_table {
	long offset_fnd; // offset to pedia.fnd for the search title hashed
	long next_entry_idx; // byte 1: length of hash string byte 2~4: index (of struct hash_table array) of the next entry with the same hash key
} SEARCH_HASH_TABLE;

typedef struct _search_hash_string {
	char str[MAX_SEARCH_STRING_HASHED_LEN + 1];
} SEARCH_HASH_STRING;

#ifdef WIKIPCF
void init_search_hash(void);
long add_search_hash(char *sSearchString, int len, long offset_fnd);
void save_search_hash(void);
#else
void init_search_hash(void);
int copy_fnd_to_buf(long offset, char *buf, int len);
int copy_str_to_buf(long offset, char *buf, int len);
long get_search_hash_offset_fnd(char *sSearchString, int len);
#endif

#endif
