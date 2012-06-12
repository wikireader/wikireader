/*
 * Copyright (c) 2009 Openmoko Inc.
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
#include <string.h>
#include <inttypes.h>

#ifdef WIKIPCF
#include <wchar.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#else
#include <file-io.h>
#include <guilib.h>
#include <malloc-simple.h>
#include <msg.h>
#include <lcd.h>
#include <input.h>
#include "delay.h"
#include <tick.h>
#include "history.h"
#include "search.h"
#include "glyph.h"
#include "wikilib.h"
#include "restricted.h"
#include "wiki_info.h"
#endif

#include "bmf.h"
#include "lcd_buf_draw.h"
#include "search.h"
#include "bigram.h"
#include "utf8.h"

#define MAX_SCROLL_SECONDS 3
#define LIST_SCROLL_SPEED_FRICTION 0.3
#define ARTICLE_SCROLL_SPEED_FRICTION 0.3
#define SCROLL_UNIT_SECOND 0.1
#define LINK_INVERT_ACTIVATION_TIME_THRESHOLD 0.1
#define LIST_LINK_INVERT_ACTIVATION_TIME_THRESHOLD 0.35
#define RESTRICTED_MARK_LINK 0xFFFFFF
#define PREVIOUS_ARTICLE_LINK 0xFFFFFE
#define EXTERNAL_ARTICLE_LINK 0xFFFFFD
#define SHOW_LANGUAGE_LINK 0xFFFFFC
#define HIDE_LANGUAGE_LINK 0xFFFFFB
#define PREVIOUS_ARTICLE_LINKABLE_SIZE 20
#define ARTICLE_LINK_SOMETHING_BEFORE 0x20
#define ARTICLE_LINK_NOTHING_BEFORE 0x10
#define ARTICLE_LINK_SOMETHING_AFTER 0x02
#define ARTICLE_LINK_NOTHING_AFTER 0x01

#ifndef WIKIPCF
extern long finger_move_speed;
extern int last_display_mode;
extern int display_mode;
#endif
pcffont_bmf_t pcfFonts[FONT_COUNT];
static int lcd_draw_buf_inited = 0;
LCD_DRAW_BUF lcd_draw_buf;
unsigned char * file_buffer;
int restricted_article = 0;
int lcd_draw_buf_pos  = 0;
int lcd_draw_cur_y_pos = 0;
int lcd_draw_init_y_pos = 0;

int request_display_next_page = 0;
int request_y_pos = 0;
int cur_render_y_pos = 0;
int article_start_y_pos = 0;
int bShowLanguageLinks = 0;
int bShowPositioner = 0;

ARTICLE_LINK articleLink[MAX_ARTICLE_LINKS];
EXTERNAL_LINK externalLink[MAX_EXTERNAL_LINKS];
char articleLinkBeforeAfter[MAX_ARTICLE_LINKS];

#ifndef WIKIPCF
int link_to_be_activated = -1;
unsigned long link_to_be_activated_start_time = 0;
int link_to_be_inverted = -1;
unsigned long link_to_be_inverted_start_time = 0;
int link_currently_activated = -1;
int link_currently_inverted = -1;
#endif

int article_link_count;
int language_link_count;
int display_first_page = 0;

void display_link_article(long idx_article);
void drawline_in_framebuffer_copy(unsigned char *buffer,int start_x,int start_y,int end_x,int end_y);
void buf_draw_char_external(LCD_DRAW_BUF *lcd_draw_buf_external,ucs4_t u,int start_x,int end_x,int start_y,int end_y);
void repaint_framebuffer(unsigned char *buf, int pos, int b_repaint_invert_link);
void repaint_invert_link(void);
char* FontFile(int idx);
void msg_info(char *data);
int framebuffer_size();
int framebuffer_width();
int framebuffer_height();

unsigned char *framebuffer_copy;
char msg_out[1024];
extern unsigned char *framebuffer;
long article_scroll_increment;
unsigned long time_scroll_article_last=0;
int stop_render_article = 0;
int b_show_scroll_bar = 0;
long saved_idx_article = 0;
long saved_prev_idx_article = 0;

#define MIN_BAR_LEN 20
#ifndef WIKIPCF
void show_scroll_bar(int bShow)
{
	int bar_len;
	int bar_pos;
	int i;
	int byte_idx;
	char c;
	static char frame_bytes[LCD_HEIGHT_LINES];
	static int b_frame_bytes;

	if (lcd_draw_buf.current_y < LCD_HEIGHT_LINES)
		return;
	if (bShow <= 0)
	{
		if (b_frame_bytes)
		{
			if (!bShow && finger_move_speed == 0)
			{
#ifdef INCLUDED_FROM_KERNEL
				//delay_us(10000);
#endif
			}
			for (i = 0; i < LCD_HEIGHT_LINES; i++)
			{
				byte_idx = (236 + LCD_VRAM_WIDTH_PIXELS * i) / 8;
				framebuffer[byte_idx] = frame_bytes[i];
			}
			b_frame_bytes = 0;
		}
	}
	else
	{
		bar_len = LCD_HEIGHT_LINES * LCD_HEIGHT_LINES / lcd_draw_buf.current_y;
		if (bar_len > LCD_HEIGHT_LINES)
			bar_len = LCD_HEIGHT_LINES;
		else if (bar_len < MIN_BAR_LEN)
			bar_len = MIN_BAR_LEN;
		if (lcd_draw_buf.current_y > LCD_HEIGHT_LINES)
			bar_pos = (LCD_HEIGHT_LINES - bar_len) * lcd_draw_cur_y_pos / (lcd_draw_buf.current_y - LCD_HEIGHT_LINES);
		else
			bar_pos = 0;
		if (bar_pos < 0)
			bar_pos = 0;
		else if (bar_pos + bar_len > LCD_HEIGHT_LINES)
			bar_pos = LCD_HEIGHT_LINES - bar_len;

		for (i = 0; i < LCD_HEIGHT_LINES; i++)
		{
			if (bar_pos <= i && i < bar_pos + bar_len)
				c = 0x07;
			else
				c = 0;
			byte_idx = (236 + LCD_VRAM_WIDTH_PIXELS * i) / 8;
			frame_bytes[i] = framebuffer[byte_idx];
			framebuffer[byte_idx] = (framebuffer[byte_idx] & 0xF0) | c;
		}
		b_frame_bytes = 1;
	}
}
#endif

void init_lcd_draw_buf()
{
	int i,framebuffersize,fd;

	if (!lcd_draw_buf_inited)
	{
		framebuffersize = framebuffer_size();
		framebuffer_copy = (unsigned char*)Xalloc(framebuffersize);

		lcd_draw_buf.screen_buf = (unsigned char *)Xalloc(LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);

		for (i=0; i < FONT_COUNT; i++)
		{
			pcfFonts[i].file = FontFile(i);
			if (!pcfFonts[i].file[0])
			{
				pcfFonts[i].fd = -1;
			}
			else
			{
				fd = load_bmf(&pcfFonts[i]);
				if(fd >= 0)
				{
					if (i == ITALIC_FONT_IDX - 1)
					{
						pcfFonts[i].bPartialFont = 1;
						pcfFonts[i].supplement_font = &pcfFonts[DEFAULT_ALL_FONT_IDX - 1];
					}
					else if (i == DEFAULT_FONT_IDX - 1)
					{
						pcfFonts[i].bPartialFont = 1;
						pcfFonts[i].supplement_font = &pcfFonts[DEFAULT_ALL_FONT_IDX - 1];
					}
					else if (i == TITLE_FONT_IDX - 1)
					{
						pcfFonts[i].bPartialFont = 1;
						pcfFonts[i].supplement_font = &pcfFonts[TITLE_ALL_FONT_IDX - 1];
					}
					else if (i == SUBTITLE_FONT_IDX - 1)
					{
						pcfFonts[i].bPartialFont = 1;
						pcfFonts[i].supplement_font = &pcfFonts[SUBTITLE_ALL_FONT_IDX - 1];
					}
					else
					{
						pcfFonts[i].bPartialFont = 0;
					}
					pcfFonts[i].fd = fd;
				}
#if !defined(INCLUDED_FROM_KERNEL)
				else
				{
					printf("Missing font: '%s'\n", pcfFonts[i].file);
					exit(1);
				}
#endif
			}

		}
		lcd_draw_buf_inited = 1;
	}
	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = 0;
	lcd_draw_buf.drawing = 0;
	lcd_draw_buf.pPcfFont = NULL;
	lcd_draw_buf.line_height = 0;
	lcd_draw_buf.align_adjustment = 0;


	if (lcd_draw_buf.screen_buf)
		memset(lcd_draw_buf.screen_buf, 0, LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);
}

void draw_string(unsigned char *s)
{
	//ucs4_t u;
	unsigned char **p = &s;

	buf_draw_UTF8_str(p);
	//while (**p && (u = UTF8_to_UCS4(p)))
	//{
	//	buf_draw_char(u);
	//}
}

void draw_article_positioner(int y_pos)
{
	static int last_positioner_y = 0;

	if (!bShowPositioner)
		return;
	if (y_pos == 0)
		last_positioner_y = article_start_y_pos;
	else
	{
		while (y_pos - last_positioner_y - article_start_y_pos > LCD_HEIGHT_LINES / 2)
		{
			last_positioner_y += LCD_HEIGHT_LINES / 2;
			lcd_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y - 2);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y - 1);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 1, last_positioner_y - 1);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 1, last_positioner_y);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 2, last_positioner_y);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y + 1);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 1, last_positioner_y + 1);
			lcd_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y + 2);
		}
	}
}

void init_file_buffer()
{
	file_buffer = (unsigned char*)Xalloc(FILE_BUFFER_SIZE);
}

// default font is indexed 0, font id 0 is indexed 1, etc.
char* FontFile(int idx) {
	switch(idx)
	{
	case ITALIC_FONT_IDX - 1:
		return FONT_FILE_ITALIC;
		break;
	case DEFAULT_FONT_IDX - 1:
		return FONT_FILE_DEFAULT;
		break;
	case DEFAULT_ALL_FONT_IDX - 1:
		return FONT_FILE_DEFAULT_ALL;
		break;
	case TITLE_FONT_IDX - 1:
		return FONT_FILE_TITLE;
		break;
	case SUBTITLE_FONT_IDX - 1:
		return FONT_FILE_SUBTITLE;
		break;
	case TITLE_ALL_FONT_IDX - 1:
		return FONT_FILE_TITLE_ALL;
		break;
	case SUBTITLE_ALL_FONT_IDX - 1:
		return FONT_FILE_SUBTITLE_ALL;
		break;
	default:
		return "";
		break;
	}
}

void buf_draw_UTF8_str_in_copy_buffer(char *framebuffer_copy, unsigned char **pUTF8,int start_x,int end_x,int start_y,int end_y,int offset_x,int font_idx)
{
	ucs4_t u;
	LCD_DRAW_BUF lcd_draw_buf_external;

	lcd_draw_buf_external.current_x = start_x+offset_x;
	lcd_draw_buf_external.current_y = start_y+2;
	lcd_draw_buf_external.pPcfFont = &pcfFonts[font_idx - 1];
	lcd_draw_buf_external.screen_buf = (unsigned char*)framebuffer_copy;
	lcd_draw_buf_external.line_height = pcfFonts[font_idx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;

	lcd_draw_buf_external.align_adjustment = 0;

	while (**pUTF8 > MAX_ESC_CHAR)
	{
		if ((u = UTF8_to_UCS4((unsigned char**)pUTF8)))
		{
			buf_draw_char_external(&lcd_draw_buf_external,u,start_x,end_x,start_y,end_y);
		}

	}
}

void buf_draw_UTF8_str(unsigned char **pUTF8)
{
#ifndef WIKIPCF
	unsigned char c, c2;
	char c3;
	long v_line_bottom;
	ucs4_t u;
	int font_idx;
	int nWidth;
	int nHeight;
	int nBytes;
	int nImageY;
	int i, j;
	int nByteIdx, nBitIdx;

	if (NULL == pUTF8 || NULL == *pUTF8) {
		return;
	}

	c = **pUTF8;
	if (c <= MAX_ESC_CHAR)
	{
		(*pUTF8)++;
		switch(c)
		{
		case ESC_0_SPACE_LINE: /* space line */
			c2 = **pUTF8;
			(*pUTF8)++;
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.actual_height;
			lcd_draw_buf.line_height = c2;
			lcd_draw_buf.actual_height = lcd_draw_buf.line_height;
			lcd_draw_buf.align_adjustment = 0;
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
			draw_article_positioner(lcd_draw_buf.current_y);
			break;
		case ESC_1_NEW_LINE_DEFAULT_FONT: /* new line with default font and line space */
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.actual_height;
			lcd_draw_buf.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
			lcd_draw_buf.line_height = pcfFonts[DEFAULT_FONT_IDX - 1].Fmetrics.linespace + LINE_SPACE_ADDON;
			lcd_draw_buf.actual_height = lcd_draw_buf.line_height;
			lcd_draw_buf.align_adjustment = 0;
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
			draw_article_positioner(lcd_draw_buf.current_y);
			break;
		case ESC_2_NEW_LINE_SAME_FONT: /* new line with previous font and line space */
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.actual_height;
			lcd_draw_buf.actual_height = lcd_draw_buf.line_height;
			lcd_draw_buf.align_adjustment = 0;
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
			draw_article_positioner(lcd_draw_buf.current_y);
			break;
		case ESC_3_NEW_LINE_WITH_FONT: /* new line with specified font and line space */
			c2 = **pUTF8;
			(*pUTF8)++;
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.actual_height;
			font_idx = c2 & 0x07;
			if (font_idx > FONT_COUNT)
				font_idx = DEFAULT_FONT_IDX;
			lcd_draw_buf.pPcfFont = &pcfFonts[font_idx - 1];
			lcd_draw_buf.line_height = c2 >> 3;
			lcd_draw_buf.actual_height = lcd_draw_buf.line_height;
			lcd_draw_buf.align_adjustment = 0;
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
			draw_article_positioner(lcd_draw_buf.current_y);
			break;
		case ESC_4_CHANGE_FONT: /* change font */
			c2 = **pUTF8;
			(*pUTF8)++;
			font_idx = c2 & 0x07;
			if (font_idx > FONT_COUNT)
				font_idx = DEFAULT_FONT_IDX;
			lcd_draw_buf.pPcfFont = &pcfFonts[font_idx - 1];
			lcd_draw_buf.align_adjustment = ((signed char)c2 >> 3);
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height + lcd_draw_buf.align_adjustment >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.align_adjustment = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			if (lcd_draw_buf.current_y + lcd_draw_buf.align_adjustment < lcd_draw_buf.line_height - 1)
				lcd_draw_buf.align_adjustment = lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			break;
		case ESC_5_RESET_TO_DEFAULT_FONT: /* reset to the default font */
			lcd_draw_buf.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
			break;
		case ESC_6_RESET_TO_DEFAULT_ALIGN: /* reset to the default vertical alignment */
			lcd_draw_buf.vertical_adjustment = 0;
			break;
		case ESC_7_FORWARD: /* forward */
			c2 = **pUTF8;
			(*pUTF8)++;
			lcd_draw_buf.current_x += c2;
			if (lcd_draw_buf.current_x > LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - lcd_draw_buf.vertical_adjustment)
				lcd_draw_buf.current_x = LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - lcd_draw_buf.vertical_adjustment;
			break;
		case ESC_8_BACKWARD: /* backward */
			c2 = **pUTF8;
			(*pUTF8)++;
			if (lcd_draw_buf.current_x < c2)
				lcd_draw_buf.current_x = 0;
			else
				lcd_draw_buf.current_x -= c2;
			break;
		case ESC_9_ALIGN_ADJUSTMENT: /* vertical alignment adjustment */
			c3 = **pUTF8;
			(*pUTF8)++;
			lcd_draw_buf.vertical_adjustment += c3;
			//if (lcd_draw_buf.current_y + lcd_draw_buf.line_height + lcd_draw_buf.align_adjustment >= LCD_BUF_HEIGHT_PIXELS)
			//	lcd_draw_buf.align_adjustment = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			//if (lcd_draw_buf.current_y + lcd_draw_buf.align_adjustment < lcd_draw_buf.line_height - 1)
			//	lcd_draw_buf.align_adjustment = lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			break;
		case ESC_10_HORIZONTAL_LINE: /* drawing horizontal line */
			c2 = **pUTF8;
			(*pUTF8)++;
			if ((long)c2 > lcd_draw_buf.current_x)
				c2 = (unsigned char)lcd_draw_buf.current_x;
			buf_draw_horizontal_line(lcd_draw_buf.current_x - (unsigned long)c2 + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment,
						 lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment);
			break;
		case ESC_11_VERTICAL_LINE: /* drawing vertical line */
			c2 = **pUTF8;
			(*pUTF8)++;
			v_line_bottom = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
			v_line_bottom -= lcd_draw_buf.align_adjustment;
			if (v_line_bottom < 0)
				v_line_bottom = 0;
			if ((long)c2 > v_line_bottom)
				c2 = (unsigned char)v_line_bottom;
			buf_draw_vertical_line(v_line_bottom - (unsigned long)c2, v_line_bottom - 1);
			break;
		case ESC_12_FULL_HORIZONTAL_LINE: /* drawing horizontal line from left-most pixel to right-most pixel */
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.actual_height;
			lcd_draw_buf.line_height = 1;
			lcd_draw_buf.actual_height = lcd_draw_buf.line_height;
			lcd_draw_buf.align_adjustment = 0;
			buf_draw_horizontal_line(LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment, LCD_BUF_WIDTH_PIXELS);
			break;
		case ESC_13_FULL_VERTICAL_LINE: /* drawing vertical line from top of the line to the bottom */
			lcd_draw_buf.current_x += 1;
			buf_draw_vertical_line(lcd_draw_buf.current_y, lcd_draw_buf.current_y + lcd_draw_buf.line_height - 1);
			lcd_draw_buf.current_x += 2;
			break;
		case ESC_14_BITMAP: /* bitmap */
			c2 = **pUTF8;
			(*pUTF8)++;
			nWidth = c2;
			c2 = **pUTF8;
			(*pUTF8)++;
			nHeight = c2;
			c2 = **pUTF8;
			(*pUTF8)++;
			nHeight |= c2 << 8;
			if (lcd_draw_buf.current_x == 0)
				lcd_draw_buf.current_x = LCD_EXTRA_LEFT_MARGIN_FOR_IMAGE;
			else
				lcd_draw_buf.current_x++;
			if (lcd_draw_buf.line_height < nHeight + 1)
				lcd_draw_buf.actual_height = nHeight + 3;
			nImageY = lcd_draw_buf.current_y + 3;
			if ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment) % 8 == 0)
			{
				nBytes = (nWidth + 7) / 8;
				if (nBytes > LCD_BUF_WIDTH_BYTES - 1)
					nBytes = LCD_BUF_WIDTH_BYTES - 1;
				nByteIdx = (lcd_draw_buf.current_x  + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment) / 8;
				for (i = 0; i < nHeight; i++)
				{
					memcpy(&lcd_draw_buf.screen_buf[nImageY * LCD_BUF_WIDTH_BYTES + nByteIdx], *pUTF8, nBytes);
					*pUTF8 += (nWidth + 7) / 8;
					nImageY++;
				}
			}
			else
			{
				for (i = 0; i < nHeight; i++)
				{
					for(j = 0; j < nWidth; j++)
					{
						nByteIdx = j / 8;
						nBitIdx = 7 - (j % 8);
						if ((*pUTF8)[nByteIdx] & (1 << nBitIdx))
						{
							lcd_set_pixel(lcd_draw_buf.screen_buf, lcd_draw_buf.current_x +
								      LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment + j, nImageY);
						}
					}
					*pUTF8 += (nWidth + 7) / 8;
					nImageY++;
				}
			}
			lcd_draw_buf.current_x += nWidth;
			break;
		default:
			break;
		}
	}

	while (**pUTF8 > MAX_ESC_CHAR) /* stop at end of string or escape character */
	{
		if ((u = UTF8_to_UCS4(pUTF8)))
		{
			buf_draw_char(u);
			if(display_first_page==0 && lcd_draw_buf.current_y > LCD_HEIGHT_LINES + article_start_y_pos)
			{
				display_first_page = 1;
				lcd_draw_cur_y_pos = article_start_y_pos;
				finger_move_speed = 0;
				repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 0);
				if (lcd_draw_init_y_pos < article_start_y_pos)
					lcd_draw_init_y_pos = article_start_y_pos;
				if (lcd_draw_init_y_pos > article_start_y_pos)
				{
					display_article_with_pcf(lcd_draw_init_y_pos);
				}
			}
		}
	}
