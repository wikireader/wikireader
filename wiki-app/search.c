/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Holger Hans Peter Freyther <zecke@openmoko.org>
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

#include "search.h"
#include "msg.h"
#include "wl-keyboard.h"
#include <stdlib.h>
#include <string.h>
#include <guilib.h>
#include <glyph.h>
#include <lcd.h>
#include <file-io.h>
#include <input.h>
#include <malloc-simple.h>

#include "Alloc.h"
#include "Bra.h"
#include "LzmaDec.h"
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "bigram.h"
#include "history.h"
#include "wikilib.h"
#include "search_hash.h"
#include "wiki_info.h"

#if !defined(INCLUDED_FROM_KERNEL)
#include "time.h"
#else
#include <tick.h>
#endif

#define DBG_SEARCH 0

#define DELAYED_SEARCH_TIME 0.3
unsigned long time_search_string_changed = 0;
bool search_string_changed = false;
bool search_string_changed_remove = false;
int more_search_results = 0;
extern ARTICLE_LINK articleLink[MAX_ARTICLE_LINKS];
extern int article_link_count;
extern long saved_idx_article;
int search_interrupted = 0;

typedef struct _search_results {
	char title[NUMBER_OF_FIRST_PAGE_RESULTS][MAX_TITLE_SEARCH];
	char title_search[NUMBER_OF_FIRST_PAGE_RESULTS][MAX_TITLE_SEARCH];
	uint32_t idx_article[NUMBER_OF_FIRST_PAGE_RESULTS];  // index (wiki.idx) for loading the article
	uint32_t offset_list[NUMBER_OF_FIRST_PAGE_RESULTS];  // offset (wiki.fnd) of each search title in list
	uint32_t offset_next;		// offset (wiki.fnd) of the next title after the list
	uint32_t count;
	uint32_t result_populated;
	int32_t cur_selected;		// -1 when no selection.
} SEARCH_RESULTS;
static SEARCH_RESULTS *result_list = NULL;

typedef struct _search_info {
	int32_t inited;
	int32_t fd_pfx;
	int32_t fd_idx;
	int32_t fd_dat[MAX_DAT_FILES];
	uint32_t max_article_idx;
	uint32_t prefix_index_table[SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT];
	uint32_t b_prefix_index_block_loaded[SEARCH_CHR_COUNT];
	char buf[NUMBER_OF_FIRST_PAGE_RESULTS * sizeof(TITLE_SEARCH)];	// buf correspond to result_list
	uint32_t buf_len;
	uint32_t offset_current;	// offset (wiki.fnd) of the content of buffer
} SEARCH_INFO;
static SEARCH_INFO *search_info = NULL;

#define SIZE_PREFIX_INDEX_TABLE SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(long)
//static struct search_state state;
//static struct search_state last_first_hit;

static char search_string[MAX_TITLE_SEARCH];
static int  search_string_pos[MAX_TITLE_SEARCH];
static int search_str_len = 0;

//static char s_find_first = 1;

