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

#define MAX_WIKIS 3
#define MAX_WIKIS_PER_DEVICE 3
#define MAX_LINKS_IN_LICENSE_TEXT 4
#define LICENSE_LINK_START '['
#define LICENSE_LINK_END ']'
#define MAX_LICENSE_TEXT_LEN 1024
#define MAX_LICENSE_TEXT_PIXEL_LINES 512

typedef struct _WIKI_LIST {
	int wiki_id;
	char wiki_folder[10];
} WIKI_LIST;

typedef struct _WIKI_LICENSE_DRAW {
	char *buf;
	int lines;
	int link_count;
	ARTICLE_LINK links[MAX_LINKS_IN_LICENSE_TEXT];
} WIKI_LICENSE_DRAW;

extern int nCurrentWiki;
void init_wiki_info(void);
int get_wiki_count(void);
unsigned char *get_nls_text(char *key);
char *get_wiki_file_path(int nWikiIdx, char *file_name);
char *get_wiki_name(int idx);
void wiki_selection(void);
void set_wiki(int idx);
int get_wiki_idx_from_id(int wiki_id);
int get_wiki_id_from_idx(int wiki_idx);
WIKI_LICENSE_DRAW *wiki_license_draw(void);

#endif
