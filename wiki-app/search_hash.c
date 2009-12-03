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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#ifdef WIKIPCF
#include <malloc.h>
#else
#include <malloc-simple.h>
#include <file-io.h>
#include "msg.h"
#include "search.h"
#endif
#include "search_hash.h"
#include "lcd_buf_draw.h"

// pedia.hsh format:
//	The first 4 bytes contain the hash entry count
//	Each hash entry is defined as struct SEARCH_HASH_TABLE

SEARCH_HASH_TABLE *search_hash_table;
SEARCH_HASH_STRING *search_hash_strings;
uint32_t nHashEntries = 0;
#ifdef WIKIPCF
FILE *fdHsh;
long nNeedMoreEntries = 0;
#else
int fdHsh = 999;
int fdFnd = 999;
int *bHashBlockLoaded;
#define FND_BUF_COUNT 2048
#define ENTRIES_PER_HASH_BLOCK 256
// FND_BUF_BLOCK_SIZE needs to be larger than MAX_RESULTS * sizeof(TITLE_SEARCH)
#define FND_BUF_BLOCK_SIZE 2048
struct _fnd_buf {
	uint32_t offset;
	uint32_t len;
	uint32_t used_seq;
	char buf[FND_BUF_BLOCK_SIZE];
} *fnd_bufs;

long nUsedSeq = 1;
long lenFnd = 0;
#endif

static unsigned long hash_key(char *s, int len)
{
	unsigned long hash = 5381;
	char str[MAX_SEARCH_STRING_HASHED_LEN + 1];
	int i;

	if (len > MAX_SEARCH_STRING_HASHED_LEN)
		len = MAX_SEARCH_STRING_HASHED_LEN;
	memcpy(str, s, len);
	str[len] = '\0';
	i = 0;
	while(i < len) {
		int c = str[i];
		if ('A' <= c && c <='Z')
			c += 32;
		hash = ((hash << 5) + hash) + c;
		i++;
	}
	return hash % MAX_SEARCH_HASH_KEY;
}

#ifdef WIKIPCF
void create_search_hash(const char *filename)
{
	search_hash_table = malloc(sizeof(SEARCH_HASH_TABLE) * MAX_SEARCH_HASH_TABLE_ENTRIES);
	search_hash_strings = malloc(sizeof(SEARCH_HASH_STRING) * MAX_SEARCH_HASH_TABLE_ENTRIES);
	if (!search_hash_table || !search_hash_strings)
	{
		printf("malloc search_hash_table/search_hash_strings error\n");
		exit(-1);
	}
	nHashEntries = MAX_SEARCH_HASH_KEY;
	memset(search_hash_table, 0, sizeof(SEARCH_HASH_TABLE) * MAX_SEARCH_HASH_TABLE_ENTRIES);
	fdHsh = fopen(filename, "wb");
	if (!fdHsh)
	{
		printf("cannot open file '%s', error: %s\n", filename, strerror(errno));
		exit(-1);
	}
}

long add_search_hash(char *sInput, int len, long offset_fnd)
{
	long nHashKey;
	char sSearchString[MAX_SEARCH_STRING_HASHED_LEN + 1];

	memcpy(sSearchString, sInput, len);
	sSearchString[len] = '\0';
	nHashKey = hash_key(sSearchString, len);
	if (search_hash_table[nHashKey].offset_fnd)
	{
		if (strcmp(search_hash_strings[nHashKey].str, sSearchString))
		{
			int bFound = 0;
			while (!bFound && (search_hash_table[nHashKey].next_entry_idx & 0x0FFFFFFF))
			{
				nHashKey = search_hash_table[nHashKey].next_entry_idx & 0x0FFFFFFF;
				if (!strcmp(search_hash_strings[nHashKey].str, sSearchString))
					bFound = 1;
			}
			if (!bFound)
			{
				if (nHashEntries >= MAX_SEARCH_HASH_TABLE_ENTRIES)
				{
					nNeedMoreEntries++;
				}
				else
				{
					search_hash_table[nHashKey].next_entry_idx |= nHashEntries;
					search_hash_table[nHashEntries].next_entry_idx = len << 28;
					search_hash_table[nHashEntries].offset_fnd = offset_fnd;
					strncpy(search_hash_strings[nHashEntries].str, sSearchString, MAX_SEARCH_STRING_HASHED_LEN);
					search_hash_strings[nHashEntries].str[MAX_SEARCH_STRING_HASHED_LEN] = '\0';
					nHashEntries++;
				}
			}
		}
	}
	else
	{
		search_hash_table[nHashKey].next_entry_idx = (len << 28);
		search_hash_table[nHashKey].offset_fnd = offset_fnd;
		strncpy(search_hash_strings[nHashKey].str, sSearchString, MAX_SEARCH_STRING_HASHED_LEN);
		search_hash_strings[nHashKey].str[MAX_SEARCH_STRING_HASHED_LEN] = '\0';
	}
	return nHashKey;
}

