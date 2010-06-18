/*
 * guilib - a minimal pixel framework
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Holger Hans Peter Freyther <zecke@openmoko.org>
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

/* wiki-lib includes */
#include <wikilib.h>
#include <file-io.h>

/* gui-lib includes */
#include "guilib.h"
#include "glyph.h"
#include <regs.h>
#include <lcd.h>
#include <samo.h>
#include "lcd_buf_draw.h"
#include "search.h"

#include "msg.h"

#define DBG_GLYPH 0

void render_glyph(int start_x, int start_y, const struct glyph *glyph, char *buf)
{
	int x, y, w, bit = 0;
	const char *d = glyph->data;

	for (y = start_y; y < start_y + glyph->height; y++) {
		for (x = start_x, w = glyph->width; w > 0;) {
			int use;
			unsigned byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;

			if (byte >= LCD_BUF_HEIGHT_PIXELS * LCD_BUF_WIDTH_BYTES)
				return;

			use = MIN(8 - (x % 8) , w);
			use = MIN(8 - bit, use);
#ifdef DISPLAY_INVERTED
			buf[byte] &= ~((*d << bit & (unsigned char)(0xff << (8 - use))) >> (x % 8));
#else
			buf[byte] |= (*d << bit & (unsigned char)(0xff << (8 - use))) >> (x % 8);
#endif
			bit += use;
			x += use;
			w -= use;
			if (bit == 8) {
				bit = 0;
				d++;
			}
		}
	}
}

int buf_render_string(char *buf, const int font, int start_x,
		  int start_y, char *string, int text_length, int inverted)
{
	int x;
	int width;
	long len = text_length;
	long lenLast = 0;
	long widthLast = 0;
	char *p = (char *)string;
	unsigned char *q;
	int nCharBytes;
	ucs4_t c;

	if (start_x < 0)
		width = 0;
	else
		width = start_x;
	while (len > 0 && width < LCD_BUF_WIDTH_PIXELS)
	{
		lenLast = len;
		widthLast = width;
		width += get_UTF8_char_width(font, &p, &len, &nCharBytes);
	}
	if (width > LCD_BUF_WIDTH_PIXELS)
	{
		text_length -= lenLast;
		width = widthLast;
	}

	if (start_x < 0) // to be centered
	{
		start_x = (LCD_BUF_WIDTH_PIXELS - width) / 2;
		if (start_x < 0)
			start_x = 0;
	}

	x = start_x;
	q = (unsigned char *)string;
	while (*q) {
		c = UTF8_to_UCS4(&q);
		x = buf_draw_bmf_char(buf, c,font-1,x,start_y, inverted);
		if(x<0)
			return 0;
	}
	return x;
}

int render_string(const int font, int start_x,
		  int start_y, char *string, int text_length, int inverted)
{
	int x;
	int width;
	long len = text_length;
	long lenLast = 0;
	long widthLast = 0;
	char *p = (char *)string;
	unsigned char *q;
	int nCharBytes;
	ucs4_t c;

	if (start_x < 0)
		width = 0;
	else
		width = start_x;
	while (len > 0 && width < LCD_BUF_WIDTH_PIXELS)
	{
		lenLast = len;
		widthLast = width;
		width += get_UTF8_char_width(font, &p, &len, &nCharBytes);
	}
	if (width > LCD_BUF_WIDTH_PIXELS)
	{
		text_length -= lenLast;
		width = widthLast;
	}

	if (start_x < 0) // to be centered
	{
		start_x = (LCD_BUF_WIDTH_PIXELS - width) / 2;
		if (start_x < 0)
			start_x = 0;
	}

	x = start_x;
	q = (unsigned char *)string;
	while (*q) {
		c = UTF8_to_UCS4(&q);
		x = draw_bmf_char(c,font-1,x,start_y, inverted, 0);
		if(x<0)
			return 0;
	}
	return x;
}

