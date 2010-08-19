/*
 * Copyright (c) 2010 Openmoko Inc.
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
#include "search_fnd.h"
#include "lcd_buf_draw.h"
#include "btree.h"

extern int search_interrupted;
#define FND_BUF_COUNT 1024
#define MAX_FND_FILES 16

// FND_BUF_BLOCK_SIZE needs to be larger than NUMBER_OF_FIRST_PAGE_RESULTS * sizeof(TITLE_SEARCH)
#define FND_BUF_BLOCK_SIZE 4096
typedef struct __attribute__ ((packed)) _fnd_buf {
	uint32_t offset;
	uint16_t len;
	int16_t prev_used_seq; // entry idx with the used sequence before this
	int16_t next_used_seq; // entry idx with the used sequence after this
	uint8_t wiki_id;
	char buf[FND_BUF_BLOCK_SIZE];
} FND_BUF, *PFND_BUF;
PFND_BUF pFndBuf;
int nIdxFndBufFirstUsed;
int nIdxFndBufLastUsed;

typedef struct __attribute__ ((packed)) _PER_WIKI_INFO {
	int bSearchFndInited;
	unsigned int nFndCount;
	int fdFnd[MAX_FND_FILES];
	unsigned long offsetFndStart[MAX_FND_FILES];
	unsigned int lenFnd[MAX_FND_FILES];
} PER_WIKI_INFO, *PPER_WIKI_INFO;
PPER_WIKI_INFO pPerWikiInfo;

long nUsedSeq = 1;
BTREE btree;

void init_search_fnd(void)
{
	static int bFirstCall = 1;
	int i;

	if (bFirstCall)
	{
		btree_init(&btree, FND_BUF_COUNT, 0);
		pFndBuf = (PFND_BUF)malloc_simple(sizeof(FND_BUF) * FND_BUF_COUNT, MEM_TAG_INDEX_M1);
		nIdxFndBufFirstUsed = 0;
		nIdxFndBufLastUsed = FND_BUF_COUNT - 1;
		for (i = 0; i < FND_BUF_COUNT; i++)
		{
			pFndBuf[i].offset = 0;
			pFndBuf[i].prev_used_seq = i - 1;
			if (i == FND_BUF_COUNT - 1)
				pFndBuf[i].next_used_seq = -1;
			else
				pFndBuf[i].next_used_seq = i + 1;
		}
		pPerWikiInfo = (PPER_WIKI_INFO)malloc_simple(sizeof(PER_WIKI_INFO) * get_wiki_count(), MEM_TAG_INDEX_M1);
		for (i = 0; i < get_wiki_count(); i++)
		{
			pPerWikiInfo[i].bSearchFndInited = 0;
		}
		bFirstCall = 0;
	}

	if (!pPerWikiInfo[nCurrentWiki].bSearchFndInited)
	{
		pPerWikiInfo[nCurrentWiki].fdFnd[0] = wl_open(get_wiki_file_path(nCurrentWiki, "wiki.fnd"), WL_O_RDONLY);
		init_bigram(pPerWikiInfo[nCurrentWiki].fdFnd[0]);
		pPerWikiInfo[nCurrentWiki].bSearchFndInited = 1;
		pPerWikiInfo[nCurrentWiki].offsetFndStart[0] = 0;
		wl_fsize(pPerWikiInfo[nCurrentWiki].fdFnd[0], &pPerWikiInfo[nCurrentWiki].lenFnd[0]);
		for (i = 1; i < MAX_FND_FILES; i++)
		{
			char file_name[13];
			sprintf(file_name, "wiki%d.fnd", i);
			pPerWikiInfo[nCurrentWiki].fdFnd[i] = wl_open(get_wiki_file_path(nCurrentWiki, file_name), WL_O_RDONLY);
			if (pPerWikiInfo[nCurrentWiki].fdFnd[i] < 0)
				break;
			pPerWikiInfo[nCurrentWiki].offsetFndStart[i] = pPerWikiInfo[nCurrentWiki].offsetFndStart[i - 1] +
				pPerWikiInfo[nCurrentWiki].lenFnd[i - 1];
			wl_fsize(pPerWikiInfo[nCurrentWiki].fdFnd[i], &pPerWikiInfo[nCurrentWiki].lenFnd[i]);
		}
		pPerWikiInfo[nCurrentWiki].nFndCount = i;
	}
}

int copy_fnd_to_buf(long offset, char *buf, int len)
{
	int idxFndBuf;
	int nCopyLen;
	long blocked_offset;
	PBTREE_ELEMENT pElement;
	long nKey;

	blocked_offset = ((offset - SIZE_BIGRAM_BUF) / FND_BUF_BLOCK_SIZE) * FND_BUF_BLOCK_SIZE + SIZE_BIGRAM_BUF;
	nKey = (nCurrentWiki << 24) | blocked_offset;
	pElement = btree_search(&btree, nKey);
	if (pElement)
	{
		idxFndBuf = pElement->data_entry_idx;
	}
	else
	{
		BTREE_ELEMENT element;
		int nFndIdx;
		idxFndBuf = nIdxFndBufFirstUsed;
		if (pFndBuf[idxFndBuf].offset)
		{
			long nKeyOld = (pFndBuf[idxFndBuf].wiki_id << 24) | pFndBuf[idxFndBuf].offset;
			btree_delete(&btree, nKeyOld);
		}
		for (nFndIdx = 0; nFndIdx < pPerWikiInfo[nCurrentWiki].nFndCount; nFndIdx++)
		{
			if (pPerWikiInfo[nCurrentWiki].offsetFndStart[nFndIdx] <= blocked_offset &&
			    blocked_offset < pPerWikiInfo[nCurrentWiki].offsetFndStart[nFndIdx] +
			    pPerWikiInfo[nCurrentWiki].lenFnd[nFndIdx])
			{
				break;
			}
		}
		pFndBuf[idxFndBuf].len = 0;
		if (nFndIdx < pPerWikiInfo[nCurrentWiki].nFndCount)
		{
			wl_seek(pPerWikiInfo[nCurrentWiki].fdFnd[nFndIdx],
				blocked_offset - pPerWikiInfo[nCurrentWiki].offsetFndStart[nFndIdx]);
			pFndBuf[idxFndBuf].len = wl_read(pPerWikiInfo[nCurrentWiki].fdFnd[nFndIdx],
							 &pFndBuf[idxFndBuf].buf[0], FND_BUF_BLOCK_SIZE);
		}
		if (nFndIdx >= pPerWikiInfo[nCurrentWiki].nFndCount || pFndBuf[idxFndBuf].len <= 0)
		{
			pFndBuf[idxFndBuf].offset = 0;
			if (idxFndBuf != nIdxFndBufFirstUsed)
			{
				pFndBuf[pFndBuf[idxFndBuf].prev_used_seq].next_used_seq = pFndBuf[idxFndBuf].next_used_seq;
				if (idxFndBuf != nIdxFndBufLastUsed)
					pFndBuf[pFndBuf[idxFndBuf].next_used_seq].prev_used_seq = pFndBuf[idxFndBuf].prev_used_seq;
				else
					nIdxFndBufFirstUsed = pFndBuf[idxFndBuf].prev_used_seq;
				pFndBuf[nIdxFndBufFirstUsed].prev_used_seq = idxFndBuf;
				pFndBuf[idxFndBuf].next_used_seq = nIdxFndBufFirstUsed;
				nIdxFndBufFirstUsed = idxFndBuf;
				pFndBuf[idxFndBuf].prev_used_seq = -1;
			}
			return 0;
		}
		if (pFndBuf[idxFndBuf].len < FND_BUF_BLOCK_SIZE && nFndIdx < pPerWikiInfo[nCurrentWiki].nFndCount - 1)
		{
			wl_seek(pPerWikiInfo[nCurrentWiki].fdFnd[nFndIdx + 1], 0);
			pFndBuf[idxFndBuf].len += wl_read(pPerWikiInfo[nCurrentWiki].fdFnd[nFndIdx + 1],
							  &pFndBuf[idxFndBuf].buf[pFndBuf[idxFndBuf].len], FND_BUF_BLOCK_SIZE - pFndBuf[idxFndBuf].len);
		}
		pFndBuf[idxFndBuf].offset = blocked_offset;
		element.key = nKey;
		element.data_entry_idx = idxFndBuf;
		element.subtree_node_idx = BTREE_INVALID_NODE_IDX;
		btree_insert(&btree, &element);
	}

	if (pFndBuf[idxFndBuf].prev_used_seq < 0)
	{
		nIdxFndBufFirstUsed = pFndBuf[idxFndBuf].next_used_seq;
		pFndBuf[nIdxFndBufFirstUsed].prev_used_seq = -1;
	}
	else if (idxFndBuf != nIdxFndBufLastUsed)
	{
		pFndBuf[pFndBuf[idxFndBuf].prev_used_seq].next_used_seq = pFndBuf[idxFndBuf].next_used_seq;
		pFndBuf[pFndBuf[idxFndBuf].next_used_seq].prev_used_seq = pFndBuf[idxFndBuf].prev_used_seq;
	}

	if (idxFndBuf != nIdxFndBufLastUsed)
	{
		pFndBuf[idxFndBuf].prev_used_seq = nIdxFndBufLastUsed;
		pFndBuf[idxFndBuf].next_used_seq = -1;
		pFndBuf[nIdxFndBufLastUsed].next_used_seq = idxFndBuf;
		nIdxFndBufLastUsed = idxFndBuf;
	}

// copy_fnd_to_buf() cannot be interrupted.  caused some unexpected results
//#ifdef INCLUDED_FROM_KERNEL
//	if (wl_input_event_pending())
//	{
//		search_interrupted = 4;
//	}
//#endif
//	if (search_interrupted)
//		goto interrupted;

	if (len > pFndBuf[idxFndBuf].len - (offset - pFndBuf[idxFndBuf].offset)) // the buf to be copied is separated into two blocks or end of file
		nCopyLen = pFndBuf[idxFndBuf].len - (offset - pFndBuf[idxFndBuf].offset);
	else
		nCopyLen = len;

	if (nCopyLen < 0)
		nCopyLen = 0;
	else
		memcpy(buf, &pFndBuf[idxFndBuf].buf[offset - pFndBuf[idxFndBuf].offset], nCopyLen);

	if (nCopyLen < len)
		nCopyLen += copy_fnd_to_buf(pFndBuf[idxFndBuf].offset + pFndBuf[idxFndBuf].len, &buf[nCopyLen], len - nCopyLen);
	return nCopyLen;
//interrupted:
//	return 0;
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
	TITLE_SEARCH aTitleSearch[SEARCH_FND_SEQUENTIAL_SEARCH_THRESHOLD];
	unsigned char sTitleSearch[MAX_TITLE_SEARCH];
	int nTitleSearch = 0;
	int bFound1 = 0;
	int bFound2 = 0;
	int i;
	int lenDuplicated;

	// Find the title that is fully spelled out.
	// The repeated characters with the previous title at the beginning of the current title will be replace by
	// a character whose binary value is the number of the repeated characters.
	while ((!bFound1 || !bFound2) && offset_fnd >= SIZE_BIGRAM_BUF + sizeof(uint32_t) && nTitleSearch < SEARCH_FND_SEQUENTIAL_SEARCH_THRESHOLD)
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
