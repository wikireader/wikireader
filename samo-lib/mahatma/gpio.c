/*
 * buttons handler
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
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

#include <stdio.h>
#include <input.h>
#include <regs.h>
#include <samo.h>
#include <wikilib.h>
#include <button.h>

#include "gpio.h"


#if BOARD_SAMO_Ax || BOARD_SAMO_Vx
static const int keymap[] = {
	WL_INPUT_KEY_RANDOM,
	WL_INPUT_KEY_SEARCH,
	WL_INPUT_KEY_HISTORY,
	WL_INPUT_KEY_POWER,
};
#else
static const int keymap[] = {
	WL_INPUT_KEY_SEARCH,
	WL_INPUT_KEY_HISTORY,
	WL_INPUT_KEY_RANDOM,
	WL_INPUT_KEY_POWER,
};
#endif


static ButtonType button;
static bool pressed;
static bool event_cached;


bool gpio_event_pending(void)
{
	if (event_cached || Button_get(&button, &pressed))
	{
		event_cached = true;
	}
	return event_cached;
}


bool gpio_get_event(struct wl_input_event *ev)
{
	if (!event_cached && !Button_get(&button, &pressed)) {
		return false;
	}

	event_cached = false;
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = keymap[button];
	ev->key_event.value = pressed;
	return true;
}


void gpio_init(void)
{
	event_cached = false;
	Button_initialise();
}
