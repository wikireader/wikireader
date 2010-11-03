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
#include <stdarg.h>
#include <inttypes.h>

#include <grifo.h>

#include "guilib.h"
#include "ustring.h"
#include "history.h"
#include "search.h"
#include "glyph.h"
#include "wikilib.h"
#include "restricted.h"
#include "wiki_info.h"
#include "bmf.h"
#include "lcd_buf_draw.h"
#include "search.h"
#include "bigram.h"
#include "utf8.h"
#include "highlight.h"

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

extern long finger_move_speed;
extern int last_display_mode;
extern int display_mode;
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
unsigned char articleLinkBeforeAfter[MAX_ARTICLE_LINKS];
PARTICLE_RENDER_INFO pArticleRenderInfo;
int nArticleRenderedLines = 0;

int link_to_be_activated = -1;
unsigned long link_to_be_activated_start_time = 0;
int link_to_be_inverted = -1;
unsigned long link_to_be_inverted_start_time = 0;
int link_currently_activated = -1;
int link_currently_inverted = -1;

int article_link_count;
int language_link_count;
int display_first_page = 0;

void display_link_article(long idx_article);
void drawline_in_framebuffer_copy(unsigned char *buffer,int start_x,int start_y,int end_x,int end_y);
void buf_draw_char_external(LCD_DRAW_BUF *lcd_draw_buf_external,ucs4_t u,int start_x,int end_x,int start_y,int end_y);
void repaint_framebuffer(unsigned char *buf, int pos, int b_repaint_invert_link);
void repaint_invert_link(void);
char* FontFile(int idx);
int framebuffer_size();
int framebuffer_width();
int framebuffer_height();

unsigned char *framebuffer_copy;
char msg_out[1024];
long article_scroll_increment;
unsigned long time_scroll_article_last=0;
int stop_render_article = 0;
int b_show_scroll_bar = 0;
long saved_idx_article = 0;
long saved_prev_idx_article = 0;

#define MIN_BAR_LEN 20
void show_scroll_bar(int bShow)
{
	int bar_len;
	int bar_pos;
	int i;
	int byte_idx;
	char c;
	static char frame_bytes[LCD_HEIGHT];
	static int b_frame_bytes;

	if (lcd_draw_buf.current_y < LCD_HEIGHT)
		return;
	if (bShow <= 0)
	{
		if (b_frame_bytes)
		{
			for (i = 0; i < LCD_HEIGHT; i++)
			{
				byte_idx = (236 + LCD_BUFFER_WIDTH * i) / 8;
				lcd_framebuffer_set_byte(byte_idx, frame_bytes[i]);
			}
			b_frame_bytes = 0;
		}
	}
	else
	{
		bar_len = LCD_HEIGHT * LCD_HEIGHT / lcd_draw_buf.current_y;
		if (bar_len > LCD_HEIGHT)
			bar_len = LCD_HEIGHT;
		else if (bar_len < MIN_BAR_LEN)
			bar_len = MIN_BAR_LEN;
		if (lcd_draw_buf.current_y > LCD_HEIGHT)
			bar_pos = (LCD_HEIGHT - bar_len) * lcd_draw_cur_y_pos / (lcd_draw_buf.current_y - LCD_HEIGHT);
		else
			bar_pos = 0;
		if (bar_pos < 0)
			bar_pos = 0;
		else if (bar_pos + bar_len > LCD_HEIGHT)
			bar_pos = LCD_HEIGHT - bar_len;

		for (i = 0; i < LCD_HEIGHT; i++)
		{
			if (bar_pos <= i && i < bar_pos + bar_len)
				c = 0x07;
			else
				c = 0;
			byte_idx = (236 + LCD_BUFFER_WIDTH * i) / 8;
			frame_bytes[i] = lcd_framebuffer_get_byte(byte_idx);
			lcd_framebuffer_set_byte(byte_idx, (frame_bytes[i] & 0xF0) | c);
		}
		b_frame_bytes = 1;
	}
}

void load_all_fonts()
{
	int i;

	if (!lcd_draw_buf_inited)
		init_lcd_draw_buf();
	for (i=0; i < FONT_COUNT; i++)
	{
		if (pcfFonts[i].fd == FONT_FD_NOT_INITED) {
			pcfFonts[i].fd = load_bmf(&pcfFonts[i]);
			if (pcfFonts[i].fd < 0) {
				fatal_error("Missing font file: %s", pcfFonts[i].file);
			}
		}
	}
}


void init_lcd_draw_buf()
{
	int i,framebuffersize;

	if (!lcd_draw_buf_inited)
	{
		framebuffersize = framebuffer_size();
		framebuffer_copy = (unsigned char*)memory_allocate(framebuffersize, "bufdraw1");
		lcd_draw_buf.screen_buf = (unsigned char *)memory_allocate(LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS, "bufdraw2");
		if (!framebuffer_copy || !lcd_draw_buf.screen_buf)
			fatal_error("lcd_draw_buf allocation error");

		for (i=0; i < FONT_COUNT; i++)
		{
			pcfFonts[i].file = FontFile(i);
			if (!pcfFonts[i].file[0])
			{
				pcfFonts[i].fd = -1;
			}
			else
			{
				pcfFonts[i].fd = FONT_FD_NOT_INITED;
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
			}

		}
		pArticleRenderInfo = (PARTICLE_RENDER_INFO)memory_allocate(sizeof(ARTICLE_RENDER_INFO) * MAX_LINES_PER_ARTICLE, "renderinfo");
		if (!pArticleRenderInfo)
			fatal_error("pArticleRenderInfo allocation error");
		lcd_draw_buf_inited = 1;
	}
	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = 0;
	lcd_draw_buf.drawing = 0;
	lcd_draw_buf.pPcfFont = NULL;
	lcd_draw_buf.line_height = 0;
	lcd_draw_buf.y_adjustment = 0;
	nArticleRenderedLines = 0;

	if (lcd_draw_buf.screen_buf)
		memset(lcd_draw_buf.screen_buf, 0, LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);
}

