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
#include <input.h>
#include "msg.h"
#include "search.h"
#include "wiki_info.h"
#endif
#include "search_hash.h"
#include "lcd_buf_draw.h"

// pedia.hsh format:
//	The first 4 bytes contain the hash entry count
//	Each hash entry is defined as struct SEARCH_HASH_TABLE

#ifdef WIKIPCF
//SEARCH_HASH_TABLE *search_hash_table;
//SEARCH_HASH_STRING *search_hash_strings;

//uint32_t nHashEntries = 0;

//FILE *fdHsh;
long nNeedMoreEntries = 0;
#else
extern int search_interrupted;
int bHashInited[MAX_WIKIS_PER_DEVICE];
//SEARCH_HASH_TABLE *search_hash_table[MAX_WIKIS_PER_DEVICE];
//SEARCH_HASH_STRING *search_hash_strings[MAX_WIKIS_PER_DEVICE];
//uint32_t nHashEntries[MAX_WIKIS_PER_DEVICE];
//int fdHsh[MAX_WIKIS_PER_DEVICE];
int fdFnd[MAX_WIKIS_PER_DEVICE];
int *bHashBlockLoaded[MAX_WIKIS_PER_DEVICE];
#define FND_BUF_COUNT 1024
#define ENTRIES_PER_HASH_BLOCK 256
// FND_BUF_BLOCK_SIZE needs to be larger than MAX_RESULTS * sizeof(TITLE_SEARCH)
#define FND_BUF_BLOCK_SIZE 2048
struct __attribute__ ((packed)) _fnd_buf {
	uint32_t offset;
	uint32_t len;
	uint32_t used_seq;
	char buf[FND_BUF_BLOCK_SIZE];
} *fnd_bufs[MAX_WIKIS_PER_DEVICE];

long nUsedSeq = 1;
long lenFnd[MAX_WIKIS_PER_DEVICE];
#endif

