//
// Authors:	Holger Hans Peter Freyther <zecke@openmoko.org>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#include "search.h"
#include "msg.h"
#include "wl-keyboard.h"
#include <stdlib.h>
#include <string.h>
#include <guilib.h>
#include <glyph.h>
#include <lcd.h>
#include <file-io.h>
#include <malloc-simple.h>
#include "bzlib.h"
#include "minilzo.h"
#include "Alloc.h"
#include "Bra.h"
#include "LzmaDec.h"
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "bigram.h"
#include "history.h"
#include "search_hash.h"
#include <tick.h>
#include <input.h>

#define DBG_SEARCH 0

unsigned int get_time_search(void);
unsigned int time_search_last = 0;
bool search_string_changed = false;
bool search_string_changed_remove = false;

typedef struct _search_results {
	char list[MAX_RESULTS][MAX_TITLE_SEARCH];
	long idx_article[MAX_RESULTS];  // index (pedia.idx) for loading the article 
	long offset_list[MAX_RESULTS];	// offset (pedia.fnd) of each search title in list
	long offset_next;		// offset (pedia.fnd) of the next title after the list
	unsigned int count;
	int base;		// the starting list index for the circular list. it is also the first item displayed
	int result_populated;
	int cur_selected;		// -1 when no selection.
//	unsigned int first_item;	// Index of the first item displayed on the list. 0 based.
} SEARCH_RESULTS;
static SEARCH_RESULTS *result_list;

typedef struct _search_info {
	int fd_pfx;
	int fd_idx;
	int fd_dat[MAX_DAT_FILES];
	long max_article_idx;
	long prefix_index_table[SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT];
	int b_prefix_index_block_loaded[SEARCH_CHR_COUNT];
	char buf[MAX_RESULTS * sizeof(TITLE_SEARCH)];	// buf correspond to result_list
	int buf_len;
	long offset_current;		// offset (pedia.fnd) of the content of buffer
} SEARCH_INFO;
static SEARCH_INFO *search_info;

#define SIZE_PREFIX_INDEX_TABLE SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(long)
//static struct search_state state;
//static struct search_state last_first_hit;

static char search_string[MAX_TITLE_SEARCH];
static int  search_string_pos[MAX_TITLE_SEARCH];
static int search_str_len = 0;

//static char s_find_first = 1;
static char prefix_index_loaded = 0;

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

void get_article_title_from_idx(long idx, char *title)
{
	ARTICLE_PTR article_ptr;
	TITLE_SEARCH title_search;
	
	title[0] = '\0';
	wl_seek(search_info->fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + 4);
	wl_read(search_info->fd_idx, (void *)&article_ptr, sizeof(article_ptr));
	if (article_ptr.file_id_compressed_len && article_ptr.offset_fnd)
	{
		copy_fnd_to_buf(article_ptr.offset_fnd, (char *)&title_search, sizeof(title_search));
		bigram_decode(title, title_search.sTitleSearch);
		title[MAX_TITLE_SEARCH - 1] = '\0';
	}
}

int load_prefix_index(void)
{
	int i;
	
	if (!prefix_index_loaded)
	{
		search_info->fd_pfx = wl_open("pedia.pfx", WL_O_RDONLY);
		search_info->fd_idx = wl_open("pedia.idx", WL_O_RDONLY);
		for (i=0; i < MAX_DAT_FILES; i++)
			search_info->fd_dat[i] = -1;
		search_info->offset_current = -1;
		if (search_info->fd_pfx >= 0 && search_info->fd_idx >= 0)
		{
			wl_read(search_info->fd_idx, (void *)&search_info->max_article_idx, sizeof(search_info->max_article_idx));
			memset((char *)search_info->b_prefix_index_block_loaded, 0, sizeof(search_info->b_prefix_index_block_loaded));
			prefix_index_loaded = 1;
		}
	}
	return prefix_index_loaded;
}

