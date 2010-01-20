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

#ifndef WL_SEARCH_H
#define WL_SEARCH_H

#include <inttypes.h>

#define TARGET_SIZE 6
#define RESULT_START 34
#define RESULT_HEIGHT 19
#define MAX_TITLE_SEARCH 64
#define NUMBER_OF_FIRST_PAGE_RESULTS 9
#define NUMBER_OF_RESULTS_KEYBOARD 5
#define PIXEL_START (RESULT_START - RESULT_HEIGHT + 2)
/* MAX_DAT_FILES cannot be less than the number of batches in the rendering process */
#define MAX_DAT_FILES 64
#define MAX_COMPRESSED_ARTICLE 256*1024

enum {
	SEARCH_RELOAD_NORMAL,
	SEARCH_RELOAD_KEEP_RESULT,
	SEARCH_RELOAD_NO_POPULATE,
	SEARCH_RELOAD_KEEP_REFRESH,
};

enum {
	SEARCH_TO_BE_RELOADED_CLEAR,
	SEARCH_TO_BE_RELOADED_SET,
	SEARCH_TO_BE_RELOADED_CHECK,
};

typedef struct _ARTICLE_PTR {
	uint32_t offset_dat;	/* offset to pedia?.dat for the article content */
	uint32_t offset_fnd;	/* offset to pedia.fnd for the title (for search) */
	uint32_t file_id_compressed_len; 	/* byte 0: bit0~1 - compress method (00 - lzo, 01 - bzlib, 10 - 7z), bit 2~7 pedia file id */
					/* byte 1~3: compressed article length */
} ARTICLE_PTR;

typedef struct _TITLE_SEARCH { /* used to mask the porinter to the remainder of the title for search */
	uint32_t idxArticle;
	char cZero; /* null character for backward search */
	char sTitleSearch[MAX_TITLE_SEARCH]; /* null terminated title for search (with bigram encoded) */
} TITLE_SEARCH;

/*
 * Highlevel search interface...
 */
void search_select_down(void);
void search_select_up(void);
int search_current_selection(void);
// const char *search_fetch_result();
int retrieve_article(long idx_article);
void memrcpy(char *dest, char *src, int len); // memory copy starting from the last byte
void random_article(void);
void get_article_title_from_idx(long idx, char *title);
long result_list_offset_next(void);
long result_list_next_result(long offset_next, long *idxArticle, char *sTitleSearch);

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
void search_reload(int flag);
void search_to_be_reloaded(int to_be_reloaded_flag, int reload_flag);

/**
 * Search for another char. It needs to be lower case
 */
int search_add_char(char c, unsigned long ev_time);

/**
 * Remove the last char from the search
 */
int search_remove_char(int bPopulate, unsigned long ev_time);

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

void search_set_selection(int new_selection);

void search_open_article(int new_selection);
int fetch_search_result(long input_offset_fnd_start, long input_offset_fnd_end, int bInit);

void search_fetch();
void search_result_display();
int clear_search_string();
int  get_search_string_len();
int check_search_string_change(void);
#endif