#endif
}

#define MAX_PIXELS_EACH_SIDE 5
void draw_language_link_arrow()
{
	int i, j;
	int nPixelsEachSide;

	if (bShowLanguageLinks)
		nPixelsEachSide = MAX_PIXELS_EACH_SIDE;
	else
		nPixelsEachSide = 0;
	for (i = LCD_TOP_MARGIN; i < LCD_TOP_MARGIN + LANGUAGE_LINK_HEIGHT; i++)
	{
		if (i == LCD_TOP_MARGIN + 1 || i == LCD_TOP_MARGIN + LANGUAGE_LINK_HEIGHT - 2)
		{
			for (j = LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 2; j < LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - 2; j++)
			{
				lcd_set_pixel(framebuffer, j, i);
			}
		}
		else if (1 + LCD_TOP_MARGIN < i && i < LCD_TOP_MARGIN + LANGUAGE_LINK_HEIGHT - 2)
		{
			for (j = LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 1; j < LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - 1; j++)
			{
				lcd_set_pixel(framebuffer, j, i);
			}
		}

		if (8 + LCD_TOP_MARGIN <= i && i <= 8 + LCD_TOP_MARGIN + MAX_PIXELS_EACH_SIDE)
		{
			if (bShowLanguageLinks)
			{
				if (nPixelsEachSide >= 0)
				{
					for (j = -nPixelsEachSide; j <= nPixelsEachSide; j++)
						lcd_clear_pixel(framebuffer, LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 11 + j, i);
					nPixelsEachSide--;
				}
			}
			else
			{
				if (nPixelsEachSide <= MAX_PIXELS_EACH_SIDE)
				{
					for (j = -nPixelsEachSide; j <= nPixelsEachSide; j++)
						lcd_clear_pixel(framebuffer, LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 11 + j, i);
					nPixelsEachSide++;
				}
			}
		}
	}
}

void repaint_framebuffer(unsigned char *buf, int pos, int b_repaint_invert_link)
{
#ifndef WIKIPCF
	int framebuffersize;
	framebuffersize = framebuffer_size();

	guilib_fb_lock();
	//guilib_clear();

	memcpy(framebuffer,buf+(pos < 0 ? 0 : pos)*LCD_VRAM_WIDTH_PIXELS/8,framebuffersize);
	if (display_mode == DISPLAY_MODE_ARTICLE && (language_link_count || restricted_article) && (pos == article_start_y_pos || pos == 0))
	{
		draw_language_link_arrow();
	}
//	if (b_repaint_invert_link)
//		repaint_invert_link();
	if (b_show_scroll_bar)
		show_scroll_bar(1);
	guilib_fb_unlock();
#endif
}

