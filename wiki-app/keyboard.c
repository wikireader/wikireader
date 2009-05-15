/*
 *  Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wl-keyboard.h"

#include "guilib.h"
#include "keyboard_image.h"
#include "input.h"
#include "wikilib.h"
#include "msg.h"

#define DBG_KEYBOARD 0

/* some control commands */
#define INTERNAL_SHIFT   (-23)
#define INTERNAL_NUMBER  (-42)

struct keyboard_key {
	/*
	 * a rect described by top left and
	 * bottom right point.
	 */
	int left_x, right_x;
	int left_y, right_y;
	char key;
};

/* qwerty keyboard by columns */
#define KEY(l_x, l_y, r_x, r_y, keycode) { .left_x = l_x, .right_x = r_x, .left_y = l_y, .right_y = r_y, .key = keycode, }
static struct keyboard_key qwerty[] = {
	KEY(0, 128, 22, 153, 'q'),
	KEY(0, 155, 22, 180, 'a'),
	KEY(0, 182, 22, 207, 'z'),

	KEY(24, 128, 46, 153, 'w'),
	KEY(24, 155, 46, 180, 's'),
	KEY(24, 182, 46, 207, 'x'),

	KEY(48, 128, 70, 153, 'e'),
	KEY(48, 155, 70, 180, 'd'),
	KEY(48, 182, 70, 207, 'c'),

	KEY(72, 128, 94, 153, 'r'),
	KEY(72, 155, 94, 180, 'f'),
	KEY(72, 182, 94, 208, 'v'),

	KEY(96, 128, 118, 153, 't'),
	KEY(96, 155, 118, 180, 'g'),
	KEY(96, 182, 142, 207, ' '),

	KEY(120, 128, 142, 153, 'y'),
	KEY(120, 155, 142, 180, 'h'),

	KEY(144, 128, 166, 153, 'u'),
	KEY(144, 155, 166, 180, 'j'),
	KEY(144, 182, 166, 207, 'b'),

	KEY(168, 128, 190, 153, 'i'),
	KEY(168, 155, 190, 180, 'k'),
	KEY(168, 182, 190, 207, 'n'),

	KEY(192, 128, 214, 153, 'o'),
	KEY(192, 155, 214, 180, 'l'),
	KEY(192, 182, 214, 207, 'm'),

	KEY(216, 128, 239, 153, 'p'),
	KEY(216, 155, 239, 180, WL_KEY_BACKSPACE),
	KEY(216, 182, 239, 207, INTERNAL_NUMBER),
};

/*
 * The secret of the position and size of the keyboard
 * is shared between search.c and this file.
 */

static int kb_mode = KEYBOARD_CHAR;

void keyboard_set_mode(int mode)
{
	kb_mode = mode;
}

int keyboard_get_mode()
{
	return kb_mode;
}

/**
 *
 */
void keyboard_paint()
{
	if (kb_mode == KEYBOARD_NONE)
		return;
	guilib_fb_lock();
	guilib_blit_image(&image_data, 0, guilib_framebuffer_height() - image_data.height);
	guilib_fb_unlock();
}

unsigned int keyboard_height()
{
	return image_data.height;
}

/**
 * Coordinates are screen absolute ones
 */
char keyboard_release(int x, int y)
{
	unsigned int i;

	if (kb_mode == KEYBOARD_CHAR) {
		for (i = 0; i < ARRAY_SIZE(qwerty); ++i) {
			if (qwerty[i].left_x <= x && qwerty[i].right_x >= x
			&& qwerty[i].left_y <= y && qwerty[i].right_y >= y) {
				DP(DBG_KEYBOARD, ("O Entered '%c'\n", qwerty[i].key));
				return qwerty[i].key;
			}
		}
	}

	return -1;
}