void search_init()
{
	result_list = (SEARCH_RESULTS *)malloc_simple(sizeof(SEARCH_RESULTS), MEM_TAG_INDEX_M1);
	search_info = (SEARCH_INFO *)malloc_simple(sizeof(SEARCH_INFO), MEM_TAG_INDEX_M1);
	int result = load_prefix_index();
	if (!result) {
		msg(MSG_ERROR, "Failed to initialize search.\n");
		/* XXX, FIXME, handle the error */
	}
	result_list->count = 0;
	result_list->base = 0;
//	result_list->first_item = 0;
	result_list->cur_selected = 0;
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
	guilib_fb_lock();
	guilib_clear();
	render_string(SEARCH_LIST_FONT_IDX, -1, 104, "Opening the article failed.", 27);
render_string(SEARCH_LIST_FONT_IDX, -1, 124, article_error, strlen(article_error));
render_string(SEARCH_LIST_FONT_IDX, -1, 144, article_error2, strlen(article_error2));
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

char *locate_double_null(char *pBuf, int len)
{
	int i = 0;
	int bLastCharNull = 0;
	int bFound = 0;
	
	while (!bFound && i < len)
	{
		if (pBuf[i])
			bLastCharNull = 0;
		else
		{
			if (bLastCharNull)
				bFound = 1;
			else
				bLastCharNull = 1;
		}
		i++;
	}
	if (bFound)
		return &pBuf[i-1];
	else
		return NULL;
}

TITLE_SEARCH *locate_proper_title_search(char *buf_middle, int len)
{
	TITLE_SEARCH *pTitleSearch;
	char *p;
	char *pBuf = buf_middle;
	
	// locate the bouble null character for the end of TITLE_SEARCH.idxArticle and TITLE_SEARCH.cZero
	// the last (most significant) byte of idxArticle should always be 0
	while (len > 0 && (p = locate_double_null(pBuf, len)))
	{
		len -= p - pBuf;
		while (len > 0 && !(*p))
		{
			p++;
			len--;
		}
		if (p - pBuf >= sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero))
		{
			pTitleSearch = (TITLE_SEARCH *)(p - (sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero)));
			if (is_proper_string(pTitleSearch->sTitleSearch, len))
				return pTitleSearch;
		}
		pBuf = p;
	}
	
	return NULL;
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
		result_list->base = 0;
		result_list->cur_selected = 0;
		offsetNextTitleSearch = 0;
	}
	if (result_list->result_populated || offset_fnd_start < 0)
		return 0;
	
	if (search_info->offset_current != offset_fnd_start)
	{
		search_info->buf_len = copy_fnd_to_buf(offset_fnd_start, search_info->buf, sizeof(search_info->buf));
		search_info->offset_current = offset_fnd_start;
		if (search_info->buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
		{
			result_list->result_populated = 1;
			goto out;
		}
	}
	
	if (!result_list->result_populated)
	{
		pTitleSearch = (TITLE_SEARCH *)&search_info->buf[offsetNextTitleSearch];
		if (offsetNextTitleSearch < search_info->buf_len &&
			is_proper_string(pTitleSearch->sTitleSearch, search_info->buf_len - offsetNextTitleSearch - 
			sizeof(pTitleSearch->idxArticle) - sizeof(pTitleSearch->cZero)))
		{
			bigram_decode(result_list->list[result_list->count], pTitleSearch->sTitleSearch);
			rc = search_string_cmp(result_list->list[result_list->count], search_string, search_str_len);
#ifndef INCLUDED_FROM_KERNEL
msg(MSG_INFO, "rc %d, [%s], [", rc, result_list->list[result_list->count]);
int i;
for (i=0;i<search_str_len;i++)
msg(MSG_INFO, "%c", search_string[i]);
msg(MSG_INFO, "]\n");
#endif
			if (!rc) // match!
			{
				if (!result_list->count)
				{
					offset_fnd_start =  offset_fnd_start + offsetNextTitleSearch;
					search_info->buf_len = copy_fnd_to_buf(offset_fnd_start, search_info->buf, sizeof(search_info->buf));
					search_info->offset_current = offset_fnd_start;
					if (search_info->buf_len < sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
					{
						result_list->result_populated = 1;
						goto out;
					}
					offsetNextTitleSearch = 0;
					pTitleSearch = (TITLE_SEARCH *)&search_info->buf[offsetNextTitleSearch];
				}
				memcpy((void *)&result_list->idx_article[result_list->count], 
					(void *)&pTitleSearch->idxArticle, sizeof(long)); // use memcpy to avoid "Unaligned data access"
				result_list->offset_list[result_list->count] = offset_fnd_start + offsetNextTitleSearch;
				offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) + 2;
				result_list->offset_next = offset_fnd_start + offsetNextTitleSearch;
				result_list->count++;
				if (result_list->count >= MAX_RESULTS)
				{
					result_list->result_populated = 1;
					goto out;
				}
			}
			else if (rc < 0)
			{
				if (offset_fnd_end <= 0)
				{
					offsetNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) + 2;
					offset_fnd_start = offset_fnd_start + offsetNextTitleSearch;
				}
				else // binary search
				{
					offset_middle = offset_fnd_start + (offset_fnd_end - offset_fnd_start) / 2 - MAX_TITLE_SEARCH; // position to the middle of the range - max length of title
					if (offset_middle <= offset_fnd_start)
						offset_fnd_end = -1;
					else
					{
						len = copy_fnd_to_buf(offset_middle, buf_middle, sizeof(buf_middle));
						if (len >= sizeof(pTitleSearch->idxArticle) + sizeof(pTitleSearch->cZero) + 2) // at lease 2 chars for pTitleSearch->sTitleSearch
						{
							pTitleSearch = locate_proper_title_search(buf_middle, len);
							if (pTitleSearch)
							{
								char local_title_search[MAX_TITLE_SEARCH];
								
								offset_middle += (char *)pTitleSearch - buf_middle;
								bigram_decode(local_title_search, pTitleSearch->sTitleSearch);
								rc = search_string_cmp(local_title_search, search_string, search_str_len);
								if (rc >= 0)	// the first mactch will be in front or at offset_middle
								{
									offset_fnd_end = offset_middle;
								}
								else		// the first mactch will be after offset_middle
								{
									offset_fnd_start = offset_middle;
									search_info->buf_len = copy_fnd_to_buf(offset_fnd_start, search_info->buf, sizeof(search_info->buf));
									search_info->offset_current = offset_fnd_start;
									if (search_info->buf_len <= 0)
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
			search_info->buf_len = copy_fnd_to_buf(offset_fnd_start, search_info->buf, sizeof(search_info->buf));
			search_info->offset_current = offset_fnd_start;
			if (search_info->buf_len <= 0)
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
			search_reload_ex(SEARCH_RELOAD_NORMAL);
		return 0;
	}
	else
	{
//		if (bInit)
//		{
//			guilib_fb_lock();
//			render_string(SEARCH_LIST_FONT_IDX, -1, 55, MESSAGE_SEARCHING, strlen(MESSAGE_SEARCHING));
//			guilib_fb_unlock();
//		}
		return 1;
	}
}

long get_prefix_index_table(int idx_prefix_index_table)
{
	int idxBlock = idx_prefix_index_table / (SEARCH_CHR_COUNT * SEARCH_CHR_COUNT);
	
	if (!search_info->b_prefix_index_block_loaded[idxBlock])
	{
		wl_seek(search_info->fd_pfx, idxBlock * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(long));
		wl_read(search_info->fd_pfx, (void *)&(search_info->prefix_index_table[idxBlock * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT]), 
			SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * sizeof(long));
		search_info->b_prefix_index_block_loaded[idxBlock]++;
	}
	return search_info->prefix_index_table[idx_prefix_index_table];
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
				if (offsetHasedSearchString[i] && 
					(i >= MAX_SEARCH_STRING_ALL_HASHED_LEN || i == search_str_len - 1))
				{
					found = 1;
					offset_search_result_start = offsetHasedSearchString[i]; // use the longest hashed search string
				}
			}
		}
	}

	if (!found && (3 >= search_str_len || search_str_len > MAX_SEARCH_STRING_ALL_HASHED_LEN))
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
	}

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
				if (search_info->prefix_index_table[idx_prefix_index_table])
				{
					found = 1;
					offset_search_result_end = search_info->prefix_index_table[idx_prefix_index_table];
				}
			}
			len_local_search_string = last_len_local_search_string - 1;
		}
	}

	return offset_search_result_end;
}

