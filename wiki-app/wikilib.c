// Copyright (c) 2009 Openmoko Inc.
//
// Authors:	Daniel Mack <daniel@caiaq.de>
// 		Holger Hans Peter Freyther <zecke@openmoko.org>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#include <stdlib.h>
#include <inttypes.h>
#include <wikilib.h>
#include <guilib.h>
#include <glyph.h>
#include <history.h>
#include <wl-keyboard.h>
#include <input.h>
#include <msg.h>
#include <file-io.h>
#include <search.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <perf.h>
#include <profile.h>
#include <malloc-simple.h>
#include <bmf.h>
#include <lcd_buf_draw.h>
#include <lcd.h>
#include <tick.h>
#include "delay.h"
#include "search_hash.h"
#include "restricted.h"
#ifndef INCLUDED_FROM_KERNEL
#include "time.h"
#else
#include "samo.h"
#endif
//#include <t_services.h>
//#include <kernel.h>
//#include <wl-time.h>

#define DBG_WL 0
#define SEARCH_FETCH_DELAY 1000000*24

struct pos {
	unsigned int x;
	unsigned int y;
};

int last_display_mode = 0;
int display_mode = DISPLAY_MODE_INDEX;
static struct keyboard_key * pre_key= NULL;
static unsigned int article_touch_down_handled = 0;
static unsigned int touch_down_on_keyboard = 0;
static unsigned int touch_down_on_list = 0;
static struct pos article_touch_down_pos;
static unsigned int touch_y_last_unreleased = 0;
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
unsigned char *article_buf_pointer;
//int is_rendering = 0;
int last_selection = 0;
unsigned long start_search_time, last_delete_time;
int last_article_move_time,touch_y_last_article;
int article_touch_count = 0;
int touch_history = 0;
extern unsigned long time_scroll_article_last;
extern unsigned long time_scroll_article_start;
bool random_press = false;
extern int stop_render_article;
int time_random_last = 0;
extern bool search_string_changed;
extern unsigned int time_search_last;
extern int b_show_scroll_bar;
bool press_delete_button = false;
extern bool search_string_changed_remove;
int history_touch_pos_y_last;
int touch_search = 0,search_touch_pos_y_last=0;
bool article_moved = false;
#define INITIAL_ARTICLE_SCROLL_PIXEL 1
#define ARTICLE_MOVED_THRESHOLD 1
#define SMOOTH_SCROLL_ACTIVATION_OFFSET_LOW_THRESHOLD 10
#define SMOOTH_SCROLL_ACTIVATION_OFFSET_HIGH_THRESHOLD 150
#define SMOOTH_SCROLL_ACTIVATION_SPPED_THRESHOLD 50
#define LIST_SMOOTH_SCROLL_SPEED_FACTOR 5
#define ARTICLE_SMOOTH_SCROLL_SPEED_FACTOR 3
int  article_scroll_pixel = INITIAL_ARTICLE_SCROLL_PIXEL;
int article_moved_pixels = 0;
extern int link_to_be_inverted;
extern int link_currently_inverted;
long finger_move_speed = 0;

void repaint_search(void)
{
	guilib_fb_lock();
	search_reload_ex(SEARCH_RELOAD_KEEP_REFRESH);
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
		keyboard_set_mode(KEYBOARD_CHAR);
		if (mode == KEYBOARD_NONE)
			restore_search_list_page();
//		search_reload_ex();
		keyboard_paint();
	} else {
		keyboard_set_mode(KEYBOARD_NONE);
		search_reload_ex(SEARCH_RELOAD_KEEP_RESULT);
	}

	//guilib_fb_unlock();
}

static void print_intro()
{
	//keyboard_set_mode(KEYBOARD_CHAR);

	guilib_fb_lock();
	guilib_clear();

	//membuffer = malloc_simple(1024*1024,MEM_TAG_ARTICLE_F1);
	keyboard_paint();
	guilib_fb_unlock();
}

static unsigned int s_article_y_pos;
static uint32_t s_article_offset = 0;

