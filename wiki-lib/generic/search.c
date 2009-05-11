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

#define DBG_SEARCH 0

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

static char first_hit = 0;
static char trigram_loaded = 0;

static void invert_selection(int old_pos, int new_pos)
{
	guilib_fb_lock();

	if (old_pos != -1)
		guilib_invert(PIXEL_START + old_pos * RESULT_HEIGHT, RESULT_HEIGHT);
	if (new_pos != -1 )
		guilib_invert(PIXEL_START + new_pos * RESULT_HEIGHT, RESULT_HEIGHT);

	guilib_fb_unlock();
}

static char* search_fetch_result()
{
	char *result;

	if (search_str_len == 0)
		return NULL;

	result = search_fast(&global_search, search_string, &state);
	if (!first_hit) {
		store_state(&global_search, &state, &last_first_hit);
		first_hit = 1;
	}

	return result;
}

void search_init()
{
	int result = load_index(&global_search, "/pedia.idx", "/pedia.jmp");
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
	unsigned int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ? 
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	unsigned int available_count;
	int y_pos, len;
	char * result;

	DP(DBG_SEARCH, ("O search_reload() start: screen_display_count %u cur_selected %d first_item %u\n", screen_display_count, result_list.cur_selected, result_list.first_item));
	guilib_fb_lock();
	guilib_clear();
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
	
	if (!found && result_list.first_item+result_list.cur_selected >= result_list.count ) {
		--result_list.first_item;
		++available_count;
	}
	if (result_list.count) {
		unsigned int i;
		unsigned int count = available_count < screen_display_count ?
					available_count : screen_display_count;

		
		for (i = result_list.first_item; i < count+result_list.first_item; i++) {
			render_string(0, 1, y_pos, result_list.list[i], strlen(result_list.list[i]) - TARGET_SIZE);
			y_pos += RESULT_HEIGHT;
		}
		if (result_list.cur_selected >= screen_display_count)
			result_list.cur_selected = screen_display_count - 1;
		invert_selection(result_list.cur_selected, -1);
	}
out:
	DP(DBG_SEARCH, ("O search_reload() end: screen_display_count %u cur_selected %d first_item %u\n", screen_display_count, result_list.cur_selected, result_list.first_item));
	guilib_fb_unlock();
}

void search_add_char(char c)
{
	if (search_str_len >= MAXSTR - 2)
		return;

	search_string[search_str_len++] = c;
	search_string[search_str_len] = '\0';
	if (search_str_len >= 4) {
		state.pattern_len = search_str_len;
		if (first_hit) {
			reset_state(&global_search, &state, &last_first_hit);
			first_hit = 0;
		}
	} else {
		first_hit = 0;
		prepare_search(&global_search, search_string, &state);
	}
	result_list.cur_selected = 0;
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
	first_hit = 0;
	prepare_search(&global_search, search_string, &state);
	if (search_str_len == 0)
		result_list.cur_selected = -1;
	else
		result_list.cur_selected = 0;
	result_list.first_item = 0;
	result_list.count = 0;
}

void search_select_down(void)
{
	unsigned int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ? 
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	unsigned int available_count = result_list.count - result_list.first_item;
	unsigned int actual_display_count = available_count < screen_display_count ?
					available_count : screen_display_count;

	/* no selection, do nothing */
	if (result_list.cur_selected < 0)
		return;

	/* bottom reached, not wrapping around */
	if (result_list.cur_selected + 1 < actual_display_count ) {
		invert_selection(result_list.cur_selected, result_list.cur_selected + 1);
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
		invert_selection(result_list.cur_selected, result_list.cur_selected - 1);
		--result_list.cur_selected;
	}
	else if (result_list.first_item > 0) {
		--result_list.first_item;
		search_reload();
		keyboard_paint();
	}
}

const char *search_current_target(void)
{
	if (result_list.cur_selected >= result_list.count)
		return NULL;

	return result_list.list[result_list.cur_selected+result_list.first_item]+(strlen(result_list.list[result_list.cur_selected+result_list.first_item])-TARGET_SIZE);
}

const char *search_current_title(void)
{
	if (result_list.cur_selected >= result_list.count - result_list.first_item)
		return NULL;

	return result_list.list[result_list.cur_selected+result_list.first_item];
}

/*
 *  - Check which item was hit
 *  - Update the selection so search_current_title returns the
 *    right item.
 *  - return the right item
 *
 *  In practice updating search_current depending on the x,y
 *  should be enough and then search_current_target can be returned.
 */
const char *search_release(int y)
{
	unsigned int i;
	int result_start = PIXEL_START;

	for (i = 0; i < result_list.count; ++i, result_start += RESULT_HEIGHT) {
		if (y >= result_start && y < result_start + RESULT_HEIGHT) {
			result_list.cur_selected = i;
			return search_current_target();
		}
	}

	return NULL;
}