int search_populate_result()
{
	int found = 0;
	long offset_search_result_start = -1;
	long offset_search_result_end = -1;

	result_list->count = 0;
	result_list->base = 0;
	result_list->cur_selected = 0;
	if (search_str_len > 0)
	{
		offset_search_result_start = get_search_result_start();
		if (offset_search_result_start > 0)
		{
			found = 1;
			offset_search_result_end = get_search_result_end();
			fetch_search_result(offset_search_result_start, offset_search_result_end, 1);
		}
		else
		{
			result_list->result_populated = 1;
		}
	}
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

void search_reload()
{
//	static const char search_result_str[] = "Search:";
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
//	int available_count;
	int y_pos;
//	const char * result;
	char *title;
//        int render_len;
	char temp_search_string[MAX_TITLE_SEARCH];

//	DP(DBG_SEARCH, ("O search_reload() start: screen_display_count %u cur_selected %d first_item %u\n", screen_display_count, result_list->cur_selected, result_list->first_item));
	guilib_fb_lock();
	if (keyboard_get_mode() == KEYBOARD_NONE)
		guilib_clear();
	else
		guilib_clear_area(0, 0, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
//	render_len = render_string(SEARCH_HEADING_FONT_IDX, 5, 5, search_result_str, strlen(search_result_str));

	if (!search_str_len)
	{
		result_list->count = 0;
		result_list->cur_selected = -1;
		render_string(SUBTITLE_FONT_IDX, -1, 55, MESSAGE_TYPE_A_WORD, strlen(MESSAGE_TYPE_A_WORD));
		goto out;
	}

	//render_string(4, 87, 10, search_string, strlen(search_string));
	capitalize(search_string, temp_search_string, search_str_len);
	if (search_str_len > 1 && search_string_pos[search_str_len - 1] >= LCD_BUF_WIDTH_PIXELS)
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
	render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, 6, temp_search_string, strlen(temp_search_string));
	y_pos = RESULT_START;

//	int found = 0;
//	available_count = result_list->count - result_list->first_item;
//	while (available_count < screen_display_count &&
//		result_list->count < MAX_RESULTS &&
//		(result = search_fetch_result())) {
//		len = strlen(result);
//		memcpy(&result_list->list[result_list->count], result, len);
//		result_list->list[result_list->count][len] = 0;
//		++result_list->count;
//		++available_count;
//		found = 1;
//	}

	if (!result_list->count) {
//		result_list->cur_selected = -1;
//		result_list->first_item = 0;
		render_string(SEARCH_LIST_FONT_IDX, -1, 55, MESSAGE_NO_RESULTS, strlen(MESSAGE_NO_RESULTS));
		goto out;
	}

//	if (!found && result_list->first_item && result_list->first_item+result_list->cur_selected >= result_list->count) {
//		--result_list->first_item;
//		++available_count;
//	}
	if (result_list->count) {
		unsigned int i, j;
		unsigned int count = result_list->count < screen_display_count ?
					result_list->count : screen_display_count;

		for (i = 0; i < count; i++) {
			j = i + result_list->base;
			if (j >= MAX_RESULTS)
				j -= MAX_RESULTS;
			title = result_list->list[j];
			render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title));
			DP(DBG_SEARCH, ("O result[%d] '%s'\n", j, title));
			y_pos += RESULT_HEIGHT;
                        if((y_pos+RESULT_HEIGHT)>guilib_framebuffer_height())
                           break;
		}
		if (result_list->cur_selected >= screen_display_count)
			result_list->cur_selected = screen_display_count - 1;
		//invert_selection(result_list->cur_selected, -1, PIXEL_START, RESULT_HEIGHT);
		invert_selection(result_list->cur_selected, -1, RESULT_START, RESULT_HEIGHT);
	}