//static unsigned long hash_key(char *s, int len)
//{
//	unsigned long hash = 5381;
//	char str[MAX_SEARCH_STRING_HASHED_LEN + 1];
//	int i;
//
//	if (len > MAX_SEARCH_STRING_HASHED_LEN)
//		len = MAX_SEARCH_STRING_HASHED_LEN;
//	memcpy(str, s, len);
//	str[len] = '\0';
//	i = 0;
//	while(i < len) {
//		int c = str[i];
//		if ('A' <= c && c <='Z')
//			c += 32;
//		hash = ((hash << 5) + hash) + c;
//		i++;
//	}
//	return hash % MAX_SEARCH_HASH_KEY;
//}
//
#ifdef WIKIPCF
//void create_search_hash(const char *filename)
//{
//	search_hash_table = malloc(sizeof(SEARCH_HASH_TABLE) * MAX_SEARCH_HASH_TABLE_ENTRIES);
//	search_hash_strings = malloc(sizeof(SEARCH_HASH_STRING) * MAX_SEARCH_HASH_TABLE_ENTRIES);
//	if (!search_hash_table || !search_hash_strings)
//	{
//		printf("malloc search_hash_table/search_hash_strings error\n");
//		exit(-1);
//	}
//	nHashEntries = MAX_SEARCH_HASH_KEY;
//	memset(search_hash_table, 0, sizeof(SEARCH_HASH_TABLE) * MAX_SEARCH_HASH_TABLE_ENTRIES);
//	fdHsh = fopen(filename, "wb");
//	if (!fdHsh)
//	{
//		printf("cannot open file '%s', error: %s\n", filename, strerror(errno));
//		exit(-1);
//	}
//}
//
//long add_search_hash(char *sInput, int len, long offset_fnd)
//{
//	long nHashKey;
//	char sSearchString[MAX_SEARCH_STRING_HASHED_LEN + 1];
//
//	memcpy(sSearchString, sInput, len);
//	sSearchString[len] = '\0';
//	nHashKey = hash_key(sSearchString, len);
//	if (search_hash_table[nHashKey].offset_fnd)
//	{
//		if (strcmp(search_hash_strings[nHashKey].str, sSearchString))
//		{
//			int bFound = 0;
//			while (!bFound && (search_hash_table[nHashKey].next_entry_idx & 0x0FFFFFFF))
//			{
//				nHashKey = search_hash_table[nHashKey].next_entry_idx & 0x0FFFFFFF;
//				if (!strcmp(search_hash_strings[nHashKey].str, sSearchString))
//					bFound = 1;
//			}
//			if (!bFound)
//			{
//				if (nHashEntries >= MAX_SEARCH_HASH_TABLE_ENTRIES)
//				{
//					nNeedMoreEntries++;
//				}
//				else
//				{
//					search_hash_table[nHashKey].next_entry_idx |= nHashEntries;
//					search_hash_table[nHashEntries].next_entry_idx = len << 28;
//					search_hash_table[nHashEntries].offset_fnd = offset_fnd;
//					strncpy(search_hash_strings[nHashEntries].str, sSearchString, MAX_SEARCH_STRING_HASHED_LEN);
//					search_hash_strings[nHashEntries].str[MAX_SEARCH_STRING_HASHED_LEN] = '\0';
//					nHashEntries++;
//				}
//			}
//		}
//	}
//	else
//	{
//		search_hash_table[nHashKey].next_entry_idx = (len << 28);
//		search_hash_table[nHashKey].offset_fnd = offset_fnd;
//		strncpy(search_hash_strings[nHashKey].str, sSearchString, MAX_SEARCH_STRING_HASHED_LEN);
//		search_hash_strings[nHashKey].str[MAX_SEARCH_STRING_HASHED_LEN] = '\0';
//	}
//	return nHashKey;
//}
//
//void save_search_hash(void)
//{
//	fwrite(&nHashEntries, sizeof(nHashEntries), 1, fdHsh);
//	fwrite(search_hash_table, sizeof(SEARCH_HASH_TABLE), nHashEntries, fdHsh);
//	free(search_hash_table);
//	fclose(fdHsh);
//	if (nNeedMoreEntries)
//		printf("Search hash table need %ld more entries!\n", nNeedMoreEntries);
//}
#else
void init_search_hash(void)
{
	static int bFirstCall = 1;
	int i;

	if (bFirstCall)
	{
		for (i = 0; i < MAX_WIKIS_PER_DEVICE; i++)
		{
			bHashInited[i] = 0;
			lenFnd[i] = 0;
		}
		bFirstCall = 0;
	}

	if (!bHashInited[nCurrentWiki])
	{
/* Disable hashing
   fdHsh[nCurrentWiki] = wl_open(get_wiki_file_path(nCurrentWiki, "wiki.hsh"), WL_O_RDONLY);
   wl_read(fdHsh[nCurrentWiki], &nHashEntries[nCurrentWiki], sizeof(nHashEntries[nCurrentWiki]));
   search_hash_table[nCurrentWiki] = (SEARCH_HASH_TABLE *)malloc_simple(sizeof(SEARCH_HASH_TABLE) * nHashEntries[nCurrentWiki], MEM_TAG_INDEX_M1);
   bHashBlockLoaded[nCurrentWiki] = (int *)malloc_simple(sizeof(int) * (nHashEntries[nCurrentWiki] / ENTRIES_PER_HASH_BLOCK), MEM_TAG_INDEX_M1);
   memset((char *)bHashBlockLoaded[nCurrentWiki], 0, sizeof(int) * (nHashEntries[nCurrentWiki] / ENTRIES_PER_HASH_BLOCK));
*/
		fdFnd[nCurrentWiki] = wl_open(get_wiki_file_path(nCurrentWiki, "wiki.fnd"), WL_O_RDONLY);
		init_bigram(fdFnd[nCurrentWiki]);
		fnd_bufs[nCurrentWiki] = (struct _fnd_buf *)malloc_simple(sizeof(struct _fnd_buf) * FND_BUF_COUNT, MEM_TAG_INDEX_M1);
		for (i = 0; i < FND_BUF_COUNT; i++)
			fnd_bufs[nCurrentWiki][i].offset = 0;
		bHashInited[nCurrentWiki] = 1;
	}
}

