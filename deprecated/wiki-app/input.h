/*
 * Copyright (c) 2009 Openmoko Inc.
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

#ifndef WL_EVENT_H
#define WL_EVENT_H

#include <stdbool.h>


#define WL_KEY_RETURN				0x0D
#define WL_KEY_SPACE				0x20
#define WL_KEY_HASH				0x23

#define WL_KEY_CLEAR				0X0A
#define WL_KEY_BACKWARD				0X09
#define WL_KEY_BACKSPACE			0x08
#define WL_KEY_NLS				0x07
#define WL_KEY_SWITCH_KEYBOARD 			0x06
#define WL_KEY_POHONE_STYLE_KEYBOARD_DEFAULT 	0x05
#define WL_KEY_POHONE_STYLE_KEYBOARD_ABC	0X04
#define WL_KEY_POHONE_STYLE_KEYBOARD_123	0X03
#define WL_KEY_SONANT 				0X02
#define WL_KEY_NO_WAIT 				0X01

#define WL_KEY_CLEAR_STR				"\x0A\x00"
#define WL_KEY_BACKWARD_STR				"\x09\x00"
#define WL_KEY_BACKSPACE_STR				"\x08\x00"
#define WL_KEY_NLS_STR					"\x07\x00"
#define WL_KEY_SWITCH_KEYBOARD_STR 			"\x06\x00"
#define WL_KEY_POHONE_STYLE_KEYBOARD_DEFAULT_STR 	"\x05\x00"
#define WL_KEY_POHONE_STYLE_KEYBOARD_ABC_STR 		"\x04\x00"
#define WL_KEY_POHONE_STYLE_KEYBOARD_123_STR 		"\x03\x00"
#define WL_KEY_SONANT_STR				"\x02\x00"
#define WL_KEY_NO_WAIT_STR				"\x01\x00"

enum {
	WL_INPUT_KEY_SEARCH	= 0x1000,
	WL_INPUT_KEY_HISTORY	= 0x1001,
	WL_INPUT_KEY_RANDOM	= 0x1002,
	WL_INPUT_KEY_POWER	= 0x1003,
};

enum {
	WL_INPUT_EV_TYPE_KEYBOARD = 0,
	WL_INPUT_EV_TYPE_TOUCH,
	WL_INPUT_EV_TYPE_SYSTEM,
	WL_INPUT_EV_TYPE_CURSOR
};

enum {
	WL_INPUT_KEY_CURSOR_UP = 0,
	WL_INPUT_KEY_CURSOR_DOWN,
	WL_INPUT_KEY_CURSOR_RIGHT,
	WL_INPUT_KEY_CURSOR_LEFT,
};

enum {
	WL_INPUT_TOUCH_UP = 0,
	WL_INPUT_TOUCH_DOWN,
	WL_INPUT_TOUCH_NONE
};

struct wl_input_event {
	int type;

	union {
		struct {
			int keycode;
			int value;
		} key_event;

		struct {
			int x, y;
			int value;
			unsigned long ticks;
		} touch_event;
	}; /* union */
};

bool wl_input_event_pending(void);
void wl_input_wait(struct wl_input_event *ev, int sleep);
#ifdef INCLUDED_FROM_KERNEL
void wl_input_reset_random_key(void);
#endif

#endif /* WL_INPUT_H */
