/*
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

#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

#include <grifo.h>

#include "ustring.h"
#include "wikilib.h"
#include "guilib.h"
#include "glyph.h"
#include "history.h"
#include "keyboard.h"
#include "search.h"
#include "bmf.h"
#include "lcd_buf_draw.h"
#include "search_fnd.h"
#include "restricted.h"
#include "wiki_info.h"
#include "utf8.h"
#include "highlight.h"

#define LCD_Y_CALIBRATION_ADJUSTMENT (-1)

struct pos {
	unsigned int x;
	unsigned int y;
};

int last_display_mode = 0;
int display_mode = DISPLAY_MODE_INDEX;
static struct keyboard_key * pre_key= NULL;
static unsigned int article_touch_down_handled = 0;
unsigned int touch_down_on_keyboard = 0;
static unsigned int touch_down_on_list = 0;
static struct pos article_touch_down_pos;
static int touch_y_last = -1; // -1 stands for no touch yet
static int touch_x_last = -1;
static unsigned long start_move_time = 0;
static unsigned long last_unreleased_time = 0;
int    last_index_y_pos;
int    enter_touch_y_pos = -1;
int    last_history_y_pos;
char * articleBuffer = 0;
int articleLength = 0;
unsigned char * membuffer = 0;
int membuffersize = 0;
int curBufferPos  = 0;
const unsigned char *article_buf_pointer;
//int is_rendering = 0;
int last_selection = 0;
unsigned long start_search_time, last_delete_time;
extern unsigned long time_scroll_article_last;
extern unsigned long time_scroll_article_start;
extern long saved_idx_article;
extern long saved_prev_idx_article;
bool random_press = false;
extern int stop_render_article;
int time_random_last = 0;
extern bool search_string_changed;
extern unsigned int time_search_last;
extern int b_show_scroll_bar;
extern int display_first_page;
bool press_delete_button = false;
extern bool search_string_changed_remove;
int touch_search = 0,search_touch_pos_y_last=0;
bool article_moved = false;
bool b_in_highlighting = false;
int  article_scroll_threshold = INITIAL_ARTICLE_SCROLL_THRESHOLD;
int article_moved_x_pixels = 0;
int article_moved_y_pixels = 0;
extern int link_to_be_inverted;
extern int link_currently_inverted;
long finger_move_speed = 0;
#ifdef OVER_SCROLL_ENABLED
extern int over_scroll_lines;
#endif
int finger_touched = 0;
extern int temperature_mode;
int logo_width = 0;
int logo_height = 0;
struct guilib_image *p_logo_bitmap = NULL;

static bool get_next_token(char *word, char *buf)
{
	static char *last = NULL;

	if (buf)
		last = buf;
	if (word && last && *last)
	{
		while (last && *last && *last != ',')
			*word++ = *last++;
		if (*last == ',')
			last++;
		*word = '\0';
		return true;
	}
	return false;
}

static unsigned char hex2c_reverse(char *word)
{
	unsigned char c = 0;

	while (*word)
	{
		if (!strncmp(word, "0x", 2))
			word++;
		else
		{
			if ('0' <= *word && *word <= '9')
				c = c * 16 + (*word - '0');
			else if ('a' <= *word && *word <= 'f')
				c = c * 16 + (*word - 'a' + 10);
		}
		word++;
	}

	// reverse bits
	c = (c & 0x0F) << 4 | (c & 0xF0) >> 4;
	c = (c & 0x33) << 2 | (c & 0xCC) >> 2;
	c = (c & 0x55) << 1 | (c & 0xAA) >> 1;

	return c;
}

#define MAX_LINE_SIZE 256
static ssize_t copy_line(char *buf, char *line, ssize_t nLineChars)
{
	ssize_t len_left;
	int i = 0;

	while (i < nLineChars && i < MAX_LINE_SIZE - 1 && line[i] != '\r' && line[i] != '\n')
		i++;
	if (i > 0)
		memcpy(buf, line, i);
	buf[i] = '\0';
	while (i < nLineChars && (line[i] == '\r' || line[i] == '\n'))
		i++;
	len_left = nLineChars - i;
	if (len_left > 0)
		memmove(line, &line[i], len_left);
	return len_left;
}

/*
** A simple implementation of loading an xbm file
** assuming the file format is like the following:
**
**  #define x_width ...
**  #define x_height ...
**  static char x_bits[] = {
**  0x..., 0x..., ...
**  ...
**  };
**
** No comments nor extra spacing characters are allowed and lower cases only
** Note: the bit order in each x_bits is reversed
*/
static void load_logo_xbm()
{
	int fd;
	char line[MAX_LINE_SIZE], buf[MAX_LINE_SIZE], word[MAX_LINE_SIZE];
	ssize_t nLineChars;
	int width_bytes = 0;
	int x = 0;
	int y = -1;
	unsigned char last_byte_truncate = 0xFF;

	fd = file_open("logo.xbm", FILE_OPEN_READ);
	if (fd >= 0)
	{
		nLineChars = file_read(fd, line, MAX_LINE_SIZE);
		while (nLineChars > 0)
		{
			nLineChars = copy_line(buf, line, nLineChars);
			if (!logo_width || !logo_height)
			{
				if (!strncmp(buf, "#define x_width ", strlen("#define x_width ")))
					logo_width = atoi(&buf[strlen("#define x_width ")]);
				if (!strncmp(buf, "#define x_height ", strlen("#define x_height ")))
					logo_height = atoi(&buf[strlen("#define x_height ")]);
			}
			else
			{
				if (!p_logo_bitmap)
				{
					width_bytes = (logo_width + 7) / 8;
					p_logo_bitmap = (struct guilib_image *)memory_allocate(width_bytes * logo_height + sizeof(unsigned int) * 2, "logo");
					if (!p_logo_bitmap)
					{
						logo_width = 0;
						logo_height = 0;
						break;
					}
					p_logo_bitmap->width = width_bytes * 8;
					p_logo_bitmap->height = logo_height;
					last_byte_truncate <<= width_bytes * 8 - logo_width;
				}
				if (y >= logo_height)
					break;
				if (!strncmp(buf, "0x", 2)) // assuming the bitmap data lines all start with 0x
				{
					x = 0; // a new bitmap line
					y++;
					get_next_token(NULL, buf); // initialize get_next_token()
					while (x < width_bytes && get_next_token(word, NULL))
					{
						p_logo_bitmap->data[x + y * width_bytes] = hex2c_reverse(word);
						if (x == width_bytes - 1)
							p_logo_bitmap->data[x + y * width_bytes] &= last_byte_truncate;
						x++;
					}
				}
			}
			nLineChars += file_read(fd, &line[nLineChars], MAX_LINE_SIZE - nLineChars);
		}
		file_close(fd);
	}
}

