/*
 *  Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lsearcher.h>
#include "search.h"
#include "msg.h"
#include "wl-keyboard.h"

#include <guilib.h>
#include <glyph.h>

static struct search_results {
	char list[MAX_RESULTS][MAXSTR];
	unsigned int count;
	unsigned int first_item;	// The index of the first item being displayed on the screen. 0 based.
} result_list;

static lindex global_search;
static struct search_state state;
static struct search_state last_first_hit;

static int search_index;
static char search_string[MAXSTR];

static char first_hit = 0;
static char trigram_loaded = 0;

static const char search_result_str[] = "Search results for:";

static int search_found = 0;

/* -1 is unselected */
static int search_current = -1;

static void invert_selection(int old_pos, int new_pos)
{
	int start = PIXEL_START;

	guilib_fb_lock();

	if (old_pos != -1) {
		guilib_invert(start + old_pos * RESULT_HEIGHT, RESULT_HEIGHT);
	}

	if (new_pos != -1 ) {
		guilib_invert(start + new_pos * RESULT_HEIGHT, RESULT_HEIGHT);
	}

	guilib_fb_unlock();
}

static void search_reset_current()
{
	search_current = -1;
}

static void search_reset_list()
{
	result_list.count = 0;
	result_list.first_item = 0;
}

static int get_display_count()
{
	int display_count = keyboard_get_mode() == KEYBOARD_NONE ?
						NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	return display_count <= result_list.count-result_list.first_item ? 
				display_count : 
				result_list.count-result_list.first_item;
}

void search_init()
{
	int result = load_index(&global_search, "/pedia.idx", "/pedia.jmp");
	if (!result) {
		msg(MSG_ERROR, "Failed to initialize search.\n");
		/* XXX, FIXME, handle the error */
	}
	search_reset_list();
}

int search_load_trigram(void)
{
	if (!trigram_loaded) {
		trigram_loaded = !load_trigram_chunk(&global_search);
	}

	return trigram_loaded;
}

void search_reset()
{
	search_index = 0;
	first_hit = 0;
}

void search_reload()
{
	int available_display_count = get_display_count();
	int screen_display_count = keyboard_get_mode() == KEYBOARD_NONE ? 
					NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
	int y_pos = 0;

	guilib_fb_lock();
	guilib_clear();
	render_string(0, 1, 10, search_result_str, strlen(search_result_str));
	render_string(0, 87, 10, search_string, strlen(search_string));
	y_pos = RESULT_START;

	if (available_display_count < screen_display_count) {
		char * result;
		int len;

		while (available_display_count < screen_display_count && 
				result_list.count < MAX_RESULTS && 
				(result = search_fetch_result())) {
			len = strlen(result);
			memcpy(&result_list.list[result_list.count], result, len);
			result_list.list[result_list.count][len] = 0;
msg(MSG_INFO, "new list[%d] '%s'\n", result_list.count, result_list.list[result_list.count]);
			++available_display_count;
			++result_list.count;
		}
	}
	
	if (available_display_count) {
		int i;

		for (i = 0; i < available_display_count; i++) {
			render_string(0, 1, y_pos,
				result_list.list[result_list.first_item+i], 
				strlen(result_list.list[result_list.first_item+i]) - TARGET_SIZE);
			y_pos += RESULT_HEIGHT;
		}
		invert_selection(search_current, -1);
	}
	
	guilib_fb_unlock();
}

void search_add(char c)
{
	if (search_index >= MAXSTR - 2)
		return;

	search_string[search_index++] = c;
	search_string[search_index] = '\0';
	if (search_index >= 4) {
		state.pattern_len = search_index;
		if (first_hit) {
			reset_state(&global_search, &state, &last_first_hit);
		}

		first_hit = 0;
		search_reset_current();
		return;
	}

	first_hit = 0;
	prepare_search(&global_search, search_string, &state);
	search_reset_current();
}

/*
 * TODO: Optimize going back... For the first three entries
 * one could remember the &state and then seek back to the
 * position and continue the search from there... For now do it
 * the easy way and wait for user testing.
 */
void search_remove_char(void)
{
	if (search_index == 0)
		return;

	search_index -= 1;

	search_string[search_index] = '\0';
	memset(&state, 0, sizeof(state));
	first_hit = 0;
	prepare_search(&global_search, search_string, &state);
	search_reset_current();
}

char* search_fetch_result()
{
	char *result;

	if (search_index == 0)
		return NULL;

	result = search_fast(&global_search, search_string, &state);
	if (!first_hit) {
		store_state(&global_search, &state, &last_first_hit);
		first_hit = 1;
	}

	return result;
}

/*
 * paint the results of the search...
 * 
 * - Every item gets put at a fixed position.
 * - We will clear the content only atfer the first the resul was fetched.
 *   This will leave the old content on the screen until we have something
 *   to show.
 * - This code is drawing the current selection as well
 */
void search_paint(void)
{
	char *result;
	int y_pos = 0;
	const int results = keyboard_get_mode() == KEYBOARD_NONE ?
				NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;

	guilib_fb_lock();
	search_found = 0;
	search_reset_list();

	while (search_found < results && (result = search_fetch_result())) {
		if (!search_found) {
			guilib_clear();
			render_string(0, 1, 10, search_result_str, strlen(search_result_str));
			y_pos = RESULT_START;
		}
		render_string(0, 87, 10, search_string, strlen(search_string));
		const int len = strlen(result);
		render_string(0, 1, y_pos, result, len - (TARGET_SIZE));
		memcpy(&result_list.list[result_list.count], result, len);
msg(MSG_INFO, "list[%d] '%s'\n", result_list.count, result_list.list[result_list.count]);

		result_list.list[result_list.count][len] = 0;
		y_pos += RESULT_HEIGHT;
		++search_found;
		++result_list.count;
	}
	
	if (!search_found) {
		guilib_clear();
		render_string(0, 1, 10, search_result_str, strlen(search_result_str));
		render_string(0, 87, 10, search_string, strlen(search_string));
		search_current = -1;
	} else {
		if (search_current == -1) {
			invert_selection(search_current, 0);
			++search_current;
		} else {
			invert_selection(search_current, -1);
		}
	}

	guilib_fb_unlock();
}

void search_select_down(void)
{
	/* bottom reached, not wrapping around */
	if (search_current + 1 == search_found)
		return;
    
	invert_selection(search_current, search_current + 1);
	++search_current;
}

void search_select_up(void)
{
	/* top reached, not wrapping around */
	if (search_current <= 0)
		return;

	invert_selection(search_current, search_current - 1);
	--search_current;
}

const char *search_current_target(void)
{
	if (search_current < 0 || search_current >= search_found)
		return NULL;

	return result_list.list[search_current]+(strlen(result_list.list[search_current])-TARGET_SIZE);
}

const char *search_current_title(void)
{
	if (search_found < 0 || search_current >= search_found)
		return NULL;

	return result_list.list[search_current];
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
	int i;
	int result_start = PIXEL_START;

	for (i = 0; i < search_found; ++i, result_start += RESULT_HEIGHT) {
		if (y >= result_start && y < result_start + RESULT_HEIGHT) {
			search_current = i;
			return search_current_target();
		}
	}

	return NULL;
}