void buf_draw_horizontal_line(unsigned long start_x, unsigned long end_x)
{
	int i;
	long h_line_y;


	h_line_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
	h_line_y -= lcd_draw_buf.align_adjustment + 1;
	if (end_x > LCD_BUF_WIDTH_PIXELS)
		end_x = LCD_BUF_WIDTH_PIXELS;

	for(i = start_x;i<end_x;i++)
	{
		lcd_set_pixel(lcd_draw_buf.screen_buf,i, h_line_y);
	}

}

void buf_draw_vertical_line(unsigned long start_y, unsigned long end_y)
{
	unsigned long idx_in_byte;
	unsigned char *p;

	if (lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment < LCD_BUF_WIDTH_PIXELS)
	{
		idx_in_byte = 7 - ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment) & 0x07);
		p = lcd_draw_buf.screen_buf + start_y * LCD_BUF_WIDTH_BYTES + ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment)>> 3);
		while (start_y <= end_y)
		{
			*p |= 1 << idx_in_byte;
			start_y++;
			p += LCD_BUF_WIDTH_BYTES;
		}
	}
}

void lcd_set_pixel(unsigned char *membuffer,int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;


	membuffer[byte] |= (1 << (7 - bit));
}

void lcd_set_framebuffer_pixel(int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;


	framebuffer[byte] |= (1 << (7 - bit));
}

void lcd_set_framebuffer_byte(char c, int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;

	framebuffer[byte] = c;
}

void lcd_clear_framebuffer_pixel(int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;


	if (framebuffer[byte] & (1 << (7 - bit)))
		framebuffer[byte] ^= (1 << (7 - bit));
}

char lcd_draw_buf_get_byte(int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;

	return lcd_draw_buf.screen_buf[byte];
}

int lcd_draw_buf_get_pixel(int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;

	if (lcd_draw_buf.screen_buf[byte] & (1 << (7 - bit)))
		return 1;
	else
		return 0;
}

void lcd_clear_pixel(unsigned char *membuffer,int x, int y)
{
	unsigned int byte = (x + LCD_VRAM_WIDTH_PIXELS * y) / 8;
	unsigned int bit  = (x + LCD_VRAM_WIDTH_PIXELS * y) % 8;

	membuffer[byte] &= ~(1 << (7 - bit));
}
void buf_draw_char(ucs4_t u)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int bytes_to_process;
	int x_base;
	int y_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index
	int j; // bitmap bit index
	unsigned char *p; // pointer to lcd draw buffer



	if(pres_bmfbm(u, lcd_draw_buf.pPcfFont, &bitmap, &Cmetrics)<0)
	{
		return;
	}
	if(u==32)
	{
		lcd_draw_buf.current_x += Cmetrics.widthDevice;
		return;
	}

	if (bitmap == NULL)
		return;

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;

	x_base = lcd_draw_buf.current_x + Cmetrics.LSBearing + LCD_LEFT_MARGIN + lcd_draw_buf.vertical_adjustment;
	if (x_base < LCD_BUF_WIDTH_PIXELS)
	{ // only draw the chracter if there is space left before the right margin of the LCD screen
		y_base = lcd_draw_buf.current_y + lcd_draw_buf.align_adjustment;
		x_offset = 0;
		y_offset = lcd_draw_buf.line_height - (lcd_draw_buf.pPcfFont->Fmetrics.descent + Cmetrics.ascent);

		x_bit_idx = x_base & 0x07;
		//p = lcd_draw_buf.screen_buf + (y_base + y_offset) * LCD_BUF_WIDTH_BYTES + (x_base >> 3);

		for (i = 0; i < bytes_to_process; i++)
		{
			j = 7;
			while (j >= 0)
			{
				if (x_offset >= Cmetrics.widthBits)
				{
					x_offset = 0;
					y_offset++;
					p = lcd_draw_buf.screen_buf + (y_base + y_offset) * LCD_BUF_WIDTH_BYTES + x_base;
				}
				if (x_offset < Cmetrics.width)
				{
					if ((bitmap[i] & (1 << j)) && x_base + x_offset < LCD_BUF_WIDTH_PIXELS)
					{
						//*p |= 1 << ((x_base + x_offset) & 0x07);
						lcd_set_pixel(lcd_draw_buf.screen_buf,x_base + x_offset, y_base+y_offset);
					}
				}
				x_offset++;
				x_bit_idx++;
				if (!(x_bit_idx & 0x07))
				{
					x_bit_idx = 0;
					p++;
				}
				j--;
			}
		}
	}
	lcd_draw_buf.current_x += Cmetrics.widthDevice;
}

int get_external_str_pixel_width(unsigned char *pIn, int font_idx)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int width = 0;
	ucs4_t u;
	unsigned char **pUTF8 = &pIn;

	while (**pUTF8 > MAX_ESC_CHAR)
	{
		if ((u = UTF8_to_UCS4((unsigned char**)pUTF8)))
		{
			pres_bmfbm(u, &pcfFonts[font_idx - 1], &bitmap, &Cmetrics);
			if (bitmap != NULL)
				width += Cmetrics.widthDevice;
		}

	}
	return width;
}

void get_external_str_pixel_rectangle(unsigned char *pIn, int font_idx, int *start_x, int *start_y, int *end_x, int *end_y)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	ucs4_t u;
	unsigned char **pUTF8 = &pIn;
	int width = 0;
	int bytes_to_process;
	int x_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index
	int j; // bitmap bit index

	*start_x = -999;
	*end_x = -999;
	*start_y = -999;
	*end_y = -999;
	while (**pUTF8 > MAX_ESC_CHAR)
	{
		if ((u = UTF8_to_UCS4((unsigned char**)pUTF8)))
		{
			pres_bmfbm(u, &pcfFonts[font_idx - 1], &bitmap, &Cmetrics);
			if (bitmap != NULL)
			{
				bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;

				x_base = width + Cmetrics.LSBearing;
				x_offset = 0;
				y_offset = pcfFonts[font_idx - 1].Fmetrics.linespace - (pcfFonts[font_idx - 1].Fmetrics.descent + Cmetrics.ascent);

				x_bit_idx = x_base & 0x07;
				for (i = 0; i < bytes_to_process; i++)
				{
					j = 7;
					while (j >= 0)
					{
						if (x_offset >= Cmetrics.widthBits)
						{
							x_offset = 0;
							y_offset++;
						}
						if (x_offset < Cmetrics.width)
						{
							if (bitmap[i] & (1 << j))
							{
								if (*start_x == -999 || *start_x > x_base + x_offset)
									*start_x = x_base + x_offset;
								if (*start_y == -999 || *start_y > y_offset)
									*start_y = y_offset;
								if (*end_x < x_base + x_offset)
									*end_x = x_base + x_offset;
								if (*end_y < y_offset)
									*end_y = y_offset;
							}
						}
						x_offset++;
						x_bit_idx++;
						if (!(x_bit_idx & 0x07))
						{
							x_bit_idx = 0;

						}
						j--;
					}
				}
				width += Cmetrics.widthDevice;
			}
		}

	}
}

void buf_draw_char_external(LCD_DRAW_BUF *lcd_draw_buf_external,ucs4_t u,int start_x,int end_x,int start_y,int end_y)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int bytes_to_process;
	int x_base;
	int y_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index
	int j; // bitmap bit index
	unsigned char *p; // pointer to lcd draw buffer

	if(pres_bmfbm(u, lcd_draw_buf_external->pPcfFont, &bitmap, &Cmetrics)<0)
		return;
	if(u==32)
	{
		lcd_draw_buf_external->current_x += Cmetrics.widthDevice;
		return;
	}
	if (bitmap == NULL)
		return;

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;

	x_base = lcd_draw_buf_external->current_x + Cmetrics.LSBearing;
	if((lcd_draw_buf_external->current_x + Cmetrics.widthDevice) > end_x)
	{
		lcd_draw_buf_external->current_x = start_x;
		x_base =  Cmetrics.LSBearing;
		lcd_draw_buf_external->current_y+=lcd_draw_buf_external->line_height;
	}
	y_base = lcd_draw_buf_external->current_y + lcd_draw_buf_external->align_adjustment;
	x_offset = 0;
	y_offset = lcd_draw_buf_external->line_height - (lcd_draw_buf_external->pPcfFont->Fmetrics.descent + Cmetrics.ascent);
	x_bit_idx = x_base & 0x07;

	for (i = 0; i < bytes_to_process; i++)
	{
		j = 7;
		while (j >= 0)
		{
			if (x_offset >= Cmetrics.widthBits)
			{
				x_offset = 0;
				y_offset++;
			}
			if (x_offset < Cmetrics.width)
			{
				if (bitmap[i] & (1 << j))
				{
					lcd_set_pixel(lcd_draw_buf_external->screen_buf,x_base + x_offset, y_base+y_offset);
				}

			}
			x_offset++;
			x_bit_idx++;
			if (!(x_bit_idx & 0x07))
			{
				x_bit_idx = 0;
				p++;
			}
			j--;
		}
	}
	lcd_draw_buf_external->current_x += Cmetrics.widthDevice;
}

int get_UTF8_char_width(int idxFont, char **pContent, long *lenContent, int *nCharBytes)
{
	ucs4_t u;
	char *pBase;
	charmetric_bmf Cmetrics;
	bmf_bm_t *bitmap;

	pBase = *pContent;
	u = UTF8_to_UCS4((unsigned char **)pContent);
	*nCharBytes = *pContent - pBase;
	*lenContent -= *nCharBytes;

	pres_bmfbm(u, &pcfFonts[idxFont - 1], &bitmap, &Cmetrics);
	if (bitmap == NULL)
		return 0;
	else
		return  Cmetrics.widthDevice;
}

bool is_word_break(ucs4_t u)
{
	unsigned char c = u & 0x000000FF;

	return ((u > 0x47F) || (u < 0x100 && strchr(" ~!@#$%^&*()-_+=[]\{}|;;':\",./", c)));
}

int extract_str_fitting_width(unsigned char **pIn, char *pOut, int max_width, int font_idx)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int width = 0;
	int widthFitted = 0;
	ucs4_t u;
	unsigned char *pOrigIn;
	int nBytesFittingWidth = 0;
	int nLastBytes, nLastWidth;

	pOrigIn = *pIn;
	pOut[0] = '\0';
	while (**pIn > MAX_ESC_CHAR && width <= max_width)
	{
		nLastBytes = *pIn - pOrigIn;
		nLastWidth = width;
		if ((u = UTF8_to_UCS4((unsigned char**)(pIn))))
		{
			pres_bmfbm(u, &pcfFonts[font_idx - 1], &bitmap, &Cmetrics);
			if (bitmap != NULL)
				width += Cmetrics.widthDevice;
			if (is_word_break(u))
			{
				if (width > max_width)
				{
					nBytesFittingWidth = nLastBytes;
					widthFitted = nLastWidth;
				}
				else
				{
					nBytesFittingWidth = *pIn - pOrigIn;
					widthFitted = width;
				}
			}
		}
	}

	if (width > max_width)
	{
		memcpy(pOut, pOrigIn, nBytesFittingWidth);
		pOut[nBytesFittingWidth] = '\0';
		*pIn = pOrigIn + nBytesFittingWidth;
	}
	else
	{
		strcpy(pOut, pOrigIn);
		widthFitted = width;
	}
	return widthFitted;
}

