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
#include <tick.h>

extern int _wl_read(int fd, void *buf, unsigned int count);
extern int _wl_open(const char *filename, int flags);
extern int _wl_seek(int fd, unsigned int pos);
extern int _wl_tell(int fd);

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
	unsigned int base;		// the starting list index for the circular list. it is also the first item displayed
	int cur_selected;		// -1 when no selection.
//	unsigned int first_item;	// Index of the first item displayed on the list. 0 based.
} SEARCH_RESULTS;
static SEARCH_RESULTS *result_list;

typedef struct _search_info {
	int fd_pfx;
	int fd_fnd;
	int fd_idx;
	int fd_dat[MAX_DAT_FILES];
	long max_article_idx;
	long prefix_index_table[SEARCH_CHR_COUNT * SEARCH_CHR_COUNT * SEARCH_CHR_COUNT];
	char buf[MAX_RESULTS * sizeof(TITLE_SEARCH)];	// buf correspond to result_list
	int buf_len;
	long offset_current;		// offset (pedia.fnd) of the content of buffer
	long offset_search_result;	// offset (pedia.fnd) of the first search_string match
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

int search_string_cmp(char *title, char *search, int len)  // assuming search consists of lowercase only
{
	int rc = 0;
	char c;
	
	while (!rc && len > 0)
	{
		c = *title;
		if ('A' <= c && c <= 'Z')
			c += 32;
		if (c == *search)
		{
			title++;
			search++;
			len--;
		}
		else if (c > *search)
			rc = 1;
		else
			rc = -1;
	}
	return rc;
}
		

int load_prefix_index(void)
{
	int i;
	
	if (!prefix_index_loaded)
	{
		search_info->fd_pfx = _wl_open("pedia.pfx", WL_O_RDONLY);
		search_info->fd_fnd = _wl_open("pedia.fnd", WL_O_RDONLY);
		init_bigram(search_info->fd_fnd);
		search_info->fd_idx = _wl_open("pedia.idx", WL_O_RDONLY);
		for (i=0; i < MAX_DAT_FILES; i++)
			search_info->fd_dat[i] = -1;
		search_info->offset_current = -1;
		search_info->offset_search_result = -1;
		if (search_info->fd_pfx >= 0 && search_info->fd_fnd >= 0 && search_info->fd_idx >= 0)
		{
			_wl_read(search_info->fd_idx, (void *)&search_info->max_article_idx, sizeof(search_info->max_article_idx));
			if (_wl_read(search_info->fd_pfx, (void *)search_info->prefix_index_table, SIZE_PREFIX_INDEX_TABLE) == 
				SIZE_PREFIX_INDEX_TABLE)
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

static void print_article_error()
{
	guilib_fb_lock();
	guilib_clear();
	render_string(SEARCH_LIST_FONT_IDX, 30, 104, "Opening the article failed.", 27);
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

void fetch_search_result(long offset_fnd)
{
	char *buf_to_read;
	int len_to_read;
	int len;
	TITLE_SEARCH *pTitleSearch;
	int idxNextTitleSearch;
	int bDone = 0;
	int rc;
	
	if (search_info->offset_current >= 0 && search_info->offset_current <= offset_fnd &&   
		offset_fnd < search_info->offset_current + sizeof(search_info->buf))
	{
		len_to_read = offset_fnd - search_info->offset_current;
		memcpy(search_info->buf, &search_info->buf[len_to_read], sizeof(search_info->buf) - len_to_read); // retaining reusable content
		buf_to_read = &(search_info->buf[sizeof(search_info->buf) - len_to_read]);
	}
	else if (search_info->offset_current >= 0 && search_info->offset_current - sizeof(search_info->buf) <= offset_fnd &&
		offset_fnd < search_info->offset_current)
	{
		len_to_read = search_info->offset_current - offset_fnd;
		memrcpy(&search_info->buf[len_to_read], search_info->buf, sizeof(search_info->buf) - (search_info->offset_current - offset_fnd)); // retaining reusable content
		buf_to_read = search_info->buf;
	}
	else
	{
		buf_to_read = search_info->buf;
		len_to_read = sizeof(search_info->buf);
	}
	_wl_seek(search_info->fd_fnd, offset_fnd);
	len = _wl_read(search_info->fd_fnd, buf_to_read, len_to_read);
	search_info->offset_current = offset_fnd;
	search_info->buf_len = sizeof(search_info->buf) - (len_to_read - len);
	result_list->count = 0;
	result_list->base = 0;
	result_list->cur_selected = 0;
	idxNextTitleSearch = 0;
	while (!bDone && result_list->count < MAX_RESULTS)
	{
		pTitleSearch = (TITLE_SEARCH *)&search_info->buf[idxNextTitleSearch];
		if (idxNextTitleSearch < search_info->buf_len &&
			is_proper_string(pTitleSearch->sTitleSearch, search_info->buf_len - idxNextTitleSearch - 
			sizeof(pTitleSearch->idxArticle) - sizeof(pTitleSearch->cZero)))
		{
			bigram_decode(result_list->list[result_list->count], pTitleSearch->sTitleSearch);
			rc = search_string_cmp(result_list->list[result_list->count], search_string, search_str_len);
			if (!rc) // match!
			{
				if (!result_list->count)
				{
					 offset_fnd =  offset_fnd + idxNextTitleSearch;
					_wl_seek(search_info->fd_fnd, offset_fnd);
					len = _wl_read(search_info->fd_fnd, search_info->buf, sizeof(search_info->buf));
					if (len <= 0)
					{
						bDone = 1;
						continue;
					}
					search_info->offset_current = offset_fnd;
					search_info->buf_len = len;
					idxNextTitleSearch = 0;
					pTitleSearch = (TITLE_SEARCH *)&search_info->buf[idxNextTitleSearch];
				}
				memcpy((void *)&result_list->idx_article[result_list->count], 
					(void *)&pTitleSearch->idxArticle, sizeof(long)); // use memcpy to avoid "Unaligned data access"
				result_list->offset_list[result_list->count] = offset_fnd + idxNextTitleSearch;
				idxNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) + 2;
				result_list->offset_next = offset_fnd + idxNextTitleSearch;
				result_list->count++;
			}
			else if (rc < 0)
			{
				idxNextTitleSearch += sizeof(pTitleSearch->idxArticle) + strlen(pTitleSearch->sTitleSearch) + 2;
			}
			else
				bDone = 1;
		}
		else
		{
			 offset_fnd = offset_fnd + idxNextTitleSearch;
			_wl_seek(search_info->fd_fnd, offset_fnd);
			len = _wl_read(search_info->fd_fnd, search_info->buf, sizeof(search_info->buf));
			if (len <= 0)
			{
				bDone = 1;
				continue;
			}
			search_info->offset_current = offset_fnd;
			search_info->buf_len = len;
			idxNextTitleSearch = 0;
			pTitleSearch = (TITLE_SEARCH *)&search_info->buf[idxNextTitleSearch];
		}
	}
}

int search_populate_result()
{
	int idx_prefix_index_table;
	char c1, c2, c3;
	int found = 0;
	
	result_list->count = 0;
	result_list->base = 0;
	result_list->cur_selected = 0;
	if (search_str_len > 0)
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
		if (search_info->prefix_index_table[idx_prefix_index_table])
		{
			found = 1;
			search_info->offset_search_result = search_info->prefix_index_table[idx_prefix_index_table];
			fetch_search_result(search_info->prefix_index_table[idx_prefix_index_table]);
		}
	}
	return found;
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
		render_string(MESSAGE_FONT_IDX, 40, 55, "Type a word or phrase", 21);
		goto out;
	}

	//render_string(4, 87, 10, search_string, strlen(search_string));
	strcpy(temp_search_string, search_string);
	if ('a' <= temp_search_string[0] && temp_search_string[0] <= 'z')
		temp_search_string[0] -= 32;
	render_string(SEARCH_HEADING_FONT_IDX, 3, 5, temp_search_string, strlen(temp_search_string));
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
			render_string(SEARCH_LIST_FONT_IDX, 3, y_pos, title, strlen(title));
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
void search_reload_ex()
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos,start_x_search=0;
	char *title;
	char temp_search_string[MAX_TITLE_SEARCH];

	guilib_fb_lock();
	if (keyboard_get_mode() == KEYBOARD_NONE)
		guilib_clear();
	else
        {
                if(search_string_changed_remove)
                {
                    if(!search_str_len)
                       start_x_search = 0;
                    else
                       start_x_search = search_string_pos[search_str_len];
                    search_string_changed_remove = false;
		    guilib_clear_area(start_x_search, 0, LCD_BUF_WIDTH_PIXELS, 30);
                }


		guilib_clear_area(0, 35, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
        }

	if (!search_str_len)
	{
		render_string(MESSAGE_FONT_IDX, 40, 55, "Type a word or phrase", 21);
		goto out;
	}

	strcpy(temp_search_string, search_string);
	if ('a' <= temp_search_string[0] && temp_search_string[0] <= 'z')
		temp_search_string[0] -= 32;
	start_x_search = render_string(SEARCH_HEADING_FONT_IDX, 3, 5, temp_search_string, strlen(temp_search_string));
        search_string_pos[search_str_len]=start_x_search;
        msg(MSG_INFO,"start_x_search:%d\n",start_x_search);
	y_pos = RESULT_START;


	if (!result_list->count) {
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
			render_string(SEARCH_LIST_FONT_IDX, 3, y_pos, title, strlen(title));
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
		render_string(MESSAGE_FONT_IDX, 40, 55, "Type a word or phrase", 21);
		goto out;
	}


	y_pos = RESULT_START;

	if (!result_list->count) {
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
			render_string(SEARCH_LIST_FONT_IDX, 3, y_pos, title, strlen(title));
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

void search_add_char(char c)
{
        if(c == 0x20 && search_str_len>0 && search_string[search_str_len-1] == 0x20)
                return;
	if (search_str_len >= MAX_TITLE_SEARCH - 2)
		return;

	if (!search_str_len && c == 0x20)
		return;

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
        search_string_changed = true;
        return;
        #endif
	search_populate_result();
        msg(MSG_INFO,"search_result_count:%d\n",result_list->count);
        result_list->cur_selected = -1;
}
void search_fetch()
{
	search_populate_result();
        result_list->cur_selected = -1;
        search_string_changed = false;
}

/*
 * TODO: Optimize going back... For the first three entries
 * one could remember the &state and then seek back to the
 * position and continue the search from there... For now do it
 * the easy way and wait for user testing.
 */
void search_remove_char(void)
{
	DP(DBG_SEARCH, ("O search_remove_char() search_str_len %d\n", search_str_len));
	if (search_str_len == 0)
		return;

	search_string[--search_str_len] = '\0';
//	memset(&state, 0, sizeof(state));
//	s_find_first = 1;
//	prepare_search(&global_search, search_string, &state);
//	result_list->cur_selected = -1;
//	result_list->first_item = 0;
//	result_list->count = 0;

         #ifdef INCLUDED_FROM_KERNEL
        time_search_last=get_time_search();
        search_string_changed = true;
        search_string_changed_remove = true;
        return;
        #endif
	search_populate_result();
        result_list->cur_selected = -1;
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
				fetch_search_result(result_list->offset_list[result_list->base]);
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
				fetch_search_result(result_list->offset_next - sizeof(search_info->buf));
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
void clear_search_string()
{
      result_list->count = 0;
      strcpy(search_string,"");
      search_str_len = 0;
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
char compressed_buf[MAX_COMPRESSED_ARTICLE];
int retrieve_article(long idx_article)
{
	ARTICLE_PTR article_ptr;
	int dat_file_id;
	unsigned int dat_article_len;
	char file_name[13];
	unsigned int file_buffer_len = FILE_BUFFER_SIZE;
	//lzo_uint lzo_file_buffer_len = FILE_BUFFER_SIZE;
	int rc;
	ELzmaStatus status;
	Byte propsEncoded[LZMA_PROPS_SIZE];
	unsigned int propsSize;
        int open_number = 0;

start:
	if (0 < idx_article && idx_article <= search_info->max_article_idx) {
		_wl_seek(search_info->fd_idx, sizeof(long) + (idx_article - 1) * sizeof(article_ptr));
		_wl_read(search_info->fd_idx, &article_ptr, sizeof(article_ptr));
		dat_file_id = ((article_ptr.file_id_compressed_len  & 0x3FFFFFFF)>> 24);
		dat_article_len = article_ptr.file_id_compressed_len & 0x00FFFFFF;
		if (dat_article_len > 0)
		{
			if (search_info->fd_dat[dat_file_id] < 0)
			{
				sprintf(file_name, "pedia%d.dat", dat_file_id);
                                msg(MSG_INFO,"wl_open file:%s\n",file_name);
				search_info->fd_dat[dat_file_id] = _wl_open(file_name, WL_O_RDONLY);
                                msg(MSG_INFO,"search_info->fd_dat[dat_file_id]:%d,dat_file_id:%d\n",search_info->fd_dat[dat_file_id],dat_file_id);
			}
			if (search_info->fd_dat[dat_file_id] >= 0)
			{
				_wl_seek(search_info->fd_dat[dat_file_id], article_ptr.offset_dat);
				_wl_read(search_info->fd_dat[dat_file_id], compressed_buf, dat_article_len);
				/*if ((article_ptr.file_id_compressed_len & 0x40000000) == 0x40000000)
				{
                                        msg(MSG_INFO,"1-BZ2_bzBuffToBuffDecompress\n");
					rc = BZ2_bzBuffToBuffDecompress(file_buffer, &file_buffer_len, compressed_buf, dat_article_len, 1, 0);
                                        msg(MSG_INFO,"BZ2_bzBuffToBuffDecompress over\n");
					if (rc == BZ_OK)
					{
						file_buffer[file_buffer_len] = '\0';
						return 0;
					}
				}*/
				//else if ((article_ptr.file_id_compressed_len & 0x80000000) == 0x80000000)
				{
					propsSize = (unsigned int)compressed_buf[0];
					memcpy(propsEncoded, compressed_buf + 1, LZMA_PROPS_SIZE);
					dat_article_len -= LZMA_PROPS_SIZE + 1;
                                        msg(MSG_INFO,"2-LzmaDecode\n");
					rc = (int)LzmaDecode(file_buffer, &file_buffer_len, compressed_buf + LZMA_PROPS_SIZE + 1, &dat_article_len,
						propsEncoded, propsSize, LZMA_FINISH_ANY, &status, &g_Alloc);
					if (rc == SZ_OK || rc == SZ_ERROR_INPUT_EOF) /* not sure why it generate SZ_ERROR_INPUT_EOF yet but result ok */
					{
                                                msg(MSG_INFO,"LzmaDecode ok\n");
						file_buffer[file_buffer_len] = '\0';
						return 0;
					}
                                        else
                                        {
					   wl_close(search_info->fd_idx);
					   search_info->fd_idx = _wl_open("pedia.idx", WL_O_RDONLY);

                                           msg(MSG_INFO,"LzmaDecode failed,close file\n");
	                                  wl_close(search_info->fd_dat[dat_file_id]);
                                          search_info->fd_dat[dat_file_id] = -1;
                                        }
				}
				/*else
				{
                                        msg(MSG_INFO,"3-lzo1x_decompress\n");
					rc = lzo1x_decompress((void *)compressed_buf, (lzo_uint)dat_article_len, (void *)file_buffer, 
						&lzo_file_buffer_len, wrkmem);
                                        msg(MSG_INFO,"lzo1x_decompress over\n");			
                                        if (rc == LZO_E_OK)
					{
						file_buffer[lzo_file_buffer_len] = '\0';
						return 0;
					}
				}*/
			}
                        else
                        {
                        }
		}
                else
                {
			msg(MSG_INFO,"close index file\n");
			wl_close(search_info->fd_idx);
			search_info->fd_idx = _wl_open("pedia.idx", WL_O_RDONLY);
                        if(open_number<2)
                        {
                           open_number++;
                           goto start;
                        }
                }
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
	
	if (idx > search_info->max_article_idx)
		idx -= search_info->max_article_idx;
	_wl_seek(search_info->fd_idx, (idx - 1) * sizeof(ARTICLE_PTR) + 4);
	_wl_read(search_info->fd_idx, (void *)&article_ptr, sizeof(article_ptr));
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
		_wl_seek(search_info->fd_fnd, article_ptr.offset_fnd);
		_wl_read(search_info->fd_fnd, (void *)&title_search, sizeof(title_search));
		bigram_decode(title, title_search.sTitleSearch);
		title[MAX_TITLE_SEARCH - 1] = '\0';
		return idx;
	}
}

void random_article(void)
{
	long idx_article;
	char title[MAX_TITLE_SEARCH];
	long clock_ticks;
		
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
   msg(MSG_INFO,"in set_result_list_base,offset_count :%d\n",offset_count);

   if(result_list->count <= screen_display_count)
   {
     msg(MSG_INFO,"result_list->count:%d<=screen_display_count:%d\n",result_list->count,screen_display_count); 
     return 0;
   }

   if(offset>=0)
      first_item_count = offset_count+result_list->base;
   else
      first_item_count = result_list->base-offset_count;

   msg(MSG_INFO,"first_item_count:%d\n",first_item_count);

   if(first_item_count<0)
       result_list->base = 0;
   else if(first_item_count > (result_list->count - screen_display_count))
   {
       msg(MSG_INFO,"first_item_count:%d>(result_list->count:%d - screen_display_count:%d)\n",first_item_count,result_list->count,screen_display_count);
       result_list->base = result_list->count-screen_display_count;
       if(result_list->base < 0)
           result_list->base = 0;

   }
   else
       result_list->base = first_item_count;

   msg(MSG_INFO,"base from %d to %d,offset:%d\n",base_last,result_list->base,offset);
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
