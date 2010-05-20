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
#include <ctype.h>
#include <string.h>
#include <file-io.h>
#include <guilib.h>
#include <lcd.h>
#include <input.h>
#include "glyph.h"
#include "sha1.h"
#include "search.h"
#include "wikilib.h"
#include "lcd_buf_draw.h"
#include "wl-keyboard.h"
#include "restricted.h"
#include "bigram.h"
#include "history.h"
#include "delay.h"
#include "wiki_info.h"

#define BLACK_SPACE_START RESULT_START

enum restricted_screen_e {

	RESTRICTED_SCREEN_FIRST_TIME_PASSWORD,
	RESTRICTED_SCREEN_SET_PASSWORD,
	RESTRICTED_SCREEN_CONFIRM_PASSWORD,
	RESTRICTED_SCREEN_CHECK_PASSWORD,
	RESTRICTED_SCREEN_FILTER_ON_OFF,
	RESTRICTED_SCREEN_FILTER_OPTION,
	RESTRICTED_SCREEN_CHANGE_PASSWORD,
};

enum filter_option_e {

	FILTER_OPTION_TO_SET_NONE,
	FILTER_OPTION_TO_SET_ON,
	FILTER_OPTION_TO_SET_OFF,
};

extern int restricted_article;
extern int display_mode;
extern int last_display_mode;
extern int article_offset;
static char password_string[MAX_PASSWORD_LEN];
static char password1[MAX_PASSWORD_LEN];
static int password_str_len = 0;
static int restricted_screen_mode;
static int filter_option_to_set = FILTER_OPTION_TO_SET_NONE;
int restriction_filter_off = -1;
char restriction_pass1[20];
extern long saved_idx_article;
int init_filtering = 0;
void first_time_password(int flag)
{
	int i;
	unsigned char *pText;
	char str[256];
	int width;

	guilib_fb_lock();
	memset(&framebuffer[(BLACK_SPACE_START - 6)* LCD_VRAM_WIDTH_PIXELS / 8], 0, 6 * LCD_VRAM_WIDTH_PIXELS / 8);
	memset(&framebuffer[BLACK_SPACE_START * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF, (LCD_HEIGHT_LINES - BLACK_SPACE_START)* LCD_VRAM_WIDTH_PIXELS / 8);

	framebuffer[149 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[149 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[149 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;
	for (i = 150; i <= 168; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x1F;
	}
	framebuffer[169 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[169 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[169 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;

	if (flag)
	{
		pText = get_nls_text("protection");
		width = extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 10, str, strlen(str), 1);
		width = extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 30, str, strlen(str), 1);
		extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 50, str, strlen(str), 1);
		extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 70, str, strlen(str), 1);
	}
	else
	{
		pText = get_nls_text("restricted");
		extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 10, str, strlen(str), 1);
		extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 30, str, strlen(str), 1);
		extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 50, str, strlen(str), 1);
		extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
		if (strlen(str))
			render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 70, str, strlen(str), 1);
	}
	pText = get_nls_text("set_password");
	render_string(SUBTITLE_FONT_IDX, -1, 151, pText, strlen(pText), 0);
	guilib_fb_unlock();
	display_mode = DISPLAY_MODE_RESTRICTED;
	keyboard_set_mode(KEYBOARD_RESTRICTED);
	restricted_screen_mode = RESTRICTED_SCREEN_FIRST_TIME_PASSWORD;
}

