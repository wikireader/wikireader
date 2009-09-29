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

enum restricted_screen_e {

	RESTRICTED_SCREEN_SET_PASSWORD,
	RESTRICTED_SCREEN_CONFIRM_PASSWORD,
	RESTRICTED_SCREEN_CHECK_PASSWORD,
	RESTRICTED_SCREEN_FILTER_OPTION,
};

extern int display_mode;
static char password_string[MAX_PASSWORD_LEN];
static int password_str_len = 0;
static int restricted_screen;
int restriction_filter_off = -1;
char restriction_pass1[20];
long saved_idx_article;
void set_password()
{
 	char title[MAX_TITLE_SEARCH];
 	int i;
 	
	guilib_fb_lock();
	get_article_title_from_idx(saved_idx_article, title);
	render_string(TITLE_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, title, strlen(title), 0);
	memset(&framebuffer[RESULT_START * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF, (LCD_HEIGHT_LINES - RESULT_START)* LCD_VRAM_WIDTH_PIXELS / 8);
	framebuffer[157 * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[157 * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[157 * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;
	for (i = 158; i <= 176; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF0;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x03;
	}
	framebuffer[177 * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[177 * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[177 * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;

	framebuffer[185 * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[185 * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[185 * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;
	for (i = 186; i <= 203; i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF0;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x03;
	}
	framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[204 * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, RESULT_START + 10, "This article is restricted due to", 33, 1);
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, RESULT_START + 30, "its adult content.  Please set a", 32, 1);
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, RESULT_START + 50, "password in odrder to view it.", 30, 1);
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, 159, "Set Password", 12, 0);
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, 187, "Cancel", 6, 0);
	guilib_fb_unlock();
	display_mode = DISPLAY_MODE_RESTRICTED;
	keyboard_set_mode(KEYBOARD_RESTRICTED);
	restricted_screen = RESTRICTED_SCREEN_SET_PASSWORD;
}

void handle_password_key(char keycode)
{
	int mode = keyboard_get_mode();

	switch (mode)
	{
		case KEYBOARD_PASSWORD_CHAR:
		case KEYBOARD_PASSWORD_NUM:
			if (keycode == WL_KEY_BACKSPACE) {
				password_remove_char();
			} else if (is_supported_search_char(keycode)) {
				password_add_char(tolower(keycode));
			}
			break;
		case KEYBOARD_RESTRICTED:
			if (keycode == 'Y')
			{
				keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
				keyboard_paint();
			}
			else if (keycode == 'N')
			{
			}
	}
}

void get_password(void)
{
	int i;
	
	display_mode = DISPLAY_MODE_RESTRICTED;
	keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
	guilib_fb_lock();
	keyboard_paint();
	memset(&framebuffer[RESULT_START * LCD_VRAM_WIDTH_PIXELS / 8], 0xFF, 
		(LCD_HEIGHT_LINES - RESULT_START - keyboard_height())* LCD_VRAM_WIDTH_PIXELS / 8);
	framebuffer[(RESULT_START + 10) * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[(RESULT_START + 10) * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[(RESULT_START + 10) * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;
	for (i = (RESULT_START + 11); i <= (RESULT_START + 29); i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF0;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x03;
	}
	framebuffer[(RESULT_START + 30) * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[(RESULT_START + 30) * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[(RESULT_START + 30) * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;

	framebuffer[(RESULT_START + 38) * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[(RESULT_START + 38) * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[(RESULT_START + 38) * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;
	for (i = (RESULT_START + 39); i <= (RESULT_START + 57); i++)
	{
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF0;
		memset(&framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
		framebuffer[i * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x03;
	}
	framebuffer[(RESULT_START + 58) * LCD_VRAM_WIDTH_PIXELS / 8] = 0xF8;
	memset(&framebuffer[(RESULT_START + 58) * LCD_VRAM_WIDTH_PIXELS / 8 + 1], 0, 13); 
	framebuffer[(RESULT_START + 58) * LCD_VRAM_WIDTH_PIXELS / 8 + 14] = 0x07;
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, RESULT_START + 12, "Set Password", 12, 0);
	render_string(SUBTITLE_FONT_IDX, LCD_LEFT_MARGIN, RESULT_START + 40, "Cancel", 6, 0);
	guilib_fb_unlock();
}

int check_password(char *password)
{
	SHA1Context sha;
	
	if (!strlen(password))
		return -1;
	
	SHA1Reset(&sha);
	SHA1Input(&sha, (const unsigned char *) password, strlen(password));
	if (!memcmp(sha.Message_Digest, restriction_pass1, 20))
		return 0;
	else
		return -1;
}

int check_restriction(long idx_article)
{
	int fd;
	int len;
	char restriction_pass2[20];
	SHA1Context sha;

	saved_idx_article = idx_article;
	if (restriction_filter_off == -1)
	{
		fd = wl_open("pass1", WL_O_RDONLY);
		if (fd >= 0)
		{
			len = wl_read(fd, restriction_pass1, 20);
			wl_close(fd);
			if (len < 20)
				memset(restriction_pass1, 0, 20);
			
			fd = wl_open("pass2", WL_O_RDONLY);
			if (fd >= 0)
			{
				len = wl_read(fd, restriction_pass2, 20);
				wl_close(fd);
			}
			if (len < 20)
				memset(restriction_pass1, 0, 20);
	
			SHA1Reset(&sha);
			SHA1Input(&sha, (const unsigned char *) restriction_pass1, 20);
			if (!memcmp(sha.Message_Digest, restriction_pass2, 20))
				restriction_filter_off = 1;
			else
				restriction_filter_off = 0;
		}
		else
		{
			set_password();
		}
		return -1;
	} else if (restriction_filter_off)
		return 0; // ok
	else
	{
		get_password();
		return -1;
	}
}

int password_add_char(char c)
{
        if(c == 0x20 && password_str_len > 0 && password_string[password_str_len-1] == 0x20)
                return -1;
	if (password_str_len >= MAX_PASSWORD_LEN - 2) // reserve one byte for '\0'
		return -1;

	if (!password_str_len && c == 0x20)
		return -1;

	if ('A' <= c && c <= 'Z')
		c += 32;
	password_string[password_str_len++] = toupper(c);
	password_string[password_str_len] = '\0';
	render_string(TITLE_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, password_string, strlen(password_string), 0);
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
	x = render_string(TITLE_FONT_IDX, LCD_LEFT_MARGIN, LCD_TOP_MARGIN, password_string, strlen(password_string), 0);
	guilib_clear_area(x, LCD_TOP_MARGIN, 239, RESULT_START - 1);
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