void draw_string(const unsigned char *s)
{
	//ucs4_t u;
	const unsigned char **p = &s;

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
		while (y_pos - last_positioner_y - article_start_y_pos > LCD_HEIGHT / 2)
		{
			last_positioner_y += LCD_HEIGHT / 2;
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y - 2);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y - 1);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 1, last_positioner_y - 1);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 1, last_positioner_y);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 2, last_positioner_y);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y + 1);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 1, last_positioner_y + 1);
			guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, 0, last_positioner_y + 2);
		}
	}
}

void init_file_buffer()
{
	file_buffer = (unsigned char*)memory_allocate(FILE_BUFFER_SIZE, "bufdraw3");
	if (!file_buffer)
		fatal_error("file_buffer allocation error");
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

void buf_draw_UTF8_str_in_copy_buffer(unsigned char *framebuffer_copy, const unsigned char **pUTF8,
				      int start_x,int end_x,int start_y,int end_y,int offset_x,int font_idx)
{
	ucs4_t u;
	LCD_DRAW_BUF lcd_draw_buf_external;

	lcd_draw_buf_external.current_x = start_x+offset_x;
	lcd_draw_buf_external.current_y = start_y+2;
	lcd_draw_buf_external.pPcfFont = &pcfFonts[font_idx - 1];
	lcd_draw_buf_external.screen_buf = framebuffer_copy;
	lcd_draw_buf_external.line_height = pcfFonts[font_idx - 1].Fmetrics.linespace + LINE_SPACE_ADDON;

	lcd_draw_buf_external.y_adjustment = 0;

	while (**pUTF8 > MAX_ESC_CHAR)
	{
		if ((u = UTF8_to_UCS4(pUTF8)))
		{
			buf_draw_char_external(&lcd_draw_buf_external,u,start_x,end_x,start_y,end_y);
		}

	}
}

void buf_draw_UTF8_str(const unsigned char **pUTF8)
{
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
			lcd_draw_buf.y_adjustment = 0;
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
			lcd_draw_buf.y_adjustment = 0;
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.current_y = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - 1;
			draw_article_positioner(lcd_draw_buf.current_y);
			break;
		case ESC_2_NEW_LINE_SAME_FONT: /* new line with previous font and line space */
			lcd_draw_buf.current_x = 0;
			lcd_draw_buf.current_y += lcd_draw_buf.actual_height;
			lcd_draw_buf.actual_height = lcd_draw_buf.line_height;
			lcd_draw_buf.y_adjustment = 0;
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
			lcd_draw_buf.y_adjustment = 0;
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
			lcd_draw_buf.y_adjustment = ((signed char)c2 >> 3);
			if (lcd_draw_buf.current_y + lcd_draw_buf.line_height + lcd_draw_buf.y_adjustment >= LCD_BUF_HEIGHT_PIXELS)
				lcd_draw_buf.y_adjustment = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			if (lcd_draw_buf.current_y + lcd_draw_buf.y_adjustment < lcd_draw_buf.line_height - 1)
				lcd_draw_buf.y_adjustment = lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			break;
		case ESC_5_RESET_TO_DEFAULT_FONT: /* reset to the default font */
			lcd_draw_buf.pPcfFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
			break;
		case ESC_6_RESET_TO_DEFAULT_ALIGN: /* reset to the default vertical alignment */
			lcd_draw_buf.x_adjustment = 0;
			break;
		case ESC_7_FORWARD: /* forward */
			c2 = **pUTF8;
			(*pUTF8)++;
			lcd_draw_buf.current_x += c2;
			if (lcd_draw_buf.current_x > LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - lcd_draw_buf.x_adjustment)
				lcd_draw_buf.current_x = LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - lcd_draw_buf.x_adjustment;
			break;
		case ESC_8_BACKWARD: /* backward */
			c2 = **pUTF8;
			(*pUTF8)++;
			if (lcd_draw_buf.current_x < c2)
				lcd_draw_buf.current_x = 0;
			else
				lcd_draw_buf.current_x -= c2;
			break;
		case ESC_9_Y_ADJUSTMENT: /* vertical alignment adjustment */
			c3 = (char)(**pUTF8);
			(*pUTF8)++;
			lcd_draw_buf.x_adjustment += c3;
			//if (lcd_draw_buf.current_y + lcd_draw_buf.line_height + lcd_draw_buf.y_adjustment >= LCD_BUF_HEIGHT_PIXELS)
			//	lcd_draw_buf.y_adjustment = LCD_BUF_HEIGHT_PIXELS - lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			//if (lcd_draw_buf.current_y + lcd_draw_buf.y_adjustment < lcd_draw_buf.line_height - 1)
			//	lcd_draw_buf.y_adjustment = lcd_draw_buf.line_height - lcd_draw_buf.current_y - 1;
			break;
		case ESC_10_HORIZONTAL_LINE: /* drawing horizontal line */
			c2 = **pUTF8;
			(*pUTF8)++;
			if ((long)c2 > lcd_draw_buf.current_x)
				c2 = (unsigned char)lcd_draw_buf.current_x;
			buf_draw_horizontal_line(lcd_draw_buf.current_x - (unsigned long)c2 + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment,
						 lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment);
			break;
		case ESC_11_VERTICAL_LINE: /* drawing vertical line */
			c2 = **pUTF8;
			(*pUTF8)++;
			v_line_bottom = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
			v_line_bottom -= lcd_draw_buf.y_adjustment;
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
			lcd_draw_buf.y_adjustment = 0;
			buf_draw_horizontal_line(LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment, LCD_BUF_WIDTH_PIXELS);
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
			if ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment) % 8 == 0)
			{
				nBytes = (nWidth + 7) / 8;
				if (nBytes > LCD_BUF_WIDTH_BYTES - 1)
					nBytes = LCD_BUF_WIDTH_BYTES - 1;
				nByteIdx = (lcd_draw_buf.current_x  + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment) / 8;
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
							guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, lcd_draw_buf.current_x +
										LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment + j, nImageY);
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
		const unsigned char *pTemp = *pUTF8; // save the position before UTF8_to_UCS4 changes pUTF8
		if ((u = UTF8_to_UCS4(pUTF8)))
		{
			if ((lcd_draw_buf.current_x <= 0 || nArticleRenderedLines == 0) && nArticleRenderedLines < MAX_LINES_PER_ARTICLE)
			{
				nArticleRenderedLines++;
				pArticleRenderInfo[nArticleRenderedLines - 1].start_y = lcd_draw_buf.current_y;
				pArticleRenderInfo[nArticleRenderedLines - 1].end_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height - 1;
				pArticleRenderInfo[nArticleRenderedLines - 1].pBuf = pTemp;
				pArticleRenderInfo[nArticleRenderedLines - 1].pPcfFont = lcd_draw_buf.pPcfFont;
			}

			buf_draw_char(u);
			if(display_first_page==0 && lcd_draw_buf.current_y > LCD_HEIGHT + article_start_y_pos)
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
				lcd_set_pixel(j, i, LCD_BLACK);
			}
		}
		else if (1 + LCD_TOP_MARGIN < i && i < LCD_TOP_MARGIN + LANGUAGE_LINK_HEIGHT - 2)
		{
			for (j = LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 1; j < LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN - 1; j++)
			{
				lcd_set_pixel(j, i, LCD_BLACK);
			}
		}

		if (8 + LCD_TOP_MARGIN <= i && i <= 8 + LCD_TOP_MARGIN + MAX_PIXELS_EACH_SIDE)
		{
			if (bShowLanguageLinks)
			{
				if (nPixelsEachSide >= 0)
				{
					for (j = -nPixelsEachSide; j <= nPixelsEachSide; j++)
						lcd_set_pixel(LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 11 + j, i, LCD_WHITE);
					nPixelsEachSide--;
				}
			}
			else
			{
				if (nPixelsEachSide <= MAX_PIXELS_EACH_SIDE)
				{
					for (j = -nPixelsEachSide; j <= nPixelsEachSide; j++)
						lcd_set_pixel(LCD_BUF_WIDTH_PIXELS - LANGUAGE_LINK_WIDTH - LCD_LEFT_MARGIN + 11 + j, i, LCD_WHITE);
					nPixelsEachSide++;
				}
			}
		}
	}
}