int render_string_and_clear(const int font, int start_x,
		  int start_y, char *string, int text_length, int inverted,
		  int clear_start_x, int clear_start_y, int clear_end_x, int clear_end_y)
{
	int x;
	int width;
	int height;
	long len = text_length;
	long lenLast = 0;
	long widthLast = 0;
	char *p = (char *)string;
	unsigned char *q;
	int nCharBytes;
	ucs4_t c;

	if (clear_start_x >= 0 && clear_start_y < start_y)
	{
		if (clear_end_y < start_y)
		{
			guilib_clear_area(clear_start_x, clear_start_y, clear_end_x, clear_end_y);
			clear_start_x = -1; // no more area to clear
		}
		else
		{
			guilib_clear_area(clear_start_x, clear_start_y, clear_end_x, start_y - 1);
			clear_start_y = start_y;
		}
	}

	if (start_x < 0)
		width = 0;
	else
		width = start_x;
	height = GetFontLinespace(font);
	while (len > 0 && width < LCD_BUF_WIDTH_PIXELS)
	{
		lenLast = len;
		widthLast = width;
		width += get_UTF8_char_width(font, &p, &len, &nCharBytes);
	}
	if (width > LCD_BUF_WIDTH_PIXELS)
	{
		text_length -= lenLast;
		width = widthLast;
	}

	if (start_x < 0) // to be centered
	{
		start_x = (LCD_BUF_WIDTH_PIXELS - width) / 2;
		if (start_x < 0)
			start_x = 0;
	}

	if (clear_start_y < start_y + height)
	{
		if (clear_start_x < start_x)
			guilib_clear_area(clear_start_x, clear_start_y, start_x - 1,
				clear_end_y < start_y + height ? clear_end_y : start_y + height - 1);
		if (clear_end_x >= width)
			guilib_clear_area(width, clear_start_y, clear_end_x,
				clear_end_y < start_y + height ? clear_end_y : start_y + height - 1);
		if (clear_end_y >= start_y + height)
			clear_start_y = start_y + height;
		else
			clear_start_x = -1;
	}

	if (clear_start_x >= 0)
		guilib_clear_area(clear_start_x, clear_start_y, clear_end_x, clear_end_y);

	x = start_x;
	q = (unsigned char *)string;
	while (*q) {
		c = UTF8_to_UCS4(&q);
		x = draw_bmf_char(c,font-1,x,start_y, inverted, 1);
		if(x<0)
			return 0;
	}
	return x;
}

// if search string is longer than the LCD width, keep the right of it to fit
int render_string_right(const int font, int start_x,
			int start_y, char *string, int text_length, int inverted)
{
	int i;
	int x;
	int utf8_chars = 0;
	int widths[MAX_TITLE_SEARCH];
	int lens[MAX_TITLE_SEARCH];
	int width = start_x;
	long len = text_length;
	char *p = (char *)string;
	unsigned char *q;
	int nCharBytes;
	int rc;
	ucs4_t c;

	while (len > 0 && utf8_chars < MAX_TITLE_SEARCH)
	{
		lens[utf8_chars] = len;
		widths[utf8_chars] = get_UTF8_char_width(font, &p, &len, &nCharBytes);
		width += widths[utf8_chars];
		utf8_chars++;
	}

	rc = width;
	if (width > LCD_BUF_WIDTH_PIXELS)
	{
		int width_to_descrease = width - LCD_BUF_WIDTH_PIXELS;

		width = 0;
		for (i = 0; i < utf8_chars && width < width_to_descrease; i++)
			width += widths[i];
		if (i < utf8_chars)
		{
			string = &string[text_length - lens[i]];
			text_length = lens[i];
		}
	}

	x = start_x;
	q = (unsigned char *)string;
	while (*q) {
		c = UTF8_to_UCS4(&q);
		x = draw_bmf_char(c,font-1,x,start_y, inverted, 1);
		if(x<0)
			return 0;
	}
	guilib_clear_area(x, start_y, LCD_BUF_WIDTH_PIXELS - 1, start_y + GetFontLinespace(font));
	return rc;
}