void repaint_search(void)
{
	guilib_fb_lock();
	search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_KEEP_REFRESH);
	keyboard_paint();
	guilib_fb_unlock();
}

/* this is only called for the index page */
static void toggle_soft_keyboard(void)
{
	//guilib_fb_lock();
	int mode = keyboard_get_mode();

	/* Set the keyboard mode to what we want to change to. */
	if (mode == KEYBOARD_NONE || search_result_count()==0) {
		keyboard_set_mode(wiki_default_keyboard());
		if (mode == KEYBOARD_NONE)
			restore_search_list_page();
		keyboard_paint();
	} else {
		keyboard_set_mode(KEYBOARD_NONE);
		search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_KEEP_RESULT);
	}

	//guilib_fb_unlock();
}

static void print_intro()
{
	//keyboard_set_mode(KEYBOARD_CHAR);

	guilib_fb_lock();
	guilib_clear();

	keyboard_paint();
	guilib_fb_unlock();
}

static unsigned int s_article_y_pos;
static uint32_t s_article_offset = 0;

void invert_selection(int old_pos, int new_pos, int start_pos, int height)
{
	guilib_fb_lock();

	if (old_pos != -1)
	{
		guilib_invert(start_pos - 2 + old_pos * height, height);
		guilib_invert_area(0, start_pos - 2 + old_pos * height, 0, start_pos - 2 + old_pos * height);
		guilib_invert_area(0, start_pos - 2 + old_pos * height + height - 1, 0, start_pos - 2 + old_pos * height + height - 1);
		guilib_invert_area(LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + old_pos * height, LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + old_pos * height);
		guilib_invert_area(LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + old_pos * height + height - 1, LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + old_pos * height + height - 1);
	}
	if (new_pos != -1)
	{
		guilib_invert(start_pos - 2 + new_pos * height, height);
		guilib_invert_area(0, start_pos - 2 + new_pos * height, 0, start_pos - 2 + new_pos * height);
		guilib_invert_area(0, start_pos - 2 + new_pos * height + height - 1, 0, start_pos - 2 + new_pos * height + height - 1);
		guilib_invert_area(LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + new_pos * height, LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + new_pos * height);
		guilib_invert_area(LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + new_pos * height + height - 1, LCD_BUF_WIDTH_PIXELS - 1, start_pos - 2 + new_pos * height + height - 1);
	}

	guilib_fb_unlock();
}
void invert_area(int start_x, int start_y, int end_x, int end_y)
{
	guilib_fb_lock();
	guilib_invert_area(start_x,start_y,end_x,end_y);
	guilib_fb_unlock();
}

int article_open(const char *article)
{
	s_article_offset = strtoul(article, 0 /* endptr */, 16 /* base */);
	s_article_y_pos = 0;
	return 0;
}

void article_display_pcf(int yPixel)
{
	int pos;
	int copysize;
	int framebuffersize = guilib_framebuffer_size();
	uint8_t *framebuffer = lcd_get_framebuffer();

	pos = curBufferPos+((yPixel*LCD_BUFFER_WIDTH)/8);
	if(pos<0 || pos>membuffersize)
	{
		return;
	}

	copysize = membuffersize-pos;


	if(copysize>framebuffersize)
		copysize = framebuffersize;

	else
	{
		return;
	}

	guilib_fb_lock();
	guilib_clear();

	memcpy(framebuffer, membuffer+pos, copysize);

	guilib_fb_unlock();

	curBufferPos = pos;
}

void handle_search_key(struct keyboard_key *key, unsigned long ev_time)
{
	int rc = 0;
	static unsigned long last_ev_time = 0;
	static struct keyboard_key *last_key = NULL;
	static const unsigned char *last_key_utf8_char = NULL;
	struct keyboard_key *temp_last_key;
	const unsigned char *temp_last_key_utf8_char;
	unsigned char keycode;

	temp_last_key = last_key;
	temp_last_key_utf8_char = last_key_utf8_char;
	last_key = NULL; // reset for phone style keyboard
	last_key_utf8_char = NULL;

	if (!key)
		return;
	keycode = *key->key;
	if (keycode == WL_KEY_NLS) {
		delay_us(100000);
		display_mode = DISPLAY_MODE_WIKI_SELECTION;
		keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
		keyboard_set_mode(KEYBOARD_NONE);
		wiki_selection();
		return;
	} else if (keycode == WL_KEY_TEMPERATURE) {
		delay_us(100000);
		++temperature_mode;
		temperature_mode %= 3;
		set_temperature_mode();
		guilib_fb_lock();
		keyboard_paint();
		guilib_fb_unlock();
		return;
	} else if (keycode == WL_KEY_BACKSPACE) {
		rc = search_remove_char(0, ev_time);
	} else if (keycode == WL_KEY_SWITCH_KEYBOARD ||
		   keycode == WL_KEY_POHONE_STYLE_KEYBOARD_DEFAULT ||
		   keycode == WL_KEY_POHONE_STYLE_KEYBOARD_ABC ||
		   keycode == WL_KEY_POHONE_STYLE_KEYBOARD_123) { // toggling keyboard will be handled at key down
		rc = -1;
	} else {
		KEYBOARD_MODE mode = keyboard_get_mode();
		if (wiki_keyboard_conversion_needed())
		{
			if (keycode == WL_KEY_SONANT)
			{
				rc = search_replace_japanese_sonant();
			}
			else if (keycode == WL_KEY_BACKWARD)
			{
				rc = search_replace_hiragana_backward();
			}
			else if (keycode == WL_KEY_CLEAR)
			{
				rc = clear_search_string();
				search_string_changed_remove = true;
				press_delete_button = false;
			}
			else
			{
				last_key = temp_last_key;
				last_key_utf8_char = temp_last_key_utf8_char;
				if (KEYBOARD_PHONE_STYLE < mode &&
				    key == last_key && time_diff(ev_time, last_ev_time) <= seconds_to_ticks(PHONE_STYLE_KEYIN_BEFORE_COMMIT_TIME))
				{
					last_ev_time = ev_time;
					last_key_utf8_char = next_utf8_char(last_key_utf8_char);
					if (*last_key_utf8_char == '\0')
						last_key_utf8_char = key->key;
					rc = search_replace_per_language_char(last_key_utf8_char);
				}
				else
				{
					if (ustrlen(key->key) > 1)
					{
						last_ev_time = ev_time;
						last_key = key;
						last_key_utf8_char = key->key;
					}
					else
					{
						last_key = NULL;
						last_key_utf8_char = NULL;
					}
					rc = search_add_per_language_char(key->key);
				}
			}
		}
		else if (is_supported_search_char(keycode))
			rc = search_add_char(tolower(keycode), ev_time);
		else // not supported char
			return;
	}

	guilib_fb_lock();
	if (!rc)
		search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NO_POPULATE);
