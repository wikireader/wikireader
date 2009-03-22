/*
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
 * Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
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
#include <wikilib.h>
#include <article.h>
#include <guilib.h>
#include <glyph.h>
#include <fontfile.h>
#include <history.h>
#include <keyboard.h>
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>
#include <search.h>
#include <string.h>
#include <ctype.h>

#define DISPLAY_MODE_INDEX	0
#define DISPLAY_MODE_ARTICLE	1
#define DISPLAY_MODE_HISTORY	2
#define DISPLAY_MODE_IMAGE	3

static int current_page = 0;
static int display_mode = DISPLAY_MODE_INDEX;

/* this is only called for the index page */
static void toggle_soft_keyboard(void)
{
	keyboard_set_visible(!keyboard_is_visible());

	/* TODO: This can be optimized for showing the keyboard */
	search_reload();
	keyboard_paint();
}

static void print_intro()
{
	guilib_fb_lock();
	guilib_clear();
	render_string(0, 60, 104, "Type any letter to search", 25);
	guilib_fb_unlock();
}

static void print_article_error()
{
	guilib_fb_lock();
	guilib_clear();
	render_string(0, 60, 104, "Opening the article failed.", 27);
	guilib_fb_unlock();
}

static void display_image()
{
	int x, y, new_lines = 0;
	char data;
	int fd = wl_open("image.pbm", WL_O_RDONLY);

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
}

static void handle_search_key(char keycode)
{
	if (keycode == KEY_BACKSPACE) {
		search_remove_char();
	} else if (isalnum(keycode) || isspace(keycode)) {
		msg(MSG_INFO, "Adding to search : '%c'\n", keycode);
		search_add(tolower(keycode));
	} else {
		msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
		return;
	}

	search_display_results();
	keyboard_paint();
}

static void handle_cursor(struct wl_input_event *ev)
{
	if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
			article_display(++current_page);
		else if (ev->key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
			article_display(--current_page);
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

static void handle_key(int keycode)
{
	if (keycode == WL_INPUT_KEY_SEARCH) {
		/* back to search */
		if (display_mode == DISPLAY_MODE_INDEX) {
			toggle_soft_keyboard();
		} else {
			display_mode = DISPLAY_MODE_INDEX;
			search_reload();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
		display_mode = DISPLAY_MODE_HISTORY;
		history_reset();
		history_display();
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		if (keycode == KEY_RETURN) {
			const char *target = search_current_target();
			if (target) {
				if (article_open(target) < 0)
					print_article_error();
				display_mode = DISPLAY_MODE_ARTICLE;
				current_page = 0;
				article_display(0);
				history_add(search_current_title(), target);
			}
		} else if (keycode == KEY_HASH) {
			display_mode = DISPLAY_MODE_IMAGE;
			display_image();
		} else {
			handle_search_key(keycode);
		}
	} else if (display_mode == DISPLAY_MODE_ARTICLE) {
		if (keycode == KEY_BACKSPACE) {
			display_mode = DISPLAY_MODE_INDEX;
			search_reload();
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		if (keycode == KEY_RETURN) {
			const char *target = history_current_target();
			if (target) {
				if (article_open(target) < 0)
					print_article_error();
				display_mode = DISPLAY_MODE_ARTICLE;
				current_page = 0;
				article_display(0);
				history_move_current_to_top();
			}
		}
	}
}

static void handle_touch(struct wl_input_event *ev)
{
	msg(MSG_INFO, "%s() touch event @%d,%d val %d\n", __func__,
		ev->touch_event.x,
		ev->touch_event.y,
		ev->touch_event.value);

	if (display_mode == DISPLAY_MODE_INDEX) {
		if (ev->touch_event.value == 0) {
			/*
			 * TODO: XXX: This could return the char and then we add it to
			 * the search.
			 */
			keyboard_release(ev->touch_event.x, ev->touch_event.y);
		}
	}
}

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{

	print_intro();

	/*
	 * test searching code...
	 */
	search_init();

	for (;;) {
		struct wl_input_event ev;
		int sleep = search_load_trigram();
		wl_input_wait(&ev, sleep);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_CURSOR:
			handle_cursor(&ev);
			break;
		case WL_INPUT_EV_TYPE_KEYBOARD:
			handle_key(ev.key_event.keycode);
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			handle_touch(&ev);
			break;
		}
	}

	/* never reached */
	return 0;
}