#ifndef WIKIPCF
void init_render_article(long init_y_pos)
{

	//if(lcd_draw_buf.current_y>0)
	//  memset(lcd_draw_buf.screen_buf,0,lcd_draw_buf.current_y*LCD_VRAM_WIDTH_PIXELS/8);
	if (lcd_draw_buf.screen_buf)
		memset(lcd_draw_buf.screen_buf, 0, LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);

	article_buf_pointer = NULL;
	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = 0;
	lcd_draw_buf.drawing = 0;
	lcd_draw_buf.pPcfFont = NULL;
	lcd_draw_buf.line_height = 0;
	lcd_draw_buf.actual_height = 0;
	lcd_draw_buf.align_adjustment = 0;
	lcd_draw_buf.vertical_adjustment = 0;

	display_first_page = 0;
	lcd_draw_cur_y_pos = 0;
	article_start_y_pos = 0;
	bShowLanguageLinks = 0;
	lcd_draw_init_y_pos = init_y_pos;
	finger_move_speed = 0;
	lcd_draw_buf_pos = 0;
}
#endif

void render_wikipedia_license_text(void)
{
#ifndef WIKIPCF
	long start_x, start_y, end_x, end_y;
	WIKI_LICENSE_DRAW *license_draw;
	int draw_lines;
	int i;

	license_draw = wiki_license_draw();
	lcd_draw_buf.current_y += SPACE_BEFORE_LICENSE_TEXT;
	if (license_draw->lines < LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.current_y)
		draw_lines = license_draw->lines;
	else
		draw_lines = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.current_y;
	memcpy(&lcd_draw_buf.screen_buf[lcd_draw_buf.current_y * LCD_BUF_WIDTH_BYTES], license_draw->buf, draw_lines * LCD_BUF_WIDTH_BYTES);
	for (i = 0; i < license_draw->link_count; i++)
	{
		start_x = license_draw->links[i].start_xy & 0xFF;
		start_y = (license_draw->links[i].start_xy >> 8) + lcd_draw_buf.current_y;
		end_x = license_draw->links[i].end_xy & 0xFF;
		end_y = (license_draw->links[i].end_xy >> 8) + lcd_draw_buf.current_y;
		articleLink[article_link_count].start_xy = license_draw->links[i].start_xy;
		articleLink[article_link_count].end_xy = license_draw->links[i].start_xy;
		articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
		articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
		articleLink[article_link_count++].article_id = license_draw->links[i].article_id;
	}
	lcd_draw_buf.current_y += draw_lines;
	if (lcd_draw_buf.current_y > LCD_BUF_HEIGHT_PIXELS)
		lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS;
#endif
}

int render_article_with_pcf()
{

	if (!article_buf_pointer)
		return 0;

	buf_draw_UTF8_str(&article_buf_pointer);
	if(stop_render_article == 1 && display_first_page == 1)
	{
		article_buf_pointer = NULL;
		stop_render_article = 0;
		return 0;
	}
	if(request_display_next_page > 0 && lcd_draw_buf.current_y > request_y_pos)
	{
		lcd_draw_cur_y_pos = request_y_pos - LCD_HEIGHT_LINES;
		if (bShowLanguageLinks)
		{
			if (lcd_draw_cur_y_pos < 0)
			{
				lcd_draw_cur_y_pos = 0;
			}
			else if (lcd_draw_cur_y_pos >= article_start_y_pos)
				bShowLanguageLinks = 0;
		}
		else
		{
			if (lcd_draw_cur_y_pos < article_start_y_pos)
			{
				lcd_draw_cur_y_pos = article_start_y_pos;
			}
		}
		repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
		request_display_next_page = 0;
	}
	if(!*article_buf_pointer)
	{
		render_wikipedia_license_text();
		if(display_first_page == 0 || request_display_next_page > 0)
		{
			if(request_display_next_page > 0)
			{
				lcd_draw_cur_y_pos = request_y_pos - LCD_HEIGHT_LINES;
				if (lcd_draw_cur_y_pos + LCD_HEIGHT_LINES > lcd_draw_buf.current_y)
				{
					lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
				}
				if (bShowLanguageLinks)
				{
					if (lcd_draw_cur_y_pos < 0)
					{
						lcd_draw_cur_y_pos = 0;
					}
					else if (lcd_draw_cur_y_pos >= article_start_y_pos)
						bShowLanguageLinks = 0;
				}
				else
				{
					if (lcd_draw_cur_y_pos < article_start_y_pos)
					{
						lcd_draw_cur_y_pos = article_start_y_pos;
					}
				}
			}
			else if(display_first_page==0 && lcd_draw_cur_y_pos < article_start_y_pos)
				lcd_draw_cur_y_pos = article_start_y_pos;

			repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
			display_first_page = 1;
			request_display_next_page = 0;
		}

		article_buf_pointer = NULL;

		return 0;
	}
	return 1;

}

#ifndef WIKIPCF
extern int nWikiCount;
extern int rendered_wiki_selection_count;
int render_wiki_selection_with_pcf()
{
	int rc = 0;
	int start_x, end_x, start_y, end_y;

	if (rendered_wiki_selection_count < 0)
		return rc;

	guilib_fb_lock();
	if (rendered_wiki_selection_count == 0)
	{
		init_render_article(0);
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_HEADING_FONT_IDX - 1];
		lcd_draw_buf.line_height = pcfFonts[SEARCH_HEADING_FONT_IDX - 1].Fmetrics.linespace;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = LCD_TOP_MARGIN;
		lcd_draw_buf.vertical_adjustment = 0;
		lcd_draw_buf.align_adjustment = 0;
		draw_string(get_nls_text("select_wiki"));
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_LIST_FONT_IDX - 1];
		lcd_draw_buf.line_height = HISTORY_RESULT_HEIGHT;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = HISTORY_RESULT_START;
		article_link_count = 0;
		language_link_count = 0;
	}

	if (rendered_wiki_selection_count < nWikiCount) {
		start_x = 0;
		end_x = LCD_BUF_WIDTH_PIXELS - 1;
		if (article_link_count < MAX_RESULT_LIST)
		{
			start_y = lcd_draw_buf.current_y + 1;
			end_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
			articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
			articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
			articleLink[article_link_count++].article_id = rendered_wiki_selection_count;
		}
		draw_string(get_wiki_name(rendered_wiki_selection_count));
		rendered_wiki_selection_count++;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += lcd_draw_buf.line_height;
		if (rendered_wiki_selection_count < nWikiCount)
			rc = 1;
		else
		{
			rendered_wiki_selection_count = -1;
			if(display_first_page == 0)
			{
				repaint_framebuffer(lcd_draw_buf.screen_buf,0, 0);
				display_first_page = 1;
			}
			else if (request_display_next_page > 0)
			{
				long y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
				if (y_pos < 0)
					y_pos = 0;
				lcd_draw_cur_y_pos = y_pos;
				repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
				request_display_next_page = 0;
			}
		}

		if(stop_render_article == 1 && display_first_page == 1)
		{
			rendered_wiki_selection_count = -1;
			stop_render_article = 0;
		}

		if(request_display_next_page > 0 && lcd_draw_buf.current_y >= request_y_pos+LCD_HEIGHT_LINES)
		{
			lcd_draw_cur_y_pos = request_y_pos;
			repaint_framebuffer(lcd_draw_buf.screen_buf,lcd_draw_cur_y_pos, 1);
			request_display_next_page = 0;
		}
	}

	guilib_fb_unlock();
	return rc;
}

extern int history_count;
extern int rendered_history_count;
extern HISTORY history_list[MAX_HISTORY];
int render_history_with_pcf()
{
	int rc = 0;
	int start_x, end_x, start_y, end_y;

	restricted_article = 0;
	if (rendered_history_count < 0)
		return rc;

	guilib_fb_lock();
	if (rendered_history_count == 0)
	{
		init_render_article(0);
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_HEADING_FONT_IDX - 1];
		lcd_draw_buf.line_height = pcfFonts[SEARCH_HEADING_FONT_IDX - 1].Fmetrics.linespace;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = LCD_TOP_MARGIN;
		lcd_draw_buf.vertical_adjustment = 0;
		lcd_draw_buf.align_adjustment = 0;
		draw_string(get_nls_text("history_title"));
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_LIST_FONT_IDX - 1];
		lcd_draw_buf.line_height = HISTORY_RESULT_HEIGHT;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = HISTORY_RESULT_START;
		article_link_count = 0;
		language_link_count = 0;
	}

	if (history_count == 0) {
		unsigned char *p = get_nls_text("no_history");
		int str_width = get_external_str_pixel_width(p, DEFAULT_FONT_IDX);
		lcd_draw_buf.current_x = (LCD_BUF_WIDTH_PIXELS - str_width) / 2;
		lcd_draw_buf.current_y = 95;
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_LIST_FONT_IDX - 1];
		lcd_draw_buf.line_height = pcfFonts[SEARCH_LIST_FONT_IDX - 1].Fmetrics.linespace;
		draw_string(get_nls_text("no_history"));
		rendered_history_count = -1;
		repaint_framebuffer(lcd_draw_buf.screen_buf,0, 0);
		display_first_page = 1;
	} else if (rendered_history_count < history_count) {
		start_x = 0;
		end_x = LCD_BUF_WIDTH_PIXELS - 1;
		if (article_link_count < MAX_RESULT_LIST)
		{
			start_y = lcd_draw_buf.current_y + 1;
			end_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
			articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
			articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
			articleLink[article_link_count++].article_id = history_list[rendered_history_count].idx_article;
		}
		draw_string(history_list[rendered_history_count].title);
		rendered_history_count++;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += lcd_draw_buf.line_height;
		if (rendered_history_count < history_count)
			rc = 1;
		else
		{
			rendered_history_count = -1;
			if(display_first_page == 0)
			{
				repaint_framebuffer(lcd_draw_buf.screen_buf,0, 0);
				display_first_page = 1;
			}
			else if (request_display_next_page > 0)
			{
				long y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
				if (y_pos < 0)
					y_pos = 0;
				lcd_draw_cur_y_pos = y_pos;
				repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
				request_display_next_page = 0;
			}
		}

		if(stop_render_article == 1 && display_first_page == 1)
		{
			rendered_history_count = -1;
			stop_render_article = 0;
		}

		if(request_display_next_page > 0 && lcd_draw_buf.current_y >= request_y_pos+LCD_HEIGHT_LINES)
		{
			lcd_draw_cur_y_pos = request_y_pos;
			repaint_framebuffer(lcd_draw_buf.screen_buf,lcd_draw_cur_y_pos, 1);
			request_display_next_page = 0;
		}
	}

	guilib_fb_unlock();
	return rc;
}

extern int more_search_results;
void restore_search_list_page(void)
{
	if (article_link_count > NUMBER_OF_FIRST_PAGE_RESULTS)
	{
		more_search_results = 0;
		article_link_count = NUMBER_OF_FIRST_PAGE_RESULTS;
		memcpy(framebuffer, lcd_draw_buf.screen_buf, framebuffer_size()); // copy from the LCD frame buffer (for the first page)
	}
}

