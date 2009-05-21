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
#include <article.h>
#include <guilib.h>
#include <glyph.h>
#include <fontfile.h>
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
#include "wom_reader.h"

#define DBG_WL 0

enum display_mode_e {

	DISPLAY_MODE_INDEX,
	DISPLAY_MODE_ARTICLE,
	DISPLAY_MODE_HISTORY,
	DISPLAY_MODE_IMAGE,
};

static int last_display_mode = 0;
static int display_mode = DISPLAY_MODE_INDEX;
static struct keyboard_key * pre_key= NULL;
static unsigned int article_touch_y_pos = 0;
static unsigned int article_touch_down_handled = 0;
static unsigned int touch_down_on_keyboard = 0;
static unsigned int touch_down_on_list = 0;
wom_file_t * g_womh = 0;

static void repaint_search(void)
{
	guilib_fb_lock();
	search_reload();
	keyboard_paint();
	guilib_fb_unlock();
}

/* this is only called for the index page */
static void toggle_soft_keyboard(void)
{
	guilib_fb_lock();

	/* Set the keyboard mode to what we want to change to. */
	if (keyboard_get_mode() == KEYBOARD_NONE) {
		keyboard_set_mode(KEYBOARD_CHAR);
		search_reload();
		keyboard_paint();
	} else {
		keyboard_set_mode(KEYBOARD_NONE);
		search_reload();
	}

	guilib_fb_unlock();
}

static void print_intro()
{
	guilib_fb_lock();
	guilib_clear();
	render_string(0, 73, 70, "Type a word or phrase", 21);
	keyboard_paint();
	guilib_fb_unlock();
}

static void print_article_error()
{
	guilib_fb_lock();
	guilib_clear();
	render_string(0, 60, 104, "Opening the article failed.", 27);
	guilib_fb_unlock();
}

#ifdef ENABLE_IMAGE_DISPLAY
static int display_image()
{
	static const char *image_file = "image.pbm";
	int x, y, new_lines = 0;
	char data;
	int fd = wl_open(image_file, WL_O_RDONLY);

	if (fd < 0) {
		msg(MSG_INFO, "Could not display file '%s': file not found\n", image_file);
		return -1;
	}

	/* eat the header... three lines */
	while (new_lines < 3) {
		wl_read(fd, &data, 1);
		if (data == '\n')
		    ++new_lines;
	}

	guilib_fb_lock();
	for (y = 0; y < FRAMEBUFFER_HEIGHT; ++y) {
		for (x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
			while (1) {
				wl_read(fd, &data, 1);
				if (data != '\n')
					break;
			}
			guilib_set_pixel(x, y, data == '1');
		}
	}
	guilib_fb_unlock();

	wl_close(fd);
	return 1;
}
#endif

static unsigned int s_article_y_pos;
static uint32_t s_article_offset = 0;

int article_open(const char *article)
{
	DP(DBG_WL, ("O article_open() '%s'\n", article));
	s_article_offset = strtoul(article, 0 /* endptr */, 16 /* base */);
	s_article_y_pos = 0;
	return 0;
}

void article_display(enum article_nav nav)
{
	unsigned int screen_height = guilib_framebuffer_height();

	DP(DBG_WL, ("O article_display() %i article_offset %u article_y_pos %u\n", nav, s_article_offset, s_article_y_pos));
	if (nav == ARTICLE_PAGE_NEXT)
		s_article_y_pos += screen_height;
	else if (nav == ARTICLE_PAGE_PREV)
		s_article_y_pos = (s_article_y_pos <= screen_height) ? 0 : s_article_y_pos - screen_height;
	wom_draw(g_womh, s_article_offset, framebuffer, s_article_y_pos, screen_height);
}

void open_article(const char* target, int mode)
{
	DP(DBG_WL, ("O open_article() target '%s' mode %i\n", target, mode));
	if (!target)
		return;

	if (article_open(target) < 0)
		print_article_error();
	display_mode = DISPLAY_MODE_ARTICLE;
	article_display(ARTICLE_PAGE_0);

	if (mode == ARTICLE_NEW) {
		last_display_mode = DISPLAY_MODE_INDEX;
		history_add(search_current_title(), target);
	} else if (mode == ARTICLE_HISTORY) {
		last_display_mode = DISPLAY_MODE_HISTORY;
		history_move_current_to_top(target);
	}
}

static void handle_search_key(char keycode)
{
	if (keycode == WL_KEY_BACKSPACE) {
		search_remove_char();
	} else if (isalnum(keycode) || isspace(keycode)) {
		search_add_char(tolower(keycode));
	} else {
		msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
		return;
	}

	guilib_fb_lock();
	search_reload();
	keyboard_paint();
	guilib_fb_unlock();
}

static void handle_cursor(struct wl_input_event *ev)
{
	DP(DBG_WL, ("O handle_cursor()\n"));
	if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			article_display(ARTICLE_PAGE_NEXT);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			article_display(ARTICLE_PAGE_PREV);
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			search_select_down();
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			search_select_up();
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			history_select_down();
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			history_select_up();
	}
}