static void *SzAlloc(void *p, size_t size) { p = p; return malloc_simple(size, MEM_TAG_INDEX_M1); }
static void SzFree(void *p, void *address) { p = p; free_simple(address, MEM_TAG_INDEX_M1); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

//const char* search_fetch_result()
//{
//	DP(DBG_SEARCH, ("O search_fetch_result() called\n"));
//	if (search_str_len == 0)
//		return NULL;
//#ifdef WOM_ON
//	const wom_article_index_t* idx;
//	static char result_buf[MAXSTR]; // we know that the returned pointer is copied immediately
//	if (s_find_first) {
//		s_find_first = 0;
//		idx = wom_find_article(g_womh, search_string, search_str_len);
//	} else
//		idx = wom_get_next_article(g_womh);
//	if (!idx) return 0;
//	sprintf(result_buf, "%.*s%.6x", idx->uri_len, idx->abbreviated_uri, (unsigned int) idx->offset_into_articles);
//	DP(DBG_SEARCH, ("O search_fetch_result() '%s'\n", result_buf));
//	return result_buf;
//#else // !WOM_ON
//	char* result = search_fast(&global_search, search_string, &state);
//	if (s_find_first) {
//		s_find_first = 0;
//		store_state(&global_search, &state, &last_first_hit);
//	}
//	return result;
//#endif
//}

long result_list_offset_next(void)
{
	return result_list->offset_next;
}

long result_list_next_result(long offset_next, long *idxArticle, char *sTitleSearch)
{
	TITLE_SEARCH titleSearch;

	copy_fnd_to_buf(offset_next, (void *)&titleSearch, sizeof(TITLE_SEARCH));
	retrieve_titles_from_fnd(offset_next, sTitleSearch, result_list->title[result_list->count]);
	if (!search_string_cmp(sTitleSearch, search_string, search_str_len)) // match!
	{
		*idxArticle = titleSearch.idxArticle;
		offset_next += sizeof(titleSearch.idxArticle) + strlen(titleSearch.sTitleSearch) + 2; // now point to the actual title
		offset_next += strlen(&titleSearch.sTitleSearch[strlen(titleSearch.sTitleSearch) + 1]) + 1; // skip the actual title to the next titleSearch
		return offset_next;
	}
	else
		return 0;
}

void get_article_title_from_idx(long idx, char *title)
{
	ARTICLE_PTR article_ptr;
	TITLE_SEARCH title_search;
	int wiki_id;
	int wiki_idx;
	int nTmpeCurrentWiki = nCurrentWiki;
	char sTitleSearch[MAX_TITLE_SEARCH];

	title[0] = '\0';
	wiki_id = idx >> 24;
	idx &= 0x00FFFFFF;
	if (wiki_id > 0)
	{
		wiki_idx = get_wiki_idx_from_id(wiki_id);
		if (wiki_idx < 0) // wiki not loaded
			return;
		nCurrentWiki = wiki_idx;
	}

	wl_seek(search_info[nCurrentWiki].fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + 4);
	wl_read(search_info[nCurrentWiki].fd_idx, (void *)&article_ptr, sizeof(article_ptr));
	if (article_ptr.offset_fnd)
	{
		copy_fnd_to_buf(article_ptr.offset_fnd, (char *)&title_search, sizeof(title_search));
		retrieve_titles_from_fnd(article_ptr.offset_fnd, sTitleSearch, title);
		title[MAX_TITLE_SEARCH - 1] = '\0';
	}
	nCurrentWiki = nTmpeCurrentWiki;
}

void load_prefix_index(int nWikiIdx)
{
	int i;

	if (!search_info[nWikiIdx].inited)
	{
		search_info[nWikiIdx].fd_pfx = wl_open(get_wiki_file_path(nWikiIdx, "wiki.pfx"), WL_O_RDONLY);
		search_info[nWikiIdx].fd_idx = wl_open(get_wiki_file_path(nWikiIdx, "wiki.idx"), WL_O_RDONLY);
		for (i=0; i < MAX_DAT_FILES; i++)
			search_info[nWikiIdx].fd_dat[i] = -1;
		search_info[nWikiIdx].offset_current = -1;
		if (search_info[nWikiIdx].fd_pfx >= 0 && search_info[nWikiIdx].fd_idx >= 0)
		{
			wl_read(search_info[nWikiIdx].fd_idx, (void *)&search_info[nWikiIdx].max_article_idx, sizeof(search_info[nWikiIdx].max_article_idx));
			memset((char *)search_info[nWikiIdx].b_prefix_index_block_loaded, 0, sizeof(search_info[nWikiIdx].b_prefix_index_block_loaded));
			search_info[nWikiIdx].inited = 1;
		}
		else
			fatal_error("index file open error");
	}
}

void search_init()
{
	int i;
	int nWikiCount = get_wiki_count();

	if (!result_list)
	{
		result_list = (SEARCH_RESULTS *)malloc_simple(sizeof(SEARCH_RESULTS), MEM_TAG_INDEX_M1);
		if (!result_list)
			fatal_error("search_init malloc error");
	}
	if (!search_info)
	{
		search_info = (SEARCH_INFO *)malloc_simple(sizeof(SEARCH_INFO) * nWikiCount, MEM_TAG_INDEX_M1);
		if (!search_info)
			fatal_error("search_init malloc error");
		else
		{
			for (i = 0; i < nWikiCount; i++)
			{
				search_info[i].inited = 0;
			}
		}
	}

	load_prefix_index(nCurrentWiki);
	result_list->count = 0;
	init_search_hash();
}

void memrcpy(char *dest, char *src, int len) // memory copy starting from the last byte
{
	if (len >= 0)
	{
		dest += len - 1;
		src += len - 1;

		while (len--)
		{
			*dest = *src;
			dest--;
			src--;
		}
	}
}

char article_error[100] = "";
char article_error2[100] = "";
static void print_article_error()
{
	char msg[100];

	sprintf(msg, "The article, %lx, failed to load.", saved_idx_article);
	guilib_fb_lock();
	guilib_clear();
	render_string(SEARCH_LIST_FONT_IDX, -1, 84, msg, strlen(msg), 0);
	render_string(SEARCH_LIST_FONT_IDX, -1, 104, "Please restart your WikiReader and", 34, 0);
	render_string(SEARCH_LIST_FONT_IDX, -1, 124, "try again.", 10, 0);
//render_string(SEARCH_LIST_FONT_IDX, -1, 124, article_error, strlen(article_error), 0);
//render_string(SEARCH_LIST_FONT_IDX, -1, 144, article_error2, strlen(article_error2), 0);
	guilib_fb_unlock();
}

// check if null terminator exists
int is_proper_string(char *s, int len)
{
	while (len >= 0)
	{
		if (!*s)
			return 1;
		s++;
		len--;
	}
	return 0;
}

char *strnstr(char *s1, char *s2, int len)
{
	int bFound = 0;
	int s2_len = strlen(s2);

	while (!bFound && len >= s2_len)
	{
		if (!memcmp(s1, s2, s2_len))
			bFound = 1;
		else
		{
			s1++;
			len--;
		}
	}

	if (bFound)
		return s1;
	else
		return NULL;
}

// locate the patterh of the beginning of a TITLE_SEARCH structure
// the continuous 8 bytes should look like the following:
// 0xXX00 (null terminated last two bytes of the last TITLE_SEARCH.sTitleActual where XX is not 00) +
// 0xXXXXXX00 (TITLE_SEARCH.idxArticle where XXXXXX is not 000000) +
// 0x00 (TITLE_SEARCH.cZero) +
// 0xXX (Non-zero of TITLE_SEARCH.sTitleSearch)
bool is_TITLE_SEARCH_pattern(char *pBuf)
{
	if (pBuf[0] && !pBuf[1] && (pBuf[2] || pBuf[3] || pBuf[4]) && !pBuf[5] && !pBuf[6] && pBuf[7])
		return true;
	else
		return false;
}

TITLE_SEARCH *locate_proper_title_search(char *buf_middle, int len)
{
	char *pBuf = buf_middle - 2; 	// including the possible null terminated last two bytes of the last TITLE_SEARCH
	// for finding the proper TITLE_SEARCH pattern
	int i = 0;
	bool bFound = false;

	while (!bFound && i < len + 2 - 8) // the pattern consists of 8 bytes
	{
		if (is_TITLE_SEARCH_pattern(&pBuf[i]))
			bFound = true;
		else
			i++;
	}

	if (bFound)
		return (TITLE_SEARCH *)(pBuf + i + 2);
	else
		return NULL;
}

uint32_t get_article_idx_from_offset_range(char *title, long offset_fnd_start, long offset_fnd_end)
{
	int len;
	int rc;
	uint32_t article_idx = 0;
	char sTitleSearch[MAX_TITLE_SEARCH];
	char sTitleActual[MAX_TITLE_SEARCH];
	char buf_middle[sizeof(TITLE_SEARCH) * 2];
	long offset_middle;
	static TITLE_SEARCH *pTitleSearch;
	int offsetNextTitleSearch = 0;
	bool bFound = false;

	while (!bFound && offset_fnd_start >= 0)
	{
		if (search_info[nCurrentWiki].offset_current != offset_fnd_start)
		{
			search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
			search_info[nCurrentWiki].offset_current = offset_fnd_start;
			if (search_interrupted)
			{
				goto out;
			}
			if (search_info[nCurrentWiki].buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
			{
				goto out;
			}
		}

		pTitleSearch = (TITLE_SEARCH *)&search_info[nCurrentWiki].buf[offsetNextTitleSearch];
		if (offsetNextTitleSearch < search_info[nCurrentWiki].buf_len &&
		    is_proper_string(pTitleSearch->sTitleSearch, search_info[nCurrentWiki].buf_len - offsetNextTitleSearch -
				     sizeof(pTitleSearch->idxArticle) - sizeof(pTitleSearch->cZero)))
		{
			retrieve_titles_from_fnd(offset_fnd_start + offsetNextTitleSearch,
				sTitleSearch, sTitleActual);
			rc = search_string_cmp(sTitleSearch, search_string, search_str_len);
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "offset (%x + %x, %x) ", offset_fnd_start, offsetNextTitleSearch, offset_fnd_end);
//msg(MSG_INFO, "rc %d, [%s], [%s], [", rc, sTitleSearch, sTitleActual);
//int i;
//for (i=0;i<search_str_len;i++)
//msg(MSG_INFO, "%c", search_string[i]);
//msg(MSG_INFO, "]\n");
//#endif
			if (!rc) // match!
			{
				if (!strcmp(sTitleActual, title))
				{
					article_idx = pTitleSearch->idxArticle;
					bFound = true;
				}
				else
				{
					offset_fnd_end = -1; // start sequential search
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
						strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
					offsetNextTitleSearch = 0;
				}
			}
			else if (rc < 0)
			{
				if (offset_fnd_end <= 0)
				{
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
						strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
				}
				else // binary search
				{
					offset_middle = offset_fnd_start + (offset_fnd_end - offset_fnd_start) / 2; // position to the middle of the range
					if (offset_middle <= offset_fnd_start)
						offset_fnd_end = -1;
					else
					{
						len = copy_fnd_to_buf(offset_middle, buf_middle, sizeof(buf_middle));
						if (search_interrupted)
						{
							goto out;
						}
						if (len >= sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
						{
							pTitleSearch = locate_proper_title_search(buf_middle, len);
							if (pTitleSearch)
							{
								char local_title_search[MAX_TITLE_SEARCH];
								char sTitleActual[MAX_TITLE_SEARCH];

								offset_middle += (char *)pTitleSearch - buf_middle;
								retrieve_titles_from_fnd(offset_middle, local_title_search, sTitleActual);
								rc = search_string_cmp(local_title_search, search_string, search_str_len);
								if (rc >= 0)	// the first mactch will be in front or at offset_middle
								{
									if (offset_middle == offset_fnd_end) // search from offset_fnd_start directly
										offset_fnd_end = -1;
									else
										offset_fnd_end = offset_middle;
								}
								else		// the first mactch will be after offset_middle
								{
									offset_fnd_start = offset_middle;
									search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
									search_info[nCurrentWiki].offset_current = offset_fnd_start;
									if (search_interrupted)
									{
										goto out;
									}
									if (search_info[nCurrentWiki].buf_len <= 0)
									{
										goto out;
									}
									offsetNextTitleSearch = 0;
								}
							}
						}
						else
							offset_fnd_end = -1;
					}
				}
				offsetNextTitleSearch = 0;
			}
			else
			{
				bFound = true; // return 0
				goto out;
			}
		}
		else
		{
			offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
			search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
			search_info[nCurrentWiki].offset_current = offset_fnd_start;
			if (search_interrupted)
			{
				goto out;
			}
			if (search_info[nCurrentWiki].buf_len <= 0)
			{
				goto out;
			}
			offsetNextTitleSearch = 0;
		}
	}

out:
	return article_idx;
}

int fetch_search_result(long input_offset_fnd_start, long input_offset_fnd_end, int bInit)
{
	int len;
	int rc;
	char buf_middle[sizeof(TITLE_SEARCH) * 2];
	long offset_middle;
	static TITLE_SEARCH *pTitleSearch;
	static int offsetNextTitleSearch = 0;
	static long offset_fnd_start = -1;
	static long offset_fnd_end = -1;

	if (bInit)
	{
		result_list->result_populated = 0;
		offset_fnd_start = input_offset_fnd_start;
		offset_fnd_end = input_offset_fnd_end;
		result_list->count = 0;
		offsetNextTitleSearch = 0;
	}
	if (result_list->result_populated || offset_fnd_start < 0)
		return 0;

	if (search_info[nCurrentWiki].offset_current != offset_fnd_start)
	{
		search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
		search_info[nCurrentWiki].offset_current = offset_fnd_start;
		if (search_interrupted)
		{
			search_interrupted = 7;
			goto interrupted;
		}
		if (search_info[nCurrentWiki].buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
		{
			result_list->result_populated = 1;
			goto out;
		}
	}

	if (!result_list->result_populated)
	{
		pTitleSearch = (TITLE_SEARCH *)&search_info[nCurrentWiki].buf[offsetNextTitleSearch];
		if (offsetNextTitleSearch < search_info[nCurrentWiki].buf_len &&
		    is_proper_string(pTitleSearch->sTitleSearch, search_info[nCurrentWiki].buf_len - offsetNextTitleSearch -
				     sizeof(pTitleSearch->idxArticle) - sizeof(pTitleSearch->cZero)))
		{
			if (!result_list->count)
				retrieve_titles_from_fnd(offset_fnd_start + offsetNextTitleSearch,
							 result_list->title_search[result_list->count], result_list->title[result_list->count]);
			else
				retrieve_titles_from_fnd_ref_prev(offset_fnd_start + offsetNextTitleSearch,
								  result_list->title_search[result_list->count - 1], result_list->title[result_list->count - 1],
								  result_list->title_search[result_list->count], result_list->title[result_list->count]);
			rc = search_string_cmp(result_list->title_search[result_list->count], search_string, search_str_len);
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "bInit %d, input (%x, %x) ", bInit, input_offset_fnd_start, input_offset_fnd_end);
//msg(MSG_INFO, "result count %d, offset (%x + %x, %x) ", result_list->count, offset_fnd_start, offsetNextTitleSearch, offset_fnd_end);
//msg(MSG_INFO, "rc %d, [%s], [%s], [", rc, result_list->title_search[result_list->count], result_list->title[result_list->count]);
//int i;
//for (i=0;i<search_str_len;i++)
//msg(MSG_INFO, "%c", search_string[i]);
//msg(MSG_INFO, "]\n");
//#endif
			if (!rc) // match!
			{
				if (!result_list->count)
				{
					offset_fnd_start =  offset_fnd_start + offsetNextTitleSearch;
					search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
					search_info[nCurrentWiki].offset_current = offset_fnd_start;
					if (search_interrupted)
					{
						search_interrupted = 8;
						goto interrupted;
					}
					if (search_info[nCurrentWiki].buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
					{
						result_list->result_populated = 1;
						goto out;
					}
					offsetNextTitleSearch = 0;
					pTitleSearch = (TITLE_SEARCH *)&search_info[nCurrentWiki].buf[offsetNextTitleSearch];
				}
				memcpy((void *)&result_list->idx_article[result_list->count],
				       (void *)&pTitleSearch->idxArticle, sizeof(long)); // use memcpy to avoid "Unaligned data access"
				result_list->offset_list[result_list->count] = offset_fnd_start + offsetNextTitleSearch;
				offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
					strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
				result_list->offset_next = offset_fnd_start + offsetNextTitleSearch;
				result_list->count++;
				if (result_list->count >= NUMBER_OF_FIRST_PAGE_RESULTS)
				{
					result_list->result_populated = 1;
					goto out;
				}
			}
			else if (rc < 0)
			{
				if (offset_fnd_end <= 0)
				{
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) +
						strlen(&pTitleSearch->sTitleSearch[strlen(pTitleSearch->sTitleSearch) + 1]) + 3;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
				}
				else // binary search
				{
					offset_middle = offset_fnd_start + (offset_fnd_end - offset_fnd_start) / 2; // position to the middle of the range
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "offset_middle %x\n", offset_middle);
//#endif
					if (offset_middle <= offset_fnd_start)
						offset_fnd_end = -1;
					else
					{
						len = copy_fnd_to_buf(offset_middle, buf_middle, sizeof(buf_middle));
						if (search_interrupted)
						{
							search_interrupted = 9;
							goto interrupted;
						}
						if (len >= sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
						{
							pTitleSearch = locate_proper_title_search(buf_middle, len);
							if (pTitleSearch)
							{
								char local_title_search[MAX_TITLE_SEARCH];
								char sTitleActual[MAX_TITLE_SEARCH];

								offset_middle += (char *)pTitleSearch - buf_middle;
								retrieve_titles_from_fnd(offset_middle, local_title_search, sTitleActual);
								rc = search_string_cmp(local_title_search, search_string, search_str_len);
//#ifndef INCLUDED_FROM_KERNEL
//msg(MSG_INFO, "rc %d, [%s], [%s], [", rc, local_title_search, sTitleActual);
//int i;
//for (i=0;i<search_str_len;i++)
//msg(MSG_INFO, "%c", search_string[i]);
//msg(MSG_INFO, "]\n");
//#endif
								if (rc >= 0)	// the first mactch will be in front or at offset_middle
								{
									if (offset_middle == offset_fnd_end) // search from offset_fnd_start directly
										offset_fnd_end = -1;
									else
										offset_fnd_end = offset_middle;
								}
								else		// the first mactch will be after offset_middle
								{
									offset_fnd_start = offset_middle;
									search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
									search_info[nCurrentWiki].offset_current = offset_fnd_start;
									if (search_interrupted)
									{
										search_interrupted = 10;
										goto interrupted;
									}
									if (search_info[nCurrentWiki].buf_len <= 0)
									{
										result_list->result_populated = 1;
										goto out;
									}
									offsetNextTitleSearch = 0;
								}
							}
						}
						else
							offset_fnd_end = -1;
					}
				}
				offsetNextTitleSearch = 0;
			}
			else
			{
				result_list->result_populated = 1;
				goto out;
			}
		}
		else
		{
			offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
			search_info[nCurrentWiki].buf_len = copy_fnd_to_buf(offset_fnd_start, search_info[nCurrentWiki].buf, sizeof(search_info[nCurrentWiki].buf));
			search_info[nCurrentWiki].offset_current = offset_fnd_start;
			if (search_interrupted)
			{
				search_interrupted = 11;
				goto interrupted;
			}
			if (search_info[nCurrentWiki].buf_len <= 0)
			{
				result_list->result_populated = 1;
				goto out;
			}
			offsetNextTitleSearch = 0;
		}
	}
out:
	if (result_list->result_populated)
	{
		if (!bInit) // just completed search result
			search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NORMAL);
		return 0;
	}
	else
	{
		return 1;
	}

interrupted:
	return 1;
}

long get_prefix_index_table(int idx_prefix_index_table)
{
	int idxBlock = idx_prefix_index_table / (SEARCH_CHR_COUNT * SEARCH_CHR_COUNT);

	load_prefix_index(nCurrentWiki);
	if (!search_info[nCurrentWiki].b_prefix_index_block_loaded[idxBlock])
	{
#ifdef INCLUDED_FROM_KERNEL
		if (wl_input_event_pending())
			search_interrupted = 1;
#endif
		wl_seek(search_info[nCurrentWiki].fd_pfx,
			idxBlock * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(uint32_t));
		wl_read(search_info[nCurrentWiki].fd_pfx,
			&(search_info[nCurrentWiki].prefix_index_table[idxBlock * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT]),
			SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(uint32_t));
		search_info[nCurrentWiki].b_prefix_index_block_loaded[idxBlock]++;
	}
	return search_info[nCurrentWiki].prefix_index_table[idx_prefix_index_table];
}

long get_search_result_start()
{
	long offset_search_result_start = -1;
	int idx_prefix_index_table;
	char c1, c2, c3;
	int found = 0;
	int i;
	int lenCompared;
	int lenCopied;
	long offset;
	static int lenHashedSearchString = 0;
	static char sHashedSearchString[MAX_SEARCH_STRING_HASHED_LEN];
	static long offsetHasedSearchString[MAX_SEARCH_STRING_HASHED_LEN];

	if (search_str_len > 3)
	{
		// check the length of the hashed search string can be reused
		if (search_str_len > lenHashedSearchString)
			lenCompared = lenHashedSearchString;
		else
			lenCompared = search_str_len;
		lenHashedSearchString = 0;
		for (i = 0; i < lenCompared; i++)
		{
			if (sHashedSearchString[i] != search_string[i])
				lenHashedSearchString = i;
		}

		// Check if hashed
		if (lenHashedSearchString > 3)
		{
			if (search_str_len > lenHashedSearchString)
			{
				if (search_str_len > MAX_SEARCH_STRING_HASHED_LEN)
					lenCopied = MAX_SEARCH_STRING_HASHED_LEN - lenHashedSearchString;
				else
					lenCopied = search_str_len - lenHashedSearchString;
				memcpy(&sHashedSearchString[lenHashedSearchString], &search_string[lenHashedSearchString], lenCopied);
				// check the extended part first
				for (i = 3; i < lenHashedSearchString + lenCopied; i++)
				{
					if (i >= lenHashedSearchString)
						offsetHasedSearchString[i] = get_search_hash_offset_fnd(sHashedSearchString, i + 1);
					if (search_interrupted)
					{
						search_interrupted = 12;
						goto interrupted;
					}
					if (offsetHasedSearchString[i] &&
					    (i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
					{
						found = 1;
						offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
					}
				}
				lenHashedSearchString += lenCopied;
			}

			if (!found) // not hashed at the extended part
			{
				for (i = 3; i < search_str_len && i < lenHashedSearchString; i++)
				{
					if (offsetHasedSearchString[i] &&
					    (i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
					{
						found = 1;
						offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
					}
					else
						break;
				}
			}
		}
		else
		{
			if (search_str_len > MAX_SEARCH_STRING_HASHED_LEN)
				lenHashedSearchString = MAX_SEARCH_STRING_HASHED_LEN;
			else
				lenHashedSearchString = search_str_len;
			memcpy(sHashedSearchString, search_string, lenHashedSearchString);
			for (i = 3; i < lenHashedSearchString; i++)
			{
				offsetHasedSearchString[i] = get_search_hash_offset_fnd(sHashedSearchString, i + 1);
				if (search_interrupted)
				{
					search_interrupted = 13;
					goto interrupted;
				}
				if (offsetHasedSearchString[i] &&
				    (i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
				{
					found = 1;
					offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
				}
			}
		}
	}

//	if (!found && (3 >= search_str_len || search_str_len > MAX_SEARCH_STRING_ALL_HASHED_LEN))
	if (!found)
	{
		switch(search_str_len)
		{
		case 1:
			c1 = search_string[0];
			c2 = '\0';
			c3 = '\0';
			break;
		case 2:
			c1 = search_string[0];
			c2 = search_string[1];
			c3 = '\0';
			break;
		default:
			c1 = search_string[0];
			c2 = search_string[1];
			c3 = search_string[2];
			break;
		}
		idx_prefix_index_table = bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT +
			bigram_char_idx(c2) * SEARCH_CHR_COUNT + bigram_char_idx(c3);
		if ((offset = get_prefix_index_table(idx_prefix_index_table)))
		{
			found = 1;
			offset_search_result_start = offset;
		}
		if (search_interrupted)
		{
			search_interrupted = 14;
			goto interrupted;
		}
	}

interrupted:
	return offset_search_result_start;
}

int next_search_string(char *local_search_string, int len_local_search_string)
{
	char *pSupportedChars = SUPPORTED_SEARCH_CHARS;
	int idxChar;

	if (len_local_search_string > MAX_SEARCH_STRING_HASHED_LEN)
		len_local_search_string = MAX_SEARCH_STRING_HASHED_LEN;
	if (len_local_search_string > 0)
	{
		idxChar = 0;
		while (idxChar < strlen(pSupportedChars) && pSupportedChars[idxChar] != local_search_string[len_local_search_string - 1])
			idxChar++;
		if (idxChar >= strlen(pSupportedChars) - 1)
		{
			len_local_search_string--;
			len_local_search_string = next_search_string(local_search_string, len_local_search_string);
		}
		else
			local_search_string[len_local_search_string - 1] = pSupportedChars[idxChar + 1];
	}
	return len_local_search_string;
}

long get_search_result_end()
{
	long offset_search_result_end = -1;
	char local_search_string[MAX_TITLE_SEARCH];
	int len_local_search_string;
	int last_len_local_search_string;
	int found = 0;
	int idx_prefix_index_table;
	char c1, c2, c3;
	long offset;

	if (search_str_len  > MAX_SEARCH_STRING_ALL_HASHED_LEN)
		len_local_search_string = MAX_SEARCH_STRING_ALL_HASHED_LEN;
	else
		len_local_search_string = search_str_len;
	memcpy(local_search_string, search_string, len_local_search_string);

	while (!found && len_local_search_string > 3)
	{
		last_len_local_search_string = len_local_search_string;
		len_local_search_string = next_search_string(local_search_string, len_local_search_string);
		if (!memcmp(search_string, local_search_string, len_local_search_string))
			found = 1; // returns -1 directly
		else
		{
			offset_search_result_end = get_search_hash_offset_fnd(local_search_string, len_local_search_string);
			if (search_interrupted)
			{
				search_interrupted = 15;
				goto interrupted;
			}
			if (offset_search_result_end > 0)
			{
				found = 1;
			}
		}
		len_local_search_string = last_len_local_search_string - 1;
	}

	if (!found)
	{
		if (len_local_search_string > 3)
			len_local_search_string = 3;

		while (!found && len_local_search_string > 0)
		{
			last_len_local_search_string = len_local_search_string;
			len_local_search_string = next_search_string(local_search_string, len_local_search_string);
			if (!memcmp(search_string, local_search_string, len_local_search_string))
				found = 1; // returns -1 directly
			else
			{
				switch(len_local_search_string)
				{
				case 1:
					c1 = local_search_string[0];
					c2 = '\0';
					c3 = '\0';
					break;
				case 2:
					c1 = local_search_string[0];
					c2 = local_search_string[1];
					c3 = '\0';
					break;
				default:
					c1 = local_search_string[0];
					c2 = local_search_string[1];
					c3 = local_search_string[2];
					break;
				}
				idx_prefix_index_table = bigram_char_idx(c1) * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT +
					bigram_char_idx(c2) * SEARCH_CHR_COUNT + bigram_char_idx(c3);
				if ((offset = get_prefix_index_table(idx_prefix_index_table)))
				{
					found = 1;
					offset_search_result_end = offset;
				}
				if (search_interrupted)
				{
					search_interrupted = 14;
					goto interrupted;
				}
//				if (search_info[nCurrentWiki].prefix_index_table[idx_prefix_index_table])
//				{
//					found = 1;
//					offset_search_result_end = search_info[nCurrentWiki].prefix_index_table[idx_prefix_index_table];
//				}
			}
			len_local_search_string = last_len_local_search_string - 1;
		}
	}

interrupted:
	return offset_search_result_end;
}

uint32_t get_article_idx_by_title(char *title)
{
	uint32_t article_idx = 0;
	long offset_search_result_start = -1;
	long offset_search_result_end = -1;
	int i = 0;

	search_str_len = 0;
	while (title[i] && search_str_len < MAX_TITLE_SEARCH)
	{
		if (is_supported_search_char(title[i]))
		{
			if ('A' <= title[i] && title[i] <= 'Z')
				search_string[search_str_len++] = 'a' + (title[i] - 'A');
			else
				search_string[search_str_len++] = title[i];
		}
		i++;
	}

	if (search_str_len > 0)
	{
		offset_search_result_start = get_search_result_start();
		if (!search_interrupted && offset_search_result_start > 0)
		{
			offset_search_result_end = get_search_result_end();
			if (!search_interrupted)
				article_idx = get_article_idx_from_offset_range(title, offset_search_result_start, offset_search_result_end);
		}
		search_str_len = 0;
	}

	return article_idx;
}

int search_populate_result()
{
	int found = 0;
	long offset_search_result_start = -1;
	long offset_search_result_end = -1;

	search_string_changed = false;
	result_list->count = 0;
	result_list->result_populated = 0;
	result_list->cur_selected = -1;
	if (search_str_len > 0)
	{
		offset_search_result_start = get_search_result_start();
		if (search_interrupted)
		{
			search_interrupted = 16;
			goto interrupted;
		}
		if (offset_search_result_start > 0)
		{
			found = 1;
			offset_search_result_end = get_search_result_end();
			if (search_interrupted)
			{
				search_interrupted = 17;
				goto interrupted;
			}
			fetch_search_result(offset_search_result_start, offset_search_result_end, 1);
			if (search_interrupted)
			{
				search_interrupted = 18;
				goto interrupted;
			}
		}
		else
		{
			result_list->result_populated = 1;
		}
	}
	return found;

interrupted:
	search_string_changed = true;
	return found;
}

void capitalize(char *in_str, char *out_str, int len)
{
	//char cPrev = ' ';
	int i = 0;

	while (i < len)
	{
		//if (cPrev == ' ' && 'a' <= in_str[i] && in_str[i] <= 'z')
		if (i == 0 && 'a' <= in_str[i] && in_str[i] <= 'z')
			out_str[i] = in_str[i] - 32;
		else
			out_str[i] = in_str[i];
		//cPrev = in_str[i];
		i++;
	}
	out_str[i] = '\0';
}

void search_reload(int flag)
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
		NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos,start_x_search=0;
	int end_y_pos;
	static int last_start_x_search=0;
	char *title;
	char temp_search_string[MAX_TITLE_SEARCH];
	static int bNoResultLastTime = 0;
	int keyboard_mode = keyboard_get_mode();
	char *pMsg;

	more_search_results = 0;
	guilib_fb_lock();
	if (keyboard_mode == KEYBOARD_NONE)
	{
		if (result_list->result_populated || flag == SEARCH_RELOAD_KEEP_REFRESH)
		{
			if (flag == SEARCH_RELOAD_KEEP_RESULT)
				guilib_clear_area(0, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT, 239, LCD_HEIGHT_LINES - 1);
			else
				guilib_clear();
		}
		else
			guilib_clear_area(start_x_search, 0, LCD_BUF_WIDTH_PIXELS, 30);
	}
	else
	{
		if (flag != SEARCH_RELOAD_NO_POPULATE)
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
		if (flag == SEARCH_RELOAD_KEEP_REFRESH)
			guilib_clear_area(0, 0, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		if(search_string_changed_remove)
		{
			if(!search_str_len)
				start_x_search = 0;
			else
				start_x_search = search_string_pos[search_str_len];
			search_string_changed_remove = false;
			if (start_x_search < LCD_BUF_WIDTH_PIXELS)
				guilib_clear_area(start_x_search, 0, LCD_BUF_WIDTH_PIXELS, 30);
			//else
			//{
			//	guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
			//}
		}


//		if (result_list->result_populated)
//			guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
	}

	if (!search_str_len)
	{
		search_string_changed = 0;
		bNoResultLastTime = 0;
		result_list->count = 0;
		result_list->result_populated = 1;
		result_list->cur_selected = -1;
		pMsg = get_nls_text("type_a_word");
		render_string_and_clear(SUBTITLE_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0,
					0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		keyboard_paint();
		goto out;
	}

	capitalize(search_string, temp_search_string, search_str_len);
	if (last_start_x_search >= LCD_BUF_WIDTH_PIXELS)
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
	start_x_search = render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, temp_search_string, strlen(temp_search_string), 0);
	if (last_start_x_search < LCD_BUF_WIDTH_PIXELS && start_x_search >= LCD_BUF_WIDTH_PIXELS)
	{
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
		start_x_search = render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, temp_search_string, strlen(temp_search_string), 0);
	}
	last_start_x_search = start_x_search;
	search_string_pos[search_str_len]=start_x_search;
	y_pos = RESULT_START;


	if (result_list->result_populated && flag != SEARCH_RELOAD_NO_POPULATE)
	{
		unsigned int i;
		unsigned int count = result_list->count < screen_display_count ?
			result_list->count : screen_display_count;
		if (!result_list->count) {
			if (!bNoResultLastTime)
			{
				guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
				pMsg = get_nls_text("no_results");
				render_string(SEARCH_LIST_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0);
				bNoResultLastTime = 1;
			}
			goto out;
		}
		bNoResultLastTime = 0;

		article_link_count = 0;
		for (i = 0; i < screen_display_count; i++)
		{
			end_y_pos = y_pos + RESULT_HEIGHT - 1;
			if (screen_display_count < NUMBER_OF_FIRST_PAGE_RESULTS && end_y_pos > LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1)
				end_y_pos = LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1;
			guilib_clear_area(0, y_pos, 239, end_y_pos);
			if (i < count)
			{
				if (keyboard_mode == KEYBOARD_NONE)
				{
					articleLink[article_link_count].start_xy = (unsigned  long)((y_pos - 2) << 8); // consider the difference between render_string and draw_string
					articleLink[article_link_count].end_xy = (unsigned  long)((LCD_BUF_WIDTH_PIXELS) | ((end_y_pos - 2) << 8));
					articleLink[article_link_count++].article_id = result_list->idx_article[i];
				}
				title = result_list->title[i];
				render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title), 0);
			}
			y_pos += RESULT_HEIGHT;
			if((y_pos+RESULT_HEIGHT)>guilib_framebuffer_height())
				break;
		}
		if (keyboard_mode == KEYBOARD_NONE)
		{
			if (result_list->count == NUMBER_OF_FIRST_PAGE_RESULTS)
			{
				more_search_results = 1;
			}
		}
	}
out:
	guilib_fb_unlock();
}

void search_to_be_reloaded(int to_be_reloaded_flag, int reload_flag)
{
	static bool to_be_reloaded = false;
	static int saved_reload_flag;

	switch (to_be_reloaded_flag)
	{
	case SEARCH_TO_BE_RELOADED_CLEAR:
		to_be_reloaded = false;
		break;
	case SEARCH_TO_BE_RELOADED_SET:
		if (reload_flag == SEARCH_RELOAD_NORMAL && keyboard_key_inverted())
		{
			to_be_reloaded = true;
			saved_reload_flag = reload_flag;
		}
		else
		{
			search_reload(reload_flag);
			to_be_reloaded = false;
		}
		break;
	case SEARCH_TO_BE_RELOADED_CHECK:
		if (to_be_reloaded && !keyboard_key_inverted())
		{
			search_reload(saved_reload_flag);
			to_be_reloaded = false;
		}
		break;
	default:
		break;
	}
}

void search_result_display()
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
		NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos=0;
	char *title;
	char *pMsg;

	guilib_fb_lock();
	guilib_clear_area(0, RESULT_START, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);

	if (!search_str_len)
	{
		result_list->count = 0;
		result_list->cur_selected = -1;
		pMsg = get_nls_text("type_a_word");
		render_string_and_clear(SUBTITLE_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0,
					0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		keyboard_paint();
		goto out;
	}


	y_pos = RESULT_START;

	if (result_list->result_populated && !result_list->count) {
		pMsg = get_nls_text("no_results");
		render_string(SEARCH_LIST_FONT_IDX, -1, 55, pMsg, strlen(pMsg), 0);
		goto out;
	}

	if (result_list->count) {
		unsigned int i;
		unsigned int count = result_list->count < screen_display_count ?
			result_list->count : screen_display_count;

		for (i = 0; i < count; i++) {
			title = result_list->title[i];
			render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title), 0);
			y_pos += RESULT_HEIGHT;
			if((y_pos+RESULT_HEIGHT)>guilib_framebuffer_height())
				break;
		}
		if (result_list->cur_selected >= screen_display_count)
			result_list->cur_selected = screen_display_count - 1;
		invert_selection(result_list->cur_selected, -1, RESULT_START, RESULT_HEIGHT);
	}
out:
	guilib_fb_unlock();
}

int search_add_char(char c, unsigned long ev_time)
{
	(void)ev_time;
	if((c == 0x20 && search_str_len>0 && search_string[search_str_len-1] == 0x20) ||
	   (search_str_len >= MAX_TITLE_SEARCH - 2) ||
	   (!search_str_len && c == 0x20))
		return -1;

	if (!search_str_len) // clear type_a_word message
	{
		guilib_fb_lock();
		guilib_clear_area(0, 55, 239, 80);
		guilib_fb_unlock();
	}
	if ('A' <= c && c <= 'Z')
		c += 32;
	search_string[search_str_len++] = c;
	search_string[search_str_len] = '\0';
	time_search_string_changed = get_time_ticks();
	search_string_changed = true;
	return 0;
}

int check_search_string_change(void)
{
	if (!search_string_changed)
		return 0;

	if (time_diff(get_time_ticks(), time_search_string_changed) > seconds_to_ticks(DELAYED_SEARCH_TIME))
	{
		search_interrupted = 0;
		search_populate_result();
		if (search_interrupted)
			goto interrupted;
		result_list->cur_selected = -1;
		if (result_list->result_populated)
			search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NORMAL);
	}
	return 1;

interrupted:
	return 1;
}

void search_fetch()
{
	search_populate_result();
	result_list->cur_selected = -1;
	search_string_changed = false;
}

/*
 * return value - 0: remove ok, -1: no key to remove
 */
int search_remove_char(int bPopulate, unsigned long ev_time)
{
	(void)ev_time;
	if (search_str_len == 0)
		return -1;

	search_string[--search_str_len] = '\0';

	if (bPopulate || !search_str_len)
	{
		search_populate_result();
		search_string_changed_remove = true;
		result_list->cur_selected = -1;
	}
	else
	{
		search_string_changed = true;
		search_string_changed_remove = true;
		time_search_string_changed = get_time_ticks();
	}
	return 0;
}

TITLE_SEARCH *locate_previous_title(char *buf, int len)
{
	char *p;
	int bFound = 0;

	if (len > 2)
	{
		p = buf + len - 2;
		len--;
		while (!bFound && len > sizeof(long))
		{
			if (!*p) {
				bFound = 1;
				p -= sizeof(long);
			}
			else
			{
				p--;
				len--;
			}
		}
		if (bFound)
			return (TITLE_SEARCH *)p;
	}
	return NULL;
}

int search_current_selection(void)
{
//	const char* title;

//	if (result_list->cur_selected >= result_list->count - result_list->first_item) {
//		DP(DBG_SEARCH, ("O search_current_title() NO TITLE\n"));
//		return NULL;
//	}
//	title = result_list_title(result_list->cur_selected+result_list->first_item);
//	DP(DBG_SEARCH, ("O search_current_title() '%s'\n", title));
	return result_list->cur_selected;
}

unsigned int search_result_count()
{
	return result_list->count;
}

int clear_search_string()
{
	if (search_str_len == 0)
		return -1;

	result_list->count = 0;
	strcpy(search_string,"");
	search_str_len = 0;
	return 0;
}

int get_search_string_len()
{
	return search_str_len;
}

int search_result_selected()
{
	return result_list->cur_selected;
}

extern unsigned char *file_buffer;
extern int restricted_article;
extern int current_article_wiki_id;

char *compressed_buf = NULL;
int retrieve_article(long idx_article_with_wiki_id)
{
	ARTICLE_PTR article_ptr;
	long idx_article;
	int nWikiIdx;

	if (!compressed_buf)
		compressed_buf = (char *)malloc_simple(MAX_COMPRESSED_ARTICLE, MEM_TAG_INDEX_M1);

	current_article_wiki_id = (unsigned long)idx_article_with_wiki_id >> 24;
	if (current_article_wiki_id == 0)
		nWikiIdx = nCurrentWiki;
	else
	{
		nWikiIdx = get_wiki_idx_from_id(current_article_wiki_id );
		if (nWikiIdx >= 0)
			load_prefix_index(nWikiIdx);
		else
		{
			print_article_error();
			return -1;
		}
	}

	idx_article = idx_article_with_wiki_id & 0x00FFFFFF;

	if (nWikiIdx >= 0 && compressed_buf && 0 < idx_article && idx_article <= search_info[nWikiIdx].max_article_idx) {
		wl_seek(search_info[nWikiIdx].fd_idx, sizeof(uint32_t) + (idx_article - 1) * sizeof(article_ptr));
		wl_read(search_info[nWikiIdx].fd_idx, &article_ptr, sizeof(article_ptr));

		int dat_file_id = article_ptr.file_id;

		if (search_info[nWikiIdx].fd_dat[dat_file_id] < 0)
		{
			char file_name[13];
			sprintf(file_name, "wiki%d.dat", dat_file_id);
			search_info[nWikiIdx].fd_dat[dat_file_id] = wl_open(get_wiki_file_path(nWikiIdx, file_name),
									    WL_O_RDONLY);
		}
		if (search_info[nWikiIdx].fd_dat[dat_file_id] >= 0)
		{
			CONCAT_ARTICLE_INFO concat_article_infos[MAX_ARTICLES_PER_COMPRESSION];
			uint8_t nArticlesConcatnated;
			uint32_t dat_article_len;

			wl_seek(search_info[nWikiIdx].fd_dat[dat_file_id], article_ptr.offset_dat & 0x7FFFFFFF);

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], &nArticlesConcatnated,
				sizeof(nArticlesConcatnated));

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], &concat_article_infos,
				nArticlesConcatnated * sizeof(CONCAT_ARTICLE_INFO));

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], &dat_article_len, sizeof(dat_article_len));

			wl_read(search_info[nWikiIdx].fd_dat[dat_file_id], compressed_buf, dat_article_len);

			dat_article_len -= LZMA_PROPS_SIZE;

			ELzmaStatus status;
			SizeT file_buffer_len = FILE_BUFFER_SIZE;
			SizeT compressed_buffer_len = dat_article_len;
			int rc = (int)LzmaDecode(file_buffer,
						 &file_buffer_len,
						 (const Byte *)compressed_buf + LZMA_PROPS_SIZE,
						 &compressed_buffer_len,
						 (const Byte *)compressed_buf, LZMA_PROPS_SIZE,
						 LZMA_FINISH_ANY,
						 &status, &g_Alloc);

			if (rc == SZ_OK || rc == SZ_ERROR_INPUT_EOF) // can generate SZ_ERROR_INPUT_EOF but result is OK
			{
				int i;
				for (i = 0; i < nArticlesConcatnated; i++)
				{
					if (concat_article_infos[i].article_id == idx_article)
					{
						if (concat_article_infos[i].offset_article & 0x80000000) {
							restricted_article = 1;
						} else {
							restricted_article = 0;
						}
						uint32_t offset = concat_article_infos[i].offset_article & ~0x80000000;
						memcpy(file_buffer, &file_buffer[offset], concat_article_infos[i].article_len);
						file_buffer[concat_article_infos[i].article_len] = '\0';
						return 0;
					}
				}
			}
		}

	}
	print_article_error();
	return -1;
}

