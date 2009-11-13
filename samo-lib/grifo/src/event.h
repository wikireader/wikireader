/*
 * event queue
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#if !defined(_EVENT_H_)
#define _EVENT_H_ 1

#include "standard.h"

//+MakeSystemCalls: types
typedef enum {
	EVENT_NONE = 0,
	EVENT_KEY,
	EVENT_TOUCH_DOWN,
	EVENT_TOUCH_MOTION,
	EVENT_TOUCH_UP,
	EVENT_BUTTON_DOWN,
	EVENT_BUTTON_UP,
	EVENT_BATTERY_LOW,
} event_item_t;

typedef enum {
	BUTTON_RANDOM,
	BUTTON_SEARCH,
	BUTTON_HISTORY,
	BUTTON_POWER,
} button_t;

typedef struct event_struct {
	event_item_t item_type;
	unsigned long time_stamp;
	union {
		struct {
			uint8_t code;
		} key;

		struct {
			button_t code;
		} button;

		struct {
			int x, y;
		} touch;

		struct {
			uint32_t millivolts;
		} battery;
	};
} event_t;
//-MakeSystemCalls: types


void Event_initialise();

// clear the whole queue
void Event_flush(void);

// copy to buffer
// return false if buffer is already full
bool Event_put(const event_t *event);

// copy from buffer
// return EVENT_NONE if buffer was empty
event_item_t Event_get(event_t *event);

// wait for an event, if no event within timeout
// then call the callback function and shutdown
event_item_t Event_wait(event_t *event, Standard_BoolCallBackType *callback, void *arg);

#endif