//
//	keyboard_paint();
	guilib_fb_unlock();
}

static void handle_button_release(button_t keycode)
{
//	static long idx_article = 0;
	static int first_time_random = 0;
	//int mode;
	struct keyboard_key *key;

	finger_move_speed = 0;
	keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0); // reset invert immediately
	//mode = keyboard_get_mode();
	if (keycode == BUTTON_POWER) {
		history_list_save(HISTORY_SAVE_POWER_OFF);
		delay_us(250000);
		power_off();
	} else if (keycode == BUTTON_SEARCH) {
		article_buf_pointer = NULL;
		/* back to search */
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
			clear_article_pos_info(); // to clear the previous article positioning information for list links to work properly
			search_set_selection(-1);
			display_mode = DISPLAY_MODE_INDEX;
			keyboard_set_mode(wiki_default_keyboard());
			repaint_search();
		}
	} else if ((keycode == BUTTON_HISTORY)&&(display_mode != DISPLAY_MODE_ARTICLE)) {
		if (display_mode != DISPLAY_MODE_HISTORY) {
			clear_article_pos_info(); // to clear the previous article positioning information for list links to work properly
			article_buf_pointer = NULL;
			history_reload();
			display_mode = DISPLAY_MODE_HISTORY;
			keyboard_set_mode(KEYBOARD_NONE);
		} else {
			if (keyboard_get_mode() == KEYBOARD_CLEAR_HISTORY)
			{
				keyboard_set_mode(KEYBOARD_NONE);
				guilib_fb_lock();
				//keyboard_paint();
				draw_clear_history(1);
				guilib_fb_unlock();
			} else if (history_get_count() > 0 && display_first_page) {
				keyboard_set_mode(KEYBOARD_CLEAR_HISTORY);
				guilib_fb_lock();
				//keyboard_paint();
				draw_clear_history(0);
				guilib_fb_unlock();
			}
		}
	} else if ((keycode == BUTTON_HISTORY)&&(display_mode == DISPLAY_MODE_ARTICLE)) {
      // Add here page next Karthik
             display_article_with_pcf(-200);
	} else if ((keycode == BUTTON_RANDOM)&&(display_mode != DISPLAY_MODE_ARTICLE)) {
		if (first_time_random < 4)
			first_time_random++;
		if (first_time_random == 3)
		{
			first_time_random = 4;
			if (init_article_filter())
				return;
		}
		article_buf_pointer = NULL;
		display_mode = DISPLAY_MODE_ARTICLE;
		last_display_mode = DISPLAY_MODE_INDEX;
		random_article();
	} else if ((keycode == BUTTON_RANDOM)&&(display_mode == DISPLAY_MODE_ARTICLE)) {
		// Add here page next Karthik
		display_article_with_pcf(200);
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		article_buf_pointer = NULL;
		key = keyboard_locate_key((char)keycode);
		if (key)
			handle_search_key(key, timer_get());
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (keycode == 'Y' || keycode == 'y') {
			history_clear();
			debug_printf("History Cleared\n"); // inform test program history was cleared
		}
		clear_article_pos_info(); // to clear the previous article positioning information for list links to work properly
		article_buf_pointer = NULL;
		history_reload();
		keyboard_set_mode(KEYBOARD_NONE);
	} else if (display_mode == DISPLAY_MODE_ARTICLE) {
//		article_buf_pointer = NULL;
		if (keycode == WL_KEY_BACKSPACE) {
			if (last_display_mode == DISPLAY_MODE_INDEX) {
				display_mode = DISPLAY_MODE_INDEX;
				repaint_search();
			} else if (last_display_mode == DISPLAY_MODE_HISTORY) {
				display_mode = DISPLAY_MODE_HISTORY;
				history_reload();
			}
		}
	}
}

static void average_xy(int *average_x, int *average_y, int last_5_x[], int last_5_y[], unsigned long last_5_y_time_ticks[])
{
	int i;

	*average_y = 999; // take the toppest y
	*average_x = -1;
	for (i = 4; i >= 0; i--)
	{
		if (last_5_x[i] >= 0 && time_diff(last_5_y_time_ticks[i], last_5_y_time_ticks[0]) < seconds_to_ticks(SAME_CLICK_TIME_THRESHOLD))
		{
			if (*average_x < 0)
			{
				*average_x = last_5_x[i];
			}
			else
			{
				*average_x += last_5_x[i];
				*average_x /= 2; // the latter point got the higher weighting
			}
			if (*average_y > last_5_y[i])
				*average_y = last_5_y[i];
		}
	}
	if (0 < *average_y + LCD_Y_CALIBRATION_ADJUSTMENT && *average_y + LCD_Y_CALIBRATION_ADJUSTMENT < LCD_HEIGHT)
		*average_y = *average_y + LCD_Y_CALIBRATION_ADJUSTMENT;
}

