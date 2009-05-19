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

#ifndef WL_SEARCH_H
#define WL_SEARCH_H

#define TARGET_SIZE 6 
#define RESULT_START 20
#define RESULT_HEIGHT 10
#define MAX_RESULTS 30
#define NUMBER_OF_RESULTS 19
#define NUMBER_OF_RESULTS_KEYBOARD 11
#define PIXEL_START (RESULT_START - RESULT_HEIGHT + 2)

/*
 * Highlevel search interface...
 */
void search_select_down(void);
void search_select_up(void);
const char *search_current_title(void);
const char *search_release(int y);

/**
 * Initialize the search engine. Once.
 */
void search_init();

/**
 * Load trigrams. return if done..
 */
int search_load_trigram(void);

/**
 * Repaint, reselect the current screen..
 */
void search_reload(void);

/**
 * Search for another char. It needs to be lower case
 */
void search_add_char(char c);

/**
 * Remove the last char from the search
 */
void search_remove_char(void);

/**
 * Return search result count
 */
unsigned int search_result_count();

/**
 * Return the index of search result currently selected item
 */
int search_result_selected();

/**
 * Return the index of the first item displayed on the screen
 */
unsigned int search_result_first_item();

void invert_selection(int old_pos, int new_pos);

void search_set_selection(int new_selection);

#endif
