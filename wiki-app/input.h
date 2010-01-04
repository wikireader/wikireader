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


#define WL_KEY_BACKSPACE	8
#define WL_KEY_RETURN		13
#define WL_KEY_ESC		27
#define WL_KEY_SPACE		32
#define WL_KEY_HASH		35
#define WL_KEY_PLUS		43
#define WL_KEY_MINUS		45
#define WL_KEY_UP		65
#define WL_KEY_DOWN		66

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
	WL_INPUT_KEY_CURSOR_DOWN
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

#endif /* WL_INPUT_H */