void search_set_selection(int new_selection)
{

	result_list->cur_selected = new_selection;
}
void search_open_article(int new_selection)
{
	int list_idx;

	list_idx = new_selection;
	if (list_idx >= NUMBER_OF_FIRST_PAGE_RESULTS)
		list_idx -= NUMBER_OF_FIRST_PAGE_RESULTS;
	display_link_article(result_list->idx_article[list_idx]);
}

long find_closest_idx(long idx, char *title)
{
	ARTICLE_PTR article_ptr;
	TITLE_SEARCH title_search;
	static int count = 0;
	char sTitleSearch[MAX_TITLE_SEARCH];

	title[0] = '\0';

	if (idx > search_info[nCurrentWiki].max_article_idx)
		idx -= search_info[nCurrentWiki].max_article_idx;
	wl_seek(search_info[nCurrentWiki].fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + sizeof(uint32_t));
	wl_read(search_info[nCurrentWiki].fd_idx, &article_ptr, sizeof(article_ptr));

	if (!article_ptr.offset_fnd)
	{
		if (count < 10)
		{
			count++;
			return find_closest_idx(idx + 1, title);
		}
		else
		{
			count = 0;
			return 0;
		}
	}
	else
	{
		count = 0;
		copy_fnd_to_buf(article_ptr.offset_fnd, (char *)&title_search, sizeof(title_search));
		retrieve_titles_from_fnd(article_ptr.offset_fnd, sTitleSearch, title);
		title[MAX_TITLE_SEARCH - 1] = '\0';
		return idx;
	}
}

#if !defined(INCLUDED_FROM_KERNEL)
#include <time.h>
#include <sys/time.h>
#endif

extern int last_display_mode;
void random_article(void)
{
	long idx_article;
	char title[MAX_TITLE_SEARCH];
	unsigned long clock_ticks;

#if defined(INCLUDED_FROM_KERNEL)
	clock_ticks = Tick_get();
#else
	//clock_ticks = clock();  // this gets CPU time used, it not suitable for random
	struct timeval t;         // get elapsed time in us but do not care about overflow
	gettimeofday(&t, NULL);   // this more-or-less simulates what happens on WR hardware
	clock_ticks = t.tv_sec * 1000000 + t.tv_usec;
#endif
	idx_article = clock_ticks % search_info[nCurrentWiki].max_article_idx + 1;
	idx_article = find_closest_idx(idx_article, title);

	if (idx_article)
	{
		last_display_mode = DISPLAY_MODE_INDEX; // for history_save not to log the last article index
		display_link_article(idx_article);
	}
}
