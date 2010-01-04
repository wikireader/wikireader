/*
 * touchscreen handler
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Matt Hsu <matt_hsu@openmoko.org>
 *           Christopher Hall <hsw@openmoko.com>
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

#include <input.h>
#include <msg.h>
#include <wikilib.h>
#include <regs.h>
#include <ctp.h>

#include "samo.h"
#include "touchscreen.h"


static int x, y;
static bool pressed;
static unsigned long ticks;
static bool event_cached;


bool touchscreen_event_pending(void)
{
       if (event_cached || CTP_get(&x, &y, &pressed, &ticks))
       {
	       event_cached = true;
       }
       return event_cached;
}


bool touchscreen_get_event(struct wl_input_event *ev)
{

	if (!event_cached && !CTP_get(&x, &y, &pressed, &ticks)) {
		return false;
	}

	event_cached = false;
	ev->type = WL_INPUT_EV_TYPE_TOUCH;
	ev->touch_event.x = x >> 1;
	ev->touch_event.y = y >> 1;
	ev->touch_event.ticks = ticks;
	ev->touch_event.value = pressed
		? WL_INPUT_TOUCH_DOWN
		: WL_INPUT_TOUCH_UP;
	return true;
}

void touchscreen_init(void)
{
	event_cached = false;
	CTP_initialise();
}
