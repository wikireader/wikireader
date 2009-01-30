/*
 * mahatma - a simple kernel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
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

#include <guilib.h>
#include <wikilib.h>
#include <input.h>

#include "regs.h"
#include "msg-output.h"
#include "touchscreen.h"

static char last_char = 0;

void serial_init(void)
{

}

void serial_in(int port)
{
	switch (port) {
	case 0: /* debug console */
		last_char = REG_EFSIF0_RXD;
		break;
	case 1:
		touchscreen_read_char(REG_EFSIF1_RXD);
		break;
	}
}

void serial_out(int port)
{
	char c;

	switch (port) {
	case 0: /* debug console */
		if (get_msg_char(&c))
			REG_EFSIF0_TXD = c;
		break;
	case 1: /* touchscreen controller, nothing to do */
		break;
	}
}

int serial_get_event(struct wl_input_event *ev)
{
	if (!last_char)
		return 0;

	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = last_char;
	ev->key_event.value = 1;
	last_char = 0;

	return 1;
}