int render_search_result_with_pcf(void)
{
	int rc = 0;
	int start_x, end_x, start_y, end_y;
	long idxArticle;
	char sTitleActual[MAX_TITLE_ACTUAL];
	static long offset_next = 0;

	if (!more_search_results)
	{
		display_first_page = 1;
		return rc;
	}

	guilib_fb_lock();
	if (article_link_count == NUMBER_OF_FIRST_PAGE_RESULTS) // has not rendered any results beyond the first page
	{
		offset_next = result_list_offset_next();
		init_render_article(0);
		memcpy(lcd_draw_buf.screen_buf, framebuffer, framebuffer_size()); // copy from the LCD frame buffer (for the first page)
		display_first_page = 1;
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_LIST_FONT_IDX - 1];
		lcd_draw_buf.line_height = RESULT_HEIGHT;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = (RESULT_START - 2) + RESULT_HEIGHT * NUMBER_OF_FIRST_PAGE_RESULTS;
		lcd_draw_cur_y_pos = 0;
	}

	if ((offset_next = result_list_next_result(offset_next, &idxArticle, sTitleActual)))
	{
		start_x = 0;
		end_x = LCD_BUF_WIDTH_PIXELS - 1;
		if (article_link_count < MAX_RESULT_LIST)
		{
			start_y = lcd_draw_buf.current_y + 1;
			end_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
			articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
			articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
			articleLink[article_link_count++].article_id = idxArticle;
			draw_string(sTitleActual);
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.line_height;
			rc = 1;
		}
		else
			more_search_results = 0;

		if (stop_render_article == 1)
		{
			more_search_results = 0;
			stop_render_article = 0;
		}

		if(request_display_next_page > 0 && lcd_draw_buf.current_y >= request_y_pos+LCD_HEIGHT_LINES)
		{
			lcd_draw_cur_y_pos = request_y_pos;
			repaint_framebuffer(lcd_draw_buf.screen_buf,lcd_draw_cur_y_pos, 1);
			request_display_next_page = 0;
		}
	}
	else
	{
		more_search_results = 0;
		if (request_display_next_page > 0)
		{
			long y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
			if (y_pos < 0)
				y_pos = 0;
			lcd_draw_cur_y_pos = y_pos;
			repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
			request_display_next_page = 0;
		}
	}

	guilib_fb_unlock();
	return rc;
}

void display_article_with_pcf(int y_move)
{
	if(lcd_draw_buf.current_y<=LCD_HEIGHT_LINES + article_start_y_pos || request_display_next_page ||
	   (display_mode == DISPLAY_MODE_INDEX && article_link_count <= NUMBER_OF_FIRST_PAGE_RESULTS))
		return;

	if(article_buf_pointer && (lcd_draw_cur_y_pos+y_move+LCD_HEIGHT_LINES) > lcd_draw_buf.current_y)
	{
		request_display_next_page = 1;
		request_y_pos = lcd_draw_cur_y_pos + y_move + LCD_HEIGHT_LINES;

		display_str(get_nls_text("please_wait"));

		return;
	}
//	if ((lcd_draw_cur_y_pos == 0 && start_y < 0) ||
//	    ((lcd_draw_cur_y_pos+LCD_HEIGHT_LINES)>lcd_draw_buf.current_y && start_y >= 0))
//	{
//		return;
//	}

	lcd_draw_cur_y_pos += y_move;
	if (!bShowLanguageLinks && lcd_draw_cur_y_pos < article_start_y_pos)
		lcd_draw_cur_y_pos = article_start_y_pos;
	else if (bShowLanguageLinks && lcd_draw_cur_y_pos >= article_start_y_pos)
		bShowLanguageLinks = 0;
	if ((lcd_draw_cur_y_pos+LCD_HEIGHT_LINES)>lcd_draw_buf.current_y)
	{
		lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
	}
	if (lcd_draw_cur_y_pos < 0)
	{
		lcd_draw_cur_y_pos = 0;
	}
	if (display_mode == DISPLAY_MODE_ARTICLE)
	{
		if (lcd_draw_cur_y_pos > article_start_y_pos)
			history_log_y_pos(lcd_draw_cur_y_pos - article_start_y_pos);
		else
			history_log_y_pos(0);
	}

	repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
	display_first_page = 1;
}

float scroll_speed()
{
	float speed = 0;

	if (finger_move_speed)
	{
		if (display_mode == DISPLAY_MODE_ARTICLE)
			speed = (float)finger_move_speed * ARTICLE_SCROLL_SPEED_FRICTION;
		else
			speed = (float)finger_move_speed * LIST_SCROLL_SPEED_FRICTION;
		if (abs(speed) < 1 / SCROLL_UNIT_SECOND)
			speed = 0;
	}
	return speed;
}

void scroll_article(void)
{
	unsigned long time_now, delay_time;


	if(finger_move_speed == 0)
		return;

	if (!display_first_page || request_display_next_page ||
	    ((display_mode == DISPLAY_MODE_INDEX || display_mode == DISPLAY_MODE_HISTORY || display_mode == DISPLAY_MODE_WIKI_SELECTION) &&
	     article_link_count <= NUMBER_OF_FIRST_PAGE_RESULTS))
	{
		finger_move_speed = 0;
		return;
	}

	time_now = get_time_ticks();
	delay_time = time_diff(time_now, time_scroll_article_last);

	if (delay_time >= seconds_to_ticks(SCROLL_UNIT_SECOND))
	{
		time_scroll_article_last = time_now;

		if (finger_move_speed)
		{
			article_scroll_increment = (float)finger_move_speed * ((float)delay_time / (float)seconds_to_ticks(1));
			finger_move_speed = scroll_speed();
			lcd_draw_cur_y_pos += article_scroll_increment;
			if(lcd_draw_cur_y_pos < article_start_y_pos)
			{
				if (!bShowLanguageLinks)
					lcd_draw_cur_y_pos = article_start_y_pos;
				else if (lcd_draw_cur_y_pos < 0)
					lcd_draw_cur_y_pos = 0;
			}
			else if (bShowLanguageLinks)
			{
				if (lcd_draw_cur_y_pos >= article_start_y_pos)
				{
					bShowLanguageLinks = 0;
				}
			}
			else if (lcd_draw_cur_y_pos > lcd_draw_buf.current_y - LCD_HEIGHT_LINES)
			{
				lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT_LINES;
			}
			if (display_mode == DISPLAY_MODE_ARTICLE)
			{
				if (lcd_draw_cur_y_pos > article_start_y_pos)
					history_log_y_pos(lcd_draw_cur_y_pos - article_start_y_pos);
				else
					history_log_y_pos(0);
			}
		}

		repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);

		if (finger_move_speed == 0 && b_show_scroll_bar)
		{
			b_show_scroll_bar = 0;
			show_scroll_bar(0); // clear scroll bar
		}
	}
}

void draw_icon(char *pStr)
{
	int start_x, start_y, end_x, end_y;
	int i, j;

	get_external_str_pixel_rectangle(pStr, SUBTITLE_FONT_IDX, &start_x, &start_y, &end_x, &end_y);
	for (i = 0; i < LANGUAGE_LINK_HEIGHT; i++)
	{
		if (i == 1 || i == LANGUAGE_LINK_HEIGHT - 2)
		{
			for (j = lcd_draw_buf.current_x + LCD_LEFT_MARGIN + 1; j < lcd_draw_buf.current_x + LCD_LEFT_MARGIN + LANGUAGE_LINK_WIDTH - 2; j++)
			{
				lcd_set_pixel(lcd_draw_buf.screen_buf, j, lcd_draw_buf.current_y + i);
			}
		}
		else if (1 < i && i < LANGUAGE_LINK_HEIGHT - 2)
		{
			for (j = lcd_draw_buf.current_x + LCD_LEFT_MARGIN; j < lcd_draw_buf.current_x + LCD_LEFT_MARGIN + LANGUAGE_LINK_WIDTH - 1; j++)
			{
				lcd_set_pixel(lcd_draw_buf.screen_buf, j, lcd_draw_buf.current_y + i);
			}
		}
	}
	buf_render_string(lcd_draw_buf.screen_buf, SUBTITLE_FONT_IDX, lcd_draw_buf.current_x + LCD_LEFT_MARGIN +
			  (LANGUAGE_LINK_WIDTH - (end_x - start_x + 1)) / 2 - start_x,
			  lcd_draw_buf.current_y + (LANGUAGE_LINK_HEIGHT - (end_y - start_y + 1)) / 2 - start_y, pStr, strlen(pStr), 1);
	lcd_draw_buf.current_x += LANGUAGE_LINK_WIDTH + LANGUAGE_LINK_WIDTH_GAP;
}

void draw_restricted_mark()
{
	draw_icon(get_nls_text("r"));
}

void draw_external_link(char *link_str)
{
	char *pLang;

	pLang = get_lang_link_display_text(link_str);
	draw_icon(pLang);
}

int duplicate_wiki_lang(char *link_str1, char *link_str2)
{
	char *p;
	int len;

	p = strchr(link_str1, ':');
	if (p)
	{
		len = p - link_str1;
		if (!memcmp(link_str1, link_str2, len))
			return 1;
	}
	return 0;
}

void add_show_hide_language_link()
{
	articleLink[1].start_xy = 0;
	articleLink[1].end_xy = 0;
	articleLink[1].article_id = SHOW_LANGUAGE_LINK;
	externalLink[1].link_str = NULL;
	articleLink[2].start_xy = 0;
	articleLink[2].end_xy = 0;
	articleLink[2].article_id = HIDE_LANGUAGE_LINK;
	externalLink[2].link_str = NULL;
	article_link_count = 3;
	lcd_draw_buf.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
	lcd_draw_buf.line_height = pcfFonts[DEFAULT_FONT_IDX - 1].Fmetrics.linespace;
	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = LCD_TOP_MARGIN;
	lcd_draw_buf.vertical_adjustment = 0;
	lcd_draw_buf.align_adjustment = 0;
}