void invert_selection(int old_pos, int new_pos, int start_pos, int height)
{
	guilib_fb_lock();

	if (old_pos != -1)
		guilib_invert(start_pos - 2 + old_pos * height, height);
	if (new_pos != -1)
		guilib_invert(start_pos - 2 + new_pos * height, height);

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
	DP(DBG_WL, ("O article_open() '%s'\n", article));
	s_article_offset = strtoul(article, 0 /* endptr */, 16 /* base */);
	s_article_y_pos = 0;
	return 0;
}


#if 0
// this is no longer used
void article_display(enum article_nav nav)
{
	unsigned int screen_height = guilib_framebuffer_height();

	DP(DBG_WL, ("O article_display() %i article_offset %u article_y_pos %u\n", nav, s_article_offset, s_article_y_pos));
	if (nav == ARTICLE_PAGE_NEXT)
		s_article_y_pos += screen_height;
	else if (nav == ARTICLE_PAGE_PREV)
		s_article_y_pos = (s_article_y_pos <= screen_height) ? 0 : s_article_y_pos - screen_height;
//	wom_draw(g_womh, s_article_offset, framebuffer, s_article_y_pos, screen_height);
}
#endif

#if 0
// this is no longer used
int article_open_pcf(const char *filename)
{

	FILE * pFile;
	long lSize;
	size_t result;

	pFile = fopen (filename , "rb" );

	if (pFile==NULL)
		return -1;

	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);

	rewind (pFile);

	// allocate memory to contain the whole file:
	articleBuffer = (char*) malloc (sizeof(char)*lSize);
	if (articleBuffer == NULL)
	{
		fclose(pFile);
		return -1;
	}

	// copy the file into the buffer:
	result = fread (articleBuffer,1,lSize,pFile);
	if (result != lSize)
	{
		fclose(pFile);
		return -1;
	}

	// terminate
	fclose (pFile);

	framebuffer_article=(char*)malloc(guilib_framebuffer_width()*nLine);
	memset(framebuffer_article,0,guilib_framebuffer_width()*nLine);

	render_article(articleBuffer,articleLength,framebuffer_article);

	return lSize;
}
#endif

void article_display_pcf(int yPixel)
{
	int pos;
	int copysize;
	int framebuffersize;

	framebuffersize  = 	guilib_framebuffer_size();

	pos = curBufferPos+((yPixel*LCD_VRAM_WIDTH_PIXELS)/8);
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

	memcpy(framebuffer,membuffer+pos,copysize);

	guilib_fb_unlock();

	curBufferPos = pos;
}

static void handle_search_key(char keycode)
{
	int rc = 0;

	if (keycode == WL_KEY_BACKSPACE) {
		rc = search_remove_char(1);
	} else if (is_supported_search_char(keycode)) {
		rc = search_add_char(tolower(keycode));
	} else {
		if(keycode == -42) // toggling keyboard will be handled at key down
		{
			rc = -1;
		}
		else
		{
			return;
		}
	}

	guilib_fb_lock();
	//search_reload();
	if (!rc)
		search_reload_ex(SEARCH_RELOAD_NORMAL);
//	keyboard_paint();
	guilib_fb_unlock();
}

static void handle_cursor(struct wl_input_event *ev)
{
	DP(DBG_WL, ("O handle_cursor()\n"));
	if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			display_article_with_pcf(50);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			display_article_with_pcf(-50);
	} else if (display_mode == DISPLAY_MODE_INDEX && keyboard_get_mode() == KEYBOARD_NONE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			display_article_with_pcf(50);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			display_article_with_pcf(-50);
	} else if (display_mode == DISPLAY_MODE_HISTORY && keyboard_get_mode() == KEYBOARD_NONE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			display_article_with_pcf(50);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			display_article_with_pcf(-50);
	}
}