static void handle_key_release(int keycode)
{
	DP(DBG_WL, ("O handle_key_release()\n"));
	if (keycode == WL_INPUT_KEY_SEARCH) {
		/* back to search */
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
			display_mode = DISPLAY_MODE_INDEX;
			repaint_search();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
		display_mode = DISPLAY_MODE_HISTORY;
		history_reset();
		history_display();
	} else if (keycode == WL_INPUT_KEY_RANDOM) {
		/* msg(MSG_INFO, "random\n"); */
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		if (keycode == WL_KEY_RETURN) {
			const char* target = search_current_title();
			if (target && strlen(target) >= TARGET_SIZE)
				open_article(&target[strlen(target)-TARGET_SIZE], ARTICLE_NEW);
#if ENABLE_IMAGE_DISPLAY
		} else if (keycode == WL_KEY_HASH) {
			if (display_image() == 1)
				display_mode = DISPLAY_MODE_IMAGE;
#elif PROFILER_ON
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
		if (keycode == WL_KEY_BACKSPACE) {
			if (last_display_mode == DISPLAY_MODE_INDEX) {
				display_mode = DISPLAY_MODE_INDEX;
				repaint_search();
			} else if (last_display_mode == DISPLAY_MODE_HISTORY) {
				display_mode = DISPLAY_MODE_HISTORY;
				history_reset();
				history_display();
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (keycode == WL_KEY_RETURN) {
			open_article(history_current_target(), ARTICLE_HISTORY);
		}
	}
}

static void handle_touch(struct wl_input_event *ev)
{
	DP(DBG_WL, ("%s() touch event @%d,%d val %d\n", __func__,
		ev->touch_event.x, ev->touch_event.y, ev->touch_event.value));

	if (display_mode == DISPLAY_MODE_INDEX) {
		struct keyboard_key * key;

		key = keyboard_get_data(ev->touch_event.x, ev->touch_event.y);
		if (ev->touch_event.value == 0) {
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
				if (!touch_down_on_list) {
					touch_down_on_keyboard = 0;
					touch_down_on_list = 0;
					goto out;
				}

				const char *title= search_current_title();
				if (title && strlen(title) >= TARGET_SIZE)
					open_article(&title[strlen(title)-TARGET_SIZE], ARTICLE_NEW);
				else
					repaint_search();
			}
			touch_down_on_keyboard = 0;
			touch_down_on_list = 0;
		} else {
			if (key) {
				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_keyboard = 1;

				if (pre_key && pre_key->key == key->key) goto out;

				if (pre_key)
					guilib_invert_area(pre_key->left_x, pre_key->left_y, pre_key->right_x, pre_key->right_y);
				if (touch_down_on_keyboard) {
					guilib_invert_area(key->left_x, key->left_y, key->right_x, key->right_y);
					pre_key = key;
				}
			} else {
				if (!touch_down_on_keyboard && !touch_down_on_list)
					touch_down_on_list = 1;
				if (pre_key) {
					guilib_invert_area(pre_key->left_x, pre_key->left_y, pre_key->right_x, pre_key->right_y);
					pre_key = NULL;
				}

				if (!search_result_count()) goto out;

				unsigned int new_selection = ((unsigned int)ev->touch_event.y - RESULT_START) / RESULT_HEIGHT;
				if (new_selection == search_result_selected()) goto out;

				unsigned int avail_count = keyboard_get_mode() == KEYBOARD_NONE ? NUMBER_OF_RESULTS : NUMBER_OF_RESULTS_KEYBOARD;
				avail_count = search_result_count()-search_result_first_item() > avail_count ? avail_count : search_result_count()-search_result_first_item();
				if (new_selection >= avail_count) goto out;
				if (touch_down_on_keyboard) goto out;
				invert_selection(search_result_selected(), new_selection);
				search_set_selection(new_selection);
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		const char *target = history_release(ev->touch_event.y);
		if (target)
			open_article(target, ARTICLE_NEW);
	} else {
		if (ev->touch_event.value == 0) {
			if (article_touch_y_pos > ev->touch_event.y &&
					abs(article_touch_y_pos - ev->touch_event.y) > 20)
				article_display(ARTICLE_PAGE_NEXT);
			else if (article_touch_y_pos < ev->touch_event.y &&
					abs(article_touch_y_pos - ev->touch_event.y) > 20)
				article_display(ARTICLE_PAGE_PREV);
			article_touch_down_handled = 0;
		} else {
			if (!article_touch_down_handled) {
				article_touch_y_pos = ev->touch_event.y;
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
	g_womh = wom_open("wiki.dat");
	return 0;
}

int wikilib_run(void)
{

	print_intro();

	/*
	 * test searching code...
	 */
	search_init();
	history_list_init();

	for (;;) {
		struct wl_input_event ev;
		int sleep = search_load_trigram();
		wl_input_wait(&ev, sleep);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_CURSOR:
			handle_cursor(&ev);
			break;
		case WL_INPUT_EV_TYPE_KEYBOARD:
			if (ev.key_event.value != 0)
				handle_key_release(ev.key_event.keycode);
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			handle_touch(&ev);
			break;
		}
	}

	/* never reached */
	return 0;
}