void repaint_framebuffer(unsigned char *buf, int pos, int b_repaint_invert_link)
{
	(void)b_repaint_invert_link; // *** unused argument
	int framebuffersize;
	framebuffersize = framebuffer_size();

	guilib_fb_lock();
	//guilib_clear();

	memcpy(lcd_get_framebuffer(),buf+(pos < 0 ? 0 : pos)*LCD_BUFFER_WIDTH/8,framebuffersize);
	if (display_mode == DISPLAY_MODE_ARTICLE && (language_link_count || restricted_article) && (pos == article_start_y_pos || pos == 0))
	{
		draw_language_link_arrow();
	}
//	if (b_repaint_invert_link)
//		repaint_invert_link();
	if (b_show_scroll_bar)
		show_scroll_bar(1);
	guilib_fb_unlock();
}

void buf_draw_horizontal_line(unsigned long start_x, unsigned long end_x)
{
	unsigned long i;
	long h_line_y;


	h_line_y = lcd_draw_buf.current_y + lcd_draw_buf.line_height;
	h_line_y -= lcd_draw_buf.y_adjustment + 1;
	if (end_x > LCD_BUF_WIDTH_PIXELS)
		end_x = LCD_BUF_WIDTH_PIXELS;

	for(i = start_x;i<end_x;i++)
	{
		guilib_buffer_set_pixel(lcd_draw_buf.screen_buf,i, h_line_y);
	}

}

void buf_draw_vertical_line(unsigned long start_y, unsigned long end_y)
{
	unsigned long idx_in_byte;
	unsigned char *p;

	if (lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment < LCD_BUF_WIDTH_PIXELS)
	{
		idx_in_byte = 7 - ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment) & 0x07);
		p = lcd_draw_buf.screen_buf + start_y * LCD_BUF_WIDTH_BYTES + ((lcd_draw_buf.current_x + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment)>> 3);
		while (start_y <= end_y)
		{
			*p |= 1 << idx_in_byte;
			start_y++;
			p += LCD_BUF_WIDTH_BYTES;
		}
	}
}

char lcd_draw_buf_get_byte(int x, int y)
{
	unsigned int byte = (x + LCD_BUFFER_WIDTH * y) / 8;

	return lcd_draw_buf.screen_buf[byte];
}

