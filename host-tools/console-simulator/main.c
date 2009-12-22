/*
 * ncurses based simulator
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include <wikilib.h>
#include <guilib.h>
#include <msg.h>
#include <input.h>

static int loglevel = MSG_LEVEL_MAX;

/* empty dummies - no framebuffer here */
void fb_set_pixel(int x, int y, int val) {}
void fb_refresh(void) {}
void fb_clear(void) {}

unsigned char *framebuffer = NULL;

int wl_input_wait(struct wl_input_event *ev, int sleep)
{
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = getch();
	ev->key_event.value = 1;
	return 0;
}

void set_loglevel(int level)
{
	loglevel = level;
}

void msg(int level, const char *format, ...)
{
	va_list ap;
	int attrs;

	if (level > loglevel)
		return;

	switch (level) {
	case MSG_DEBUG:
		attrs = COLOR_PAIR(3);
		break;
	case MSG_ERROR:
		attrs = COLOR_PAIR(2);
		break;
	default:
		attrs = COLOR_PAIR(1);
		break;
	}

	va_start(ap, format);
	attron(attrs);
	vwprintw(stdscr, format, ap);
	attroff(attrs);
	va_end(ap);
}

int main(int argc, char *argv[])
{
	framebuffer = (unsigned char *)malloc(guilib_framebuffer_size());

	initscr();
	keypad(stdscr, TRUE);
	noecho();
	cbreak();
	scrollok(stdscr, TRUE);
	idlok(stdscr, TRUE);

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);

	wikilib_init();
	guilib_init();
	wikilib_run();

	/* never reached */
	return 0;
}