void display_retrieved_article(long idx_article)
{
	int i;
	int offset;
	ARTICLE_HEADER article_header;
	char title[MAX_TITLE_ACTUAL];
	int bKeepPos = 0;
	int nCurrentWikiId = get_wiki_id_from_idx(nCurrentWiki);
	int nArticleWikiId;
	int nLinkWikiId;
	unsigned int start_x, end_x, start_y, end_y;

	nArticleWikiId = idx_article >> 24;
	if (last_display_mode == DISPLAY_MODE_HISTORY)
	{
		init_render_article(history_get_y_pos());
		bKeepPos = 1;
	}
	else
	{
		init_render_article(0);
	}
	memset(articleLinkBeforeAfter, 0, sizeof(articleLinkBeforeAfter));
	memcpy(&article_header,file_buffer,sizeof(ARTICLE_HEADER));
	articleLink[0].start_xy = 0;
	articleLink[0].end_xy = 0;
	articleLink[0].article_id = PREVIOUS_ARTICLE_LINK;
	externalLink[0].link_str = NULL;
	article_link_count = 1;
	language_link_count = 0;

	offset = sizeof(ARTICLE_HEADER) + sizeof(ARTICLE_LINK) * article_header.article_link_count;
	// externalLink[] is for storing the pointer to the language link string.
	// A corresponding artileLink (with the same index) will be used to store the start_xy and end_xy information.
	// The corresponding articleLink.article_id will be set to EXTERNAL_ARTICLE_LINK for distinguishing with normal article links.
	if ((idx_article & 0x00FFFFFF) > MAX_STATIC_ARTICLE_ID)
	{
		while (offset < article_header.offset_article && article_link_count < MAX_EXTERNAL_LINKS)
		{
			char *link_str;
			link_str = file_buffer + offset;
			if (wiki_lang_exist(link_str))
			{
				int bDuplicated = 0;
				for (i = 1; i < article_link_count; i++)
				{
					if (externalLink[i].link_str && duplicate_wiki_lang(link_str, externalLink[i].link_str))
					{
						externalLink[i].link_str = link_str;
						bDuplicated = 1;
					}
				}
				if (!bDuplicated)
				{
					if (article_link_count == 1) // add_show_hide_language_link() has not been called yet
					{
						add_show_hide_language_link();
					}
					if (lcd_draw_buf.current_x + LCD_LEFT_MARGIN + 2 * LANGUAGE_LINK_WIDTH + 2 * LANGUAGE_LINK_WIDTH_GAP >= LCD_BUF_WIDTH_PIXELS)
					{
						lcd_draw_buf.current_x = 0;
						lcd_draw_buf.current_y += LANGUAGE_LINK_HEIGHT + LANGUAGE_LINK_WIDTH_GAP;
					}
					start_x = lcd_draw_buf.current_x;
					start_y = lcd_draw_buf.current_y;
					end_x = lcd_draw_buf.current_x + LANGUAGE_LINK_WIDTH - 1;
					end_y = lcd_draw_buf.current_y + LANGUAGE_LINK_HEIGHT - 1;
					externalLink[article_link_count].link_str = link_str;
					draw_external_link(externalLink[article_link_count].link_str);
					articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
					articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
					articleLink[article_link_count].article_id = EXTERNAL_ARTICLE_LINK;
					article_link_count++;
					language_link_count++;
				}
			}
			offset += strlen(file_buffer + offset) + 1;
		}
	}

	if (restricted_article)
	{
		if (article_link_count == 1) // add_show_hide_language_link() has not been called yet
		{
			add_show_hide_language_link();
		}
		start_x = lcd_draw_buf.current_x;
		start_y = lcd_draw_buf.current_y;
		end_x = lcd_draw_buf.current_x + LANGUAGE_LINK_WIDTH - 1;
		end_y = lcd_draw_buf.current_y + LANGUAGE_LINK_HEIGHT - 1;
		draw_restricted_mark();
		articleLink[article_link_count].start_xy = (unsigned  long)(start_x | (start_y << 8));
		articleLink[article_link_count].end_xy = (unsigned  long)(end_x | (end_y << 8));
		articleLink[article_link_count].article_id = RESTRICTED_MARK_LINK;
		article_link_count++;
	}

	if (article_link_count > 1)
	{
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y += LANGUAGE_LINK_HEIGHT + LANGUAGE_LINK_WIDTH_GAP;
	}
	article_start_y_pos = lcd_draw_buf.current_y;
	draw_article_positioner(0);
	if (lcd_draw_init_y_pos < article_start_y_pos)
		lcd_draw_init_y_pos = article_start_y_pos;

	if(article_header.article_link_count > MAX_ARTICLE_LINKS - article_link_count)
		article_header.article_link_count = MAX_ARTICLE_LINKS - article_link_count;

	offset = sizeof(ARTICLE_HEADER);
	for(i = 0; i < article_header.article_link_count && article_link_count < MAX_ARTICLE_LINKS; i++)
	{
		memcpy(&articleLink[article_link_count],file_buffer+offset,sizeof(ARTICLE_LINK));
		nLinkWikiId = articleLink[article_link_count].article_id >> 24;
		if (((!nArticleWikiId || nArticleWikiId == nCurrentWikiId || get_wiki_idx_from_id(nArticleWikiId) >= 0) && !nLinkWikiId) ||
		    (nLinkWikiId && get_wiki_idx_from_id(nLinkWikiId) >= 0))
		{
			if (nArticleWikiId && !nLinkWikiId)
				articleLink[article_link_count].article_id |= nArticleWikiId << 24;
			if (article_start_y_pos)
			{
				start_x = articleLink[article_link_count].start_xy & 0xFF;
				start_y = (articleLink[article_link_count].start_xy >> 8) & 0xFFFFFF;
				end_x = articleLink[article_link_count].end_xy & 0xFF;
				end_y = (articleLink[article_link_count].end_xy >> 8) & 0xFFFFFF;
				articleLink[article_link_count].start_xy = (unsigned  long)(start_x | ((start_y + article_start_y_pos) << 8));
				articleLink[article_link_count].end_xy = (unsigned  long)(end_x | ((end_y + article_start_y_pos) << 8));
			}
			article_link_count++;
		}
		offset+=sizeof(ARTICLE_LINK);
	}

	article_buf_pointer = file_buffer+article_header.offset_article;

	display_first_page = 0; // use this to disable scrolling until the first page of the linked article is loaded
	//get_article_title_from_idx(idx_article, title);
	extract_title_from_article(NULL, title);
	history_add(idx_article, title, bKeepPos);
	saved_prev_idx_article = 0;
}

int isArticleLinkSelectedSequentialSearch(int x,int y, int start_i, int end_i)
{
	int i;
	int x_diff, y_diff;
	int last_x_diff = 999;
	int last_y_diff = 999;
	int rc = -1;
	int article_link_start_y_pos;
	int article_link_start_x_pos;
	int article_link_end_y_pos;
	int article_link_end_x_pos;
	int left_margin;

	if (display_mode == DISPLAY_MODE_ARTICLE)
		left_margin = LCD_LEFT_MARGIN;
	else
		left_margin = 0;

	i = start_i;
	while (i <= end_i)
	{
		article_link_start_x_pos = (articleLink[i].start_xy & 0x000000ff) + left_margin;
		article_link_start_y_pos = (articleLink[i].start_xy >> 8);
		article_link_end_x_pos = (articleLink[i].end_xy & 0x000000ff) + left_margin;
		article_link_end_y_pos = (articleLink[i].end_xy >> 8);

		if (y < article_link_start_y_pos)
			y_diff = article_link_start_y_pos - y;
		else if (y > article_link_end_y_pos)
			y_diff = y - article_link_end_y_pos;
		else
			y_diff = 0;

		if (x < article_link_start_x_pos)
			x_diff = article_link_start_x_pos - x;
		else if (x > article_link_end_x_pos)
			x_diff = x - article_link_end_x_pos;
		else
			x_diff = 0;

		if (x_diff <= LINK_X_DIFF_ALLOWANCE && y_diff <= LINK_Y_DIFF_ALLOWANCE)
		{
			if (((last_x_diff && !x_diff) || (last_y_diff && !y_diff)) ||
			    (x_diff < last_x_diff && y_diff < last_y_diff))
			{
				rc = i;
				last_x_diff = x_diff;
				last_y_diff = y_diff;
			}
		}
		i++;
	}
	return rc;
}

int isArticleLinkSelected(int x,int y)
{
	int i, start_i, end_i;
	int bFound;
	int article_link_start_y_pos;
	int article_link_start_x_pos;
	int article_link_end_y_pos;
	int article_link_end_x_pos;
	//char msg[1024];
	int left_margin;
	int origin_y;

	if (!display_first_page || request_display_next_page)
		return -1;

	if (display_mode == DISPLAY_MODE_ARTICLE)
		left_margin = LCD_LEFT_MARGIN;
	else
		left_margin = 0;

	origin_y = y;
	y += lcd_draw_cur_y_pos;

	if (link_currently_activated >= 0 && link_currently_activated < article_link_count)
	{
		article_link_start_x_pos = (articleLink[link_currently_activated].start_xy & 0x000000ff) + left_margin - LINK_X_DIFF_ALLOWANCE;
		article_link_start_y_pos = (articleLink[link_currently_activated].start_xy >> 8) - INITIAL_ARTICLE_SCROLL_THRESHOLD;
		article_link_end_x_pos = (articleLink[link_currently_activated].end_xy & 0x000000ff) + left_margin + LINK_X_DIFF_ALLOWANCE;
		article_link_end_y_pos = (articleLink[link_currently_activated].end_xy >> 8) + INITIAL_ARTICLE_SCROLL_THRESHOLD;
		if (y>=article_link_start_y_pos && y<=article_link_end_y_pos && x>=article_link_start_x_pos && x<=article_link_end_x_pos)
			return link_currently_activated; // if more than on links are matched, the last matched one got the higher priority
	}

	if (display_mode == DISPLAY_MODE_ARTICLE && x < PREVIOUS_ARTICLE_LINKABLE_SIZE &&
	    LCD_HEIGHT_LINES - PREVIOUS_ARTICLE_LINKABLE_SIZE <= origin_y && origin_y < LCD_HEIGHT_LINES &&
	    history_get_previous_idx(saved_idx_article, 0))
	{
		return 0; // PREVIOUS_ARTICLE_LINK
	}
	if (display_mode == DISPLAY_MODE_ARTICLE && (language_link_count || restricted_article) && lcd_draw_cur_y_pos == article_start_y_pos &&
	    LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN - LINK_X_DIFF_ALLOWANCE <= x && x <= LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN + LINK_X_DIFF_ALLOWANCE &&
	    article_start_y_pos + LCD_TOP_MARGIN - LINK_Y_DIFF_ALLOWANCE <= y && y <= article_start_y_pos + LCD_TOP_MARGIN + LANGUAGE_LINK_HEIGHT + LINK_Y_DIFF_ALLOWANCE)
	{
		return 1; // SHOW_LANGUAGE_LINK
	}
	if (display_mode == DISPLAY_MODE_ARTICLE && (language_link_count || restricted_article) && lcd_draw_cur_y_pos == 0 &&
	    LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN - LINK_X_DIFF_ALLOWANCE <= x && x <= LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN  + LINK_X_DIFF_ALLOWANCE &&
	    LCD_TOP_MARGIN - LINK_Y_DIFF_ALLOWANCE <= y && y <= LCD_TOP_MARGIN + LANGUAGE_LINK_HEIGHT + LINK_Y_DIFF_ALLOWANCE)
	{
		return 2; // HIDE_LANGUAGE_LINK
	}

	start_i = 0;
	end_i = article_link_count - 1;
	i = article_link_count / 2;
	bFound = 0;
	while (start_i <= end_i && !articleLink[start_i].start_xy && !articleLink[start_i].end_xy)
		start_i++; // skip the special links - PREVIOUS_ARTICLE_LINK, SHOW_LANGUAGE_LINK, HIDE_LANGUAGE_LINK
	while (!bFound && start_i <= end_i)
	{
		if (y < (int)(articleLink[i].start_xy >> 8) - LINK_Y_DIFF_ALLOWANCE)
		{
			end_i = i - 1;
			i = (start_i + end_i) / 2;
			continue;
		}
		if (y > (int)(articleLink[i].end_xy >> 8) + LINK_Y_DIFF_ALLOWANCE)
		{
			start_i = i + 1;
			i = (start_i + end_i) / 2;
			continue;
		}
		// y range identified
		return isArticleLinkSelectedSequentialSearch(x, y, start_i, end_i);
	}
	return -1;
}

#ifndef INCLUDED_FROM_KERNEL
int load_init_article(long idx_init_article)
{
	int fd;
	char file[128];
	long len;
	int i;
	int offset,start_x,start_y,end_x,end_y;
	ARTICLE_HEADER article_header;

	if (idx_init_article > 0)
	{
		sprintf(file, "./dat/%ld/%ld/%ld/%ld", (idx_init_article / 1000000), (idx_init_article / 10000) % 100, (idx_init_article / 100) % 100, idx_init_article);
		fd = wl_open(file, WL_O_RDONLY);
		if (fd >= 0)
		{
			len = wl_read(fd, (void *)file_buffer, FILE_BUFFER_SIZE - 1);
			file_buffer[len] = '\0';
			wl_close(fd);
		}

		memcpy(&article_header,file_buffer,sizeof(ARTICLE_HEADER));
		offset = sizeof(ARTICLE_HEADER);

		if(article_header.article_link_count>MAX_ARTICLE_LINKS)
			article_header.article_link_count = MAX_ARTICLE_LINKS;

		for(i = 0; i< article_header.article_link_count;i++)
		{
			memcpy(&articleLink[i],file_buffer+offset,sizeof(ARTICLE_LINK));
			offset+=sizeof(ARTICLE_LINK);

			start_y = articleLink[i].start_xy >>8;
			start_x = articleLink[i].start_xy & 0x000000ff;
			end_y   = articleLink[i].end_xy  >>8;
			end_x   = articleLink[i].end_xy & 0x000000ff;

		}
		article_link_count = article_header.article_link_count;

		article_buf_pointer = file_buffer+article_header.offset_article;
		init_render_article(0);
		return 0;
	}
	return -1;
}
#endif
#endif