//int nHashJumps;
//long get_search_hash_offset_fnd(char *sSearchString, int len)
//{
//	long nHashKey;
//	//TITLE_SEARCH title_search;
//	char sTitleSearch[MAX_TITLE_SEARCH];
//	char sTitleActual[MAX_TITLE_ACTUAL];
//	int bFound = 0;
//	int lenHashed;
//	int idxBlock;
//
//	nHashJumps = 0;
//	nHashKey = hash_key(sSearchString, len);
//	idxBlock = nHashKey / ENTRIES_PER_HASH_BLOCK;
//	if (!bHashBlockLoaded[nCurrentWiki][idxBlock])
//	{
//		wl_seek(fdHsh[nCurrentWiki], idxBlock * ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE) + sizeof(nHashEntries[nCurrentWiki]));
//		wl_read(fdHsh[nCurrentWiki], &search_hash_table[nCurrentWiki][idxBlock * ENTRIES_PER_HASH_BLOCK],
//			ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE));
//		bHashBlockLoaded[nCurrentWiki][idxBlock]++;
//#ifdef INCLUDED_FROM_KERNEL
//		if (wl_input_event_pending())
//		{
//			search_interrupted = 2;
//			goto interrupted;
//		}
//#endif
//	}
//
//	while (!bFound && nHashKey >= 0 && search_hash_table[nCurrentWiki][nHashKey].offset_fnd)
//	{
//		if (search_hash_table[nCurrentWiki][nHashKey].offset_fnd > 0)
//		{
//			retrieve_titles_from_fnd(search_hash_table[nCurrentWiki][nHashKey].offset_fnd, sTitleSearch, sTitleActual);
//			//copy_fnd_to_buf(search_hash_table[nCurrentWiki][nHashKey].offset_fnd, (char *)&title_search, sizeof(title_search));
//			if (search_interrupted)
//			{
//				search_interrupted = 6;
//				goto interrupted;
//			}
//			//bigram_decode(sDecoded, title_search.sTitleSearch, MAX_TITLE_SEARCH);
//		}
//		else
//			sTitleSearch[0] = '\0';
//		lenHashed = (search_hash_table[nCurrentWiki][nHashKey].next_entry_idx >> 28) & 0x000000FF;
//		sTitleSearch[lenHashed] = '\0';
//		if (!search_string_cmp(sTitleSearch, sSearchString, len))
//			bFound = 1;
//		if (!bFound)
//		{
//			if (search_hash_table[nCurrentWiki][nHashKey].next_entry_idx  & 0x0FFFFFFF)
//			{
//				nHashJumps++;
//				nHashKey = search_hash_table[nCurrentWiki][nHashKey].next_entry_idx & 0x0FFFFFFF;
//				idxBlock = nHashKey / ENTRIES_PER_HASH_BLOCK;
//				if (!bHashBlockLoaded[nCurrentWiki][idxBlock])
//				{
//					wl_seek(fdHsh[nCurrentWiki], idxBlock * ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE) + sizeof(nHashEntries[nCurrentWiki]));
//					wl_read(fdHsh[nCurrentWiki], &search_hash_table[nCurrentWiki][idxBlock * ENTRIES_PER_HASH_BLOCK],
//						ENTRIES_PER_HASH_BLOCK * sizeof(SEARCH_HASH_TABLE));
//					bHashBlockLoaded[nCurrentWiki][idxBlock]++;
//#ifdef INCLUDED_FROM_KERNEL
//					if (wl_input_event_pending())
//					{
//						search_interrupted = 3;
//						goto interrupted;
//					}
//#endif
//				}
//			}
//			else
//				nHashKey = -1;
//		}
//	}
//	if (bFound)
//	{
//		return search_hash_table[nCurrentWiki][nHashKey].offset_fnd;
//	}
//	else
//	{
//		return 0;
//	}
//interrupted:
//	return 0;
//}

