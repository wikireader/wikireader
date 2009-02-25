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

static const char search_result[] = "Search results:";
static const int search_result_len = 15;

static void handle_search_key(char keycode)
{
    char *result;
    int y_pos = 10;

    if (keycode == 8) {
	search_remove_char();
    } else if (isalnum(keycode) || isspace(keycode)) {
	msg(MSG_INFO, "Adding to search : '%c'\n", keycode);
	search_add(tolower(keycode));
    } else {
	msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
	return;
    }


    /* paint the results */
    guilib_fb_lock();
    int found = 0;

    while (y_pos < FRAMEBUFFER_HEIGHT && (result = search_fetch_result())) {
	if (!found) {
	    guilib_clear();
	    y_pos += 2 + render_string(0, 1, y_pos, search_result, search_result_len);
	    found = 1;
	}

	y_pos += 2 + render_string(0, 1, y_pos, result, strlen(result) - 7);
    }

    if (!found) {
	guilib_clear();
	y_pos += 2 + render_string(0, 1, y_pos, search_result, search_result_len);
    }

    search_print_stats();
    guilib_fb_unlock();
}

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	void *a;
	int fd, i, current_page = 0;
	char got_escape = 0;
	char last_key = 0;

	a = malloc(512);
	msg(MSG_INFO, " a = %p\n", a);
	
	msg(MSG_INFO, " 1. run\n");
	fd = wl_open("/kernel", WL_O_RDONLY);
	msg(MSG_INFO, " fd = %d\n", fd);

	wl_read(fd, a, 512);
	wl_close(fd);

	for (i = 0; i < 10; i++) {
		msg(MSG_INFO, " 2. run\n");
		fd = wl_open("/kernel", WL_O_RDONLY);
		wl_read(fd, a, 512);
		wl_read(fd, a, 512);
		wl_read(fd, a, 512);
		wl_close(fd);
	}

//	dump_cache_stats();

	/*
	 * test searching code...
	 */
	search_init();

	article_open("/smplpedi.cde");
	article_display(current_page);

	for (;;) {
		struct wl_input_event ev;
		int sleep = search_load_trigram();
		wl_input_wait(&ev, sleep);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_KEYBOARD:

			if (got_escape) {
				if (last_key == 0) {
					last_key = ev.key_event.keycode;
				} else if (last_key == 91) {
					if (ev.key_event.keycode == 66) {
						article_display(++current_page);
					} else if (ev.key_event.keycode == 65) {
						article_display(--current_page);
					}

					last_key = 0;
					got_escape = 0;
				} else {
					last_key = 0;
					got_escape = 0;
				}
			} else if (ev.key_event.keycode == 27) {
				got_escape = 1;
			} else {
				handle_search_key(ev.key_event.keycode);
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