#ifndef WIKIPCF
void display_link_article(long idx_article)
{
	request_y_pos = article_start_y_pos;
	if (idx_article == RESTRICTED_MARK_LINK)
	{
		bShowLanguageLinks = 0;
		display_article_with_pcf(article_start_y_pos);
		display_first_page = 1;
		filter_option();
		return;
	}
	else if (idx_article == PREVIOUS_ARTICLE_LINK)
	{
		idx_article = history_get_previous_idx(saved_idx_article, 1);
		if (!idx_article)
			return;
		last_display_mode = DISPLAY_MODE_HISTORY;
		saved_prev_idx_article = 0;
	}
	else if (idx_article == SHOW_LANGUAGE_LINK)
	{
		bShowLanguageLinks = 1;
		display_article_with_pcf(-article_start_y_pos);
		display_first_page = 1;
		return;
	}
	else if (idx_article == HIDE_LANGUAGE_LINK)
	{
		bShowLanguageLinks = 0;
		display_article_with_pcf(article_start_y_pos);
		display_first_page = 1;
		return;
	}
	else if (last_display_mode == DISPLAY_MODE_HISTORY)
		history_set_y_pos(idx_article);

	saved_idx_article = idx_article;
	file_buffer[0] = '\0';

	if (retrieve_article(idx_article))
	{
		return; // article not exist
	}

	if (restricted_article && check_restriction(idx_article))
		return;

	display_retrieved_article(idx_article);
}

void display_str(unsigned char *str)
{
	int start_x,end_x,start_y,end_y;
	int offset_x,offset_y;
	int str_width;
	unsigned char *p;

	p = str;

	// framebuffer_size ==0 iwhen WIKPCF is defined
	// This causes fatal linker errors of recent gcc (Ubuntu 9.04)
	int framebuffersize = framebuffer_size();
	memset(framebuffer_copy,0,framebuffersize);

	start_x = 0;
	end_x   = framebuffer_width();
	start_y = framebuffer_height()/2-9;
	end_y   = framebuffer_height()/2+9;

//       drawline_in_framebuffer_copy(framebuffer_copy,start_x,start_y,end_x,end_y);

	str_width = get_external_str_pixel_width(p, DEFAULT_FONT_IDX);
	offset_x = (end_x - str_width) / 2 - start_x;
	offset_y = 0;
	buf_draw_UTF8_str_in_copy_buffer((char *)framebuffer_copy,&str,start_x,end_x,start_y,end_y,offset_x,DEFAULT_FONT_IDX);

	repaint_framebuffer(framebuffer_copy,-1, 0);

}
#endif

int div_wiki(int a,int b)
{
	int c =0,m = 0;

	if(a<b)
		return 0;

	for(;;)
	{
		c += b;
		if(c >= a)
			return m;
		m++;
	}
}
void drawline_in_framebuffer_copy(unsigned char *buffer,int start_x,int start_y,int end_x,int end_y)
{
	int i,m;
	for(i = start_y;i<end_y;i++)
		for(m = start_x;m<end_x;m++)
			lcd_clear_pixel(buffer,m,i);

	for(i = start_x ; i < end_x ; i++)
		lcd_set_pixel(buffer,i, start_y);

	for(i = start_y ; i < end_y ; i++)
		lcd_set_pixel(buffer,start_x,i);

	for(i = start_x ; i < end_x ; i++)
		lcd_set_pixel(buffer,i,end_y);

	for(i = start_y ; i < end_y ; i++)
		lcd_set_pixel(buffer,end_x,i);
}

#ifndef WIKIPCF

// when set_article_link_number is called, the link will be "activated" in a pre-defined period
// the activated link will be inverted
void set_article_link_number(int num, unsigned long event_time)
{
	if (link_currently_activated < 0)
	{
		link_to_be_activated = num;
		link_to_be_activated_start_time = event_time;
	}
	else if (link_currently_activated != num || link_to_be_activated != num) // if on another link, deactivate both links since figer moves
	{
		reset_article_link_number();
	}
}

void reset_article_link_number(void)
{
	if (link_currently_inverted >= 0)
	{
		if (b_show_scroll_bar)
			show_scroll_bar(-1);
		invert_link(link_currently_inverted);
		if (b_show_scroll_bar)
			show_scroll_bar(-1);
	}
	link_currently_activated = -1;
	link_to_be_activated = -1;
	link_to_be_inverted = -1;
	link_currently_inverted = -1;
}

void init_invert_link(void)
{
	link_to_be_activated = -1;
	link_currently_activated = -1;
	link_to_be_inverted = -1;
	link_currently_inverted = -1;
}


int get_activated_article_link_number()
{
	return link_currently_activated;
}

void clear_article_pos_info()
{
	lcd_draw_cur_y_pos = 0;
	article_start_y_pos = 0;
}

void repaint_invert_link()
{
	if (link_currently_inverted >= 0)
	{
		invert_link(link_currently_inverted);
	}
}

void invert_link_area(int article_link_number)
{
	int start_x,start_y,end_x,end_y;
	int left_margin;

	if (display_mode == DISPLAY_MODE_ARTICLE)
		left_margin = LCD_LEFT_MARGIN;
	else
		left_margin = 0;

	start_y = (articleLink[article_link_number].start_xy >>8) - lcd_draw_cur_y_pos;
	start_x = (articleLink[article_link_number].start_xy & 0x000000ff) + left_margin - 1;
	if (start_x < 0)
		start_x = 0;
	end_y   = (articleLink[article_link_number].end_xy  >>8) - lcd_draw_cur_y_pos;
	end_x   = (articleLink[article_link_number].end_xy & 0x000000ff) + left_margin;
	if (end_x >= LCD_BUF_WIDTH_PIXELS)
		end_x = LCD_BUF_WIDTH_PIXELS - 1;

	if (start_y >= 0 || end_y < LCD_HEIGHT_LINES)
	{
		// guilib_invert_area will only invert (x, y) within LCD range
		guilib_fb_lock();
		guilib_invert_area(start_x, start_y, end_x, end_y);
		guilib_invert_area(start_x, start_y, start_x, start_y);
		guilib_invert_area(start_x, end_y, start_x, end_y);
		guilib_invert_area(end_x, start_y, end_x, start_y);
		guilib_invert_area(end_x, end_y, end_x, end_y);
		guilib_fb_unlock();
	}
}

int is_lcd_buf_area_blank(int start_x, int start_y, int end_x, int end_y)
{
	int x, y;

	if (end_x < 0 || start_x > LCD_BUF_WIDTH_PIXELS - 1)
		return 1;
	x = start_x;
	while (x <= end_x)
	{
		if (x % 8 || end_x - x < 8)
		{
			for (y = start_y; y <= end_y; y++)
			{
				if (lcd_draw_buf_get_pixel(x, y))
					return 0;
			}
			x++;
		}
		else
		{
			for (y = start_y; y <= end_y; y++)
			{
				if (lcd_draw_buf_get_byte(x, y))
					return 0;
			}
			x += 8;
		}
	}
	return 1;
}

int nothing_before_link(int article_link_number)
{
	int start_x,start_y,end_x,end_y;

	start_y = (articleLink[article_link_number].start_xy >> 8);
	start_x = (articleLink[article_link_number].start_xy & 0x000000ff) + LCD_LEFT_MARGIN - 1;
	if (start_x < 0)
		start_x = 0;
	end_y   = (articleLink[article_link_number].end_xy  >>8);
	end_x   = (articleLink[article_link_number].end_xy & 0x000000ff) + LCD_LEFT_MARGIN;
	if (is_lcd_buf_area_blank(0, start_y, start_x - 2, end_y))
		return ARTICLE_LINK_NOTHING_BEFORE;
	else
		return ARTICLE_LINK_SOMETHING_BEFORE;
}

int nothing_after_link(int article_link_number)
{
	int start_x,start_y,end_x,end_y;

	start_y = (articleLink[article_link_number].start_xy >> 8);
	start_x = (articleLink[article_link_number].start_xy & 0x000000ff) + LCD_LEFT_MARGIN - 1;
	if (start_x < 0)
		start_x = 0;
	end_y   = (articleLink[article_link_number].end_xy  >>8);
	end_x   = (articleLink[article_link_number].end_xy & 0x000000ff) + LCD_LEFT_MARGIN;
	if (is_lcd_buf_area_blank(end_x + 2, start_y, LCD_BUF_WIDTH_PIXELS - 1, end_y))
		return ARTICLE_LINK_NOTHING_AFTER;
	else
		return ARTICLE_LINK_SOMETHING_AFTER;
}

void invert_link(int article_link_number)
{
	long article_id;
	int local_link_number;
	int bNothingBeforeLink;
	int bNothingAfterLink;

	if(article_link_number<0)
		return;

	article_id = articleLink[article_link_number].article_id;
	invert_link_area(article_link_number);

	if (display_mode == DISPLAY_MODE_ARTICLE && article_id != EXTERNAL_ARTICLE_LINK)
	{
		if (!articleLinkBeforeAfter[article_link_number])
			articleLinkBeforeAfter[article_link_number] = nothing_before_link(article_link_number) | nothing_after_link(article_link_number);
		bNothingBeforeLink = articleLinkBeforeAfter[article_link_number] & ARTICLE_LINK_NOTHING_BEFORE;
		local_link_number = article_link_number - 1;
		while (bNothingBeforeLink && local_link_number >= 0 && article_id == articleLink[local_link_number].article_id)
		{
			if (!articleLinkBeforeAfter[local_link_number])
				articleLinkBeforeAfter[local_link_number] = nothing_before_link(local_link_number) | nothing_after_link(local_link_number);
			bNothingAfterLink = articleLinkBeforeAfter[local_link_number] & ARTICLE_LINK_NOTHING_AFTER;
			if (bNothingAfterLink)
			{
				invert_link_area(local_link_number);
				bNothingBeforeLink = articleLinkBeforeAfter[local_link_number] & ARTICLE_LINK_NOTHING_BEFORE;
				local_link_number--;
			}
			else
				bNothingBeforeLink = 0;
		}

		bNothingAfterLink = articleLinkBeforeAfter[article_link_number] & ARTICLE_LINK_NOTHING_AFTER;
		local_link_number = article_link_number + 1;
		while (bNothingAfterLink && local_link_number < article_link_count && article_id == articleLink[local_link_number].article_id)
		{
			if (!articleLinkBeforeAfter[local_link_number])
				articleLinkBeforeAfter[local_link_number] = nothing_before_link(local_link_number) | nothing_after_link(local_link_number);
			bNothingBeforeLink = articleLinkBeforeAfter[local_link_number] & ARTICLE_LINK_NOTHING_BEFORE;
			if (bNothingBeforeLink)
			{
				invert_link_area(local_link_number);
				bNothingAfterLink = articleLinkBeforeAfter[local_link_number] & ARTICLE_LINK_NOTHING_AFTER;
				local_link_number++;
			}
			else
				bNothingAfterLink = 0;
		}
	}
}