int copy_fnd_to_buf(long offset, char *buf, int len)
{
	int i = 0;
	int bFound = 0;
	int nLeastUsedSeq = 0;
	int iLeastUsed = 0;
	int nCopyLen;
	long blocked_offset;

	if (lenFnd[nCurrentWiki] > 0 && offset >= lenFnd[nCurrentWiki])
		return 0;

	while (!bFound && i < FND_BUF_COUNT)
	{
		if (fnd_bufs[nCurrentWiki][i].offset)
		{
			if (fnd_bufs[nCurrentWiki][i].offset <= offset && offset - fnd_bufs[nCurrentWiki][i].offset < FND_BUF_BLOCK_SIZE)
			{
				bFound = 1;
			}
			else
			{
				if (nLeastUsedSeq == 0 || fnd_bufs[nCurrentWiki][i].used_seq < nLeastUsedSeq)
				{
					nLeastUsedSeq = fnd_bufs[nCurrentWiki][i].used_seq;
					iLeastUsed = i;
				}
			}
		}
		else // the block of the offset to be read into the null entry
		{
			blocked_offset = ((offset - SIZE_BIGRAM_BUF) / FND_BUF_BLOCK_SIZE) * FND_BUF_BLOCK_SIZE + SIZE_BIGRAM_BUF;
			wl_seek(fdFnd[nCurrentWiki], blocked_offset);
			fnd_bufs[nCurrentWiki][i].len = wl_read(fdFnd[nCurrentWiki], &fnd_bufs[nCurrentWiki][i].buf,
								sizeof(fnd_bufs[nCurrentWiki][i].buf));
#ifdef INCLUDED_FROM_KERNEL
			if (wl_input_event_pending())
			{
				search_interrupted = 4;
			}
#endif
			if (fnd_bufs[nCurrentWiki][i].len < FND_BUF_BLOCK_SIZE)
			{
				lenFnd[nCurrentWiki] = wl_tell(fdFnd[nCurrentWiki]);
				if (fnd_bufs[nCurrentWiki][i].len <= 0)
				{
					fnd_bufs[nCurrentWiki][i].offset = 0;
					return 0;
				}
			}
			fnd_bufs[nCurrentWiki][i].offset = blocked_offset;
			bFound = 1;
			if (search_interrupted)
				goto interrupted;
		}
		if (!bFound)
			i++;
	}

	if (!bFound)
	{
		i = iLeastUsed;
		blocked_offset = ((offset - SIZE_BIGRAM_BUF) / FND_BUF_BLOCK_SIZE) * FND_BUF_BLOCK_SIZE + SIZE_BIGRAM_BUF;
		wl_seek(fdFnd[nCurrentWiki], blocked_offset);
		fnd_bufs[nCurrentWiki][i].len = wl_read(fdFnd[nCurrentWiki], fnd_bufs[nCurrentWiki][i].buf,
							sizeof(fnd_bufs[nCurrentWiki][i].buf));
#ifdef INCLUDED_FROM_KERNEL
		if (wl_input_event_pending())
		{
			search_interrupted = 5;
		}
#endif
		if (fnd_bufs[nCurrentWiki][i].len < FND_BUF_BLOCK_SIZE)
		{
			lenFnd[nCurrentWiki] = wl_tell(fdFnd[nCurrentWiki]);
			if (fnd_bufs[nCurrentWiki][i].len <= 0)
			{
				fnd_bufs[nCurrentWiki][i].offset = 0;
				return 0;
			}
		}
		fnd_bufs[nCurrentWiki][i].offset = blocked_offset;
		if (search_interrupted)
			goto interrupted;
	}
	fnd_bufs[nCurrentWiki][i].used_seq = nUsedSeq++;

	if (len > fnd_bufs[nCurrentWiki][i].len - (offset - fnd_bufs[nCurrentWiki][i].offset)) // the buf to be copied is separated into two blocks or end of file
		nCopyLen = fnd_bufs[nCurrentWiki][i].len - (offset - fnd_bufs[nCurrentWiki][i].offset);
	else
		nCopyLen = len;

	if (nCopyLen < 0)
		nCopyLen = 0;
	else
		memcpy(buf, &fnd_bufs[nCurrentWiki][i].buf[offset - fnd_bufs[nCurrentWiki][i].offset], nCopyLen);

	if (nCopyLen < len)
		nCopyLen += copy_fnd_to_buf(fnd_bufs[nCurrentWiki][i].offset + fnd_bufs[nCurrentWiki][i].len, &buf[nCopyLen], len - nCopyLen);
	return nCopyLen;
interrupted:
	return 0;
}

long locate_previous_title_search(long offset_fnd)
{
	long len_buf;
	int nZeros;
	int i;
	char buf[sizeof(TITLE_SEARCH)];

	if (offset_fnd > sizeof(TITLE_SEARCH))
		len_buf = sizeof(TITLE_SEARCH);
	else
		len_buf = offset_fnd;
	offset_fnd -= len_buf;
	copy_fnd_to_buf(offset_fnd, buf, len_buf);
	i = len_buf - 1;
	nZeros = 0;
	while (i >= sizeof(uint32_t) && nZeros < 3)
	{
		if (!buf[i])
			nZeros++;
		i--;
	}
	i -= sizeof(uint32_t) - 1;
	return offset_fnd + i;
}