void enter_password_screen(char *msg)
{
	int i;
	unsigned char *pText;

	display_mode = DISPLAY_MODE_RESTRICTED;
	keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
	guilib_fb_lock();
	keyboard_paint();
	memset(&framebuffer[(BLACK_SPACE_START - 6)* LCD_VRAM_WIDTH_PIXELS / 8], 0, 6 * LCD_VRAM_WIDTH_PIXELS / 8);
	memset(&framebuffer[BLACK_SPACE_START * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF,
	       (LCD_HEIGHT_LINES - BLACK_SPACE_START - keyboard_height())* LCD_VRAM_WIDTH_PIXELS / 8);
	render_string(SUBTITLE_FONT_IDX, -1, 50, msg, strlen(msg), 1);

	framebuffer[82 * LCD_VRAM_WIDTH_PIXELS / 8 + 4] = 0xFC;
	memset(&framebuffer[82 * LCD_VRAM_WIDTH_PIXELS / 8 + 5], 0, 16);
	framebuffer[82 * LCD_VRAM_WIDTH_PIXELS / 8 + 21] = 0x07;

	framebuffer[82 * LCD_VRAM_WIDTH_PIXELS / 8 + 22] = 0x80;
	memset(&framebuffer[82 * LCD_VRAM_WIDTH_PIXELS / 8 + 23], 0, 2);
	framebuffer[82 * LCD_VRAM_WIDTH_PIXELS / 8 + 25] = 0x1F;

	for (i = 83; i <= 103; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 4] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 5], 0, 16);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 21] = 0x03;

		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 22] = 0x00;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 23], 0, 2);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 25] = 0x0F;
	}

	framebuffer[104 * LCD_VRAM_WIDTH_PIXELS / 8 + 4] = 0xFC;
	memset(&framebuffer[104 * LCD_VRAM_WIDTH_PIXELS / 8 + 5], 0, 16);
	framebuffer[104 * LCD_VRAM_WIDTH_PIXELS / 8 + 21] = 0x07;

	framebuffer[104 * LCD_VRAM_WIDTH_PIXELS / 8 + 22] = 0x80;
	memset(&framebuffer[104 * LCD_VRAM_WIDTH_PIXELS / 8 + 23], 0, 2);
	framebuffer[104 * LCD_VRAM_WIDTH_PIXELS / 8 + 25] = 0x1F;

	pText = get_nls_text("ok");
	render_string(SUBTITLE_FONT_IDX, 180, 85, pText, strlen(pText), 0);
	guilib_fb_unlock();
}

