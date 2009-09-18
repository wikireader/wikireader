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

#include <string.h>
#include "guilib.h"
#include "keyboard_all_image.h"
#include "input.h"
#include "wikilib.h"
#include "msg.h"

#define DBG_KEYBOARD 0

/* some control commands */
#define INTERNAL_SHIFT   (-23)
#define INTERNAL_NUMBER  (-42)

extern unsigned int get_time(void);

static struct guilib_image *image_data;
int keyboard_type = 0;

/* qwerty keyboard by columns */
#define KEY(l_x, l_y, r_x, r_y, keycode) { .left_x = l_x, .right_x = r_x, .left_y = l_y, .right_y = r_y, .key = keycode, }
static struct keyboard_key qwerty_char[] = {
	KEY(0, 126, 23, 152, 'q'),
	KEY(0, 153, 23, 180, 'a'),
	KEY(0, 181, 23, 207, 'z'),

	KEY(24, 126, 47, 152, 'w'),
	KEY(24, 153, 47, 180, 's'),
	KEY(24, 181, 47, 207, 'x'),

	KEY(48, 126, 71, 152, 'e'),
	KEY(48, 153, 71, 180, 'd'),
	KEY(48, 181, 71, 207, 'c'),

	KEY(72, 126, 95, 152, 'r'),
	KEY(72, 153, 95, 180, 'f'),
	KEY(72, 181, 95, 207, 'v'),

	KEY(96, 126, 119, 152, 't'),
	KEY(96, 153, 119, 180, 'g'),
	KEY(96, 181, 143, 207, ' '),

	KEY(120, 126, 143, 152, 'y'),
	KEY(120, 153, 143, 180, 'h'),

	KEY(144, 126, 167, 152, 'u'),
	KEY(144, 153, 167, 180, 'j'),
	KEY(144, 181, 167, 207, 'b'),

	KEY(168, 126, 191, 152, 'i'),
	KEY(168, 153, 191, 180, 'k'),
	KEY(168, 181, 191, 207, 'n'),

	KEY(192, 126, 215, 152, 'o'),
	KEY(192, 153, 215, 180, 'l'),
	KEY(192, 181, 215, 207, 'm'),

	KEY(216, 126, 239, 152, 'p'),
	KEY(216, 153, 239, 180, WL_KEY_BACKSPACE),
	KEY(216, 181, 239, 207, INTERNAL_NUMBER),
};
static struct keyboard_key qwerty_num[] = {
	KEY(0, 126, 23, 152, '1'),
	KEY(0, 153, 23, 180, '*'),
	KEY(0, 181, 23, 207, '@'),
                              
	KEY(24, 126, 47, 152, '2'),
	KEY(24, 153, 47, 180, '$'),
	KEY(24, 181, 47, 207, '?'),
                              
	KEY(48, 126, 71, 152, '3'),
	KEY(48, 153, 71, 180, '%'),
	KEY(48, 181, 71, 207, '!'),
                              
	KEY(72, 126, 95, 152, '4'),
	KEY(72, 153, 95, 180, '#'),
	KEY(72, 181, 95, 207, '&'),
                              
	KEY(96, 126, 119, 152, '5'),
	KEY(96, 153, 119, 180, '('),
	KEY(96, 181, 143, 207, ' '),
                              
	KEY(120, 126, 143, 152, '6'),
	KEY(120, 153, 143, 180, ')'),
                              
	KEY(144, 126, 167, 152, '7'),
	KEY(144, 153, 167, 180, '-'),
	KEY(144, 181, 167, 207, ','),
                              
	KEY(168, 126, 191, 152, '8'),
	KEY(168, 153, 191, 180, '+'),
	KEY(168, 181, 191, 207, '.'),
                              
	KEY(192, 126, 215, 152, '9'),
	KEY(192, 153, 215, 180, '='),
	KEY(192, 181, 215, 207, '\''),
                              
