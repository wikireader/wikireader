//
// Authors:	Holger Hans Peter Freyther <zecke@openmoko.org>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#include <lsearcher.h>
#include "search.h"
#include "msg.h"
#include "wl-keyboard.h"
#include <guilib.h>
#include <glyph.h>
#include <lcd.h>
#include "wikilib.h"
#include "wom_reader.h"

#define DBG_SEARCH 0
#define WOM_ON

static struct search_results {
	char list[MAX_RESULTS][MAXSTR];
	unsigned int count;
	int cur_selected;	// -1 when no selection.
	unsigned int first_item;// Index of the first item displayed on the list. 0 based.
} result_list;

static lindex global_search;
static struct search_state state;
static struct search_state last_first_hit;

static char search_string[MAXSTR];
static int search_str_len = 0;

static char s_find_first = 1;
static char trigram_loaded = 0;

const char* search_fetch_result()
{
	DP(DBG_SEARCH, ("O search_fetch_result() called\n"));
	if (search_str_len == 0)
		return NULL;
#ifdef WOM_ON
	const wom_article_index_t* idx;
	static char result_buf[MAXSTR]; // we know that the returned pointer is copied immediately
	if (s_find_first) {
		s_find_first = 0;
		idx = wom_find_article(g_womh, search_string, search_str_len);
	} else
		idx = wom_get_next_article(g_womh);
	if (!idx) return 0;
	sprintf(result_buf, "%.*s%.6x", idx->uri_len, idx->abbreviated_uri, (unsigned int) idx->offset_into_articles);
	DP(DBG_SEARCH, ("O search_fetch_result() '%s'\n", result_buf));
	return result_buf;
#else // !WOM_ON
	char* result = search_fast(&global_search, search_string, &state);
	if (s_find_first) {
		s_find_first = 0;
		store_state(&global_search, &state, &last_first_hit);
	}
	return result;
#endif
}

void search_init()
{
	int result = load_index(&global_search, "pedia.idx", "pedia.jmp");
	if (!result) {
		msg(MSG_ERROR, "Failed to initialize search.\n");
		/* XXX, FIXME, handle the error */
	}
	result_list.count = 0;
	result_list.first_item = 0;
	result_list.cur_selected = -1;
}

int search_load_trigram(void)
{
	if (!trigram_loaded)
		trigram_loaded = !load_trigram_chunk(&global_search);
	return trigram_loaded;
}

void search_reload()
{
	static const char search_result_str[] = "Search results for:";
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int available_count;
	int y_pos, len;
	const char * result;

	DP(DBG_SEARCH, ("O search_reload() start: screen_display_count %u cur_selected %d first_item %u\n", screen_display_count, result_list.cur_selected, result_list.first_item));
	guilib_fb_lock();
	if (keyboard_get_mode() == KEYBOARD_NONE)
		guilib_clear();
	else
		guilib_clear_area(0, 0, 239, LCD_HEIGHT_LINES - KEYBOARD_HEIGHT - 1);
	render_string(0, 1, 10, search_result_str, strlen(search_result_str));

	if (!search_str_len) goto out;

	render_string(0, 87, 10, search_string, strlen(search_string));
	y_pos = RESULT_START;

	int found = 0;
	available_count = result_list.count - result_list.first_item;
	while (available_count < screen_display_count &&
		result_list.count < MAX_RESULTS &&
		(result = search_fetch_result())) {
		len = strlen(result);
		memcpy(&result_list.list[result_list.count], result, len);
		result_list.list[result_list.count][len] = 0;
		++result_list.count;
		++available_count;
		found = 1;
	}

	if (!result_list.count) {
		result_list.cur_selected = -1;
		result_list.first_item = 0;
		goto out;
	}

	if (!found && result_list.first_item && result_list.first_item+result_list.cur_selected >= result_list.count) {
		--result_list.first_item;
		++available_count;
	}
	if (result_list.count) {
		unsigned int i;
		unsigned int count = available_count < screen_display_count ?
					available_count : screen_display_count;


		for (i = result_list.first_item; i < count+result_list.first_item; i++) {
			render_string(0, 1, y_pos, result_list.list[i], strlen(result_list.list[i]) - TARGET_SIZE);
			DP(DBG_SEARCH, ("O result[%d] '%s'\n", i, &result_list.list[i]));
			y_pos += RESULT_HEIGHT;
		}
		if (result_list.cur_selected >= screen_display_count)
			result_list.cur_selected = screen_display_count - 1;
		invert_selection(result_list.cur_selected, -1, PIXEL_START, RESULT_HEIGHT);
	}
out:
	DP(DBG_SEARCH, ("O search_reload() end: screen_display_count %u cur_selected %d first_item %u\n", screen_display_count, result_list.cur_selected, result_list.first_item));
	guilib_fb_unlock();
}