out:
//	DP(DBG_SEARCH, ("O search_reload() end: screen_display_count %u cur_selected %d first_item %u\n", screen_display_count, result_list->cur_selected, result_list->first_item));
	guilib_fb_unlock();
}
void search_reload_ex(int flag)
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos,start_x_search=0;
	int end_y_pos;
	static int last_start_x_search=0;
	char *title;
	char temp_search_string[MAX_TITLE_SEARCH];
	static int bNoResultLastTime = 0;


	guilib_fb_lock();
	if (keyboard_get_mode() == KEYBOARD_NONE)
        {
		if (result_list->result_populated)
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
		bNoResultLastTime = 0;
		guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
		result_list->count = 0;
		result_list->cur_selected = -1;
		render_string(SUBTITLE_FONT_IDX, -1, 55, MESSAGE_TYPE_A_WORD, strlen(MESSAGE_TYPE_A_WORD));
		goto out;
	}

	capitalize(search_string, temp_search_string, search_str_len);
	if (last_start_x_search >= LCD_BUF_WIDTH_PIXELS)
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
	start_x_search = render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, 6, temp_search_string, strlen(temp_search_string));
	if (last_start_x_search < LCD_BUF_WIDTH_PIXELS && start_x_search >= LCD_BUF_WIDTH_PIXELS)
	{	
		guilib_clear_area(0, 0, LCD_BUF_WIDTH_PIXELS, 30);
		start_x_search = render_string_right(SEARCH_HEADING_FONT_IDX, LCD_LEFT_MARGIN, 6, temp_search_string, strlen(temp_search_string));
	}
	last_start_x_search = start_x_search;
        search_string_pos[search_str_len]=start_x_search;
	y_pos = RESULT_START;


	if (result_list->result_populated && flag != SEARCH_RELOAD_NO_POPULATE)
	{
		if (result_list->result_populated && !result_list->count) {
			if (!bNoResultLastTime)
			{
				guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
				render_string(SEARCH_LIST_FONT_IDX, -1, 55, MESSAGE_NO_RESULTS, strlen(MESSAGE_NO_RESULTS));
				bNoResultLastTime = 1;
			}
			goto out;
		}
		bNoResultLastTime = 0;
	
		if (result_list->result_populated) {
			unsigned int i, j;
			unsigned int count = result_list->count < screen_display_count ?
						result_list->count : screen_display_count;
	
			for (i = 0; i < screen_display_count; i++)
			{
				end_y_pos = y_pos + RESULT_HEIGHT - 1;
				if (end_y_pos > LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1)
					 end_y_pos = LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1;
				guilib_clear_area(0, y_pos, 239, end_y_pos);
				if (i < count)
				{
					j = i + result_list->base;
					if (j >= MAX_RESULTS)
						j -= MAX_RESULTS;
					title = result_list->list[j];
					render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title));
					DP(DBG_SEARCH, ("O result[%d] '%s'\n", j, title));
	                        }
				y_pos += RESULT_HEIGHT;
		                if((y_pos+RESULT_HEIGHT)>guilib_framebuffer_height())
		                	break;
			}
			if (result_list->cur_selected >= screen_display_count)
				result_list->cur_selected = screen_display_count - 1;
			invert_selection(result_list->cur_selected, -1, RESULT_START, RESULT_HEIGHT);
		}
	}