static void handle_keyboard_en(event_t *ev, int last_5_x[], int last_5_y[], unsigned long last_5_y_time_ticks[])
{
	KEYBOARD_MODE mode;
	struct keyboard_key * key;
	int i;
	int average_x, average_y;
	long time_now;

	article_buf_pointer = NULL;
	if (!touch_down_on_keyboard && !touch_down_on_list)
	{
		last_5_x[0] = ev->touch.x;
		last_5_y[0] = ev->touch.y;
		last_5_y_time_ticks[0] = ev->time_stamp;
		for (i = 1; i < 5; i++)
		{
			last_5_x[i] = -1;
			last_5_y[i] = -1;
		}
	}
	else
	{
		for (i = 4; i >= 1; i--)
		{
			last_5_x[i] = last_5_x[i-1];
			last_5_y[i] = last_5_y[i-1];
			last_5_y_time_ticks[i] = last_5_y_time_ticks[i-1];
		}
		last_5_x[0] = ev->touch.x;
		last_5_y[0] = ev->touch.y;
		last_5_y_time_ticks[0] = ev->time_stamp;
	}
	average_xy(&average_x, &average_y, last_5_x, last_5_y, last_5_y_time_ticks);
	key = keyboard_get_data(average_x, average_y);
	if (ev->item_type == EVENT_TOUCH_UP) {
		keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY, ev->time_stamp); // reset invert with delay
		enter_touch_y_pos = -1;
		touch_search = 0;
		//if (pre_key && pre_key != key && keyboard_adjacent_keys(pre_key, key))
		//{
		//	touch_down_on_keyboard = 0;
		//	touch_down_on_list = 0;
		//	pre_key = NULL;
		//	goto out;
		//}

		pre_key = NULL;
		if (press_delete_button)
		{
			handle_search_key(NULL, 0); // reset pre_key for phone style keyboard
			touch_down_on_keyboard = 0;
			touch_down_on_list = 0;
			press_delete_button = false;
			goto out;
		}
		if (key) {
			if (!touch_down_on_keyboard) {
				touch_down_on_keyboard = 0;
				touch_down_on_list = 0;
				goto out;
			}
			handle_search_key(key, ev->time_stamp);
		}
		else {
			if (!touch_down_on_list || ev->touch.y < RESULT_START - RESULT_HEIGHT) {
				touch_down_on_keyboard = 0;
				touch_down_on_list = 0;
				goto out;
			}
			if(search_result_count()==0)
				goto out;

			//search_set_selection(last_selection);
			//search_open_article(last_selection);
			if(search_result_selected()>=0)
			{
				display_mode = DISPLAY_MODE_ARTICLE;
				last_display_mode = DISPLAY_MODE_INDEX;
				search_open_article(search_result_selected());
			}
		}
		touch_down_on_keyboard = 0;
		touch_down_on_list = 0;
	} else {
		if (pre_key && (!key || ustrcmp(pre_key->key, key->key)))
		{
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
			//goto out;
		}

		if(enter_touch_y_pos<0)  //record first touch y pos
			enter_touch_y_pos = ev->touch.y;
		last_index_y_pos = ev->touch.y;
		if (!key || *key->key!=WL_KEY_BACKSPACE || !press_delete_button)
		{
			start_search_time = ev->time_stamp;
			last_delete_time = start_search_time;
		}
		if (key) {
			if(*key->key==WL_KEY_BACKSPACE)//press "<" button
			{
				if (!press_delete_button)
				{
					press_delete_button = true;
					if(get_search_string_len()>0)
					{
						time_now = timer_get();
						if (!search_remove_char(0, time_now))
						{
							search_string_changed_remove = true;
							search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NO_POPULATE);
						}
						last_delete_time = time_now;
					}
				}
			}
			else
			{
				press_delete_button = false;
				if(*key->key == WL_KEY_SWITCH_KEYBOARD)
				{
					keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
					if (!touch_down_on_keyboard)
					{
						mode = keyboard_get_mode();
						if(mode == wiki_default_keyboard())
							keyboard_set_mode(KEYBOARD_NUM);
						else if(mode == KEYBOARD_NUM)
						{
							if (wiki_is_japanese() || wiki_is_korean())
								keyboard_set_mode(KEYBOARD_CHAR);
							else
								keyboard_set_mode(wiki_default_keyboard());
						} else if (mode == KEYBOARD_CHAR) { // mode != wiki_default_keyboard() && mode == KEYBOARD_CHAR
							keyboard_set_mode(wiki_default_keyboard());
						}
						guilib_fb_lock();
						keyboard_paint();
						guilib_fb_unlock();
					}
				}
			}

			if (!touch_down_on_keyboard && !touch_down_on_list)
				touch_down_on_keyboard = 1;

			if (pre_key && !ustrcmp(pre_key->key, key->key)) goto out;

			if (touch_down_on_keyboard) {
				keyboard_key_invert(key);
				//if (pre_key && !keyboard_adjacent_keys(pre_key, key))
				//{
				//	handle_search_key(pre_key, ev->time_stamp);
				//	pre_key = NULL;
				//}
				//else
				pre_key = key;
			}
		} else {
			if (!touch_down_on_keyboard && !touch_down_on_list)
				touch_down_on_list = 1;
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY, ev->time_stamp); // reset invert with delay
			pre_key = NULL;

			if (!search_result_count()) goto out;

			if(touch_search == 0)
			{
				//last_search_y_pos = ev->touch.y;
				touch_search = 1;
			}
			else
			{
				if(search_result_selected()>=0 && abs(ev->touch.y-search_touch_pos_y_last)>5)
				{
					invert_selection(search_result_selected(),-1, RESULT_START, RESULT_HEIGHT);
					search_set_selection(-1);
				}
				goto out;
			}

			int new_selection;
			if((ev->touch.y - RESULT_START)<0)
				new_selection = -1;
			else
				new_selection = ((unsigned int)ev->touch.y - RESULT_START) / RESULT_HEIGHT;

			if (new_selection == search_result_selected()) goto out;

			unsigned int avail_count = keyboard_get_mode() == KEYBOARD_NONE ?
				NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
			avail_count = search_result_count() > avail_count ? avail_count : search_result_count();
			if (new_selection >= (int)avail_count) goto out;
			if (touch_down_on_keyboard) goto out;

			//invert_selection(search_result_selected(), new_selection, RESULT_START, RESULT_HEIGHT);
			invert_selection(-1, new_selection, RESULT_START, RESULT_HEIGHT);

			last_selection = new_selection ;
			search_set_selection(new_selection);
			search_touch_pos_y_last = ev->touch.y;
		}
	}
out:
	return;
}

