/*
    Wikipedia reader tools
    ncurses based simulator

    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>
    Copyright (c) 2009 Matt Hsu <matt_hsu@openmoko.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>

#include <wikilib.h>
#include <guilib.h>
#include <msg.h>
#include <input.h>

/* empty dummies - no framebuffer here */
void fb_set_pixel(int x, int y, int val) {}
void fb_refresh(void) {}
void fb_clear(void) {}

static unsigned char framebuffer_data[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];
unsigned char *framebuffer = &framebuffer_data[0];

int wl_input_wait(struct wl_input_event *ev, int sleep)
{
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = getc(stdin);
	ev->key_event.value = 1;
	return 0;
}

int main(int argc, char *argv[])
{
	wikilib_init();
	guilib_init();
	wikilib_run();

	/* never reached */
	return 0;
}

