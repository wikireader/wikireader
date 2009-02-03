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

#include <msg.h>
#include "regs.h"
#include "msg-output.h"
#include "touchscreen.h"

static char last_char = 0;
static int transfer_running[2];

int serial_transfer_running(int port)
{
#if 0
	switch (port) {
	case 0:
		return (REG_EFSIF0_STATUS >> 5) & 1;
	case 1:
		return (REG_EFSIF1_STATUS >> 5) & 1;
	default:
		return 0;
	}
#endif
	return transfer_running[port];
}

void serial_init(void)
{
	transfer_running[0] = 0;
	transfer_running[1] = 0;

	//REG_INT_ESIF01 = 0x36;
	REG_INT_ESIF01 = 0x6;
	REG_INT_PLCDC_PSIO0 = 0x70;
}

void serial_filled(int port)
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

void serial_drained(int port)
{
	char c;

	switch (port) {
	case 0: /* debug console */
		if (get_msg_char(&c))
			serial_out(0, c);
		else
			transfer_running[0] = 0;
		break;
	case 1: /* touchscreen controller, nothing to do */
		break;
	}
}

void serial_out(int port, char c)
{
	if (port != 0)
		return;

	transfer_running[port] = 1;
	REG_EFSIF0_TXD = c;
}

int serial_get_event(struct wl_input_event *ev)
{
	if (!last_char)
		return 0;

	msg(MSG_INFO, ".");

	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = last_char;
	ev->key_event.value = 1;
	last_char = 0;

	return 1;
}

