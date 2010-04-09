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

#ifndef SEARCH_HASH_H
#define SEARCH_HASH_H

#include <inttypes.h>

#include "bigram.h"

#define MAX_SEARCH_HASH_TABLE_ENTRIES (4 * 256 * 1024)
#define MAX_SEARCH_HASH_KEY (256 * 1024)
#define MAX_SEARCH_STRING_HASHED_LEN 15
#define MAX_SEARCH_STRING_ALL_HASHED_LEN 5
#define SEARCH_HASH_SEQUENTIAL_SEARCH_THRESHOLD 64

typedef struct __attribute__ ((packed)) _search_hash_table {
	uint32_t offset_fnd; // offset to pedia.fnd for the search title hashed
	uint32_t next_entry_idx; // byte 1: length of hash string byte 2~4: index (of struct hash_table array) of the next entry with the same hash key
} SEARCH_HASH_TABLE;

typedef struct __attribute__ ((packed)) _search_hash_string {
	char str[MAX_SEARCH_STRING_HASHED_LEN + 1];
} SEARCH_HASH_STRING;

#ifdef WIKIPCF
void create_search_hash(const char *filename);
long add_search_hash(char *sSearchString, int len, long offset_fnd);
void save_search_hash(void);
#else
void init_search_hash(void);
int copy_fnd_to_buf(long offset, char *buf, int len);
int copy_str_to_buf(long offset, char *buf, int len);
long get_search_hash_offset_fnd(char *sSearchString, int len);
void retrieve_titles_from_fnd(long offset_fnd, unsigned char *sTitleSearch, unsigned char *sTitleActual);
#endif

#endif
