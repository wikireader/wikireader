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
#include "serial.h"
#include "irq.h"

#define BUFSIZE 10

static char console_buf[BUFSIZE];
static int  console_read = 0;
static int  console_write = 0;

int serial_transfer_running(int port)
{
	switch (port) {
	case 0:
		return (REG_EFSIF0_STATUS >> 5) & 1;
	case 1:
		return (REG_EFSIF1_STATUS >> 5) & 1;
	default:
		return 0;
	}
}

void serial_init(void)
{
	//REG_INT_ESIF01 = 0x36;
	REG_INT_ESIF01 = 0x6;
	REG_INT_PLCDC_PSIO0 = 0x70;

	msg(MSG_INFO, "r %p w %p\n", &console_read, &console_write);
}

void serial_reset(void)
{
	console_read = 0;
	console_write = 0;
}

void serial_filled(int port, char c)
{
	if (c == 0)
		return;

	switch (port) {
	case 0: /* debug console */
		DISABLE_IRQ();
		console_buf[console_write] = c;
		console_write++;
		console_write %= BUFSIZE;
		ENABLE_IRQ();
//		msg(MSG_INFO, " IN %d r %d w %d\n", c, console_read, console_write);
		break;
	case 1:
		touchscreen_read_char(c);
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
		break;
	case 1: /* touchscreen controller, nothing to do */
		break;
	}
}

void serial_out(int port, char c)
{
	if (port != 0)
		return;

	REG_EFSIF0_TXD = c;
}

int serial_get_event(struct wl_input_event *ev)
{
	if (console_read == console_write)
		return 0;
msg(MSG_INFO, " OUT. %d %d \n", console_read, console_write);
	DISABLE_IRQ();
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = console_buf[console_read];
	ev->key_event.value = 1;
	console_read++;
	console_read %= BUFSIZE;
	ENABLE_IRQ();
	return 1;
}