// invert link when timeout
int check_invert_link()
{
	if (link_to_be_activated >= 0 && time_diff(get_time_ticks(), link_to_be_activated_start_time) >=
	    seconds_to_ticks(LINK_ACTIVATION_TIME_THRESHOLD))
	{
		link_currently_activated = link_to_be_activated;
		link_to_be_inverted = link_to_be_activated;
		link_to_be_inverted_start_time = link_to_be_activated_start_time;
		link_to_be_activated = -1;
		if (link_currently_inverted >= 0)
		{
			invert_link(link_currently_inverted);
			link_currently_inverted = -1;
		}
	}

	if (link_to_be_inverted >= 0 &&
	    ((display_mode == DISPLAY_MODE_ARTICLE &&
	      time_diff(get_time_ticks(), link_to_be_inverted_start_time) >= seconds_to_ticks(LINK_INVERT_ACTIVATION_TIME_THRESHOLD)) ||
	     time_diff(get_time_ticks(), link_to_be_inverted_start_time) >= seconds_to_ticks(LIST_LINK_INVERT_ACTIVATION_TIME_THRESHOLD)))
	{
		if (link_currently_inverted >= 0)
			invert_link(link_currently_inverted);
		invert_link(link_to_be_inverted);
		link_currently_inverted = link_to_be_inverted;
		link_to_be_inverted = -1;
	}

	if (link_to_be_activated >= 0 || link_to_be_inverted >= 0)
		return 1;
	else
		return 0;
}

void open_article_link(int x,int y)
{
	int article_link_number;
	long idx_article;

	article_link_number = isArticleLinkSelected(x,y);
	if(article_link_number >= 0)
	{
		idx_article = articleLink[article_link_number].article_id;
		if (idx_article == EXTERNAL_ARTICLE_LINK)
		{
			idx_article = wiki_lang_link_search(externalLink[article_link_number].link_str);
			if (!idx_article)
				return;
		}
		display_link_article(idx_article);
	}
}

void open_article_link_with_link_number(int article_link_number)
{
	long idx;

	if (article_link_number < 0 || articleLink[article_link_number].article_id <= 0)
		return;
	display_first_page = 0; // use this to disable scrolling until the first page of the linked article is loaded
	idx = articleLink[article_link_number].article_id;
	if (idx == RESTRICTED_MARK_LINK)
	{
#ifdef INCLUDED_FROM_KERNEL
		delay_us(100000);
#endif
		invert_link(article_link_number);
	}
	else if (idx == EXTERNAL_ARTICLE_LINK)
	{
		idx = wiki_lang_link_search(externalLink[article_link_number].link_str);
		if (!idx)
			return;
	}
	display_link_article(idx);
}
#endif

void msg_info(char *data)
{
#ifdef WIKIPCF
	printf(data);
#else
	msg(MSG_INFO,data);
#endif
}
int framebuffer_size()
{
#ifdef WIKIPCF
	return 0;
#else
	return guilib_framebuffer_size();
#endif
}
int framebuffer_width()
{
#ifdef WIKIPCF
	return 0;
#else
	return guilib_framebuffer_width();
#endif

}
int framebuffer_height()
{
#ifdef WIKIPCF
	return 0;
#else
	return guilib_framebuffer_height();
#endif

}

int strchr_idx(char *s, char c)
{
	int rc = -1;
	int i = 0;

	while (rc < 0 && s[i])
	{
		if (s[i] == c)
			rc = i;
		i++;
	}
	return rc;
}

#ifndef WIKIPCF
int draw_bmf_char(ucs4_t u,int font,int x,int y, int inverted, int b_clear)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	//pcf_SCcharmet_t sm;
	int bytes_to_process;
	int x_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index
	int j; // bitmap bit index
	pcffont_bmf_t *pPcfFont;

	pPcfFont = &pcfFonts[font];

	pres_bmfbm(u, pPcfFont, &bitmap, &Cmetrics);
	if (bitmap == NULL && u != 32)
	{
		return -1;
	}

	if (b_clear)
	{
		for (i = 0; i < pPcfFont->Fmetrics.linespace; i++)
		{
			for (j = 0; j < Cmetrics.widthDevice; j++)
			{
				if (inverted)
					lcd_set_framebuffer_pixel(x + j, y + i);
				else
					lcd_clear_framebuffer_pixel(x + j, y + i);
			}
		}
	}
	if(u==32)
	{
		x += Cmetrics.widthDevice;
		return x;
	}

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;

	x_base = x + Cmetrics.LSBearing;
	x_offset = 0;
	y_offset = pPcfFont->Fmetrics.linespace - (pPcfFont->Fmetrics.descent + Cmetrics.ascent);


	x_bit_idx = x_base & 0x07;

	if (x + Cmetrics.widthDevice > LCD_BUF_WIDTH_PIXELS)
		return -1;

	for (i = 0; i < bytes_to_process; i++)
	{
		j = 7;
		while (j >= 0)
		{
			if (x_offset >= Cmetrics.widthBits)
			{
				x_offset = 0;
				y_offset++;
			}
			if (x_offset < Cmetrics.width)
			{
				if (bitmap[i] & (1 << j))
				{
					if (inverted)
						lcd_clear_framebuffer_pixel(x_base + x_offset, y+y_offset);
					else
						lcd_set_framebuffer_pixel(x_base + x_offset, y+y_offset);
				}
				//else if (b_clear)
				//{
				//	if (inverted)
				//		lcd_set_framebuffer_pixel(x_base + x_offset, y+y_offset);
				//	else
				//		lcd_clear_framebuffer_pixel(x_base + x_offset, y+y_offset);
				//}

			}
			x_offset++;
			x_bit_idx++;
			if (!(x_bit_idx & 0x07))
			{
				x_bit_idx = 0;

			}
			j--;
		}
	}
	x += Cmetrics.widthDevice;
	return x;
}

int buf_draw_bmf_char(char *buf, ucs4_t u,int font,int x,int y, int inverted)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	//pcf_SCcharmet_t sm;
	int bytes_to_process;
	int x_base;
	int x_offset;
	int y_offset;
	int x_bit_idx;
	int i; // bitmap byte index
	int j; // bitmap bit index
	pcffont_bmf_t *pPcfFont;

	pPcfFont = &pcfFonts[font];

	pres_bmfbm(u, pPcfFont, &bitmap, &Cmetrics);
	if (bitmap == NULL)
	{
		return -1;
	}

	if(u==32)
	{
		x += Cmetrics.widthDevice;
		return x;
	}

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;

	x_base = x + Cmetrics.LSBearing;
	x_offset = 0;
	y_offset = pPcfFont->Fmetrics.linespace - (pPcfFont->Fmetrics.descent + Cmetrics.ascent);


	x_bit_idx = x_base & 0x07;

	if (x + Cmetrics.widthDevice >= LCD_BUF_WIDTH_PIXELS)
		return -1;

	for (i = 0; i < bytes_to_process; i++)
	{
		j = 7;
		while (j >= 0)
		{
			if (x_offset >= Cmetrics.widthBits)
			{
				x_offset = 0;
				y_offset++;
			}
			if (x_offset < Cmetrics.width)
			{
				if (bitmap[i] & (1 << j))
				{
					if (inverted)
					{
						unsigned int byte = ((x_base+x_offset) + LCD_VRAM_WIDTH_PIXELS * (y+y_offset)) / 8;
						unsigned int bit  = ((x_base+x_offset) + LCD_VRAM_WIDTH_PIXELS * (y+y_offset)) % 8;
						buf[byte] ^= (1 << (7 - bit));
					}
					else
					{
						unsigned int byte = ((x_base+x_offset) + LCD_VRAM_WIDTH_PIXELS * (y+y_offset)) / 8;
						unsigned int bit  = ((x_base+x_offset) + LCD_VRAM_WIDTH_PIXELS * (y+y_offset)) % 8;
						buf[byte] |= (1 << (7 - bit));
					}
				}

			}
			x_offset++;
			x_bit_idx++;
			if (!(x_bit_idx & 0x07))
			{
				x_bit_idx = 0;

			}
			j--;
		}
	}
	x += Cmetrics.widthDevice;
	return x;
}
#endif

int GetFontLinespace(int font)
{
	return pcfFonts[font - 1].Fmetrics.linespace;
}

void msg_on_lcd(int x, int y, char *fmt, ...)
{
#ifdef INCLUDED_FROM_KERNEL
	va_list args;
	char msg[100];
	va_start(args, fmt);
	vsprintf (msg, fmt, args);
	guilib_clear_area(x, y, 239, y+18);
	render_string(DEFAULT_FONT_IDX, x, y, msg, strlen(msg), 0);
	va_end(args);
#endif
}

void msg_on_lcd_clear(int x, int y)
{
#ifdef INCLUDED_FROM_KERNEL
//	guilib_fb_lock();
	guilib_clear_area(x, y, 239, y+18);
//	guilib_fb_unlock();
#endif
}

void extract_title_from_article(unsigned char *article_buf, unsigned char *title)
{
	unsigned char c, c2;
	int bDone = 0;
	ARTICLE_HEADER article_header;
	int lenTitle = 0;

	if (!article_buf)
		article_buf = file_buffer;
	memcpy(&article_header, article_buf, sizeof(ARTICLE_HEADER));
	article_buf += article_header.offset_article;

	while (!bDone)
	{
		c = *article_buf;
		if (c <= MAX_ESC_CHAR)
		{
			article_buf++;
			switch(c)
			{
			case ESC_0_SPACE_LINE:
				article_buf++;
				break;
			case ESC_2_NEW_LINE_SAME_FONT:
				title[lenTitle++] = ' '; // append a blank for wrapped title
				break;
			case ESC_3_NEW_LINE_WITH_FONT:
				c2 = *article_buf++;
				if ((c2 & 0x07) != TITLE_FONT_IDX)
					bDone = 1;
				break;
			default:
				bDone = 1;
				break;
			}
		}
		else
		{
			title[lenTitle++] = *article_buf++;
			if (lenTitle >= MAX_TITLE_ACTUAL - 1)
				bDone = 1;
		}
	}
	title[lenTitle] = '\0';
}

void draw_progress_bar(int progressCount)
{
	int x;

	if (!progressCount)
	{
		memset(framebuffer, 0, LCD_BUF_WIDTH_BYTES * 4);
	}
	else
	{
		if (progressCount > LCD_BUF_WIDTH_PIXELS)
			progressCount = LCD_BUF_WIDTH_PIXELS;

		x = 0;
		while (x < progressCount)
		{
			if (x % 8 || progressCount - x < 8)
			{
				lcd_set_framebuffer_pixel(x, 1);
				lcd_set_framebuffer_pixel(x, 2);
				x++;
			}
			else
			{
				lcd_set_framebuffer_byte(0xFF, x, 1);
				lcd_set_framebuffer_byte(0xFF, x, 2);
				x += 8;
			}
		}

		while (x < LCD_BUF_WIDTH_PIXELS)
		{
			if (x % 8 || LCD_BUF_WIDTH_PIXELS - x < 8)
			{
				lcd_clear_framebuffer_pixel(x, 1);
				lcd_clear_framebuffer_pixel(x, 2);
				x++;
			}
			else
			{
				lcd_set_framebuffer_byte(0, x, 1);
				lcd_set_framebuffer_byte(0, x, 2);
				x += 8;
			}
		}
	}
}