void save_search_hash(void)
{
	fwrite(&nHashEntries, sizeof(nHashEntries), 1, fdHsh);
	fwrite(search_hash_table, sizeof(SEARCH_HASH_TABLE), nHashEntries, fdHsh);
	free(search_hash_table);
	fclose(fdHsh);
	if (nNeedMoreEntries)
		printf("Search hash table need %ld more entries!\n", nNeedMoreEntries);
}
#else
void init_search_hash(void)
{
	int i;

	fdHsh = wl_open("pedia.hsh", WL_O_RDONLY);
	wl_read(fdHsh, &nHashEntries, sizeof(nHashEntries));
	search_hash_table = (SEARCH_HASH_TABLE *)malloc_simple(sizeof(SEARCH_HASH_TABLE) * nHashEntries, MEM_TAG_INDEX_M1);
	bHashBlockLoaded = (int *)malloc_simple(sizeof(int) * (nHashEntries / ENTRIES_PER_HASH_BLOCK), MEM_TAG_INDEX_M1);
	memset((char *)bHashBlockLoaded, 0, sizeof(int) * (nHashEntries / ENTRIES_PER_HASH_BLOCK));
	fdFnd = wl_open("pedia.fnd", WL_O_RDONLY);
	init_bigram(fdFnd);
	fnd_bufs = (struct _fnd_buf *)malloc_simple(sizeof(struct _fnd_buf) * FND_BUF_COUNT, MEM_TAG_INDEX_M1);
	for (i = 0; i < FND_BUF_COUNT; i++)
		fnd_bufs[i].offset = 0;
}

int nHashJumps;
long get_search_hash_offset_fnd(char *sSearchString, int len)
{
	long nHashKey;
	TITLE_SEARCH title_search;
	char sDecoded[MAX_TITLE_SEARCH];
	int bFound = 0;
	int lenHashed;
	int idxBlock;

	nHashJumps = 0;
	nHashKey = hash_key(sSearchString, len);
	idxBlock = nHashKey / ENTRIES_PER_HASH_BLOCK;
	if (!bHashBlockLoaded[idxBlock])
	{
		wl_seek(fdHsh, idxBlock * ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE) + sizeof(nHashEntries));
		wl_read(fdHsh, &search_hash_table[idxBlock * ENTRIES_PER_HASH_BLOCK],
			ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE));
		bHashBlockLoaded[idxBlock]++;
	}

	while (!bFound && nHashKey >= 0 && search_hash_table[nHashKey].offset_fnd)
	{
		if (search_hash_table[nHashKey].offset_fnd > 0)
		{
			copy_fnd_to_buf(search_hash_table[nHashKey].offset_fnd, (char *)&title_search, sizeof(title_search));
			bigram_decode(sDecoded, title_search.sTitleSearch, MAX_TITLE_SEARCH);
		}
		else
			sDecoded[0] = '\0';
		lenHashed = (search_hash_table[nHashKey].next_entry_idx >> 28) & 0x000000FF;
		sDecoded[lenHashed] = '\0';
		if (!search_string_cmp(sDecoded, sSearchString, len))
			bFound = 1;
		if (!bFound)
		{
			if (search_hash_table[nHashKey].next_entry_idx  & 0x0FFFFFFF)
			{
				nHashJumps++;
				nHashKey = search_hash_table[nHashKey].next_entry_idx & 0x0FFFFFFF;
				idxBlock = nHashKey / ENTRIES_PER_HASH_BLOCK;
				if (!bHashBlockLoaded[idxBlock])
				{
					wl_seek(fdHsh, idxBlock * ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE) + sizeof(nHashEntries));
					wl_read(fdHsh, &search_hash_table[idxBlock * ENTRIES_PER_HASH_BLOCK],
						ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE));
					bHashBlockLoaded[idxBlock]++;
				}
			}
			else
				nHashKey = -1;
		}
	}
	if (bFound)
	{
		return search_hash_table[nHashKey].offset_fnd;
	}
	else
	{
		return 0;
	}
}

