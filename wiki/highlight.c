/*
 * Copyright (c) 2010 Openmoko Inc.
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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>

#include "ustring.h"
#include "highlight.h"
#include "lcd_buf_draw.h"
#include "search.h"
#include "wikilib.h"
#include "glyph.h"
#include "guilib.h"

#define HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT 30
#define HIGHLIGHT_SEARCH_BOX_STEM_WIDTH 9
#define HIGHLIGHT_SEARCH_BOX_X_MARGIN 5
#define HIGHLIGHT_SEARCH_BOX_Y_MARGIN 5
#define HIGHLIGHT_SEARCH_BOX_MAX_LANGUAGE_LINKS 8
#define HIGHLIGHT_SEARCH_BOX_HEIGHT					\
	(LANGUAGE_LINK_HEIGHT + HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT +	\
	 HIGHLIGHT_SEARCH_BOX_Y_MARGIN * 2) + 1
#define HIGHLIGHT_SEARCH_BOX_WIDTH					\
	(HIGHLIGHT_SEARCH_BOX_MAX_LANGUAGE_LINKS * LANGUAGE_LINK_WIDTH + \
	 (HIGHLIGHT_SEARCH_BOX_MAX_LANGUAGE_LINKS - 1) * LANGUAGE_LINK_WIDTH_GAP + \
	 HIGHLIGHT_SEARCH_BOX_X_MARGIN * 2)

int highlight_start_x = -1;
int highlight_start_y = -1;
int highlight_end_x = -1;
int highlight_end_y = -1;
unsigned char highlight_search_string_actual[MAX_TITLE_ACTUAL];
int highlight_invert_start_x = -1;
int highlight_invert_end_x = -1;
int highlight_invert_start_y_top = -1;
int highlight_invert_start_y_bottom = -1;
int highlight_invert_end_y_top = -1;
int highlight_invert_end_y_bottom = -1;
bool b_highlight_search_box_enabled = false;
int highlight_search_box_start_x = -1;
int highlight_search_box_start_y = -1;
int highlight_search_box_stem_x = -1;
int highlight_search_box_stem_y = -1;
int highlight_search_box_search_string_y = -1;

void window_draw_line(int start_x, int start_y, int end_x, int end_y)
{
	lcd_window_move_to(start_x, start_y);
	lcd_window_line_to(end_x, end_y);
}

void window_draw_box(int start_x, int start_y, int end_x, int end_y)
{
	int i;

	for (i = 0; i < 2; i++)
	{
		window_draw_line(start_x + 2, start_y + i, end_x - 2, start_y + i);
		window_draw_line(start_x + 2, end_y - i, end_x - 2, end_y - i);
		window_draw_line(start_x + i, start_y + 2, start_x + i, end_y - 2);
		window_draw_line(end_x - i, start_y + 2, end_x - i, end_y - 2);
		window_draw_line(start_x + 2, start_y + i, start_x + i, start_y + 2);
		window_draw_line(start_x + 2, end_y - i, start_x + i, end_y - 2);
		window_draw_line(end_x - 2, start_y + i, end_x - i, start_y + 2);
		window_draw_line(end_x - 2, end_y - i, end_x - i, end_y - 2);
	}
}

void highlight_reset(int x, int y, bool bRepaint)
{
	lcd_draw_highlight(0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, bRepaint);
	wikilib_reset_highlighting();
	highlight_start_x = x;
	highlight_start_y = y;
	highlight_end_x = -1;
	highlight_end_y = -1;
	if (b_highlight_search_box_enabled)
	{
		b_highlight_search_box_enabled = false;
		lcd_window_disable();
		highlight_search_box_start_x = -1;
		highlight_search_box_start_y = -1;
		highlight_search_box_stem_x = -1;
		highlight_search_box_stem_y = -1;
	}
}

void highlight_search_box_draw_stem()
{
	int end_y;
	int end_x1, end_x2;
	int x;
	int i;

	end_x1 = highlight_search_box_stem_x - HIGHLIGHT_SEARCH_BOX_STEM_WIDTH / 2;
	if (end_x1 < highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_X_MARGIN)
		end_x1 = highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_X_MARGIN;
	end_x2 = end_x1 + HIGHLIGHT_SEARCH_BOX_STEM_WIDTH - 1;
	if (end_x2 > highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_WIDTH - HIGHLIGHT_SEARCH_BOX_X_MARGIN)
	{
		end_x2 = highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_WIDTH - HIGHLIGHT_SEARCH_BOX_X_MARGIN;
		end_x1 = end_x2 - HIGHLIGHT_SEARCH_BOX_STEM_WIDTH + 1;
	}
	lcd_window_set_colour(LCD_WHITE);
	if (highlight_search_box_stem_y == 0)
	{ // search box below stem
		end_y = HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT - 1;
		for (x = end_x1 + 3; x < end_x2 - 2; x++)
		{
			lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
			lcd_window_line_to(x, end_y + 2);
		}
		lcd_window_set_pixel(end_x1 + 2, end_y + 2, LCD_WHITE);
		lcd_window_set_pixel(end_x2 - 2, end_y + 2, LCD_WHITE);
	}
	else
	{
		end_y = HIGHLIGHT_SEARCH_BOX_HEIGHT - HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT + 1;
		for (x = end_x1 + 3; x < end_x2 - 2; x++)
		{
			lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
			lcd_window_line_to(x, end_y - 2);
		}
		lcd_window_set_pixel(end_x1 + 2, end_y - 2, LCD_WHITE);
		lcd_window_set_pixel(end_x2 - 2, end_y - 2, LCD_WHITE);
	}
	lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
	lcd_window_line_to(end_x1 - 1, end_y);
	lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
	lcd_window_line_to(end_x1 - 2, end_y);
	lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
	lcd_window_line_to(end_x2 + 1, end_y);
	lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
	lcd_window_line_to(end_x2 + 2, end_y);

	lcd_window_set_colour(LCD_BLACK);
	for (i = 0; i < 3; i++)
	{
		lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
		lcd_window_line_to(end_x1 + i, end_y);
		lcd_window_move_to(highlight_search_box_stem_x, highlight_search_box_stem_y);
		lcd_window_line_to(end_x2 - i, end_y);
	}
}

void highlight_search_box_redraw_stem()
{
	int i;
	unsigned char *draw_buffer = lcd_draw_get_cur_buffer();
	unsigned char *window_buffer = lcd_window_get_buffer();
	int window_width_bytes = lcd_window_get_byte_width();
	int box_start_x, box_start_y, box_end_x, box_end_y;

	if (highlight_search_box_stem_y == 0)
	{ // search box below stem
		for (i = 0; i < HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT; i++)
			memcpy(&window_buffer[i * window_width_bytes],
			       &draw_buffer[(i + highlight_search_box_start_y) * LCD_BUF_WIDTH_BYTES],
			       window_width_bytes);
		box_start_x = highlight_search_box_start_x;
		box_start_y = HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT;
		box_end_x = highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_WIDTH - 1;
		box_end_y = HIGHLIGHT_SEARCH_BOX_HEIGHT - 1;
	}
	else
	{
		int y_diff = HIGHLIGHT_SEARCH_BOX_HEIGHT - HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT;
		for (i = 0; i < HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT; i++)
			memcpy(&window_buffer[(y_diff + i) * window_width_bytes],
			       &draw_buffer[(i + y_diff + highlight_search_box_start_y) * LCD_BUF_WIDTH_BYTES],
			       window_width_bytes);
		box_start_x = highlight_search_box_start_x;
		box_start_y = 1;
		box_end_x = highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_WIDTH - 1;
		box_end_y = HIGHLIGHT_SEARCH_BOX_HEIGHT - HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT;
	}
	window_draw_box(box_start_x, box_start_y, box_end_x, box_end_y);
	highlight_search_box_draw_stem();
}

void highlight_search_box_init()
{
	unsigned char *draw_buffer = lcd_draw_get_cur_buffer();
	unsigned char *window_buffer = lcd_window_get_buffer();
	int window_width_bytes = lcd_window_get_byte_width();
	int i;
	int box_start_x, box_start_y, box_end_x, box_end_y;

	for (i = 0; i < HIGHLIGHT_SEARCH_BOX_HEIGHT; i++)
		memcpy(&window_buffer[i * window_width_bytes],
		       &draw_buffer[(i + highlight_search_box_start_y) * LCD_BUF_WIDTH_BYTES],
		       window_width_bytes);
	if (highlight_search_box_stem_y == 0)
	{ // search box below stem
		box_start_x = highlight_search_box_start_x;
		box_start_y = HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT;
		box_end_x = highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_WIDTH - 1;
		box_end_y = HIGHLIGHT_SEARCH_BOX_HEIGHT - 1;
	}
	else
	{
		box_start_x = highlight_search_box_start_x;
		box_start_y = 1;
		box_end_x = highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_WIDTH - 1;
		box_end_y = HIGHLIGHT_SEARCH_BOX_HEIGHT - HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT;
	}
	guilib_buffer_clear_area(window_buffer,
				 LCD_WIDTH, HIGHLIGHT_SEARCH_BOX_HEIGHT, window_width_bytes,
				 box_start_x - 1, box_start_y - 1, box_end_x + 1, box_end_y + 1);
	window_draw_box(box_start_x, box_start_y, box_end_x, box_end_y);
	highlight_search_box_draw_stem();
}

void highlight_search_box_redraw_search_string()
{
	unsigned char *window_buffer = lcd_window_get_buffer();
	int window_width_bytes = lcd_window_get_byte_width();

	buf_render_string_right(window_buffer, HIGHLIGHT_SEARCH_BOX_WIDTH - HIGHLIGHT_SEARCH_BOX_X_MARGIN * 2,
				HIGHLIGHT_SEARCH_BOX_HEIGHT - HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT - 2, window_width_bytes, SUBTITLE_FONT_IDX,
				highlight_search_box_start_x + HIGHLIGHT_SEARCH_BOX_X_MARGIN, highlight_search_box_search_string_y,
				highlight_search_string_actual, ustrlen(highlight_search_string_actual), 0);
}

void draw_highlight_search_string()
{
	int new_highlight_search_box_start_x;
	int new_highlight_search_box_start_y;
	int new_highlight_search_box_stem_x;
	int new_highlight_search_box_stem_y;

	if (highlight_invert_start_y_top > HIGHLIGHT_SEARCH_BOX_HEIGHT + LANGUAGE_LINK_HEIGHT_GAP)
	{
		new_highlight_search_box_start_y = highlight_invert_start_y_top - HIGHLIGHT_SEARCH_BOX_HEIGHT;
		new_highlight_search_box_stem_x = highlight_invert_start_x;
		new_highlight_search_box_stem_y = HIGHLIGHT_SEARCH_BOX_HEIGHT;
		highlight_search_box_search_string_y = HIGHLIGHT_SEARCH_BOX_Y_MARGIN;
	}
	else if (highlight_invert_end_y_bottom < LCD_HEIGHT - HIGHLIGHT_SEARCH_BOX_HEIGHT - LANGUAGE_LINK_HEIGHT_GAP)
	{
		new_highlight_search_box_start_y = highlight_invert_end_y_bottom + 1;
		new_highlight_search_box_stem_x = highlight_invert_end_x;
		new_highlight_search_box_stem_y = 0;
		highlight_search_box_search_string_y = HIGHLIGHT_SEARCH_BOX_STEM_HEIGHT + HIGHLIGHT_SEARCH_BOX_Y_MARGIN;
	}
	else if (highlight_invert_end_y_top > HIGHLIGHT_SEARCH_BOX_HEIGHT + LANGUAGE_LINK_HEIGHT_GAP)
	{
		new_highlight_search_box_start_y = highlight_invert_end_y_top - HIGHLIGHT_SEARCH_BOX_HEIGHT;
		new_highlight_search_box_stem_x = highlight_invert_end_x;
		new_highlight_search_box_stem_y = HIGHLIGHT_SEARCH_BOX_HEIGHT;
		highlight_search_box_search_string_y = HIGHLIGHT_SEARCH_BOX_Y_MARGIN;
	}
	else
	{
		new_highlight_search_box_start_y = LANGUAGE_LINK_HEIGHT_GAP;
		new_highlight_search_box_stem_x = highlight_invert_start_x;
		new_highlight_search_box_stem_y = HIGHLIGHT_SEARCH_BOX_HEIGHT;
		highlight_search_box_search_string_y = HIGHLIGHT_SEARCH_BOX_Y_MARGIN;
	}
	new_highlight_search_box_start_x = (LCD_WIDTH - HIGHLIGHT_SEARCH_BOX_WIDTH) / 2;

	if (!b_highlight_search_box_enabled)
	{
		highlight_search_box_start_x = new_highlight_search_box_start_x;
		highlight_search_box_start_y = new_highlight_search_box_start_y;
		highlight_search_box_stem_x = new_highlight_search_box_stem_x;
		highlight_search_box_stem_y = new_highlight_search_box_stem_y;
		lcd_window(0, highlight_search_box_start_y,
			   LCD_WIDTH, HIGHLIGHT_SEARCH_BOX_HEIGHT);
		highlight_search_box_init();
		lcd_window_enable();
		b_highlight_search_box_enabled = true;
	}
	else if (highlight_search_box_start_x != new_highlight_search_box_start_x ||
		 highlight_search_box_start_y != new_highlight_search_box_start_y)
	{
		highlight_search_box_start_x = new_highlight_search_box_start_x;
		highlight_search_box_start_y = new_highlight_search_box_start_y;
		highlight_search_box_stem_x = new_highlight_search_box_stem_x;
		highlight_search_box_stem_y = new_highlight_search_box_stem_y;
		lcd_window_disable();
		lcd_window(0, highlight_search_box_start_y,
			   LCD_WIDTH, HIGHLIGHT_SEARCH_BOX_HEIGHT);
		highlight_search_box_init();
		lcd_window_enable();
	}
	else if (highlight_search_box_stem_x != new_highlight_search_box_stem_x ||
		 highlight_search_box_stem_y != new_highlight_search_box_stem_y)
	{
		highlight_search_box_stem_x = new_highlight_search_box_stem_x;
		highlight_search_box_stem_y = new_highlight_search_box_stem_y;
		highlight_search_box_redraw_stem();
	}
	highlight_search_box_redraw_search_string();
}

bool highlight_set(int x, int y)
{
	bool rc;

	highlight_end_x = x;
	highlight_end_y = y;
	rc = lcd_draw_highlight(highlight_start_x, highlight_start_y, highlight_end_x, highlight_end_y,
				&highlight_invert_start_x, &highlight_invert_end_x,
				&highlight_invert_start_y_top, &highlight_invert_start_y_bottom, &highlight_invert_end_y_top, &highlight_invert_end_y_bottom,
				highlight_search_string_actual, true);
	draw_highlight_search_string();
	return rc;
}

void highlight_handle_search()
{
}