extern int restricted_article;
static void handle_key_release(int keycode)
{
//	static long idx_article = 0;
	static int first_time_random = 0;
	int mode;

	finger_move_speed = 0;
	keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW); // reset invert immediately
	DP(DBG_WL, ("O handle_key_release()\n"));
	mode = keyboard_get_mode();
	if (keycode == WL_INPUT_KEY_POWER) {
#ifdef INCLUDED_FROM_KERNEL
		if (history_list_save(HISTORY_SAVE_POWER_OFF))
		{
			delay_us(200000);
		}
#endif
	} else if (keycode == WL_INPUT_KEY_SEARCH) {
		article_buf_pointer = NULL;
		/* back to search */
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
			search_set_selection(-1);
			display_mode = DISPLAY_MODE_INDEX;
			keyboard_set_mode(KEYBOARD_CHAR);
			repaint_search();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
		if (display_mode != DISPLAY_MODE_HISTORY) {
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
			} else if (history_get_count() > 0) {
				keyboard_set_mode(KEYBOARD_CLEAR_HISTORY);
				guilib_fb_lock();
				//keyboard_paint();
				draw_clear_history(0);
				guilib_fb_unlock();
			}
		}
	} else if (keycode == WL_INPUT_KEY_RANDOM) {
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
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		article_buf_pointer = NULL;
		if (keycode == WL_KEY_RETURN) {
			int cur_selection = search_current_selection();
			retrieve_article(cur_selection);
#ifdef PROFILER_ON
		} else if (keycode == WL_KEY_HASH) {
			/* activate if you want to run performance tests */
			/* perf_test(); */
			malloc_status_simple();
			prof_print();
#endif
		} else {
			handle_search_key(keycode);
		}
	} else if (display_mode == DISPLAY_MODE_ARTICLE) {
		article_buf_pointer = NULL;
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

static void handle_touch(struct wl_input_event *ev)
{
	//int offset,offset_count,
	int article_link_number=-1;
	int enter_touch_y_pos_record;
	//int time_diff_search;
	int mode;
	struct keyboard_key * key;
	static int last_5_y[5];
	static unsigned long last_5_y_time_ticks[5];
	int i;

	DP(DBG_WL, ("%s() touch event @%d,%d val %d\n", __func__,
		    ev->touch_event.x, ev->touch_event.y, ev->touch_event.value));

	mode = keyboard_get_mode();
	if (display_mode == DISPLAY_MODE_INDEX && (mode == KEYBOARD_CHAR || mode == KEYBOARD_NUM))
	{
		article_buf_pointer = NULL;
		key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
		if (ev->touch_event.value == 0) {
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
			enter_touch_y_pos_record = enter_touch_y_pos;
			enter_touch_y_pos = -1;
			touch_search = 0;
			press_delete_button = false;
			pre_key = NULL;
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}
				handle_search_key(key->key);
			}
			else {
				if (!touch_down_on_list || ev->touch_event.y < RESULT_START - RESULT_HEIGHT) {
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
			if(enter_touch_y_pos<0)  //record first touch y pos
				enter_touch_y_pos = ev->touch_event.y;
			last_index_y_pos = ev->touch_event.y;
			start_search_time = ev->touch_event.ticks;
			last_delete_time = start_search_time;
			if (key) {
				if(key->key==8)//press "<" button
				{
					press_delete_button = true;
				}
				else if(key->key == -42)
				{
					mode = keyboard_get_mode();
					if(mode == KEYBOARD_CHAR)
						keyboard_set_mode(KEYBOARD_NUM);
					else if(mode == KEYBOARD_NUM)
						keyboard_set_mode(KEYBOARD_CHAR);
					guilib_fb_lock();
					keyboard_paint();
					guilib_fb_unlock();
				}

				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_keyboard = 1;

				if (pre_key && pre_key->key == key->key) goto out;

				if (touch_down_on_keyboard) {
					keyboard_key_invert(key);
					pre_key = key;
				}
			} else {
				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_list = 1;
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
				pre_key = NULL;

				if (!search_result_count()) goto out;

				if(touch_search == 0)
				{
					//last_search_y_pos = ev->touch_event.y;
					touch_search = 1;
				}
				else
				{
					if(search_result_selected()>=0 && abs(ev->touch_event.y-search_touch_pos_y_last)>5)
					{
						invert_selection(search_result_selected(),-1, RESULT_START, RESULT_HEIGHT);
						search_set_selection(-1);
					}
					goto out;
				}

				int new_selection;
				if((ev->touch_event.y - RESULT_START)<0)
					new_selection = -1;
				else
					new_selection = ((unsigned int)ev->touch_event.y - RESULT_START) / RESULT_HEIGHT;

				if (new_selection == search_result_selected()) goto out;

				unsigned int avail_count = keyboard_get_mode() == KEYBOARD_NONE ?
					NUMBER_OF_FIRST_PAGE_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
				avail_count = search_result_count() > avail_count ? avail_count : search_result_count();
				if (new_selection >= avail_count) goto out;
				if (touch_down_on_keyboard) goto out;

				//invert_selection(search_result_selected(), new_selection, RESULT_START, RESULT_HEIGHT);
				invert_selection(-1, new_selection, RESULT_START, RESULT_HEIGHT);

				last_selection = new_selection ;
				search_set_selection(new_selection);
				search_touch_pos_y_last = ev->touch_event.y;
			}
		}
	}
	else if (display_mode == DISPLAY_MODE_HISTORY && mode == KEYBOARD_CLEAR_HISTORY)
	{
		key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
		if (ev->touch_event.value == 0) {
#ifdef INCLUDED_FROM_KERNEL
			delay_us(100000 * 2);
#endif
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW);
			enter_touch_y_pos_record = enter_touch_y_pos;
			enter_touch_y_pos = -1;
			touch_search = 0;
			press_delete_button = false;
			pre_key = NULL;
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}
				if (key->key == 'Y') {
					history_clear();
					keyboard_set_mode(KEYBOARD_NONE);
					history_reload();
				} else if (key->key == 'N') {
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
				enter_touch_y_pos = ev->touch_event.y;
			last_index_y_pos = ev->touch_event.y;
			if (key) {
				if (!touch_down_on_keyboard)
					touch_down_on_keyboard = 1;

				if (pre_key && pre_key->key == key->key) goto out;

				if (touch_down_on_keyboard) {
					keyboard_key_invert(key);
					pre_key = key;
				}
			} else {
				touch_down_on_keyboard = 0;
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
				pre_key = NULL;

			}
		}
	}
	else if (display_mode == DISPLAY_MODE_RESTRICTED)
	{
		key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
		if (ev->touch_event.value == 0) {
			if (key->key == 'Y' || key->key == 'N' || key->key == 'P')
			{
#ifdef INCLUDED_FROM_KERNEL
				delay_us(100000 * 2);
#endif
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW);
			}
			else
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_DELAY); // reset invert with delay
			enter_touch_y_pos_record = enter_touch_y_pos;
			enter_touch_y_pos = -1;
			touch_search = 0;
			press_delete_button = false;
			pre_key = NULL;
			if (key) {
				if (!touch_down_on_keyboard) {
					touch_down_on_keyboard = 0;
					goto out;
				}
				handle_password_key(key->key);
			}
			touch_down_on_keyboard = 0;
		} else {
			if(enter_touch_y_pos<0)  //record first touch y pos
				enter_touch_y_pos = ev->touch_event.y;
			last_index_y_pos = ev->touch_event.y;
			start_search_time = ev->touch_event.ticks;
			last_delete_time = start_search_time;
			if (key) {
				if(key->key==8)//press "<" button
				{
					press_delete_button = true;
				}
				else if(key->key == -42)
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

				if (!touch_down_on_keyboard)
					touch_down_on_keyboard = 1;

				if (pre_key && pre_key->key == key->key) goto out;

				if (touch_down_on_keyboard) {
					keyboard_key_invert(key);
					pre_key = key;
				}
			} else {
				keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW);
				pre_key = NULL;

				search_touch_pos_y_last = ev->touch_event.y;
			}
		}
	} else {
		if (ev->touch_event.value == 0) {
			unsigned long diff_ticks = 0;
			long diff_y = 0;

			for (i = 4; i > 0; i--)
			{
				if (last_5_y[i])
				{
					diff_y =  last_5_y[i] - ev->touch_event.y;
					diff_ticks = time_diff(ev->touch_event.ticks, last_5_y_time_ticks[i]);
					break;
				}
			}

			if (diff_ticks <= 0 || abs(article_moved_pixels) > SMOOTH_SCROLL_ACTIVATION_OFFSET_HIGH_THRESHOLD ||
			    abs(article_moved_pixels) < SMOOTH_SCROLL_ACTIVATION_OFFSET_LOW_THRESHOLD)
				finger_move_speed = 0;
			else
			{
				// Event timesing is not good for short period due to the events are queued without timestamp
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

			if (finger_move_speed != 0)
			{
				time_scroll_article_last = ev->touch_event.ticks;
			}

			article_moved = false;
			if (finger_move_speed == 0 && b_show_scroll_bar)
			{
				b_show_scroll_bar = 0;
				show_scroll_bar(0); // clear scroll bar
			}
			article_scroll_pixel = INITIAL_ARTICLE_SCROLL_PIXEL;
			article_moved_pixels = 0;
			touch_y_last_unreleased = 0;
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
					display_mode = DISPLAY_MODE_ARTICLE;
					open_article_link_with_link_number(article_link_number);
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
			finger_move_speed = 0;

			if(touch_y_last_unreleased == 0)
			{
				touch_y_last_unreleased = ev->touch_event.y;
				last_unreleased_time = ev->touch_event.ticks;
				reset_article_link_number();
				article_moved_pixels = 0;
				last_5_y[0] = ev->touch_event.y;
				last_5_y_time_ticks[0] = ev->touch_event.ticks;
				for (i = 1; i < 5; i++)
					last_5_y[i] = 0;

			}
			else
			{
				article_moved_pixels += touch_y_last_unreleased - ev->touch_event.y;
				if(abs(touch_y_last_unreleased - ev->touch_event.y) >=article_scroll_pixel)
				{
					if (finger_move_speed == 0)
					{
						display_article_with_pcf(touch_y_last_unreleased - ev->touch_event.y);
					}
					touch_y_last_unreleased = ev->touch_event.y;
					for (i = 4; i >= 1; i--)
					{
						last_5_y[i] = last_5_y[i-1];
						last_5_y_time_ticks[i] = last_5_y_time_ticks[i-1];
					}
					last_5_y[0] = ev->touch_event.y;
					last_5_y_time_ticks[0] = ev->touch_event.ticks;
					b_show_scroll_bar = 1;
					article_scroll_pixel = 1;
				}
				else if (article_moved_pixels < article_scroll_pixel &&
					 time_diff(get_time_ticks(), last_unreleased_time) > seconds_to_ticks(0.075))
				{
					article_moved_pixels = 0;
					touch_y_last_unreleased = ev->touch_event.y;
					last_unreleased_time = ev->touch_event.ticks;
					last_5_y[0] = ev->touch_event.y;
					last_5_y_time_ticks[0] = ev->touch_event.ticks;
					for (i = 1; i < 5; i++)
						last_5_y[i] = 0;
				}

				if (abs(article_moved_pixels) > ARTICLE_MOVED_THRESHOLD)
				{
					article_moved = true;
					reset_article_link_number();
				}
			}

			if (!article_moved)
			{
				article_link_number =isArticleLinkSelected(ev->touch_event.x,ev->touch_event.y);
				if (article_link_number >= 0)
					set_article_link_number(article_link_number, ev->touch_event.ticks);
				else
					reset_article_link_number();
			}

			if (!article_touch_down_handled) {
				article_touch_down_pos.x = ev->touch_event.x;
				article_touch_down_pos.y = ev->touch_event.y;
				article_touch_down_handled = 1;
			}
		}
	}
out:
	return;
}