	KEY(216, 126, 239, 152, '0'),
	KEY(216, 153, 239, 180, WL_KEY_BACKSPACE),
	KEY(216, 181, 239, 207, INTERNAL_NUMBER),
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
#define KEY_GAP1 1
#define KEY_GAP2 0
#define KEY_GAP3 0
#define KEY_GAP4 1
struct keyboard_key * keyboard_get_data(int x, int y)
{
	unsigned int i;

	if (kb_mode == KEYBOARD_CHAR) {
		for (i = 0; i < ARRAY_SIZE(qwerty_char); ++i) {
			if (qwerty_char[i].left_x + KEY_GAP1 <= x && qwerty_char[i].right_x - KEY_GAP2 >= x
			&& qwerty_char[i].left_y + KEY_GAP3 <= y && qwerty_char[i].right_y - KEY_GAP4 >= y) {
				DP(DBG_KEYBOARD, ("O Entered '%c'\n", qwerty_char[i].key));
				return &qwerty_char[i];
			}
		}
	}
	else if (kb_mode == KEYBOARD_NUM) {
		for (i = 0; i < ARRAY_SIZE(qwerty_num); ++i) {
			if (qwerty_num[i].left_x + KEY_GAP1 <= x && qwerty_num[i].right_x - KEY_GAP2 >= x
			&& qwerty_num[i].left_y + KEY_GAP3 <= y && qwerty_num[i].right_y - KEY_GAP4 >= y) {
				DP(DBG_KEYBOARD, ("O Entered '%c'\n", qwerty_num[i].key));
				return &qwerty_num[i];
			}
		}
	}

	return NULL;
}

static struct keyboard_key *pre_key = NULL;
static int keyboard_key_invert_dalay = 0;
void keyboard_process_key_invert(struct keyboard_key *key)
{
	int start_x, start_y, end_x, end_y;

	start_x = key->left_x + 2;
	start_y = key->left_y + 2;
	end_x = key->right_x - 2;
	end_y = key->right_y - 2;
	if (strchr("qaz1*@", key->key))
		start_x++;
	if (strchr("p0", key->key) || key->key == WL_KEY_BACKSPACE)
		end_x--;
	if (strchr("asdfghjklzxcvbnm*$%#()-+=<@?!& ,.'", key->key) || key->key == WL_KEY_BACKSPACE)
		start_y++;
	guilib_invert_area(start_x,start_y,end_x,end_y);
	guilib_invert_area(start_x,start_y,start_x,start_y);
	guilib_invert_area(start_x,end_y,start_x,end_y);
	guilib_invert_area(end_x,start_y,end_x,start_y);
	guilib_invert_area(end_x,end_y,end_x,end_y);
	keyboard_key_invert_dalay = 0;
}

void keyboard_key_invert(struct keyboard_key *key)
{
	guilib_fb_lock();
	if (key && key->key == INTERNAL_NUMBER)
		pre_key = NULL;
	else
	{
		if (pre_key)
		{
			keyboard_key_reset_invert(KEYBOARD_RESET_INVERT_NOW);
		}
		if (key)
		{
			keyboard_process_key_invert(key);
			pre_key = key;
		}
	}
	guilib_fb_unlock();
	keyboard_key_invert_dalay = 0;
}

int keyboard_key_reset_invert(int bFlag)
{
	static long start_time;
	int rc = 0;

	if (pre_key)
	{
		if (bFlag == KEYBOARD_RESET_INVERT_DELAY)
		{
			start_time = get_time();
			keyboard_key_invert_dalay = 1;
			rc = 1;
		}
		else if (bFlag == KEYBOARD_RESET_INVERT_CHECK && keyboard_key_invert_dalay)
		{
			if (get_time() - start_time > 1000000 * 6)
				bFlag = KEYBOARD_RESET_INVERT_NOW; // reset invert immediately
			else
				rc = 1;
		}
			
		if (bFlag == KEYBOARD_RESET_INVERT_NOW)
		{
			guilib_fb_lock();
			keyboard_process_key_invert(pre_key);
			pre_key = NULL;
			guilib_fb_unlock();
		}
	}
	return rc;
}