int copy_fnd_to_buf(long offset, char *buf, int len)
{
	int i = 0;
	int bFound = 0;
	int nLeastUsedSeq = 0;
	int iLeastUsed = 0;
	int nCopyLen;
	long blocked_offset;

	if (lenFnd > 0 && offset >= lenFnd)
		return 0;

	while (!bFound && i < FND_BUF_COUNT)
	{
		if (fnd_bufs[i].offset)
		{
			if (fnd_bufs[i].offset <= offset && offset - fnd_bufs[i].offset < FND_BUF_BLOCK_SIZE)
			{
				bFound = 1;
			}
			else
			{
				if (nLeastUsedSeq == 0 || fnd_bufs[i].used_seq < nLeastUsedSeq)
				{
					nLeastUsedSeq = fnd_bufs[i].used_seq;
					iLeastUsed = i;
				}
			}
		}
		else // the block of the offset to be read into the null entry
		{
			blocked_offset = ((offset - SIZE_BIGRAM_BUF) / FND_BUF_BLOCK_SIZE) * FND_BUF_BLOCK_SIZE + SIZE_BIGRAM_BUF;
			wl_seek(fdFnd, blocked_offset);
			fnd_bufs[i].len = wl_read(fdFnd, fnd_bufs[i].buf, FND_BUF_BLOCK_SIZE);
			if (fnd_bufs[i].len < FND_BUF_BLOCK_SIZE)
			{
				lenFnd = wl_tell(fdFnd);
				if (fnd_bufs[i].len <= 0)
				{
					fnd_bufs[i].offset = 0;
					return 0;
				}
			}
			fnd_bufs[i].offset = blocked_offset;
			bFound = 1;
		}
		if (!bFound)
			i++;
	}

	if (!bFound)
	{
		i = iLeastUsed;
		blocked_offset = ((offset - SIZE_BIGRAM_BUF) / FND_BUF_BLOCK_SIZE) * FND_BUF_BLOCK_SIZE + SIZE_BIGRAM_BUF;
		wl_seek(fdFnd, blocked_offset);
		fnd_bufs[i].len = wl_read(fdFnd, fnd_bufs[i].buf, FND_BUF_BLOCK_SIZE);
		if (fnd_bufs[i].len < FND_BUF_BLOCK_SIZE)
		{
			lenFnd = wl_tell(fdFnd);
			if (fnd_bufs[i].len <= 0)
			{
				fnd_bufs[i].offset = 0;
				return 0;
			}
		}
		fnd_bufs[i].offset = blocked_offset;
	}
	fnd_bufs[i].used_seq = nUsedSeq++;

	if (len > fnd_bufs[i].len - (offset - fnd_bufs[i].offset)) // the buf to be copied is separated into two blocks or end of file
		nCopyLen = fnd_bufs[i].len - (offset - fnd_bufs[i].offset);
	else
		nCopyLen = len;

	if (nCopyLen < 0)
		nCopyLen = 0;
	else
		memcpy(buf, &fnd_bufs[i].buf[offset - fnd_bufs[i].offset], nCopyLen);

	if (nCopyLen < len)
		nCopyLen += copy_fnd_to_buf(fnd_bufs[i].offset + fnd_bufs[i].len, &buf[nCopyLen], len - nCopyLen);
	return nCopyLen;
}

#endif
