/*
 * (C) Copyright 2009 Openmoko, Inc.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

//#include "bmf.h"
//#include "wiki_render.h"
//#include "lcd_buf_draw.h"
//#include "Alloc.h"
//#include "Bra.h"
//#include "LzmaEnc.h"
#include "bigram.h"
#include "search_hash.h"


// pedia.idx format:
//	The first 4 bytes contain the article count.
// 	Each article got a ARTICLE_PTR structure entry in pedia.idx.
// 	The first ARTICLE_PTR entry is for article idx 1.
//
// pedia.pfx format:
// 	first three character indexing table - 54 * 54 * 54 entries * 4 bytes (int32_t int - file offset of pedia.fnd)
//		54 characters - null + 0~9 + a~z + ...
//
// pedia.fnd format:
// 	bigram table - 128 entries * 2 bytes
//	All titles for search are sequentially concatnated into pedia.fnd in search order.
//      Each entry consists of (see TITLE_SEARCH_REMAINDER):
//		idx of article (pointing to pedia.idx)
//		variable length and null terminated remainder (starting from the 3rd character)
#define SIZE_FIRST_THREE_CHAR_INDEXING (SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(uint32_t))


#define MAX_TITLE_SEARCH 64
#define MAX_TITLE_LEN 512


typedef struct _TITLE_SEARCH { // used to mask the porinter to the remainder of the title for search
	int32_t idxArticle;
	char cZero; // null character for backward search
	char sTitleSearch[MAX_TITLE_SEARCH]; // null terminated title for search (with bigram encoded)
} TITLE_SEARCH;


static int process_hash_sequential_search(char *sLocalTitleSearch, int32_t offsetBufFnd,
					  int lenHashSequentialSearch, char *sHashSequentialSearch, int32_t *countHashSequentialSearchForNextChar)
{
	int lenTitleSearch = strlen(sLocalTitleSearch);
	int lenSame;
	int i;
	int bHashAdded = 0;

	for (lenSame = 0; lenSame < lenTitleSearch && lenSame < lenHashSequentialSearch; lenSame++)
	{
		if (tolower(sLocalTitleSearch[lenSame]) != sHashSequentialSearch[lenSame])
			break;
		countHashSequentialSearchForNextChar[lenSame]++;
	}

	for (i = lenSame; i < lenHashSequentialSearch; i++)
	{
		if (!bHashAdded && i >= MAX_SEARCH_STRING_ALL_HASHED_LEN && i < lenTitleSearch &&
		    countHashSequentialSearchForNextChar[i] >= SEARCH_HASH_SEQUENTIAL_SEARCH_THRESHOLD)
		{
			add_search_hash(sLocalTitleSearch, i + 1, offsetBufFnd);
			bHashAdded = 1;
		}
		if (lenSame == MAX_SEARCH_STRING_ALL_HASHED_LEN)
			countHashSequentialSearchForNextChar[i]++;
		else
			countHashSequentialSearchForNextChar[i] = 0;
	}

	if (lenTitleSearch > MAX_SEARCH_STRING_HASHED_LEN)
		lenHashSequentialSearch = MAX_SEARCH_STRING_HASHED_LEN;
	else
		lenHashSequentialSearch = lenTitleSearch;

	for (i = lenSame; i < lenHashSequentialSearch; i++)
	{
		sHashSequentialSearch[i] = tolower(sLocalTitleSearch[i]);
	}


	return lenHashSequentialSearch;
}


static int32_t build_hash_tree(char *sTitleSearch, int32_t offsetBufFnd, char *bufFnd, int32_t lenBufFnd)
{
	int i;
	int lenTitleSearch;
	TITLE_SEARCH *pTitleSearch = (TITLE_SEARCH *)&bufFnd[offsetBufFnd];
	int rc;
	char *pSupportedChars = SUPPORTED_SEARCH_CHARS;
	char c;
	char sLocalTitleSearch[MAX_TITLE_LEN];
	int lenHashSequentialSearch = 0;
	char sHashSequentialSearch[MAX_SEARCH_STRING_HASHED_LEN];
	int32_t countHashSequentialSearchForNextChar[MAX_SEARCH_STRING_HASHED_LEN];

	//showMsg(3, "build_hash_tree [%s] %x\n", sTitleSearch, offsetBufFnd);
	memset(countHashSequentialSearchForNextChar, 0, sizeof(countHashSequentialSearchForNextChar));
	lenTitleSearch = strlen(sTitleSearch);
	if (lenTitleSearch < MAX_SEARCH_STRING_ALL_HASHED_LEN)
	{
		for (i = 0; i < strlen(pSupportedChars); i++)
		{
			c = pSupportedChars[i];
			if (c != ' ' || sTitleSearch[lenTitleSearch -1] != ' ') // no two continuous blanks
			{
				sTitleSearch[lenTitleSearch] = c;
				sTitleSearch[lenTitleSearch + 1] = '\0';
				bigram_decode(sLocalTitleSearch, pTitleSearch->sTitleSearch, MAX_TITLE_LEN);
				while (offsetBufFnd < lenBufFnd &&
				       (rc = search_string_cmp(sLocalTitleSearch, sTitleSearch, strlen(sTitleSearch))) < 0)
				{
					lenHashSequentialSearch = process_hash_sequential_search(sLocalTitleSearch, offsetBufFnd,
												 lenHashSequentialSearch, sHashSequentialSearch, countHashSequentialSearchForNextChar);
					offsetBufFnd += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) + 2;
					pTitleSearch = (TITLE_SEARCH *)&bufFnd[offsetBufFnd];
					bigram_decode(sLocalTitleSearch, pTitleSearch->sTitleSearch, MAX_TITLE_LEN);
				}

				if (offsetBufFnd < lenBufFnd && !rc)
				{
					add_search_hash(sTitleSearch, strlen(sTitleSearch), offsetBufFnd);
					lenHashSequentialSearch = 0;
					memset(countHashSequentialSearchForNextChar, 0, sizeof(countHashSequentialSearchForNextChar));
					//offsetBufFnd += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) + 2;
					if (offsetBufFnd < lenBufFnd)
					{
						offsetBufFnd = build_hash_tree(sTitleSearch, offsetBufFnd, bufFnd, lenBufFnd);
						pTitleSearch = (TITLE_SEARCH *)&bufFnd[offsetBufFnd];
					}
				}
			}
		}
	}
	return offsetBufFnd;
}


void generate_pedia_hsh(const char *fnd_name, const char *pfx_name, const char *hsh_name)
{
	FILE *fdPfx, *fdFnd;
	char sTitleSearch[MAX_TITLE_LEN];
	int32_t *firstThreeCharIndexing;
	int idxFirstThreeCharIndexing;
	char *bufFnd;
	int32_t lenBufFnd;
	char *pSupportedChars = SUPPORTED_SEARCH_CHARS;
	char c1, c2, c3;
	int i, j, k;
	int32_t offsetBufFnd = 0;

	fdPfx = fopen(pfx_name, "rb");
	if (!fdPfx)
	{
		fprintf(stderr, "cannot open file '%s', error: %s\n", pfx_name, strerror(errno));
		exit(1);
	}
	fdFnd = fopen(fnd_name, "rb");
	if (!fdFnd)
	{
		fprintf(stderr, "cannot open file '%s', error: %s\n", fnd_name, strerror(errno));
		exit(1);
	}

	init_bigram(fdFnd);
	create_search_hash(hsh_name);

	firstThreeCharIndexing = (int32_t *)malloc(SIZE_FIRST_THREE_CHAR_INDEXING);
	if (!firstThreeCharIndexing)
	{
		fprintf(stderr, "malloc firstThreeCharIndexing error\n");
		exit(1);
	}

	if (SIZE_FIRST_THREE_CHAR_INDEXING != fread((void*)firstThreeCharIndexing, 1, SIZE_FIRST_THREE_CHAR_INDEXING, fdPfx))
	{
		fprintf(stderr, "fread failed\n");
		exit(1);
	}

	fseek(fdFnd, 0, SEEK_END);
	lenBufFnd = ftell(fdFnd);
	fseek(fdFnd, 0, SEEK_SET);
	bufFnd = malloc(lenBufFnd);
	if (!bufFnd)
	{
		fprintf(stderr, "malloc bufFnd error\n");
		exit(1);
	}
	lenBufFnd = fread(bufFnd, 1, lenBufFnd, fdFnd);

	for (i = 0; i < strlen(pSupportedChars); i++)
	{
		c1 = pSupportedChars[i];
		if (c1 != ' ') // no initial blank
		{
			for (j = 0; j < strlen(pSupportedChars); j++)
			{
				c2 = pSupportedChars[j];
				for (k = 0; k < strlen(pSupportedChars); k++)
				{
					c3 = pSupportedChars[k];
					idxFirstThreeCharIndexing =
						bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT +
						bigram_char_idx(c2) * SEARCH_CHR_COUNT + bigram_char_idx(c3);
					if (firstThreeCharIndexing[idxFirstThreeCharIndexing])
					{
						sTitleSearch[0] = c1;
						sTitleSearch[1] = c2;
						sTitleSearch[2] = c3;
						sTitleSearch[3] = '\0';
						if (!offsetBufFnd)
							offsetBufFnd = firstThreeCharIndexing[idxFirstThreeCharIndexing];
						offsetBufFnd = build_hash_tree(sTitleSearch, offsetBufFnd, bufFnd, lenBufFnd);
					}
				}
			}
		}
	}
	save_search_hash();
	free(firstThreeCharIndexing);
	free(bufFnd);
	fclose(fdPfx);
	fclose(fdFnd);
}
