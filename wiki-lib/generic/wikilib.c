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
#include <wl-keyboard.h>
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>
#include <search.h>
#include <string.h>
#include <ctype.h>

enum display_mode_e {

	DISPLAY_MODE_INDEX,
	DISPLAY_MODE_ARTICLE,
	DISPLAY_MODE_HISTORY,
	DISPLAY_MODE_IMAGE,
};

#define ARTICLE_NEW		0
#define ARTICLE_HISTORY		1
#define DISPLAY_IMAGE_FILE	"image.pbm"

static int current_page = 0;
static int last_display_mode = 0;
static int display_mode = DISPLAY_MODE_INDEX;

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
	// Set the keyboard mode to what we want to change to.
	if (keyboard_get_mode() == KEYBOARD_NONE)
		keyboard_set_mode(KEYBOARD_CHAR);
	else
		keyboard_set_mode(KEYBOARD_NONE);

	/* TODO: This can be optimized for showing the keyboard */
	repaint_search();
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

static int display_image()
{
	int x, y, new_lines = 0;
	char data;
	int fd = wl_open(DISPLAY_IMAGE_FILE, WL_O_RDONLY);

	if (fd < 0) {
		msg(MSG_INFO, "Could not display file '%s': file not found\n", DISPLAY_IMAGE_FILE);
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

static void open_article(const char* target, int mode)
{
	if (!target)
		return;

	if (article_open(target) < 0)
		print_article_error();
	display_mode = DISPLAY_MODE_ARTICLE;
	current_page = 0;
	article_display(0);

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
		search_add(tolower(keycode));
	} else {
		msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
		return;
	}

	guilib_fb_lock();
	search_display_results();
	keyboard_paint();
	guilib_fb_unlock();
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
			repaint_search();
		}
	} else if (keycode == WL_INPUT_KEY_HISTORY) {
		display_mode = DISPLAY_MODE_HISTORY;
		history_reset();
		history_display();
	} else if (display_mode == DISPLAY_MODE_INDEX) {
		if (keycode == WL_KEY_RETURN) {
			open_article(search_current_target(), ARTICLE_NEW);
		} else if (keycode == WL_KEY_HASH) {
			if (display_image() == 1)
				display_mode = DISPLAY_MODE_IMAGE;
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
	msg(MSG_INFO, "%s() touch event @%d,%d val %d\n", __func__,
		ev->touch_event.x,
		ev->touch_event.y,
		ev->touch_event.value);

	if (display_mode == DISPLAY_MODE_INDEX) {
		if (ev->touch_event.value == 0) {
			char result = keyboard_release(ev->touch_event.x, ev->touch_event.y);
			if (result != -1)
				handle_search_key(result);
			else {
				const char *target = search_release(ev->touch_event.y);
				if (target)
					open_article(target, ARTICLE_NEW);
				else
					repaint_search();
			}
		}
	} else if (display_mode == DISPLAY_MODE_HISTORY) {
		const char *target = history_release(ev->touch_event.y);
		if (target)
			open_article(target, ARTICLE_NEW);
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

