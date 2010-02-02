/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Holger Hans Peter Freyther <zecke@openmoko.org>
 *           Matt Hsu <matt_hsu@openmoko.org>
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

#include <string.h>
#include <wikilib.h>
#include <guilib.h>
#include <glyph.h>
#include <search.h>
#include <stdlib.h>
#include <file-io.h>
#include <guilib.h>
#include <lcd.h>
#include "history.h"
#include "search.h"
#include "msg.h"
#include "lcd_buf_draw.h"
#include "wiki_info.h"
#ifndef INCLUDED_FROM_KERNEL
#include <stdio.h>
#include <errno.h>
#endif

#define DBG_HISTORY 0

#define HISTORY_MAX_ITEM	19
#define HISTORY_MAX_DISPLAY_ITEM	18U

HISTORY history_list[MAX_HISTORY];
int history_count = 0;
int rendered_history_count = -1;
int history_changed = HISTORY_SAVE_NONE;
extern int display_mode;

static inline unsigned int history_modulus(int modulus) {
	return modulus % HISTORY_MAX_DISPLAY_ITEM;
}

long history_get_previous_idx(long idx_article)
{
	int i = 0;
	int bFound = 0;

	if (!(idx_article & 0xFF000000)) // idx_article for current wiki
	{
		idx_article |= get_wiki_id_from_idx(nCurrentWiki) << 24;
	}

	while (!bFound && i < history_count)
	{
		if (idx_article == history_list[i].idx_article)
		{
			bFound = 1;
		}
		else
			i++;
	}

	if(bFound)
	{
		return history_list[i].idx_prev_article;
	}
	else
		return 0;
}

void history_reload()
{
	rendered_history_count = 0;
	render_history_with_pcf();
}

void history_add(long idx_article, long idx_prev_article, const char *title, int b_keep_pos)
{
	int i = 0;
	int bFound = 0;

	if (!(idx_article & 0xFF000000)) // idx_article for current wiki
	{
		idx_article |= get_wiki_id_from_idx(nCurrentWiki) << 24;
	}

	history_changed = HISTORY_SAVE_NORMAL;
	while (!bFound && i < history_count)
	{
		if (idx_article == history_list[i].idx_article)
		{
			HISTORY history_tmp;
			history_tmp = history_list[i];
			if (!b_keep_pos)
				history_tmp.last_y_pos = 0;
			memrcpy((void*)&history_list[1],(void*)&history_list[0],sizeof(HISTORY)*i);
			history_list[0]=history_tmp;
			if (idx_prev_article)
				history_list[0].idx_prev_article = idx_prev_article;
			bFound = 1;
		}
		else
			i++;
	}

	if(bFound)
		return;

	if (history_count >= MAX_HISTORY)
		history_count = MAX_HISTORY - 1;
	memrcpy((void*)&history_list[1],(void*)&history_list[0],sizeof(HISTORY)*history_count);
	history_list[0].idx_article = idx_article;
	history_list[0].idx_prev_article = idx_prev_article;
	strcpy(history_list[0].title, title);
	history_list[0].last_y_pos = 0;
	history_count++;
}

void history_log_y_pos(const long y_pos)
{
	if (history_changed != HISTORY_SAVE_NORMAL)
		history_changed = HISTORY_SAVE_POWER_OFF;
	history_list[0].last_y_pos = y_pos;
}

long history_get_y_pos(const long idx_article)
{
	int i = 0;

	while (i < history_count)
	{
		if (idx_article == history_list[i].idx_article)
		{
			return history_list[i].last_y_pos;
		}
		else
			i++;
	}
	return 0;
}

unsigned int history_get_count()
{
	return history_count;
}

void history_clear()
{
	history_count = 0;
	history_changed = HISTORY_SAVE_NORMAL;
}

void history_list_init(void)
{
	unsigned int len;
	int fd_hst;

	history_count = 0;
	fd_hst = wl_open("wiki.hst", WL_O_RDONLY);
	if (fd_hst >= 0)
	{
		while ((len = wl_read(fd_hst, (void *)&history_list[history_count], sizeof(HISTORY))) >= sizeof(HISTORY))
		{
			history_count++;
		}
		wl_close(fd_hst);
	}
}

int history_list_save(int level)
{
	int fd_hst;
	int rc = 0;

	if (history_changed != HISTORY_SAVE_NONE)
	{
		if (level == HISTORY_SAVE_POWER_OFF || history_changed == HISTORY_SAVE_NORMAL)
		{
			fd_hst = wl_open("wiki.hst", WL_O_CREATE);
			if (fd_hst >= 0)
			{
				wl_write(fd_hst, (void *)history_list, sizeof(HISTORY) * history_count);
				wl_close(fd_hst);
			}
			history_changed = HISTORY_SAVE_NONE;
			rc = 1;
		}
		else
			rc = -1;
	}
	return rc;
}

void draw_clear_history(int bClear)
{
	int i;
	static char localBuffer[27 * LCD_VRAM_WIDTH_PIXELS / 8];
	unsigned char *pText;

	if (bClear)
	{
		memcpy(&framebuffer[181 * LCD_VRAM_WIDTH_PIXELS / 8], localBuffer, 27 * LCD_VRAM_WIDTH_PIXELS / 8);
	}
	else
	{
		memcpy(localBuffer, &framebuffer[181 * LCD_VRAM_WIDTH_PIXELS / 8], 27 * LCD_VRAM_WIDTH_PIXELS / 8);
		memset(&framebuffer[181 * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF,  27 * LCD_VRAM_WIDTH_PIXELS / 8);

		framebuffer[184 * LCD_VRAM_WIDTH_PIXELS / 8 + 18] = 0xFE;
		memset(&framebuffer[184 * LCD_VRAM_WIDTH_PIXELS / 8 + 19], 0, 4);
		framebuffer[184 * LCD_VRAM_WIDTH_PIXELS / 8 + 23] = 0x07;

		framebuffer[184 * LCD_VRAM_WIDTH_PIXELS / 8 + 24] = 0xF8;
		memset(&framebuffer[184 * LCD_VRAM_WIDTH_PIXELS / 8 + 25], 0, 4);
		framebuffer[184 * LCD_VRAM_WIDTH_PIXELS / 8 + 29] = 0x1F;

		for (i = 185; i <= 203; i++)
		{
			framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 18] = 0xFC;
			memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 19], 0, 4);
			framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 23] = 0x03;

			framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 24] = 0xF0;
			memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 25], 0, 4);
			framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 29] = 0x0F;
		}

		framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 18] = 0xFE;
		memset(&framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 19], 0, 4);
		framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 23] = 0x07;

		framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 24] = 0xF8;
		memset(&framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 25], 0, 4);
		framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 29] = 0x1F;

		pText=get_nls_text("clear_history");
		render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, 185, pText, strlen(pText), 1);
		pText=get_nls_text("yes");
		render_string(SUBTITLE_FONT_IDX, 147 + (192 - 147 - get_external_str_pixel_width(pText, SUBTITLE_FONT_IDX)) / 2,
			      185, pText, strlen(pText), 0);
		pText=get_nls_text("no");
		render_string(SUBTITLE_FONT_IDX, 193 + (238 - 193 - get_external_str_pixel_width(pText, SUBTITLE_FONT_IDX)) / 2,
			      185, pText, strlen(pText), 0);
	}
}