int lcd_draw_buf_get_pixel(int x, int y)
{
	unsigned int byte = (x + LCD_BUFFER_WIDTH * y) / 8;
	unsigned int bit  = (x + LCD_BUFFER_WIDTH * y) % 8;

	if (lcd_draw_buf.screen_buf[byte] & (1 << (7 - bit)))
		return 1;
	else
		return 0;
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

	x_base = lcd_draw_buf.current_x + Cmetrics.LSBearing + LCD_LEFT_MARGIN + lcd_draw_buf.x_adjustment;
	if (x_base < LCD_BUF_WIDTH_PIXELS)
	{ // only draw the chracter if there is space left before the right margin of the LCD screen
		y_base = lcd_draw_buf.current_y + lcd_draw_buf.y_adjustment;
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
						guilib_buffer_set_pixel(lcd_draw_buf.screen_buf,x_base + x_offset, y_base+y_offset);
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

int get_external_str_pixel_width(const unsigned char *pIn, int font_idx)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int width = 0;
	ucs4_t u;
	const unsigned char **pUTF8 = &pIn;

	while (**pUTF8 > MAX_ESC_CHAR)
	{
		if ((u = UTF8_to_UCS4(pUTF8)))
		{
			pres_bmfbm(u, &pcfFonts[font_idx - 1], &bitmap, &Cmetrics);
			if (bitmap != NULL)
				width += Cmetrics.widthDevice;
		}

	}
	return width;
}

void get_external_str_pixel_rectangle(const unsigned char *pIn, int font_idx,
				      int *start_x, int *start_y, int *end_x, int *end_y)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	ucs4_t u;
	const unsigned char **pUTF8 = &pIn;
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
		if ((u = UTF8_to_UCS4(pUTF8)))
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
	(void)start_y; // *** unused argument
	(void)end_y; // *** unused argument

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
	y_base = lcd_draw_buf_external->current_y + lcd_draw_buf_external->y_adjustment;
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
					guilib_buffer_set_pixel(lcd_draw_buf_external->screen_buf,x_base + x_offset, y_base+y_offset);
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

int get_UTF8_char_width(int idxFont, const unsigned char **pContent, long *lenContent, int *nCharBytes)
{
	ucs4_t u;
	const unsigned char *pBase;
	charmetric_bmf Cmetrics;
	bmf_bm_t *bitmap;

	pBase = *pContent;
	u = UTF8_to_UCS4(pContent);
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

	return ((0x1100 <= u && u <= 0x11FF) || // Hangul
		(0x2200 <= u && u <= 0x2BFF) || // symbols
		(0x2E00 <= u) || // symbols or CJK
		(u < 0x100 && strchr(" ~!@#$%^&*()-_+=[]\{}|;;':\",./", c)));
}

int extract_str_fitting_width(const unsigned char **pIn, unsigned char *pOut, int max_width, int font_idx)
{
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	int width = 0;
	int widthFitted = 0;
	ucs4_t u;
	const unsigned char *pOrigIn;
	int nBytesFittingWidth = 0;
	int nLastBytes, nLastWidth;

	pOrigIn = *pIn;
	pOut[0] = '\0';
	while (**pIn > MAX_ESC_CHAR && width <= max_width)
	{
		nLastBytes = *pIn - pOrigIn;
		nLastWidth = width;
		if ((u = UTF8_to_UCS4(pIn)))
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
		ustrcpy(pOut, pOrigIn);
		widthFitted = width;
	}
	return widthFitted;
}

void init_render_article(long init_y_pos)
{

	//if(lcd_draw_buf.current_y>0)
	//  memset(lcd_draw_buf.screen_buf,0,lcd_draw_buf.current_y*LCD_BUFFER_WIDTH/8);
	highlight_reset(-1, -1, false);
	if (lcd_draw_buf.screen_buf)
		memset(lcd_draw_buf.screen_buf, 0, LCD_BUF_WIDTH_BYTES * LCD_BUF_HEIGHT_PIXELS);

	article_buf_pointer = NULL;
	lcd_draw_buf.current_x = 0;
	lcd_draw_buf.current_y = 0;
	lcd_draw_buf.drawing = 0;
	lcd_draw_buf.pPcfFont = NULL;
	lcd_draw_buf.line_height = 0;
	lcd_draw_buf.actual_height = 0;
	lcd_draw_buf.y_adjustment = 0;
	lcd_draw_buf.x_adjustment = 0;

	display_first_page = 0;
	lcd_draw_cur_y_pos = 0;
	article_start_y_pos = 0;
	bShowLanguageLinks = 0;
	lcd_draw_init_y_pos = init_y_pos;
	finger_move_speed = 0;
	lcd_draw_buf_pos = 0;
	nArticleRenderedLines = 0;
}

void render_wikipedia_license_text(void)
{
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
		lcd_draw_cur_y_pos = request_y_pos - LCD_HEIGHT;
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
				lcd_draw_cur_y_pos = request_y_pos - LCD_HEIGHT;
				if (lcd_draw_cur_y_pos + LCD_HEIGHT > lcd_draw_buf.current_y)
				{
					lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT;
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
		lcd_draw_buf.x_adjustment = 0;
		lcd_draw_buf.y_adjustment = 0;
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
				long y_pos = lcd_draw_buf.current_y - LCD_HEIGHT;
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

		if(request_display_next_page > 0 && lcd_draw_buf.current_y >= request_y_pos+LCD_HEIGHT)
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
		lcd_draw_buf.x_adjustment = 0;
		lcd_draw_buf.y_adjustment = 0;
		draw_string(get_nls_text("history_title"));
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_LIST_FONT_IDX - 1];
		lcd_draw_buf.line_height = HISTORY_RESULT_HEIGHT;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = HISTORY_RESULT_START;
		article_link_count = 0;
		language_link_count = 0;
	}

	if (history_count == 0) {
		const unsigned char *p = get_nls_text("no_history");
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
				long y_pos = lcd_draw_buf.current_y - LCD_HEIGHT;
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

		if(request_display_next_page > 0 && lcd_draw_buf.current_y >= request_y_pos+LCD_HEIGHT)
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
		memcpy(lcd_get_framebuffer(), lcd_draw_buf.screen_buf, framebuffer_size()); // copy from the LCD frame buffer (for the first page)
	}
}

int render_search_result_with_pcf(void)
{
	int rc = 0;
	int start_x, end_x, start_y, end_y;
	long idxArticle;
	unsigned char sTitleActual[MAX_TITLE_ACTUAL];
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
		memcpy(lcd_draw_buf.screen_buf, lcd_get_framebuffer(), framebuffer_size()); // copy from the LCD frame buffer (for the first page)
		display_first_page = 1;
		lcd_draw_buf.pPcfFont = &pcfFonts[SEARCH_LIST_FONT_IDX - 1];
		lcd_draw_buf.line_height = RESULT_HEIGHT;
		lcd_draw_buf.current_x = 0;
		lcd_draw_buf.current_y = (RESULT_START - 2) + RESULT_HEIGHT * NUMBER_OF_FIRST_PAGE_RESULTS;
		lcd_draw_cur_y_pos = 0;
	}

	while (more_search_results && !rc)
	{ // loop until the title not in result list or no more
		if ((offset_next = result_list_next_result(offset_next, &idxArticle, sTitleActual)))
		{
			start_x = 0;
			end_x = LCD_BUF_WIDTH_PIXELS - 1;
			if (article_link_count < MAX_RESULT_LIST)
			{
				if (!is_title_in_result_list(idxArticle, sTitleActual))
				{ // if the title is not in the list, add it
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
			}
			else
				more_search_results = 0;

			if (stop_render_article == 1)
			{
				more_search_results = 0;
				stop_render_article = 0;
			}

			if(request_display_next_page > 0 && lcd_draw_buf.current_y >= request_y_pos+LCD_HEIGHT)
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
				long y_pos = lcd_draw_buf.current_y - LCD_HEIGHT;
				if (y_pos < 0)
					y_pos = 0;
				lcd_draw_cur_y_pos = y_pos;
				repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 1);
				request_display_next_page = 0;
			}
		}
	}

	guilib_fb_unlock();
	return rc;
}

