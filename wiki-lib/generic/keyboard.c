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

#include "keyboard.h"

#include "guilib.h"
#include "keyboard_image.h"
#include "input.h"
#include "wikilib.h"

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
	KEY(0, 130, 20, 154, 'q'),
	KEY(0, 156, 20, 180, 'a'),
	KEY(0, 182, 20, 208, 'z'),

	KEY(22, 130, 42, 154, 'w'),
	KEY(22, 156, 42, 180, 's'),
	KEY(22, 182, 42, 208, 'x'),

	KEY(44, 130, 64, 154, 'e'),
	KEY(44, 156, 64, 180, 'd'),
	KEY(44, 182, 64, 208, 'c'),

	KEY(66, 130, 86, 154, 'r'),
	KEY(66, 156, 86, 180, 'f'),
	KEY(66, 182, 86, 208, 'v'),

	KEY(88, 130, 108, 154, 't'),
	KEY(88, 156, 108, 180, 'g'),
	KEY(88, 182, 130, 208, ' '),

	KEY(110, 130, 130, 154, 'y'),
	KEY(110, 156, 130, 180, 'h'),

	KEY(132, 130, 152, 154, 'u'),
	KEY(132, 156, 152, 180, 'j'),
	KEY(132, 182, 152, 208, 'b'),

	KEY(154, 130, 174, 154, 'i'),
	KEY(154, 156, 174, 180, 'k'),
	KEY(154, 182, 174, 208, 'n'),

	KEY(176, 130, 196, 154, 'o'),
	KEY(174, 156, 196, 180, 'l'),
	KEY(174, 182, 196, 208, 'm'),

	KEY(198, 130, 218, 154, 'p'),

	KEY(220, 130, 240, 154, WL_KEY_BACKSPACE),
	KEY(198, 156, 240, 180, INTERNAL_SHIFT),
	KEY(198, 182, 240, 208, INTERNAL_NUMBER),
};

/*
 * The secret of the position and size of the keyboard
 * is shared between search.c and this file.
 */

static keyboard_mode_e kb_mode = KEYBOARD_CHAR;

void keyboard_set_mode(keyboard_mode_e mode)
{
	kb_mode = mode;
}

keyboard_mode_e keyboard_get_mode()
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
	guilib_blit_image(&image_data, 0, FRAMEBUFFER_HEIGHT - image_data.height);
	guilib_fb_unlock();
}

/**
 * Coordinates are screen absolute ones
 */
char keyboard_release(int x, int y)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(qwerty); ++i) {
		if (qwerty[i].left_x <= x && qwerty[i].right_x >= x
		    && qwerty[i].left_y <= y && qwerty[i].right_y >= y)
			return qwerty[i].key;
	}

	return -1;
}