static void handle_keyboard_phone_style(event_t *ev, int last_5_x[], int last_5_y[], unsigned long last_5_y_time_ticks[])
{
	struct keyboard_key * key;
	int i;
	int average_x, average_y;
	long time_now;

	article_buf_pointer = NULL;
	if (!touch_down_on_keyboard && !touch_down_on_list)
	{
		last_5_x[0] = ev->touch.x;
		last_5_y[0] = ev->touch.y;
		last_5_y_time_ticks[0] = ev->time_stamp;
		for (i = 1; i < 5; i++)
		{
			last_5_x[i] = -1;
			last_5_y[i] = -1;
		}
	}
	else
	{
		for (i = 4; i >= 1; i--)
		{
			last_5_x[i] = last_5_x[i-1];
			last_5_y[i] = last_5_y[i-1];
			last_5_y_time_ticks[i] = last_5_y_time_ticks[i-1];
		}
		last_5_x[0] = ev->touch.x;
		last_5_y[0] = ev->touch.y;
		last_5_y_time_ticks[0] = ev->time_stamp;
	}
	average_xy(&average_x, &average_y, last_5_x, last_5_y, last_5_y_time_ticks);
	key = keyboard_get_data(average_x, average_y);
	if (ev->item_type == EVENT_TOUCH_UP) {
		keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY, ev->time_stamp); // reset invert with delay
		enter_touch_y_pos = -1;
		touch_search = 0;

		pre_key = NULL;
		if (press_delete_button)
		{
			handle_search_key(NULL, 0); // reset pre_key for phone style keyboard
			touch_down_on_keyboard = 0;
			touch_down_on_list = 0;
			press_delete_button = false;
			goto out;
		}
		if (key) {
			if (!touch_down_on_keyboard) {
				touch_down_on_keyboard = 0;
				touch_down_on_list = 0;
				goto out;
			}
			handle_search_key(key, ev->time_stamp);
		}
		else {
			if (!touch_down_on_list || ev->touch.y < RESULT_START - RESULT_HEIGHT) {
				touch_down_on_keyboard = 0;
				touch_down_on_list = 0;
				goto out;
			}
			if(search_result_count()==0)
				goto out;

			//search_set_selection(last_selection);
			//search_open_article(last_selection);
			if(search_result_selected()>=0)
			{
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
				display_mode = DISPLAY_MODE_ARTICLE;
				last_display_mode = DISPLAY_MODE_INDEX;
				search_open_article(search_result_selected());
			}
		}
		touch_down_on_keyboard = 0;
		touch_down_on_list = 0;
	} else {
		if(enter_touch_y_pos<0)  //record first touch y pos
			enter_touch_y_pos = ev->touch.y;
		last_index_y_pos = ev->touch.y;
		if (!key || *key->key!=WL_KEY_BACKSPACE || !press_delete_button)
		{
			start_search_time = ev->time_stamp;
			last_delete_time = start_search_time;
		}
		if (key) {
			if(*key->key==WL_KEY_BACKSPACE)//press "<" button
			{
				if (!press_delete_button)
				{
					press_delete_button = true;
					if(get_search_string_len()>0)
					{
						time_now = timer_get();
						if (!search_remove_char(0, time_now))
						{
							search_string_changed_remove = true;
							search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NO_POPULATE);
						}
						last_delete_time = time_now;
					}
				}
			}
			else
			{
				press_delete_button = false;
				if(*key->key == WL_KEY_POHONE_STYLE_KEYBOARD_DEFAULT ||
				   *key->key == WL_KEY_POHONE_STYLE_KEYBOARD_ABC ||
				   *key->key == WL_KEY_POHONE_STYLE_KEYBOARD_123)
				{
					keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
					if (!touch_down_on_keyboard)
					{
						if (*key->key == WL_KEY_POHONE_STYLE_KEYBOARD_DEFAULT)
							keyboard_set_mode(wiki_default_keyboard());
						else if (*key->key == WL_KEY_POHONE_STYLE_KEYBOARD_ABC)
							keyboard_set_mode(KEYBOARD_PHONE_STYLE_ABC);
						else if (*key->key == WL_KEY_POHONE_STYLE_KEYBOARD_123)
							keyboard_set_mode(KEYBOARD_PHONE_STYLE_123);
						guilib_fb_lock();
						keyboard_paint();
						guilib_fb_unlock();
					}
				}
			}

			if (!touch_down_on_keyboard && !touch_down_on_list)
				touch_down_on_keyboard = 1;

			if (pre_key && !ustrcmp(pre_key->key, key->key)) goto out;

			if (touch_down_on_keyboard) {
				keyboard_key_invert(key);
				//if (pre_key && !keyboard_adjacent_keys(pre_key, key))
				//{
				//	handle_search_key(pre_key, ev->time_stamp);
				//	pre_key = NULL;
				//}
				//else
				pre_key = key;
			}
		} else {
			if (!touch_down_on_keyboard && !touch_down_on_list)
				touch_down_on_list = 1;
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY, ev->time_stamp); // reset invert with delay
			pre_key = NULL;

			if (!search_result_count()) goto out;

			if(touch_search == 0)
			{
				//last_search_y_pos = ev->touch.y;
				touch_search = 1;
			}
			else
			{
				if(search_result_selected()>=0 && abs(ev->touch.y-search_touch_pos_y_last)>5)
				{
					invert_selection(search_result_selected(),-1, RESULT_START, RESULT_HEIGHT);
					search_set_selection(-1);
				}
				goto out;
			}

			int new_selection;
			if((ev->touch.y - RESULT_START)<0)
				new_selection = -1;
			else
				new_selection = ((unsigned int)ev->touch.y - RESULT_START) / RESULT_HEIGHT;

			if (new_selection == search_result_selected()) goto out;

			unsigned int avail_count = keyboard_get_mode() == KEYBOARD_NONE ?
				NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
			avail_count = search_result_count() > avail_count ? avail_count : search_result_count();
			if (new_selection >= (int)avail_count) goto out;
			if (touch_down_on_keyboard) goto out;

			//invert_selection(search_result_selected(), new_selection, RESULT_START, RESULT_HEIGHT);
			invert_selection(-1, new_selection, RESULT_START, RESULT_HEIGHT);

			last_selection = new_selection ;
			search_set_selection(new_selection);
			search_touch_pos_y_last = ev->touch.y;
		}
	}
out:
	return;
}

