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
#include <guilib.h>
#include <glyph.h>
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>
#include <search.h>
#include <string.h>

static void handle_search_key(char keycode)
{
    char *result;
    int j = 0, y_pos = 10;

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
	    y_pos += 2 + render_string(0, "Search results:", 1, y_pos);
	    found = 1;
	}

	y_pos += 2 + render_string(0, result, 1, y_pos);
    }

    if (!found) {
	guilib_clear();
	y_pos += 2 + render_string(0, "Search results:", 1, y_pos);
    }

    search_print_stats();
    guilib_fb_unlock();
}

#define DISPLAY_PAGE 23

static unsigned char buf[512];
static unsigned int *buf_ptr;
static int available = 0;
static void display()
{
	int fd = wl_open("/smplpedi.cde", WL_O_RDONLY);
	int font, x, y, glyph, len, i;
	int run = 0, r_len;

#define READ_UINT(var, fd) \
	if (available == 0) { \
	    available = wl_read(fd, buf, 512); \
	    buf_ptr = (unsigned int*)&buf[0]; \
	    if (available < 4) break; \
	} \
	var = *buf_ptr; \
	buf_ptr++; \
	available -= 4; \

	do {
		READ_UINT(font, fd)
		READ_UINT(len, fd)

		for (i = 0; i < len; ++i) {
			READ_UINT(x, fd)
			READ_UINT(y, fd)
			READ_UINT(glyph, fd)

			if (y < DISPLAY_PAGE * FRAMEBUFFER_HEIGHT)
				continue;
			if (y > (DISPLAY_PAGE+1) * FRAMEBUFFER_HEIGHT)
				break;

			if (font >= guilib_nr_fonts())
				continue;
			render_glyph(x % FRAMEBUFFER_WIDTH, y % FRAMEBUFFER_HEIGHT, get_glyph(font, glyph));
		}
	} while(1);
}

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	void *a;
	int fd, i;

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

	display();

	for (;;) {
		struct wl_input_event ev;
		wl_input_wait(&ev);

		switch (ev.type) {
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