void display_article_with_pcf(int y_move)
{
	if(lcd_draw_buf.current_y<=LCD_HEIGHT + article_start_y_pos || request_display_next_page ||
	   (display_mode == DISPLAY_MODE_INDEX && article_link_count <= NUMBER_OF_FIRST_PAGE_RESULTS))
		return;

	if(article_buf_pointer && (lcd_draw_cur_y_pos+y_move+LCD_HEIGHT) > lcd_draw_buf.current_y)
	{
		request_display_next_page = 1;
		request_y_pos = lcd_draw_cur_y_pos + y_move + LCD_HEIGHT;

		display_str(get_nls_text("please_wait"));

		return;
	}
//	if ((lcd_draw_cur_y_pos == 0 && start_y < 0) ||
//	    ((lcd_draw_cur_y_pos+LCD_HEIGHT)>lcd_draw_buf.current_y && start_y >= 0))
//	{
//		return;
//	}

	lcd_draw_cur_y_pos += y_move;
	if (!bShowLanguageLinks && lcd_draw_cur_y_pos < article_start_y_pos)
		lcd_draw_cur_y_pos = article_start_y_pos;
	else if (bShowLanguageLinks && lcd_draw_cur_y_pos >= article_start_y_pos)
		bShowLanguageLinks = 0;
	if ((lcd_draw_cur_y_pos+LCD_HEIGHT)>lcd_draw_buf.current_y)
	{
		lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT;
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

	time_now = timer_get();
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
			else if (lcd_draw_cur_y_pos > lcd_draw_buf.current_y - LCD_HEIGHT)
			{
				lcd_draw_cur_y_pos = lcd_draw_buf.current_y - LCD_HEIGHT;
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

void draw_icon(const unsigned char *pStr)
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
				guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, j, lcd_draw_buf.current_y + i);
			}
		}
		else if (1 < i && i < LANGUAGE_LINK_HEIGHT - 2)
		{
			for (j = lcd_draw_buf.current_x + LCD_LEFT_MARGIN; j < lcd_draw_buf.current_x + LCD_LEFT_MARGIN + LANGUAGE_LINK_WIDTH - 1; j++)
			{
				guilib_buffer_set_pixel(lcd_draw_buf.screen_buf, j, lcd_draw_buf.current_y + i);
			}
		}
	}
	buf_render_string(lcd_draw_buf.screen_buf, LCD_BUF_WIDTH_PIXELS, LCD_BUF_WIDTH_BYTES,
			  SUBTITLE_FONT_IDX, lcd_draw_buf.current_x + LCD_LEFT_MARGIN +
			  (LANGUAGE_LINK_WIDTH - (end_x - start_x + 1)) / 2 - start_x,
			  lcd_draw_buf.current_y + (LANGUAGE_LINK_HEIGHT - (end_y - start_y + 1)) / 2 - start_y, pStr, ustrlen(pStr), 1);
	lcd_draw_buf.current_x += LANGUAGE_LINK_WIDTH + LANGUAGE_LINK_WIDTH_GAP;
}

void draw_restricted_mark()
{
	draw_icon(get_nls_text("r"));
}

void draw_external_link(const unsigned char *link_str)
{
	const unsigned char *pLang;

	pLang = get_lang_link_display_text(link_str);
	draw_icon(pLang);
}

int duplicate_wiki_lang(const unsigned char *link_str1, const unsigned char *link_str2)
{
	const unsigned char *p;
	int len;

	p = ustrchr(link_str1, ':');
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
	lcd_draw_buf.x_adjustment = 0;
	lcd_draw_buf.y_adjustment = 0;
}

void display_retrieved_article(long idx_article)
{
	int i;
	unsigned int offset;
	ARTICLE_HEADER article_header;
	unsigned char title[MAX_TITLE_ACTUAL];
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
			unsigned char *link_str;
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
			offset += ustrlen(file_buffer + offset) + 1;
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
	    LCD_HEIGHT - PREVIOUS_ARTICLE_LINKABLE_SIZE <= origin_y && origin_y < LCD_HEIGHT &&
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

	if (restricted_article && check_restriction())
		return;

	display_retrieved_article(idx_article);
}

void display_str(const unsigned char *str)
{
	int start_x,end_x,start_y,end_y;
	int offset_x,offset_y;
	int str_width;
	const unsigned char *p;

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
	buf_draw_UTF8_str_in_copy_buffer(framebuffer_copy,&str,start_x,end_x,start_y,end_y,offset_x,DEFAULT_FONT_IDX);

	repaint_framebuffer(framebuffer_copy,-1, 0);

}

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
			guilib_buffer_clear_pixel(buffer,m,i);

	for(i = start_x ; i < end_x ; i++)
		guilib_buffer_set_pixel(buffer,i, start_y);

	for(i = start_y ; i < end_y ; i++)
		guilib_buffer_set_pixel(buffer,start_x,i);

	for(i = start_x ; i < end_x ; i++)
		guilib_buffer_set_pixel(buffer,i,end_y);

	for(i = start_y ; i < end_y ; i++)
		guilib_buffer_set_pixel(buffer,end_x,i);
}

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

	if (start_y >= 0 || end_y < LCD_HEIGHT)
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
	uint32_t article_id;
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
	if (link_to_be_activated >= 0 && time_diff(timer_get(), link_to_be_activated_start_time) >=
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
	      time_diff(timer_get(), link_to_be_inverted_start_time) >= seconds_to_ticks(LINK_INVERT_ACTIVATION_TIME_THRESHOLD)) ||
	     time_diff(timer_get(), link_to_be_inverted_start_time) >= seconds_to_ticks(LIST_LINK_INVERT_ACTIVATION_TIME_THRESHOLD)))
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
		delay_us(100000);
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