static void handle_touch(event_t *ev)
{
	//int offset,offset_count,
	int article_link_number=-1;
	//int time_diff_search;
	int mode;
	struct keyboard_key * key;
	static int last_5_x[5];
	static int last_5_y[5];
	static unsigned long last_5_y_time_ticks[5];
	int i;
	int average_x, average_y;
	long time_now;

	mode = keyboard_get_mode();
	if (display_mode == DISPLAY_MODE_INDEX && (KEYBOARD_NONE < mode && mode < KEYBOARD_PHONE_STYLE))
	{
		handle_keyboard_en(ev, last_5_x, last_5_y, last_5_y_time_ticks);
	}
	else if (display_mode == DISPLAY_MODE_INDEX && KEYBOARD_PHONE_STYLE < mode)
	{
		handle_keyboard_phone_style(ev, last_5_x, last_5_y, last_5_y_time_ticks);
	}
	else if (display_mode == DISPLAY_MODE_HISTORY && mode == KEYBOARD_CLEAR_HISTORY)
	{
		key = keyboard_get_data(ev->touch.x, ev->touch.y);
		if (ev->item_type == EVENT_TOUCH_UP) {
			delay_us(100000 * 2);
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
			enter_touch_y_pos = -1;
			touch_search = 0;
			pre_key = NULL;
			if (press_delete_button)
			{
				press_delete_button = false;
				goto out;
			}
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}
				if (*key->key == 'Y') {
					history_clear();
					keyboard_set_mode(KEYBOARD_NONE);
					history_reload();
				} else if (*key->key == 'N') {
					keyboard_set_mode(KEYBOARD_NONE);
					guilib_fb_lock();
					draw_clear_history(1);
					guilib_fb_unlock();
				}
			}
			else {
				touch_down_on_keyboard = 0;
				touch_down_on_list = 0;
				goto out;
			}
		} else {
			if(enter_touch_y_pos<0)  //record first touch y pos
				enter_touch_y_pos = ev->touch.y;
			last_index_y_pos = ev->touch.y;
			if (key) {
				if (!touch_down_on_keyboard)
					touch_down_on_keyboard = 1;

				if (pre_key && !ustrcmp(pre_key->key, key->key)) goto out;

				if (touch_down_on_keyboard) {
					keyboard_key_invert(key);
					pre_key = key;
				}
			} else {
				touch_down_on_keyboard = 0;
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY, ev->time_stamp); // reset invert with delay
				pre_key = NULL;

			}
		}
	}
	else if (display_mode == DISPLAY_MODE_RESTRICTED)
	{
		key = keyboard_get_data(ev->touch.x, ev->touch.y);
		if (ev->item_type == EVENT_TOUCH_UP) {
			if (*key->key == 'Y' || *key->key == 'N' || *key->key == 'P')
			{
				delay_us(100000 * 2);
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
			}
			else
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY, ev->time_stamp); // reset invert with delay
			enter_touch_y_pos = -1;
			touch_search = 0;
			//if (pre_key && pre_key != key && keyboard_adjacent_keys(pre_key, key))
			//{
			//	touch_down_on_keyboard = 0;
			//	touch_down_on_list = 0;
			//	pre_key = NULL;
			//	goto out;
			//}

			pre_key = NULL;
			if (press_delete_button)
			{
				touch_down_on_keyboard = 0;
				touch_down_on_list = 0;
				press_delete_button = false;
				goto out;
			}
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					goto out;
				}
				handle_password_key(*key->key);
			}
			touch_down_on_keyboard = 0;
		} else {
			if (pre_key && (!key || ustrcmp(pre_key->key, key->key)))
			{
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
				//goto out;
			}

			if(enter_touch_y_pos<0)  //record first touch y pos
				enter_touch_y_pos = ev->touch.y;
			last_index_y_pos = ev->touch.y;
			if (!key || *key->key!=WL_KEY_BACKSPACE || !press_delete_button)
			{
				start_search_time = ev->time_stamp;
				last_delete_time = start_search_time;
			}
			if (key) {
				if(*key->key==WL_KEY_BACKSPACE)//press "<" button
				{
					if (!press_delete_button)
					{
						press_delete_button = true;
						if (get_password_string_len()>0)
						{
							time_now = timer_get();
							password_remove_char();
							last_delete_time = time_now;
						}
					}
				}
				else
				{
					press_delete_button = false;
					if(*key->key == WL_KEY_SWITCH_KEYBOARD)
					{
						mode = keyboard_get_mode();
						if(mode == KEYBOARD_PASSWORD_CHAR)
							keyboard_set_mode(KEYBOARD_PASSWORD_NUM);
						else if(mode == KEYBOARD_PASSWORD_NUM)
							keyboard_set_mode(KEYBOARD_PASSWORD_CHAR);
						guilib_fb_lock();
						keyboard_paint();
						guilib_fb_unlock();
					}
				}

				if (!touch_down_on_keyboard)
					touch_down_on_keyboard = 1;

				if (pre_key && !ustrcmp(pre_key->key, key->key)) goto out;

				if (touch_down_on_keyboard) {
					keyboard_key_invert(key);
					pre_key = key;
				}
			} else {
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW, 0);
				pre_key = NULL;

				search_touch_pos_y_last = ev->touch.y;
			}
		}
	} else {
		if (ev->item_type == EVENT_TOUCH_UP) {
			unsigned long diff_ticks = 0;
			long diff_y = 0;

			if (b_in_highlighting)
			{
				highlight_set(ev->touch.x, ev->touch.y);
				highlight_handle_search();
				highlight_reset(-1, -1, true);
				article_scroll_threshold = INITIAL_ARTICLE_SCROLL_THRESHOLD;
				article_moved_x_pixels = 0;
				article_moved_y_pixels = 0;
				touch_x_last = -1;
				touch_y_last = -1;
				start_move_time = 0;
				return;
			}

			finger_touched = 0;
			time_scroll_article_last = ev->time_stamp;
			for (i = 4; i > 0; i--)
			{
				if (last_5_y[i] >= 0)
				{
					diff_y =  last_5_y[i] - ev->touch.y;
					diff_ticks = time_diff(ev->time_stamp, last_5_y_time_ticks[i]);
					break;
				}
			}

			if (diff_ticks <= 0 || abs(article_moved_y_pixels) > SMOOTH_SCROLL_ACTIVATION_OFFSET_HIGH_THRESHOLD ||
			    abs(article_moved_y_pixels) < SMOOTH_SCROLL_ACTIVATION_OFFSET_LOW_THRESHOLD)
				finger_move_speed = 0;
			else
			{
				finger_move_speed = (float)diff_y * ((float)seconds_to_ticks(1) / (float)diff_ticks);
				if (abs(finger_move_speed) > SMOOTH_SCROLL_ACTIVATION_SPPED_THRESHOLD)
				{
					if (finger_move_speed > 0)
					{
						if (display_mode == DISPLAY_MODE_ARTICLE)
							finger_move_speed = ARTICLE_SMOOTH_SCROLL_SPEED_FACTOR *
								(finger_move_speed - SMOOTH_SCROLL_ACTIVATION_SPPED_THRESHOLD);
						else
							finger_move_speed = LIST_SMOOTH_SCROLL_SPEED_FACTOR *
								(finger_move_speed - SMOOTH_SCROLL_ACTIVATION_SPPED_THRESHOLD);
					}
					else
					{
						if (display_mode == DISPLAY_MODE_ARTICLE)
							finger_move_speed = ARTICLE_SMOOTH_SCROLL_SPEED_FACTOR *
								(finger_move_speed + SMOOTH_SCROLL_ACTIVATION_SPPED_THRESHOLD);
						else
							finger_move_speed = LIST_SMOOTH_SCROLL_SPEED_FACTOR *
								(finger_move_speed + SMOOTH_SCROLL_ACTIVATION_SPPED_THRESHOLD);
					}
				}
				else
					finger_move_speed = 0;
			}

			article_moved = false;
			if (finger_move_speed == 0 && b_show_scroll_bar)
			{
				b_show_scroll_bar = 0;
				show_scroll_bar(0); // clear scroll bar
			}
			article_scroll_threshold = INITIAL_ARTICLE_SCROLL_THRESHOLD;
			article_moved_x_pixels = 0;
			article_moved_y_pixels = 0;
			touch_x_last = -1;
			touch_y_last = -1;
			start_move_time = 0;

			article_link_number = get_activated_article_link_number();
			if(article_link_number>=0)
			{
				if (link_to_be_inverted >= 0)
				{
					if (link_currently_inverted >= 0)
						invert_link(link_currently_inverted);
					invert_link(link_to_be_inverted);
				}
				if (finger_move_speed == 0)
				{
					init_invert_link();
					last_display_mode = display_mode;
					if (display_mode == DISPLAY_MODE_WIKI_SELECTION)
					{
						set_wiki(article_link_number);
						search_init();
						search_set_selection(-1);
						display_mode = DISPLAY_MODE_INDEX;
						keyboard_set_mode(wiki_default_keyboard());
						repaint_search();
					}
					else
					{
						if (display_mode == DISPLAY_MODE_ARTICLE)
						{
							saved_prev_idx_article = saved_idx_article;
							if (!(saved_prev_idx_article & 0xFF000000)) // idx_article for current wiki
							{
								saved_prev_idx_article |= get_wiki_id_from_idx(nCurrentWiki) << 24;
							}
						}
						else
							saved_prev_idx_article = 0;
						display_mode = DISPLAY_MODE_ARTICLE;
						open_article_link_with_link_number(article_link_number);
					}
				}
				else
				{
					if (link_currently_inverted >= 0)
						invert_link(link_currently_inverted);
					init_invert_link();
				}
				return;
			}

			reset_article_link_number();
			article_touch_down_handled = 0;
		} else {
			finger_touched = 1;
			finger_move_speed = 0;

			if(touch_y_last < 0) // initial touch down
			{
				touch_x_last = ev->touch.x;
				touch_y_last = ev->touch.y;
				last_unreleased_time = ev->time_stamp;
				reset_article_link_number();
				article_moved_x_pixels = 0;
				article_moved_y_pixels = 0;
				last_5_x[0] = ev->touch.x;
				last_5_y[0] = ev->touch.y;
				last_5_y_time_ticks[0] = ev->time_stamp;
				for (i = 1; i < 5; i++)
				{
					last_5_x[i] = -1;
					last_5_y[i] = -1;
				}
				highlight_reset(ev->touch.x, ev->touch.y, false);
			}
			else
			{
				article_moved_x_pixels += touch_x_last - ev->touch.x;
				article_moved_y_pixels += touch_y_last - ev->touch.y;
				if(!b_in_highlighting && (article_moved || abs(article_moved_y_pixels) > article_scroll_threshold))
				{
					if (!article_moved)
					{
						article_moved = true;
						reset_article_link_number();
						if (article_moved_y_pixels > 0)
							article_moved_y_pixels -= article_scroll_threshold;
						else
							article_moved_y_pixels += article_scroll_threshold;
						display_article_with_pcf(article_moved_y_pixels);
						article_scroll_threshold = ARTICLE_MOVING_SCROLL_THRESHOLD;
					}
					else if (finger_move_speed == 0)
					{
						display_article_with_pcf(touch_y_last - ev->touch.y);
					}
					for (i = 4; i >= 1; i--)
					{
						last_5_x[i] = last_5_x[i-1];
						last_5_y[i] = last_5_y[i-1];
						last_5_y_time_ticks[i] = last_5_y_time_ticks[i-1];
					}
					last_5_x[0] = ev->touch.x;
					last_5_y[0] = ev->touch.y;
					last_5_y_time_ticks[0] = ev->time_stamp;
					b_show_scroll_bar = 1;
				}
				else
				{
					if (!b_in_highlighting)
					{
						if (abs(article_moved_x_pixels) > INITIAL_HIGHLIGHT_THRESHOLD)
						{
							//reset_article_link_number();
							//b_in_highlighting = highlight_set(ev->touch.x, ev->touch.y);
						}
					}
					else
					{
						highlight_set(ev->touch.x, ev->touch.y);
						//if (ev->touch.y < HIGHLIGHT_SCROLLING_SPOT_HEIGHT ||
						//	ev->touch.y >= LCD_HEIGHT - HIGHLIGHT_SCROLLING_SPOT_HEIGHT)
						//{
						//	article_moved = true;
						//	finger_move_speed = 20;
						//}
						//else
						//{
						//	finger_move_speed = 0;
						//}
					}
				}
				touch_x_last = ev->touch.x;
				touch_y_last = ev->touch.y;
			}

			if (!b_in_highlighting && !article_moved && get_activated_article_link_number() < 0)
			{
				average_xy(&average_x, &average_y, last_5_x, last_5_y, last_5_y_time_ticks);
				article_link_number = isArticleLinkSelected(average_x, average_y);
				if (article_link_number >= 0)
					set_article_link_number(article_link_number, ev->time_stamp);
			}

			if (!article_touch_down_handled) {
				article_touch_down_pos.x = ev->touch.x;
				article_touch_down_pos.y = ev->touch.y;
				article_touch_down_handled = 1;
			}
		}
	}