void search_add_char(char c)
{
	if (search_str_len >= MAXSTR - 2)
		return;

	if (!search_str_len && c == 0x20)
		return;

	search_string[search_str_len++] = c;
	search_string[search_str_len] = '\0';
	if (search_str_len >= 4) {
		state.pattern_len = search_str_len;
		if (!s_find_first) // already found something before?
			reset_state(&global_search, &state, &last_first_hit);
	} else
		prepare_search(&global_search, search_string, &state);
	s_find_first = 1;
	result_list.cur_selected = -1;
	result_list.first_item = 0;
	result_list.count = 0;
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
	memset(&state, 0, sizeof(state));
	s_find_first = 1;
	prepare_search(&global_search, search_string, &state);
	result_list.cur_selected = -1;
	result_list.first_item = 0;
	result_list.count = 0;
}

void search_select_down(void)
{
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int available_count = result_list.count - result_list.first_item;
	int actual_display_count = available_count < screen_display_count ?
					available_count : screen_display_count;

	/* no selection, do nothing */
	if (result_list.cur_selected < 0)
		return;

	/* bottom reached, not wrapping around */
	if (result_list.cur_selected + 1 < actual_display_count ) {
		invert_selection(result_list.cur_selected, result_list.cur_selected + 1, PIXEL_START, RESULT_HEIGHT);
		++result_list.cur_selected;
	}
	else if (result_list.count < MAX_RESULTS ||
			result_list.cur_selected + 1 < available_count) {
		++result_list.first_item;
		search_reload();
		keyboard_paint();
	}
	DP(DBG_SEARCH, ("O search_select_down() cur_selected %d\n", result_list.cur_selected));
}

void search_select_up(void)
{
	DP(DBG_SEARCH, ("O search_select_up() cur_selected %d\n", result_list.cur_selected));
	/* no selection, do nothing */
	if (result_list.cur_selected < 0)
		return;

	if (result_list.cur_selected > 0) {
		invert_selection(result_list.cur_selected, result_list.cur_selected - 1, PIXEL_START, RESULT_HEIGHT);
		--result_list.cur_selected;
	}
	else if (result_list.first_item > 0) {
		--result_list.first_item;
		search_reload();
		keyboard_paint();
	}
}

const char *search_current_title(void)
{
	const char* title;

	if (result_list.cur_selected >= result_list.count - result_list.first_item) {
		DP(DBG_SEARCH, ("O search_current_title() NO TITLE\n"));
		return NULL;
	}
	title = result_list.list[result_list.cur_selected+result_list.first_item];
	DP(DBG_SEARCH, ("O search_current_title() '%s'\n", title));
	return title;
}

unsigned int search_result_count()
{
	return result_list.count;
}

int search_result_selected()
{
	return result_list.cur_selected;
}

unsigned int search_result_first_item()
{
	return result_list.first_item;
}

void search_set_selection(int new_selection)
{
	result_list.cur_selected = new_selection;
}
