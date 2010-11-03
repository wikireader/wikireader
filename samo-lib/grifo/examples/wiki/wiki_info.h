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

#ifndef WIKI_INFO_H
#define WIKI_INFO_H

#include "lcd_buf_draw.h"
#include "keyboard.h"

#define MAX_LINKS_IN_LICENSE_TEXT 4
#define LICENSE_LINK_START '['
#define LICENSE_LINK_END ']'
#define NLS_TEXT_REPLACEMENT_START '<'
#define NLS_TEXT_REPLACEMENT_END '>'
#define MAX_LICENSE_TEXT_LEN 1024
#define MAX_LICENSE_TEXT_PIXEL_LINES 512
typedef enum {
	WIKI_CAT_INVALID,
	WIKI_CAT_ENCYCLOPAEDIA,
	WIKI_CAT_TRAVEL,
	WIKI_CAT_DICTIONARY,
	WIKI_CAT_QUOTE,
	WIKI_CAT_SOURCE,
	WIKI_CAT_BOOKS,
	WIKI_CAT_GUTENBERG,
	WIKI_CAT_OTHERS
} WIKI_CAT_E;

typedef struct _WIKI_LIST {
	int wiki_serial_id;
	int wiki_id;
	WIKI_CAT_E wiki_cat;
	char wiki_lang[10];
	char wiki_folder[10];
	KEYBOARD_MODE wiki_default_keyboard;
	int wiki_nls_idx; // 0 for using wiki_name entry in wiki.nls, 2 for using wiki_name2, etc.
} WIKI_LIST;

typedef struct _WIKI_LICENSE_DRAW {
	char *buf;
	int lines;
	int link_count;
	ARTICLE_LINK links[MAX_LINKS_IN_LICENSE_TEXT];
} WIKI_LICENSE_DRAW, *PWIKI_LICENSE_DRAW;

typedef struct _ACTIVE_WIKI {
	int WikiInfoIdx; // index to wiki_info[]
	unsigned char *WikiNls;
	long WikiNlsLen;
} ACTIVE_WIKI, *PACTIVE_WIKI;

extern int nCurrentWiki;
bool wiki_lang_exist(const unsigned char *lang_link_str);
uint32_t wiki_lang_link_search(const unsigned char *lang_link_str);
void init_wiki_info(void);
int get_wiki_count(void);
const unsigned char *get_nls_text(const char *key);
const unsigned char *get_lang_link_display_text(const unsigned char *lang_link_str);
char *get_wiki_file_path(int nWikiIdx, char *file_name);
const unsigned char *get_wiki_name(int idx);
void wiki_selection(void);
void set_wiki(int idx);
int get_wiki_idx_from_id(int wiki_id);
int get_wiki_id_from_idx(unsigned int wiki_idx);
WIKI_LICENSE_DRAW *wiki_license_draw(void);
bool wiki_keyboard_conversion_needed();
bool wiki_is_TC();
bool wiki_is_japanese();
bool wiki_is_korean();
KEYBOARD_MODE wiki_default_keyboard();

#endif