out:
	guilib_fb_unlock();
}
void search_result_display()
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos=0;
	char *title;

	guilib_fb_lock();
	guilib_clear_area(0, RESULT_START, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);

	if (!search_str_len)
	{
		result_list->count = 0;
		result_list->cur_selected = -1;
		render_string(SUBTITLE_FONT_IDX, -1, 55, MESSAGE_TYPE_A_WORD, strlen(MESSAGE_TYPE_A_WORD));
		goto out;
	}


	y_pos = RESULT_START;

	if (!result_list->count) {
		render_string(SEARCH_LIST_FONT_IDX, -1, 55, MESSAGE_NO_RESULTS, strlen(MESSAGE_NO_RESULTS));
		goto out;
	}

	if (result_list->count) {
		unsigned int i, j;
		unsigned int count = result_list->count < screen_display_count ?
					result_list->count : screen_display_count;

		for (i = 0; i < count; i++) {
			j = i + result_list->base;
			if (j >= MAX_RESULTS)
				j -= MAX_RESULTS;
			title = result_list->list[j];
			render_string(SEARCH_LIST_FONT_IDX, LCD_LEFT_MARGIN, y_pos, title, strlen(title));
			DP(DBG_SEARCH, ("O result[%d] '%s'\n", j, title));
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

int search_add_char(char c)
{
        if(c == 0x20 && search_str_len>0 && search_string[search_str_len-1] == 0x20)
                return -1;
	if (search_str_len >= MAX_TITLE_SEARCH - 2)
		return -1;

	if (!search_str_len && c == 0x20)
		return -1;

	if ('A' <= c && c <= 'Z')
		c += 32;
	search_string[search_str_len++] = c;
	search_string[search_str_len] = '\0';
//	if (search_str_len >= 4) {
//		state.pattern_len = search_str_len;
//		if (!s_find_first) // already found something before?
//			reset_state(&global_search, &state, &last_first_hit);
//	} else
//		prepare_search(&global_search, search_string, &state);
//	s_find_first = 1;
//	result_list->cur_selected = -1;
//	result_list->first_item = 0;
//	result_list->count = 0;
         #ifdef INCLUDED_FROM_KERNEL
        time_search_last=get_time_search();
        //return;
        #endif
        search_string_changed = true;
	search_populate_result();
        result_list->cur_selected = -1;
        return 0;
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
int search_remove_char(int bPopulate)
{
	DP(DBG_SEARCH, ("O search_remove_char() search_str_len %d\n", search_str_len));
	if (search_str_len == 0)
		return -1;

	search_string[--search_str_len] = '\0';
//	memset(&state, 0, sizeof(state));
//	s_find_first = 1;
//	prepare_search(&global_search, search_string, &state);
//	result_list->cur_selected = -1;
//	result_list->first_item = 0;
//	result_list->count = 0;

         #ifdef INCLUDED_FROM_KERNEL
        time_search_last=get_time_search();
        //return;
        #endif
        search_string_changed = true;
        search_string_changed_remove = true;
	if (bPopulate || !search_str_len)
		search_populate_result();
        result_list->cur_selected = -1;
        return 0;
}

int result_list_down(int nRows)
{
	int nRowsToMove = 0;
	int bDone;
	TITLE_SEARCH *pTitleSearch;
	char sTempTitle[MAX_TITLE_SEARCH];
	int idx;
	
	if (nRows >= result_list->count)
		nRows = result_list->count - 1;
	if (nRows > 0)
	{
		nRowsToMove = nRows;
		bDone = 0;
		while (nRowsToMove >= 0 && !bDone)
		{
			if (result_list->offset_next - search_info->offset_current + sizeof(TITLE_SEARCH) < search_info->buf_len)
			{
				pTitleSearch = (TITLE_SEARCH *)&search_info->buf[result_list->offset_next - search_info->offset_current];
				bigram_decode(sTempTitle, pTitleSearch->sTitleSearch);
				if (search_string_cmp(sTempTitle, search_string, search_str_len))
				{
					result_list->base++;
					if (result_list->base >= MAX_RESULTS)
						result_list->base = 0;
					idx = result_list->base + result_list->count;
					if (idx >= MAX_RESULTS)
						result_list->base -= MAX_RESULTS;
					
					result_list->idx_article[idx] = pTitleSearch->idxArticle;
					result_list->offset_list[idx] = result_list->offset_next;
					result_list->offset_next += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch);
					nRowsToMove--;
				}
				else
					bDone = 1;
			}
			else
				fetch_search_result(result_list->offset_list[result_list->base], -1, 1);
		}
	}
	return (nRows - nRowsToMove);
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
int result_list_up(int nRows)
{
	int nRowsToMove = 0;
	int bDone;
	TITLE_SEARCH *pTitleSearch;
	char sTempTitle[MAX_TITLE_SEARCH];
	int idx;
	
	if (nRows >= result_list->count)
		nRows = result_list->count - 1;
	if (nRows > 0)
	{
		nRowsToMove = nRows;
		bDone = 0;
		while (nRowsToMove >= 0 && !bDone)
		{
			if ((pTitleSearch = locate_previous_title(search_info->buf, 
				result_list->offset_list[result_list->base] - search_info->offset_current)) != NULL)
			{
				bigram_decode(sTempTitle, pTitleSearch->sTitleSearch);
				if (search_string_cmp(sTempTitle, search_string, search_str_len))
				{
					if (result_list->count < MAX_RESULTS - 1)
						result_list->count++;
					else
					{
						idx = result_list->base + result_list->count;
						if (idx >= MAX_RESULTS)
							result_list->base -= MAX_RESULTS;
						result_list->offset_next = result_list->offset_list[result_list->base];
						result_list->base--;
						if (result_list->base < 0)
							result_list->base = MAX_RESULTS - 1;
					}
					result_list->idx_article[result_list->base] = pTitleSearch->idxArticle;
					result_list->offset_list[result_list->base] = result_list->offset_next;
					nRowsToMove--;
				}
				else
					bDone = 1;
			}
			else
				fetch_search_result(result_list->offset_next - sizeof(search_info->buf), -1, 1);
		}
	}
	return nRows - nRowsToMove;
}

void search_select_down(void)
{
//	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
//					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
//	int available_count = result_list->count - result_list->first_item;
//	int actual_display_count = available_count < screen_display_count ?
//					available_count : screen_display_count;

	/* no selection, do nothing */
//	if (result_list->cur_selected < 0)
//		return;

	/* bottom reached, not wrapping around */
//	if (result_list->cur_selected + 1 < actual_display_count ) {
//		invert_selection(result_list->cur_selected, result_list->cur_selected + 1, PIXEL_START, RESULT_HEIGHT);
//		++result_list->cur_selected;
//	}
//	if (actual_display_count < available_count) {
//		++result_list->first_item;
//		if (result_list->first_item >= MAX_RESULTS)
//			result_list->first_item -= MAX_RESULTS;
//		search_reload();
//		keyboard_paint();
//	}
        return;
	if (result_list_down(1))
	{
		result_list->cur_selected++;
		if (result_list->cur_selected >= MAX_RESULTS)
			result_list->cur_selected = 0;
		search_reload();
		keyboard_paint();
	}
//	DP(DBG_SEARCH, ("O search_select_down() cur_selected %d\n", result_list->cur_selected));
}

void search_select_up(void)
{
//	DP(DBG_SEARCH, ("O search_select_up() cur_selected %d\n", result_list->cur_selected));
	/* no selection, do nothing */
//	if (result_list->cur_selected < 0)
//		return;
//
//	if (result_list->cur_selected > 0) {
//		invert_selection(result_list->cur_selected, result_list->cur_selected - 1, PIXEL_START, RESULT_HEIGHT);
//		--result_list->cur_selected;
//	}
//	if (result_list->first_item != result_list->base) {
//		--result_list->first_item;
//		if (result_list->first_item < 0)
//			result_list->first_item = MAX_RESULTS - 1;
//		search_reload();
//		keyboard_paint();
//	}
        return;
	if (result_list_up(1))
	{
		result_list->cur_selected--;
		if (result_list->cur_selected < 0)
			result_list->cur_selected = MAX_RESULTS - 1;
		search_reload();
		keyboard_paint();
	}
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
//return -1;
}

unsigned int search_result_first_item()
{
//	return result_list->first_item;
return result_list->base;
}

extern unsigned char * file_buffer;
#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
//static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);
char *compressed_buf = NULL;
int retrieve_article(long idx_article)
{
	ARTICLE_PTR article_ptr;
	int dat_file_id = 0;
	unsigned int dat_article_len = 0;
	char file_name[13];
	unsigned int file_buffer_len = FILE_BUFFER_SIZE;
	//lzo_uint lzo_file_buffer_len = FILE_BUFFER_SIZE;
	int rc = 0;
	ELzmaStatus status;
	Byte propsEncoded[LZMA_PROPS_SIZE];
	unsigned int propsSize;
//        int open_number = 0;

	if (!compressed_buf)
		compressed_buf = (char *)malloc_simple(MAX_COMPRESSED_ARTICLE, MEM_TAG_INDEX_M1);
//start:
	if (compressed_buf && 0 < idx_article && idx_article <= search_info->max_article_idx) {
		wl_seek(search_info->fd_idx, sizeof(long) + (idx_article - 1) * sizeof(article_ptr));
		wl_read(search_info->fd_idx, &article_ptr, sizeof(article_ptr));
		dat_file_id = ((article_ptr.file_id_compressed_len  & 0x3FFFFFFF)>> 24);
		dat_article_len = article_ptr.file_id_compressed_len & 0x00FFFFFF;
		if (dat_article_len > 0)
		{
			if (search_info->fd_dat[dat_file_id] < 0)
			{
				sprintf(file_name, "pedia%d.dat", dat_file_id);
				search_info->fd_dat[dat_file_id] = wl_open(file_name, WL_O_RDONLY);
			}
			if (search_info->fd_dat[dat_file_id] >= 0)
			{
				wl_seek(search_info->fd_dat[dat_file_id], article_ptr.offset_dat & 0x7FFFFFFF);
				wl_read(search_info->fd_dat[dat_file_id], compressed_buf, dat_article_len);
				propsSize = (unsigned int)compressed_buf[0];
				memcpy(propsEncoded, compressed_buf + 1, LZMA_PROPS_SIZE);
				dat_article_len -= LZMA_PROPS_SIZE + 1;
				rc = (int)LzmaDecode(file_buffer, &file_buffer_len, compressed_buf + LZMA_PROPS_SIZE + 1, &dat_article_len,
					propsEncoded, propsSize, LZMA_FINISH_ANY, &status, &g_Alloc);
				if (rc == SZ_OK || rc == SZ_ERROR_INPUT_EOF) /* not sure why it generate SZ_ERROR_INPUT_EOF yet but result ok */
				{
					file_buffer[file_buffer_len] = '\0';
					return 0;
				}
			}
		}
//                else
//                {
//			wl_close(search_info->fd_idx);
//			search_info->fd_idx = wl_open("pedia.idx", WL_O_RDONLY);
//                        if(open_number<2)
//                        {
//                           open_number++;
//                           goto start;
//                        }
//                }
	}
if (!compressed_buf)
sprintf(article_error, "compressed_buf allocation error");
else
{
sprintf(article_error, "idx=%ld, fid=%d, offset=%lx", idx_article, 
dat_file_id, sizeof(long) + (idx_article - 1) * sizeof(article_ptr));
sprintf(article_error2, "len=%d, rc=%d", dat_article_len, rc);
}
	print_article_error();
	return -1;
}	

void search_set_selection(int new_selection)
{
	//int list_idx;
	
	result_list->cur_selected = new_selection;
	/*list_idx = result_list->base + new_selection;
	if (list_idx >= MAX_RESULTS)
		list_idx -= MAX_RESULTS;
	if (!display_link_article(result_list->idx_article[list_idx]))
		history_add(result_list->idx_article[list_idx], result_list->list[list_idx]);*/
}
void search_open_article(int new_selection)
{
	int list_idx;
	
	list_idx = result_list->base + new_selection;
	if (list_idx >= MAX_RESULTS)
		list_idx -= MAX_RESULTS;
	if (!display_link_article(result_list->idx_article[list_idx]))
		history_add(result_list->idx_article[list_idx], result_list->list[list_idx]);
}

long find_closest_idx(long idx, char *title)
{
	ARTICLE_PTR article_ptr;
	TITLE_SEARCH title_search;
	static int count = 0;
	
	title[0] = '\0';
	if (idx > search_info->max_article_idx)
		idx -= search_info->max_article_idx;
	wl_seek(search_info->fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + 4);
	wl_read(search_info->fd_idx, (void *)&article_ptr, sizeof(article_ptr));
	if (!article_ptr.file_id_compressed_len || !article_ptr.offset_fnd)
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
		bigram_decode(title, title_search.sTitleSearch);
		title[MAX_TITLE_SEARCH - 1] = '\0';
		return idx;
	}
}

void random_article(void)
{
	long idx_article;
	char title[MAX_TITLE_SEARCH];
	unsigned long clock_ticks;
		
#ifdef INCLUDED_FROM_KERNEL
	clock_ticks = Tick_get();
#else
	clock_ticks = clock();
#endif
	idx_article = clock_ticks % search_info->max_article_idx + 1;
	idx_article = find_closest_idx(idx_article, title);
	if (idx_article)
	{
		if (!display_link_article(idx_article))
			history_add(idx_article, title);
	}
}
int set_result_list_base(int offset,int offset_count)
{
   int first_item_count;
   int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
   int base_last;
    
   base_last = result_list->base;

   if(result_list->count <= screen_display_count)
   {
     return 0;
   }

   if(offset>=0)
      first_item_count = offset_count+result_list->base;
   else
      first_item_count = result_list->base-offset_count;

   if(first_item_count<0)
       result_list->base = 0;
   else if(first_item_count > (result_list->count - screen_display_count))
   {
       result_list->base = result_list->count-screen_display_count;
       if(result_list->base < 0)
           result_list->base = 0;

   }
   else
       result_list->base = first_item_count;

   if(base_last!=result_list->base)
      return 1;
   else
      return 0;
}
unsigned int get_time_search(void)
{
        long clock_ticks;

#ifdef INCLUDED_FROM_KERNEL
	clock_ticks = Tick_get();
#else
	clock_ticks = clock();
#endif

	return (unsigned int)clock_ticks;
}