int framebuffer_size()
{
	return guilib_framebuffer_size();
}
int framebuffer_width()
{
	return guilib_framebuffer_width();
}
int framebuffer_height()
{
	return guilib_framebuffer_height();
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
					lcd_set_pixel(x + j, y + i, LCD_BLACK);
				else
					lcd_set_pixel(x + j, y + i, LCD_WHITE);
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
						lcd_set_pixel(x_base + x_offset, y+y_offset, LCD_WHITE);
					else
						lcd_set_pixel(x_base + x_offset, y+y_offset, LCD_BLACK);
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

int buf_draw_bmf_char(unsigned char *buf, int buf_width_pixels, int buf_width_bytes,
		      ucs4_t u,int font,int x,int y, int inverted, int b_clear)
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
	unsigned int byte;
	unsigned int bit;

	pPcfFont = &pcfFonts[font];

	pres_bmfbm(u, pPcfFont, &bitmap, &Cmetrics);
	if (bitmap == NULL)
	{
		return -1;
	}

	bytes_to_process = Cmetrics.widthBytes * Cmetrics.height;
	x_base = x + Cmetrics.LSBearing;
	x_offset = 0;
	y_offset = pPcfFont->Fmetrics.linespace - (pPcfFont->Fmetrics.descent + Cmetrics.ascent);
	x_bit_idx = x_base & 0x07;

	if (b_clear)
	{
		for (i = 0; i <= pPcfFont->Fmetrics.linespace; i++)
			// need to clear 1 pixel more than linespace for subtitle font
		{
			for (j = 0; j < Cmetrics.widthDevice; j++)
			{
				byte = ((x + j) + buf_width_bytes * 8 * (y + i)) / 8;
				bit  = ((x + j) + buf_width_bytes * 8 * (y + i)) % 8;
				if (inverted)
					buf[byte] |= (1 << (7 - bit));
				else
					buf[byte] &= ~(1 << (7 - bit));
			}
		}
	}
	if(u==32)
	{
		x += Cmetrics.widthDevice;
		return x;
	}

	if (x + Cmetrics.widthDevice >= buf_width_pixels)
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
				byte = ((x_base+x_offset) + buf_width_bytes * 8 * (y+y_offset)) / 8;
				bit  = ((x_base+x_offset) + buf_width_bytes * 8 * (y+y_offset)) % 8;
				if (bitmap[i] & (1 << j))
				{
					if (inverted)
					{
						buf[byte] ^= (1 << (7 - bit));
					}
					else
					{
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

int GetFontLinespace(int font)
{
	return pcfFonts[font - 1].Fmetrics.linespace;
}

void msg_on_lcd(int x, int y, char *fmt, ...)
{
	va_list args;
	unsigned char msg[100];
	va_start(args, fmt);
	vsprintf ((char *)msg, fmt, args);
	guilib_clear_area(x, y, 239, y+18);
	render_string(DEFAULT_FONT_IDX, x, y, msg, ustrlen(msg), 0);
	va_end(args);
}

void msg_on_lcd_clear(int x, int y)
{
	guilib_clear_area(x, y, 239, y+18);
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

int find_start_line(int y)
{
	int iStart, iEnd;
	int iMiddle = -1;
	int iBestFit = -1;
	bool bFound = false;

	y += lcd_draw_cur_y_pos;
	if (nArticleRenderedLines)
	{
		iStart = 0;
		iEnd = nArticleRenderedLines - 1;

		while (!bFound && iStart <= iEnd)
		{
			iMiddle = (iStart + iEnd) / 2;
			if (y >= (int)pArticleRenderInfo[iMiddle].start_y)
			{
				if (iMiddle >= iEnd || y < (int)pArticleRenderInfo[iMiddle + 1].start_y)
					bFound = true;
				else
					iStart = iMiddle + 1;
			}
			else
				iEnd = iMiddle - 1;
		}

		if (bFound)
		{
			if (iMiddle > 0)
				iMiddle--;
			while (iMiddle <= iEnd)
			{
				if ((int)pArticleRenderInfo[iMiddle].start_y <= y && y <= (int)pArticleRenderInfo[iMiddle].end_y)
				{
					iBestFit = iMiddle;
					break;
				}

				if (iBestFit < 0 && (int)pArticleRenderInfo[iMiddle].start_y - HIGHTLIGHT_Y_DIFF_ALLOWANCE <= y &&
				    y <= (int)pArticleRenderInfo[iMiddle].end_y + HIGHTLIGHT_Y_DIFF_ALLOWANCE)
					iBestFit = iMiddle;
				iMiddle++;
			}
		}
	}
	return iBestFit;
}

int find_end_line(int y)
{
	int iStart, iEnd;
	int iMiddle = -1;
	int iBestFit = -1;
	bool bFound = false;

	y += lcd_draw_cur_y_pos;
	if (nArticleRenderedLines)
	{
		iStart = 0;
		iEnd = nArticleRenderedLines - 1;

		while (!bFound && iStart <= iEnd)
		{
			iMiddle = (iStart + iEnd) / 2;
			if (y >= (int)pArticleRenderInfo[iMiddle].start_y)
			{
				if (iMiddle >= iEnd || y < (int)pArticleRenderInfo[iMiddle + 1].start_y)
					bFound = true;
				else
					iStart = iMiddle + 1;
			}
			else
				iEnd = iMiddle - 1;
		}

		if (bFound)
		{
			if (iMiddle > 0)
				iMiddle--;
			while (iMiddle <= iEnd)
			{
				if ((int)pArticleRenderInfo[iMiddle].start_y <= y && (y <= (int)pArticleRenderInfo[iMiddle].end_y ||
										      (iMiddle < nArticleRenderedLines - 1 && y < (int)pArticleRenderInfo[iMiddle + 1].start_y)))
				{
					iBestFit = iMiddle;
					break;
				}

				if (iBestFit < 0 && (int)pArticleRenderInfo[iMiddle].start_y - HIGHTLIGHT_Y_DIFF_ALLOWANCE <= y &&
				    y <= (int)pArticleRenderInfo[iMiddle].end_y + HIGHTLIGHT_Y_DIFF_ALLOWANCE)
					iBestFit = iMiddle;
				iMiddle++;
			}
		}
	}
	return iBestFit;
}

bool process_esc_code(unsigned char c, const unsigned char **p, pcffont_bmf_t **pFont, int *last_x)
{
	unsigned char c2;
	int font_idx;
	bool bNewLine = false;
	int nWidth, nHeight;
	int x_adjustment = 0;
	int i;

	switch(c)
	{
	case ESC_0_SPACE_LINE: /* space line */
		(*p)++;
		bNewLine = true;
		break;
	case ESC_1_NEW_LINE_DEFAULT_FONT: /* new line with default font and line space */
		bNewLine = true;
		break;
	case ESC_2_NEW_LINE_SAME_FONT: /* new line with previous font and line space */
		bNewLine = true;
		break;
	case ESC_3_NEW_LINE_WITH_FONT: /* new line with specified font and line space */
		c2 = **p;
		(*p)++;
		font_idx = c2 & 0x07;
		if (font_idx > FONT_COUNT)
			font_idx = DEFAULT_FONT_IDX;
		*pFont = &pcfFonts[font_idx - 1];
		bNewLine = true;
		break;
	case ESC_4_CHANGE_FONT: /* change font */
		c2 = **p;
		(*p)++;
		font_idx = c2 & 0x07;
		if (font_idx > FONT_COUNT)
			font_idx = DEFAULT_FONT_IDX;
		*pFont = &pcfFonts[font_idx - 1];
		break;
	case ESC_5_RESET_TO_DEFAULT_FONT: /* reset to the default font */
		*pFont = &pcfFonts[DEFAULT_FONT_IDX - 1];
		break;
	case ESC_6_RESET_TO_DEFAULT_ALIGN: /* reset to the default vertical alignment */
		x_adjustment = 0;
		break;
	case ESC_7_FORWARD: /* forward */
		c2 = **p;
		(*p)++;
		*last_x += c2;
		break;
	case ESC_8_BACKWARD: /* backward */
		c2 = **p;
		(*p)++;
		if (*last_x < c2)
			*last_x = 0;
		else
			*last_x -= c2;
		break;
	case ESC_9_Y_ADJUSTMENT: /* vertical alignment adjustment */
		c2 = **p;
		(*p)++;
		x_adjustment += c2;
		break;
	case ESC_10_HORIZONTAL_LINE: /* drawing horizontal line */
		(*p)++;
		break;
	case ESC_11_VERTICAL_LINE: /* drawing vertical line */
		(*p)++;
		break;
	case ESC_12_FULL_HORIZONTAL_LINE: /* drawing horizontal line from left-most pixel to right-most pixel */
		break;
	case ESC_13_FULL_VERTICAL_LINE: /* drawing vertical line from top of the line to the bottom */
		*last_x += 1;
		*last_x += 2;
		break;
	case ESC_14_BITMAP: /* bitmap */
		c2 = **p;
		(*p)++;
		nWidth = c2;
		c2 = **p;
		(*p)++;
		nHeight = c2;
		c2 = **p;
		(*p)++;
		nHeight |= c2 << 8;
		if (*last_x == 0)
			*last_x = LCD_EXTRA_LEFT_MARGIN_FOR_IMAGE;
		else
			++last_x;
		for (i = 0; i < nHeight; i++)
			*p += (nWidth + 7) / 8;
		*last_x += nWidth;
		break;
	default:
		break;
	}
	return bNewLine;
}

int find_start_pos(const unsigned char *pBuf, pcffont_bmf_t *pFont, int start_x, int *bytes_before)
{
	int last_word_break_x = LCD_LEFT_MARGIN;
	const unsigned char *p_buf_last_word_break = pBuf;
	int last_x = LCD_LEFT_MARGIN;
	const unsigned char *p = pBuf;
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	unsigned char c;
	bool bNewLine = false;
	ucs4_t u;

	while (!bNewLine && *p && last_x < start_x)
	{
		c = *p;
		if (c <= MAX_ESC_CHAR)
		{
			p++;
			bNewLine = process_esc_code(c, &p, &pFont, &last_x);
		}
		else
		{
			int temp_last_word_break_x = last_x;
			const unsigned char *p_temp_buf_last_word_break = p;
			if ((u = UTF8_to_UCS4(&p)))
			{
				if (pres_bmfbm(u, pFont, &bitmap, &Cmetrics) >= 0)
					last_x += Cmetrics.widthDevice;
				if (is_word_break(u))
				{
					if (last_x < start_x)
					{
						last_word_break_x = last_x;
						p_buf_last_word_break = p;
					}
					else
					{
						last_word_break_x = temp_last_word_break_x;
						p_buf_last_word_break = p_temp_buf_last_word_break;
					}
				}
			}
		}
	}
	if (bytes_before)
		*bytes_before = p_buf_last_word_break - pBuf;
	return last_word_break_x;
}

int find_end_pos(const unsigned char *pBuf, pcffont_bmf_t *pFont, int end_x, int *used_bytes)
{
	int last_x = LCD_LEFT_MARGIN;
	const unsigned char *p = pBuf;
	bmf_bm_t *bitmap;
	charmetric_bmf Cmetrics;
	unsigned char c;
	bool bNewLine = false;
	ucs4_t u;

	while (!bNewLine && *p && last_x < end_x)
	{
		c = *p;
		if (c <= MAX_ESC_CHAR)
		{
			p++;
			bNewLine = process_esc_code(c, &p, &pFont, &last_x);
		}
		else
		{
			if ((u = UTF8_to_UCS4(&p)))
			{
				if (pres_bmfbm(u, pFont, &bitmap, &Cmetrics) >= 0)
					last_x += Cmetrics.widthDevice;
			}
		}
	}
	if (used_bytes)
		*used_bytes = p - pBuf;
	return last_x;
}

void concat_search_string(const unsigned char *pBuf, int nBytes, unsigned char *search_string_actual)
{
	int last_x = LCD_LEFT_MARGIN;
	const unsigned char *p = pBuf;
	pcffont_bmf_t *pFont;
	unsigned char c;
	bool bNewLine = false;
	bool bFirstChar = true;
	ucs4_t u;
	int len_search_string_actual = ustrlen(search_string_actual);

	if (nBytes < 0)
		nBytes = MAX_TITLE_ACTUAL;

	while (!bNewLine && *p && nBytes > 0)
	{
		c = *p;
		if (c <= MAX_ESC_CHAR)
		{
			const unsigned char *pOld = p;
			p++;
			bNewLine = process_esc_code(c, &p, &pFont, &last_x);
			nBytes -= p - pOld;
		}
		else
		{
			const unsigned char *pOld = p;
			int len;
			if ((u = UTF8_to_UCS4(&p)))
			{
				len = p - pOld;
				if (bFirstChar)
				{
					bFirstChar = false;
					if (!is_word_break(u) && len_search_string_actual > 0 && len_search_string_actual + 1 < MAX_TITLE_ACTUAL)
						search_string_actual[len_search_string_actual++] = ' ';
				}
				if (len + len_search_string_actual >= MAX_TITLE_ACTUAL)
					break;
				memcpy(&search_string_actual[len_search_string_actual], pOld, len);
				//memset(&search_string_actual[len_search_string_actual], 'x', len);
				len_search_string_actual += len;
				nBytes -= len;
			}
		}
	}
	search_string_actual[len_search_string_actual] = '\0';
}

void draw_highlight_area(int start_line, int end_line, int start_x, int end_x,
			 int *invert_start_x, int *invert_end_x,
			 int *invert_start_y_top, int *invert_start_y_bottom, int *invert_end_y_top, int *invert_end_y_bottom,
			 unsigned char *search_string_actual, bool bRepaint)
{
	static int iLineStart = -1;
	static int iLineEnd = -1;
	static int iXStart = -1;
	static int iXEnd = -1;
	int i;
	int xs, ys, xe, ye;
	int byets_before_start, bytes_used_to_end;
	int start_byte, nBytes;

	if (iLineStart >= 0 && bRepaint)
	{ // invert the original highlight
		for (i = iLineStart; i <= iLineEnd; i++)
		{
			if (i == iLineStart)
				xs = iXStart;
			else
				xs = 0;
			ys = pArticleRenderInfo[i].start_y;
			if (i == iLineEnd)
				xe = iXEnd;
			else
				xe = LCD_WIDTH -1;
			if (i < nArticleRenderedLines - 1)
				ye = pArticleRenderInfo[i + 1].start_y - 1;
			else
				ye = pArticleRenderInfo[i].end_y;
			guilib_buffer_invert_area(lcd_draw_buf.screen_buf, xs, ys, xe, ye);
		}
	}

	if (!start_line && !end_line && !start_x && !end_x)
	{
		iLineStart = -1;
	}
	else if (start_line >= 0 && end_line >= 0)
	{
		if (start_line <= end_line)
		{
			iLineStart = start_line;
			iLineEnd = end_line;
		}
		else
		{
			iLineStart = end_line;
			iLineEnd = start_line;
		}
		if (start_line < end_line || (start_line == end_line && start_x <= end_x))
		{
			iXStart = find_start_pos(pArticleRenderInfo[iLineStart].pBuf, pArticleRenderInfo[iLineStart].pPcfFont,
						 start_x, &byets_before_start);
			iXEnd = find_end_pos(pArticleRenderInfo[iLineEnd].pBuf, pArticleRenderInfo[iLineEnd].pPcfFont,
					     end_x, &bytes_used_to_end);
		}
		else
		{
			iXStart = find_start_pos(pArticleRenderInfo[iLineStart].pBuf, pArticleRenderInfo[iLineStart].pPcfFont,
						 end_x, &byets_before_start);
			iXEnd = find_end_pos(pArticleRenderInfo[iLineEnd].pBuf, pArticleRenderInfo[iLineEnd].pPcfFont,
					     start_x, &bytes_used_to_end);
		}

		for (i = iLineStart; i <= iLineEnd; i++)
		{
			if (i == iLineStart)
				xs = iXStart;
			else
				xs = 0;
			ys = pArticleRenderInfo[i].start_y;
			if (i == iLineEnd)
				xe = iXEnd;
			else
				xe = LCD_WIDTH -1;
			if (i < nArticleRenderedLines - 1)
				ye = pArticleRenderInfo[i + 1].start_y - 1;
			else
				ye = pArticleRenderInfo[i].end_y;
			guilib_buffer_invert_area(lcd_draw_buf.screen_buf, xs, ys, xe, ye);

			if (i == iLineStart)
			{
				if (invert_start_x)
					*invert_start_x = xs;
				if (invert_start_y_top)
					*invert_start_y_top = ys - lcd_draw_cur_y_pos;
				if (invert_start_y_bottom)
					*invert_start_y_bottom = ye - lcd_draw_cur_y_pos;
			}

			if (i == iLineEnd)
			{
				if (invert_end_x)
					*invert_end_x = xe;
				if (invert_end_y_top)
					*invert_end_y_top = ys - lcd_draw_cur_y_pos;
				if (invert_end_y_bottom)
					*invert_end_y_bottom = ye - lcd_draw_cur_y_pos;
			}

			if (search_string_actual)
			{
				if (i == iLineStart)
					start_byte = byets_before_start;
				else
					start_byte = 0;
				if (i == iLineEnd)
					nBytes = bytes_used_to_end - start_byte;
				else
					nBytes = -1;
				concat_search_string(&pArticleRenderInfo[i].pBuf[start_byte], nBytes, search_string_actual);
			}
		}
	}
	else
	{ // reset highlight
		iLineStart = -1;
	}
	if (bRepaint)
		repaint_framebuffer(lcd_draw_buf.screen_buf, lcd_draw_cur_y_pos, 0);
}

bool lcd_draw_highlight(int start_x, int start_y, int end_x, int end_y,
			int *invert_start_x, int *invert_end_x,
			int *invert_start_y_top, int *invert_start_y_bottom, int *invert_end_y_top, int *invert_end_y_bottom,
			unsigned char *search_string_actual, bool bRepaint)
{
	int iLineStart, iLineEnd;

	if (search_string_actual)
		search_string_actual[0] = '\0';

	if (!start_x && !start_y && !end_x && !end_y)
	{
		iLineStart = 0;
		iLineEnd = 0;
	}
	else
	{
		iLineStart = find_start_line(start_y);
		if (iLineStart >= 0)
			iLineEnd = find_end_line(end_y);
		else
			iLineEnd = -1;
	}

	if (iLineStart >= 0 && iLineEnd >= 0)
	{
		draw_highlight_area(iLineStart, iLineEnd, start_x, end_x,
				    invert_start_x, invert_end_x,
				    invert_start_y_top, invert_start_y_bottom, invert_end_y_top, invert_end_y_bottom,
				    search_string_actual, bRepaint);
		return true;
	}
	else
		return false;
}

unsigned char *lcd_draw_get_cur_buffer()
{
	return &lcd_draw_buf.screen_buf[lcd_draw_cur_y_pos * LCD_BUF_WIDTH_BYTES];
}

int lcd_draw_get_cur_y_pos()
{
	return lcd_draw_cur_y_pos;
}
