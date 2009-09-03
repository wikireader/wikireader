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
#include "keyboard_all_image.h"
#include "input.h"
#include "wikilib.h"
#include "msg.h"

#define DBG_KEYBOARD 0

/* some control commands */
#define INTERNAL_SHIFT   (-23)
#define INTERNAL_NUMBER  (-42)


static struct guilib_image *image_data;
int keyboard_type = 0;

/* qwerty keyboard by columns */
#define KEY(l_x, l_y, r_x, r_y, keycode) { .left_x = l_x, .right_x = r_x, .left_y = l_y, .right_y = r_y, .key = keycode, }
static struct keyboard_key qwerty_char[] = {
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

	KEY(96, 128, 119, 153, 't'),
	KEY(96, 155, 119, 180, 'g'),
	KEY(96, 182, 143, 207, ' '),

	KEY(121, 128, 143, 153, 'y'),
	KEY(121, 155, 143, 180, 'h'),

	KEY(145, 128, 167, 153, 'u'),
	KEY(145, 155, 167, 180, 'j'),
	KEY(145, 182, 167, 207, 'b'),

	KEY(169, 128, 191, 153, 'i'),
	KEY(169, 155, 191, 180, 'k'),
	KEY(169, 182, 191, 207, 'n'),

	KEY(193, 128, 215, 153, 'o'),
	KEY(193, 155, 215, 180, 'l'),
	KEY(193, 182, 215, 207, 'm'),

	KEY(217, 128, 239, 153, 'p'),
	KEY(217, 155, 239, 180, WL_KEY_BACKSPACE),
	KEY(217, 182, 239, 207, INTERNAL_NUMBER),
};
static struct keyboard_key qwerty_num[] = {
	KEY(0, 128, 22, 153, '1'),
	KEY(0, 155, 22, 180, '*'),
	KEY(0, 182, 22, 207, '@'),

	KEY(24, 128, 46, 153, '2'),
	KEY(24, 155, 46, 180, '$'),
	KEY(24, 182, 46, 207, '?'),

	KEY(48, 128, 70, 153, '3'),
	KEY(48, 155, 70, 180, '%'),
	KEY(48, 182, 70, 207, '!'),

	KEY(72, 128, 94, 153, '4'),
	KEY(72, 155, 94, 180, '#'),
	KEY(72, 182, 94, 208, '&'),

	KEY(96, 128, 119, 153, '5'),
	KEY(96, 155, 119, 180, '('),
	KEY(96, 182, 143, 207, ' '),

	KEY(121, 128, 143, 153, '6'),
	KEY(121, 155, 143, 180, ')'),

	KEY(145, 128, 167, 153, '7'),
	KEY(145, 155, 167, 180, '-'),
	KEY(145, 182, 167, 207, ','),

	KEY(169, 128, 191, 153, '8'),
	KEY(169, 155, 191, 180, '+'),
	KEY(169, 182, 191, 207, '.'),

	KEY(193, 128, 215, 153, '9'),
	KEY(193, 155, 215, 180, '='),
	KEY(193, 182, 215, 207, '\''),

	KEY(217, 128, 239, 153, '0'),
	KEY(217, 155, 239, 180, WL_KEY_BACKSPACE),
	KEY(217, 182, 239, 207, INTERNAL_NUMBER),
};

/*
 * The secret of the position and size of the keyboard
 * is shared between search.c and this file.
 */

static int kb_mode = KEYBOARD_CHAR;

void keyboard_set_mode(int mode)
{
	kb_mode = mode;

        if(kb_mode == KEYBOARD_CHAR) 
           image_data = &image_data_char;
        else if(kb_mode == KEYBOARD_NUM)
           image_data = &image_data_num;

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
        //msg(MSG_INFO,"enter keyboard_paint,ke_mode=%d\n",kb_mode);
	if (kb_mode == KEYBOARD_NONE)
		return;

        if(kb_mode == KEYBOARD_CHAR) 
           image_data = &image_data_char;
        else if(kb_mode == KEYBOARD_NUM)
           image_data = &image_data_num;

	guilib_fb_lock();
	guilib_blit_image(image_data, 0, guilib_framebuffer_height() - image_data->height);
	guilib_fb_unlock();
}

unsigned int keyboard_height()
{
	return image_data->height;
}

/**
 * Coordinates are screen absolute ones
 */
struct keyboard_key * keyboard_get_data(int x, int y)
{
	unsigned int i;

	if (kb_mode == KEYBOARD_CHAR) {
		for (i = 0; i < ARRAY_SIZE(qwerty_char); ++i) {
			if (qwerty_char[i].left_x <= x && qwerty_char[i].right_x >= x
			&& qwerty_char[i].left_y <= y && qwerty_char[i].right_y >= y) {
				DP(DBG_KEYBOARD, ("O Entered '%c'\n", qwerty_char[i].key));
				return &qwerty_char[i];
			}
		}
	}
	else if (kb_mode == KEYBOARD_NUM) {
		for (i = 0; i < ARRAY_SIZE(qwerty_num); ++i) {
			if (qwerty_num[i].left_x <= x && qwerty_num[i].right_x >= x
			&& qwerty_num[i].left_y <= y && qwerty_num[i].right_y >= y) {
				DP(DBG_KEYBOARD, ("O Entered '%c'\n", qwerty_num[i].key));
				return &qwerty_num[i];
			}
		}
	}

	return NULL;
}
