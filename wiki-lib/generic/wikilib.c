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
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>
#include <search.h>
#include <string.h>
#include <ctype.h>

#define DISPLAY_MODE_INDEX      0
#define DISPLAY_MODE_ARTICLE    1

static int current_page = 0;

static void handle_search_key(char keycode)
{
    if (keycode == 8) {
	search_remove_char();
    } else if (isalnum(keycode) || isspace(keycode)) {
	msg(MSG_INFO, "Adding to search : '%c'\n", keycode);
	search_add(tolower(keycode));
    } else {
	msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
	return;
    }

    search_display_results();
}

static void handle_cursor(struct wl_input_event *ev, int display_mode)
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
	}
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

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	int display_mode = DISPLAY_MODE_INDEX;

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
			handle_cursor(&ev, display_mode);
			break;
		case WL_INPUT_EV_TYPE_KEYBOARD:
			if (display_mode == DISPLAY_MODE_INDEX) {
				if (ev.key_event.keycode == 13) {
					const char *result = search_current_result();
					if (result) {
						if (article_open(result) < 0)
							print_article_error();
						display_mode = DISPLAY_MODE_ARTICLE;
						current_page = 0;
						article_display(0);
					}
				} else {
					handle_search_key(ev.key_event.keycode);
				}
			} else if (display_mode == DISPLAY_MODE_ARTICLE) {
				if (ev.key_event.keycode == 8) {
					display_mode = DISPLAY_MODE_INDEX;
					search_reload();
				}
			}
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			msg(MSG_INFO, "%s() touch event @%d,%d val %d\n", __func__,
				ev.touch_event.x,
				ev.touch_event.y,
				ev.touch_event.value);
			break;
		}
	}

	/* never reached */
	return 0;
}