void retrieve_titles_from_fnd(long offset_fnd, unsigned char *sTitleSearchOut, unsigned char *sTitleActual)
{
	TITLE_SEARCH aTitleSearch[SEARCH_HASH_SEQUENTIAL_SEARCH_THRESHOLD];
	unsigned char sTitleSearch[MAX_TITLE_SEARCH];
	int nTitleSearch = 0;
	int bFound1 = 0;
	int bFound2 = 0;
	int i;
	int lenDuplicated;

	// Find the title that is fully spelled out.
	// The repeated characters with the previous title at the beginning of the current title will be replace by
	// a character whose binary value is the number of the repeated characters.
	while ((!bFound1 || !bFound2) && offset_fnd >= SIZE_BIGRAM_BUF && nTitleSearch < SEARCH_HASH_SEQUENTIAL_SEARCH_THRESHOLD)
	{
		char *p;

		copy_fnd_to_buf(offset_fnd, (char *)&aTitleSearch[nTitleSearch], sizeof(TITLE_SEARCH));
		p = aTitleSearch[nTitleSearch].sTitleSearch;
		strncpy(sTitleSearch, p, MAX_TITLE_SEARCH);
		sTitleSearch[MAX_TITLE_SEARCH - 1] = '\0';
		//bigram_decode(sTitleSearch, p, MAX_TITLE_SEARCH);
		p += strlen(aTitleSearch[nTitleSearch].sTitleSearch) + 1; // pointing to actual title
		//bigram_decode(sTitleActual, p, MAX_TITLE_SEARCH);
		strncpy(sTitleActual, p, MAX_TITLE_ACTUAL);
		sTitleActual[MAX_TITLE_ACTUAL - 1] = '\0';
		strcpy(aTitleSearch[nTitleSearch].sTitleSearch, sTitleSearch);
		strcpy(aTitleSearch[nTitleSearch].sTitleActual, sTitleActual);
		if ((unsigned char)aTitleSearch[nTitleSearch].sTitleSearch[0] >= ' ')
			bFound1 = 1;
		if ((unsigned char)aTitleSearch[nTitleSearch].sTitleActual[0] >= ' ')
			bFound2 = 1;
		if (!bFound1 || !bFound2)
			offset_fnd = locate_previous_title_search(offset_fnd);
		nTitleSearch++;
	}

	sTitleSearch[0] = '\0';
	if (bFound1)
	{
		for (i = nTitleSearch - 1; i >= 0; i--)
		{
			if ((unsigned char)aTitleSearch[i].sTitleSearch[0] >= ' ')
			{
				strncpy(sTitleSearch, aTitleSearch[i].sTitleSearch, MAX_TITLE_SEARCH);
				sTitleSearch[MAX_TITLE_SEARCH - 1] = '\0';
			}
			else if (sTitleSearch[0])
			{
				lenDuplicated = aTitleSearch[i].sTitleSearch[0] + 1;
				memcpy(&sTitleSearch[lenDuplicated], &aTitleSearch[i].sTitleSearch[1], MAX_TITLE_SEARCH - lenDuplicated - 1);
				sTitleSearch[MAX_TITLE_SEARCH - 1] = '\0';
			}
		}
	}
	bigram_decode(sTitleSearchOut, sTitleSearch, MAX_TITLE_SEARCH);

	sTitleActual[0] = '\0';
	if (bFound2)
	{
		for (i = nTitleSearch - 1; i >= 0; i--)
		{
			if ((unsigned char)aTitleSearch[i].sTitleActual[0] >= ' ')
			{
				strncpy(sTitleActual, aTitleSearch[i].sTitleActual, MAX_TITLE_ACTUAL);
				sTitleActual[MAX_TITLE_ACTUAL - 1] = '\0';
			}
			else if (sTitleActual[0])
			{
				lenDuplicated = aTitleSearch[i].sTitleActual[0] + 1;
				memcpy(&sTitleActual[lenDuplicated], &aTitleSearch[i].sTitleActual[1], MAX_TITLE_ACTUAL - lenDuplicated - 1);
				sTitleActual[MAX_TITLE_ACTUAL - 1] = '\0';
			}
		}
	}
}
#endif
