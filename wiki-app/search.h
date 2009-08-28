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
//#define RESULT_START 20
#define RESULT_START 35
//#define RESULT_HEIGHT 10
#define RESULT_HEIGHT 19
#define MAX_RESULTS 20
#define MAX_TITLE_SEARCH 64
#define NUMBER_OF_RESULTS 9
#define NUMBER_OF_RESULTS_KEYBOARD 5
#define PIXEL_START (RESULT_START - RESULT_HEIGHT + 2)
/* MAX_DAT_FILES cannot be less than the number of batches in the rendering process */
#define MAX_DAT_FILES 16
#define MAX_COMPRESSED_ARTICLE 256*1024

typedef struct _ARTICLE_PTR {
	long offset_dat;	/* offset to pedia?.dat for the article content */
	long offset_fnd;	/* offset to pedia.fnd for the title (for search) */
	long file_id_compressed_len; 	/* byte 0: bit0~1 - compress method (00 - lzo, 01 - bzlib, 10 - 7z), bit 2~7 pedia file id */
					/* byte 1~3: compressed article length */
} ARTICLE_PTR;

typedef struct _TITLE_SEARCH { /* used to mask the porinter to the remainder of the title for search */
	long idxArticle;
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

void search_set_selection(int new_selection);

void search_open_article(int new_selection);

int set_result_list_base(int offset,int offset_count);

#endif