out:
	return;
}

int wikilib_init (void)
{
	init_lcd_draw_buf();
	init_file_buffer();
	init_wiki_info();
	return 0;
}

bool callback(void *arg)
{
	const char *message = (const char *)arg;

	debug_printf("Callback: %s\n", message);
	return false;  // request shutdown
}

int wikilib_run(void)
{
	int sleep;
	unsigned long time_now;
	event_t ev;
	int more_events = 0;
	unsigned long last_event_time = 0;
	int rc;

	wikilib_init();
	article_buf_pointer = NULL;
	search_init();
	history_list_init();
	get_temperature_mode();
	print_intro();
	load_logo_xbm();
	draw_logo_or_type_a_word(0, 0, 0, 0);
	load_all_fonts();

	for (;;) {
		if (more_events)
			sleep = 0;
		else
			sleep = 1;
		if (!more_events && display_mode == DISPLAY_MODE_ARTICLE && render_article_with_pcf())
			sleep = 0;
		else if (!more_events && display_mode == DISPLAY_MODE_INDEX && render_search_result_with_pcf())
			sleep = 0;
		else if (!more_events && display_mode == DISPLAY_MODE_HISTORY && render_history_with_pcf())
			sleep = 0;
		else if (!more_events && display_mode == DISPLAY_MODE_WIKI_SELECTION && render_wiki_selection_with_pcf())
			sleep = 0;

		if (finger_move_speed && !finger_touched)
		{
			scroll_article();
			sleep = 0;
		}

		time_now = timer_get();
		if(display_mode == DISPLAY_MODE_INDEX)
		{
			if (press_delete_button && get_search_string_len()>0)
			{
				int kb_mode = keyboard_get_mode();
				sleep = 0;
				if(kb_mode < KEYBOARD_PHONE_STYLE &&
				   time_diff(time_now, start_search_time) > seconds_to_ticks(2.1))
				{
					if (!clear_search_string())
					{
						search_string_changed_remove = true;
						search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NO_POPULATE);
					}
					press_delete_button = false;
				}
				else if (time_diff(time_now, start_search_time) > seconds_to_ticks(0.3) &&
					 time_diff(time_now, last_delete_time) > seconds_to_ticks(0.1))
				{
					if (!search_remove_char(0, time_now))
					{
						search_string_changed_remove = true;
						search_to_be_reloaded(SEARCH_TO_BE_RELOADED_SET, SEARCH_RELOAD_NO_POPULATE);
					}
					last_delete_time = time_now;
				}
			}
		}
		else if (display_mode == DISPLAY_MODE_RESTRICTED)
		{
			if (press_delete_button && get_password_string_len()>0)
			{
				sleep = 0;
				time_now = timer_get();
				if(time_diff(time_now, start_search_time) > seconds_to_ticks(2.1))
				{
					clear_password_string();
					press_delete_button = false;
				}
				else if (time_diff(time_now, start_search_time) > seconds_to_ticks(0.3) &&
					 time_diff(time_now, last_delete_time) > seconds_to_ticks(0.1))
				{
					password_remove_char();
					last_delete_time = time_now;
				}
			}
		}

		if (!more_events && display_mode == DISPLAY_MODE_INDEX && fetch_search_result(0, 0, 0))
		{
			sleep = 0;
		}

		if (!more_events && display_mode == DISPLAY_MODE_INDEX && !press_delete_button && !touch_down_on_keyboard && check_search_string_change())
		{
			sleep = 0;
		}

		if (keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_CHECK, 0)) // check if need to reset invert
			sleep = 0;

		if (keyboard_korean_special_key()) // check if need to enable the special key
			sleep = 0;

		if (check_invert_link()) // check if need to invert link
			sleep = 0;

		if (sleep)
		{
			if (time_diff(timer_get(), last_event_time) > seconds_to_ticks(5))
				rc = history_list_save(HISTORY_SAVE_POWER_OFF);
			else if (time_diff(timer_get(), last_event_time) > seconds_to_ticks(2))
				rc = history_list_save(HISTORY_SAVE_NORMAL);
			else
				rc = -1;
			if (rc > 0)
			{
				delay_us(200000); // for some reason, save may not work if no delay
			}
			else if (rc < 0)
				sleep = 0; // waiting for last_event_time timeout to save the history
		}

		if (sleep)
			event_wait(&ev, callback, "main loop callback");
		else
			event_get(&ev);
		more_events = 1;
		switch (ev.item_type) {
		case EVENT_KEY:
			if (display_mode == DISPLAY_MODE_HISTORY
			    && (ev.key.code == 'y' || ev.key.code == 'Y')) {
				history_clear();
				keyboard_set_mode(KEYBOARD_NONE);
				history_reload();
				debug_print("History Cleared\n");
			}
			break;
		case EVENT_BUTTON_UP:
			b_show_scroll_bar = 0;
			handle_button_release(ev.button.code);
			last_event_time = ev.time_stamp;
			break;
		case EVENT_TOUCH_DOWN:
		case EVENT_TOUCH_MOTION:
		case EVENT_TOUCH_UP:
			handle_touch(&ev);
			last_event_time = ev.time_stamp;
			break;
		case EVENT_NONE:
			more_events = 0;
			break;
		default:
			break;
		}
	}

	/* never reached */
	return 0;
}