int wikilib_init (void)
{
	// tbd: name more than 8.3 could not be found on the device (fatfs)
	//g_womh = wom_open("wiki.dat");
	init_lcd_draw_buf();
	init_file_buffer();
	return 0;
}

#ifndef INCLUDED_FROM_KERNEL
extern long idx_init_article;
#endif

int wikilib_run(void)
{
	int sleep;
	long time_now;
	struct wl_input_event ev;
	int more_events = 0;
	unsigned long last_event_time = 0;
	int rc;


	/*
	 * test searching code...
	 */
	article_buf_pointer = NULL;
	search_init();
	history_list_init();
	malloc_status_simple();
	print_intro();
#ifndef INCLUDED_FROM_KERNEL
	if (!load_init_article(idx_init_article))
	{
		display_mode = DISPLAY_MODE_ARTICLE;
		last_display_mode = DISPLAY_MODE_INDEX;
	}
#endif
	render_string(SUBTITLE_FONT_IDX, -1, 55, MESSAGE_TYPE_A_WORD, strlen(MESSAGE_TYPE_A_WORD), 0);

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
		if (finger_move_speed != 0)
		{
			scroll_article();
			sleep = 0;
		}

#ifdef INCLUDED_FROM_KERNEL
		time_now = get_time_ticks();
		if(display_mode == DISPLAY_MODE_INDEX)
		{
			if (press_delete_button && get_search_string_len()>0)
			{
				sleep = 0;
				if(time_diff(time_now, start_search_time) > seconds_to_ticks(3.5))
				{
					if (!clear_search_string())
					{
						search_string_changed_remove = true;
						search_reload_ex(SEARCH_RELOAD_NORMAL);
					}
					press_delete_button = false;
				}
				else if (time_diff(time_now, start_search_time) > seconds_to_ticks(0.5) &&
					 time_diff(time_now, last_delete_time) > seconds_to_ticks(0.25))
				{
					if (!search_remove_char(0))
					{
						search_string_changed_remove = true;
						search_reload_ex(SEARCH_RELOAD_NO_POPULATE);
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
				time_now = get_time_ticks();
				if(time_diff(time_now, start_search_time) > seconds_to_ticks(3.5))
				{
					clear_password_string();
					press_delete_button = false;
				}
				else if (time_diff(time_now, start_search_time) > seconds_to_ticks(0.5) &&
					 time_diff(time_now, last_delete_time) > seconds_to_ticks(0.25))
				{
					password_remove_char();
					last_delete_time = time_now;
				}
			}
		}

#endif

		if (!more_events && display_mode == DISPLAY_MODE_INDEX && fetch_search_result(0, 0, 0))
		{
			sleep = 0;
		}

		if (keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_CHECK)) // check if need to reset invert
			sleep = 0;

		if (check_invert_link()) // check if need to invert link
			sleep = 0;

		if (sleep)
		{
			if (time_diff(get_time_ticks(), last_event_time) > seconds_to_ticks(15))
				rc = history_list_save(HISTORY_SAVE_POWER_OFF);
			else
				rc = history_list_save(HISTORY_SAVE_NORMAL);
			if (rc > 0)
			{
#ifdef INCLUDED_FROM_KERNEL
				delay_us(200000); // for some reason, save may not work if no delay
#endif
			}
			else if (rc < 0)
				sleep = 0; // waiting for last_event_time timeout to save the history
		}

		wl_input_wait(&ev, sleep);
		more_events = 1;
		switch (ev.type) {
		case WL_INPUT_EV_TYPE_CURSOR:
			handle_cursor(&ev);
			last_event_time = get_time_ticks();
			break;
		case WL_INPUT_EV_TYPE_KEYBOARD:
			if (ev.key_event.value != 0)
			{
				b_show_scroll_bar = 0;
				handle_key_release(ev.key_event.keycode);
			}
			last_event_time = get_time_ticks();
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			handle_touch(&ev);
			last_event_time = ev.touch_event.ticks;
			break;
		default:
			more_events = 0;
			break;
		}
	}

	/* never reached */
	return 0;
}

unsigned long get_time_ticks(void)
{
	long clock_ticks;

#ifdef INCLUDED_FROM_KERNEL
	clock_ticks = Tick_get();
#else
	clock_ticks = clock();
#endif

	return clock_ticks;
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

unsigned long seconds_to_ticks(float sec)
{
	long clock_ticks;

#ifdef INCLUDED_FROM_KERNEL
	clock_ticks = sec * 24000000;
#else
	clock_ticks = sec * CLOCKS_PER_SEC;
#endif

	return clock_ticks;
}
