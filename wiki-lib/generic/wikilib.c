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

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	int current_page = 0;
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
		case WL_INPUT_EV_TYPE_CURSOR:
			if (ev.key_event.keycode == WL_INPUT_KEY_CURSOR_DOWN)
				article_display(++current_page);
			else if (ev.key_event.keycode == WL_INPUT_KEY_CURSOR_UP)
				article_display(--current_page);
			break;
		case WL_INPUT_EV_TYPE_KEYBOARD:
			handle_search_key(ev.key_event.keycode);
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