unsigned long time_diff(unsigned long t2, unsigned long t1)
{
	unsigned long diff;

	if (t2 < t1)
		diff = (unsigned long) 0xFFFFFFFF - t1 + t2;
	else
		diff = t2 - t1;
	return diff;
}

enum {
	Tick_TicksPerMicroSecond = 60,
	Tick_TicksPerMilliSecond = Tick_TicksPerMicroSecond * 1000,
	Tick_TicksPerSecond = Tick_TicksPerMilliSecond * 1000,
};

unsigned long seconds_to_ticks(float sec)
{
	long clock_ticks;

	clock_ticks = sec * Tick_TicksPerSecond;

	return clock_ticks;
}

void fatal_error_print(const char *file, int line, const char *format, ...)
{
	char buffer[256];

	va_list arguments;

	va_start(arguments, format);

	(void)vsnprintf(buffer, sizeof(buffer), format, arguments);

	va_end(arguments);

	debug_printf("FATAL: %s:%d: %s\n", file, line, buffer);

	static bool recursion = false;
	if (recursion) {
		panic("recursive call to fatal_error\n");
	}
	recursion = true;

	guilib_fb_lock();
	guilib_clear();
	render_string(SUBTITLE_FONT_IDX, -1, 94, (const unsigned char *)buffer, ustrlen(buffer), 0);
	guilib_fb_unlock();

	event_t ev;
	for (;;) {
		event_wait(&ev, callback, "fatal error callback");
	}
}

void wikilib_reset_highlighting()
{
	b_in_highlighting = false;
}

static void draw_logo_bitmap(int clear_start_x, int clear_start_y, int clear_end_x, int clear_end_y)
{
	int start_x, start_y;

	guilib_clear_area(clear_start_x, clear_start_y, clear_end_x, clear_end_y);
	start_x = (LCD_BUF_WIDTH_PIXELS - logo_width) / 2;
	if (start_x < 0)
		start_x = 0;
	start_y = (LCD_HEIGHT - KEYBOARD_HEIGHT - logo_height) / 2;
	guilib_blit_image(p_logo_bitmap, start_x, start_y);
}

void draw_logo_or_type_a_word(int clear_start_x, int clear_start_y, int clear_end_x, int clear_end_y)
{
	const unsigned char *pMsg;

	if (!p_logo_bitmap)
	{
		pMsg = get_nls_text("type_a_word");
		render_string_and_clear(SUBTITLE_FONT_IDX, -1, 55, pMsg, ustrlen(pMsg), 0,
					clear_start_x, clear_start_y, clear_end_x, clear_end_y);
	}
	else
	{
		draw_logo_bitmap(clear_start_x, clear_start_y, clear_end_x, clear_end_y);
	}
}

void clear_logo_or_type_a_word(int clear_start_x, int clear_start_y, int clear_end_x, int clear_end_y)
{
	guilib_clear_area(clear_start_x, clear_start_y, clear_end_x, clear_end_y);
}