void filter_on_off(void)
{
	int i;
	unsigned char *pText;
	char str[256];

	guilib_fb_lock();
	memset(&framebuffer[(BLACK_SPACE_START - 6)* LCD_VRAM_WIDTH_PIXELS / 8], 0, 6 * LCD_VRAM_WIDTH_PIXELS / 8);
	memset(&framebuffer[BLACK_SPACE_START * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF, (LCD_HEIGHT_LINES - BLACK_SPACE_START)* LCD_VRAM_WIDTH_PIXELS / 8);

	framebuffer[135 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[135 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[135 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;
	for (i = 136; i <= 155; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x1F;
	}
	framebuffer[156 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[156 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[156 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;

	framebuffer[164 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[164 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[164 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;
	for (i = 165; i <= 184; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x1F;
	}
	framebuffer[185 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[185 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[185 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;

	pText = get_nls_text("set_filter");
	extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
	if (strlen(str))
		render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 10, str, strlen(str), 1);
	extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
	if (strlen(str))
		render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 30, str, strlen(str), 1);
	extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
	if (strlen(str))
		render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 50, str, strlen(str), 1);
	extract_str_fitting_width(&pText, str, LCD_BUF_WIDTH_PIXELS - LCD_LEFT_MARGIN, SUBTITLE_FONT_IDX);
	if (strlen(str))
		render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, BLACK_SPACE_START + 70, str, strlen(str), 1);
	pText = get_nls_text("keep_filter_on");
	render_string(SUBTITLE_FONT_IDX, -1, 137, pText, strlen(pText), 0);
	pText = get_nls_text("keep_filter_off");
	render_string(SUBTITLE_FONT_IDX, -1, 166, pText, strlen(pText), 0);
	guilib_fb_unlock();
	keyboard_set_mode(KEYBOARD_FILTER_ON_OFF);
}

void filter_option(void)
{
	int i;
	unsigned char *pText;

	display_mode = DISPLAY_MODE_RESTRICTED;
	keyboard_set_mode(KEYBOARD_FILTER_OPTION);
	restricted_screen_mode = RESTRICTED_SCREEN_FILTER_OPTION;
	guilib_fb_lock();
	memset(&framebuffer[(BLACK_SPACE_START - 6)* LCD_VRAM_WIDTH_PIXELS / 8], 0, 6 * LCD_VRAM_WIDTH_PIXELS / 8);
	memset(&framebuffer[BLACK_SPACE_START * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF, (LCD_HEIGHT_LINES - BLACK_SPACE_START)* LCD_VRAM_WIDTH_PIXELS / 8);

	pText = get_nls_text("filter_options");
	render_string(TITLE_FONT_IDX, -1, 60, pText, strlen(pText), 1);

	framebuffer[106 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[106 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[106 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;
	for (i = 107; i <= 126; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x1F;
	}
	framebuffer[127 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[127 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[127 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;

	framebuffer[135 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[135 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[135 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;
	for (i = 136; i <= 155; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x1F;
	}
	framebuffer[156 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[156 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[156 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;

	framebuffer[164 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[164 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[164 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;
	for (i = 165; i <= 185; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xF8;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x1F;
	}
	framebuffer[186 * LCD_VRAM_WIDTH_PIXELS / 8 + 1] = 0xFC;
	memset(&framebuffer[186 * LCD_VRAM_WIDTH_PIXELS / 8 + 2], 0, 26);
	framebuffer[186 * LCD_VRAM_WIDTH_PIXELS / 8 + 28] = 0x3F;

	pText = get_nls_text("turn_filter_on");
	render_string(SUBTITLE_FONT_IDX, -1, 108, pText, strlen(pText), 0);
	pText = get_nls_text("turn_filter_off");
	render_string(SUBTITLE_FONT_IDX, -1, 137, pText, strlen(pText), 0);
	pText = get_nls_text("change_password");
	render_string(SUBTITLE_FONT_IDX, -1, 166, pText, strlen(pText), 0);
	guilib_fb_unlock();
	keyboard_set_mode(KEYBOARD_FILTER_OPTION);
}

void save_password(int flag)
{
	int fd;
	SHA1Context sha;

	fd = wl_open("wiki.pas", WL_O_CREATE);
	if (fd >= 0)
	{
		if (password_str_len > 0)
		{
			SHA1Reset(&sha);
			SHA1Input(&sha, (const unsigned char *) password_string, password_str_len);
			SHA1Result(&sha);
			memcpy(restriction_pass1, sha.Message_Digest, 20);
		}
		wl_write(fd, restriction_pass1, 20);
		if (flag > 1)
		{
			SHA1Reset(&sha);
			SHA1Input(&sha, (const unsigned char *) restriction_pass1, 20);
			SHA1Result(&sha);
			wl_write(fd, sha.Message_Digest, 20);
		}

		wl_close(fd);
#ifdef INCLUDED_FROM_KERNEL
		delay_us(200000); // for some reason, save may not work if no delay
#endif
	}
}

void handle_password_key(char keycode)
{
	switch (restricted_screen_mode)
	{
	case RESTRICTED_SCREEN_FIRST_TIME_PASSWORD:
	case RESTRICTED_SCREEN_SET_PASSWORD:
	case RESTRICTED_SCREEN_CONFIRM_PASSWORD:
	case RESTRICTED_SCREEN_CHECK_PASSWORD:
	case RESTRICTED_SCREEN_CHANGE_PASSWORD:
		if (keycode == 'Y') {
			if (restricted_screen_mode == RESTRICTED_SCREEN_FIRST_TIME_PASSWORD)
			{
				password_str_len = 0;
				keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
				enter_password_screen(get_nls_text("set_password"));
				restricted_screen_mode = RESTRICTED_SCREEN_SET_PASSWORD;
			}
			else if (restricted_screen_mode == RESTRICTED_SCREEN_SET_PASSWORD && password_str_len > 0)
			{
				restricted_screen_mode = RESTRICTED_SCREEN_CONFIRM_PASSWORD;
				strcpy(password1, password_string);
				password_str_len = 0;
				if (restriction_filter_off == -1)
					enter_password_screen(get_nls_text("re_enter_password"));
				else
					enter_password_screen(get_nls_text("re_enter_new_password"));
			}
			else if (restricted_screen_mode == RESTRICTED_SCREEN_CONFIRM_PASSWORD && password_str_len > 0)
			{
				if (strcmp(password1, password_string))
				{
					password_str_len = 0;
					restricted_screen_mode = RESTRICTED_SCREEN_SET_PASSWORD;
					enter_password_screen(get_nls_text("passwords_not_match"));
				}
				else
				{
					restriction_filter_off = 0;
					save_password(1);
					restricted_screen_mode = RESTRICTED_SCREEN_FILTER_ON_OFF;
					filter_on_off();
				}
			}
			else if (password_str_len > 0)
			{
				check_password();
			}
		} else if (keycode == WL_KEY_BACKSPACE) {
			password_remove_char();
		} else if (is_supported_search_char(keycode)) {
			password_add_char(tolower(keycode));
		}
		break;
	case RESTRICTED_SCREEN_FILTER_ON_OFF:
		if (keycode == 'Y') {
			restriction_filter_off = 0;
			if (init_filtering)
			{
				init_filtering = 0;
				search_set_selection(-1);
				display_mode = DISPLAY_MODE_INDEX;
				keyboard_set_mode(wiki_default_keyboard());
				repaint_search();
			}
			else
			{
				display_mode = DISPLAY_MODE_ARTICLE;
				display_retrieved_article(saved_idx_article);
			}
		} else if (keycode == 'N') {
			restriction_filter_off = 1;
			save_password(2);
			if (init_filtering)
			{
				init_filtering = 0;
				search_set_selection(-1);
				display_mode = DISPLAY_MODE_INDEX;
				keyboard_set_mode(wiki_default_keyboard());
				repaint_search();
			}
			else
			{
				display_mode = DISPLAY_MODE_ARTICLE;
				display_retrieved_article(saved_idx_article);
			}
		}
		break;
	case RESTRICTED_SCREEN_FILTER_OPTION:
		if (keycode == 'Y') {
			filter_option_to_set = FILTER_OPTION_TO_SET_ON;
			password_str_len = 0;
			check_password();
		}
		else if (keycode == 'N') {
			filter_option_to_set = FILTER_OPTION_TO_SET_OFF;
			password_str_len = 0;
			check_password();
		}
		if (keycode == 'P') {
			password_str_len = 0;
			keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
			enter_password_screen(get_nls_text("enter_old_password"));
			restricted_screen_mode = RESTRICTED_SCREEN_CHANGE_PASSWORD;
		}
		break;
	default:
		break;
	}
}

void check_password()
{
	SHA1Context sha;
	char pass_sha1[20];


	if (password_str_len <= 0)
	{
		if (restricted_screen_mode != RESTRICTED_SCREEN_FILTER_OPTION)
			filter_option_to_set = FILTER_OPTION_TO_SET_NONE;
		restricted_screen_mode = RESTRICTED_SCREEN_CHECK_PASSWORD;
		enter_password_screen(get_nls_text("enter_password"));
	}
	else
	{
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) password_string, password_str_len);
		SHA1Result(&sha);
		memcpy(pass_sha1, sha.Message_Digest, 20);
		if (memcmp(pass_sha1, restriction_pass1, 20))
		{
			password_str_len = 0;
			if (restricted_screen_mode != RESTRICTED_SCREEN_CHANGE_PASSWORD)
				restricted_screen_mode = RESTRICTED_SCREEN_CHECK_PASSWORD;
			enter_password_screen(get_nls_text("try_again"));
		}
		else
		{
			if (restricted_screen_mode == RESTRICTED_SCREEN_CHANGE_PASSWORD)
			{
				password_str_len = 0;
				keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
				enter_password_screen(get_nls_text("enter_new_password"));
				restricted_screen_mode = RESTRICTED_SCREEN_SET_PASSWORD;
			}
			else if (filter_option_to_set == FILTER_OPTION_TO_SET_ON)
			{
				restriction_filter_off = 0;
				save_password(1);
				last_display_mode = DISPLAY_MODE_ARTICLE;
				display_mode = DISPLAY_MODE_ARTICLE;
				display_retrieved_article(saved_idx_article);
			}
			else if (filter_option_to_set == FILTER_OPTION_TO_SET_OFF)
			{
				restriction_filter_off = 1;
				save_password(2);
				last_display_mode = DISPLAY_MODE_ARTICLE;
				display_mode = DISPLAY_MODE_ARTICLE;
				display_retrieved_article(saved_idx_article);
			}
			else
			{
				display_mode = DISPLAY_MODE_ARTICLE;
				display_retrieved_article(saved_idx_article);
			}
		}
	}
}

int init_article_filter(void)
{
	int fd;
	int len;
	char restriction_pass2[20];
	SHA1Context sha;
	unsigned char *pText;

	if (restriction_filter_off == -1)
	{
		fd = wl_open("wiki.pas", WL_O_RDONLY);
		if (fd >= 0)
		{
			len = wl_read(fd, restriction_pass1, 20);
			if (len < 20)
				memset(restriction_pass1, 0, 20);
			else
			{
				len = wl_read(fd, restriction_pass2, 20);
				if (len < 20)
					memset(restriction_pass2, 0, 20);

				SHA1Reset(&sha);
				SHA1Input(&sha, (const unsigned char *) restriction_pass1, 20);
				SHA1Result(&sha);
				if (!memcmp(sha.Message_Digest, restriction_pass2, 20))
					restriction_filter_off = 1;
				else
					restriction_filter_off = 0;
			}
			wl_close(fd);
		}
	}

	if (restriction_filter_off == -1)
	{
		init_filtering = 1;
		memset(&framebuffer[0], 0, (BLACK_SPACE_START - 1)* LCD_VRAM_WIDTH_PIXELS / 8);
		pText = get_nls_text("parental_controls");
		render_string(TITLE_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, pText, strlen(pText), 0);
		first_time_password(1);
		return -1;
	}
	else
		return 0;
}

int check_restriction(long idx_article)
{
	int fd;
	int len;
	char restriction_pass2[20];
	SHA1Context sha;
	char title[MAX_TITLE_ACTUAL];

	init_filtering = 0;
	if (restriction_filter_off == -1)
	{
		fd = wl_open("wiki.pas", WL_O_RDONLY);
		if (fd >= 0)
		{
			len = wl_read(fd, restriction_pass1, 20);
			if (len < 20)
				memset(restriction_pass1, 0, 20);
			else
			{
				len = wl_read(fd, restriction_pass2, 20);
				if (len < 20)
					memset(restriction_pass2, 0, 20);

				SHA1Reset(&sha);
				SHA1Input(&sha, (const unsigned char *) restriction_pass1, 20);
				SHA1Result(&sha);
				if (!memcmp(sha.Message_Digest, restriction_pass2, 20))
					restriction_filter_off = 1;
				else
					restriction_filter_off = 0;
			}
			wl_close(fd);
		}
	}

	if (restriction_filter_off == -1)
	{
		get_article_title_from_idx(saved_idx_article, title);
		memset(&framebuffer[0], 0, (BLACK_SPACE_START - 1)* LCD_VRAM_WIDTH_PIXELS / 8);
		render_string(TITLE_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, title, strlen(title), 0);
		first_time_password(0);
		return -1;
	} else if (restriction_filter_off)
		return 0; // ok
	else
	{
		get_article_title_from_idx(saved_idx_article, title);
		memset(&framebuffer[0], 0, (BLACK_SPACE_START - 1)* LCD_VRAM_WIDTH_PIXELS / 8);
		render_string(TITLE_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, title, strlen(title), 0);
		password_str_len = 0;
		check_password();
		return -1;
	}
}

int password_add_char(char c)
{
	if(c == 0x20 && password_str_len > 0 && password_string[password_str_len-1] == 0x20)
		return -1;
	if (password_str_len >= MAX_PASSWORD_LEN - 1) // reserve one byte for '\0'
		return -1;

	if (!password_str_len && c == 0x20)
		return -1;

	keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
	if ('A' <= c && c <= 'Z')
		c += 32;
	password_string[password_str_len++] = c;
	password_string[password_str_len] = '\0';
	render_string(SUBTITLE_FONT_IDX, 38, 85, password_string, strlen(password_string), 0);
	return 0;
}

/*
 * return value - 0: remove ok, -1: no key to remove
 */
int password_remove_char(void)
{
	int x;

	if (password_str_len == 0)
		return -1;

	password_string[--password_str_len] = '\0';
	x = render_string(SUBTITLE_FONT_IDX, 38, 85, password_string, strlen(password_string), 0);
	guilib_clear_area(x, 85, 172, 103);
	return 0;
}

int clear_password_string(void)
{
	if (password_str_len == 0)
		return -1;
	password_str_len = 0;
	password_string[0] = '\0';
	return 0;
}

int get_password_string_len(void)
{
	return password_str_len;
}
