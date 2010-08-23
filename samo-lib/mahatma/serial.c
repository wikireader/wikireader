/*
 * serial port handler
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

#include <ctype.h>

#include <guilib.h>
#include <wikilib.h>
#include <input.h>

#include <msg.h>
#include <regs.h>
#include <interrupt.h>
#include <tick.h>

#include "serial.h"


static uint8_t console_buffer[1024];
static unsigned int console_read;
static unsigned int console_write;

serial_buffer_type *send_queue_head;
serial_buffer_type *send_queue_tail;
const char *transmit;
volatile bool linefeed;


#define BUFFER_FULL(w, r, b) ((((w) + 1) % ARRAY_SIZE(b)) == (r))
#define BUFFER_EMPTY(w, r, b) ((w) == (r))
#define BUFFER_NEXT(p, b) (p) = (((p) + 1) % ARRAY_SIZE(b))

void serial_init(void)
{
	static bool initialised = false;
	if (!initialised) {
		InterruptType s = Interrupt_disable();
		REG_INT_ESIF01 = ESRX0;

		REG_INT_PLCDC_PSIO0 = 0x70;
		console_read = 0;
		console_write = 0;
		send_queue_head = NULL;
		send_queue_tail = NULL;
		linefeed = false;

		initialised = true;
		Interrupt_enable(s);
	}
}


void serial_put(serial_buffer_type *buffer)
{
	if (NULL != buffer) {
		// critical code - disable uart tx interrupts
		InterruptType s = Interrupt_disable();
		REG_INT_ESIF01 &= ~ESTX0;
		Interrupt_enable(s);
		buffer->link = NULL;
		if (NULL != send_queue_tail) {
			send_queue_tail->link = buffer;
		}
		send_queue_tail = buffer;
		if (NULL == send_queue_head) {
			send_queue_head = send_queue_tail;
			transmit = send_queue_head->text;
			s = Interrupt_disable();
			REG_INT_FSIF01 = FSTX0;
			Interrupt_enable(s);
			{
				uint8_t c = *transmit++;
				if ('\n' == c) {
					c = '\r';
					linefeed = true;
				}
				REG_EFSIF0_TXD = c;
			}

		}
		s = Interrupt_disable();
		REG_INT_ESIF01 |= ESTX0;
		Interrupt_enable(s);
	}
}


// in interrupt state
void serial_output_interrupt(void)
{
	if (!linefeed && '\0' == *transmit) {
		serial_buffer_type *p = send_queue_head;
		send_queue_head = send_queue_head->link;
		if (NULL != p->callback) {
			p->callback(p);
		}
		if (NULL == send_queue_head) {
			transmit = NULL;
			send_queue_tail = NULL;
		}
		else {
			transmit = send_queue_head->text;
		}
	}
	if (NULL == transmit) {
		REG_INT_ESIF01 &= ~ESTX0;
	}
	else {
		REG_INT_FSIF01 = FSTX0;
		if (linefeed) {
			linefeed = false;
			REG_EFSIF0_TXD = '\n';
		}
		else {
			uint8_t c = *transmit++;
			if ('\n' == c) {
				c = '\r';
				linefeed = true;
			}
			REG_EFSIF0_TXD = c;
		}
	}
}


// in interrupt state
void serial_input_interrupt(void)
{
	REG_INT_FSIF01 = FSRX0 | FSERR0; // clear the interrupt
	REG_EFSIF0_STATUS = 0; // clear errors
	while (0 != (REG_EFSIF0_STATUS & RDBFx)) {
		register uint8_t c = REG_EFSIF0_RXD;
		if (c != 0) {
			console_buffer[console_write] = c;
			BUFFER_NEXT(console_write, console_buffer);
		}
	}
	REG_EFSIF0_STATUS = 0; // clear errors
}

void serial_out(int port, char c)
{
	if (port != 0) {
		return;
	}
	InterruptType s = Interrupt_disable();
	REG_EFSIF0_TXD = c;
	Interrupt_enable(s);
}


bool serial_event_pending(void)
{
	return !BUFFER_EMPTY(console_write, console_read, console_buffer);
}


static int serial_get_key(void)
{
	if (BUFFER_EMPTY(console_write, console_read, console_buffer)) {
		return 0;
	}

	int k = console_buffer[console_read];
	BUFFER_NEXT(console_read, console_buffer);
	return k;
}


static enum {
	state_normal = 0,
	state_escape,
	state_escape_parameter,
	state_touch,
	state_x_high,
	state_x_low,
	state_y_high,
	state_y_low,
} state;

int serial_get_event(struct wl_input_event *ev)
{
	static int numeric_prefix;
	static int touch, x, y;

	int keycode = serial_get_key();

	if (0 == keycode) {
		return 0;
	}

	//msg(MSG_INFO, "Raw: %3d 0x%02x\n", keycode, keycode);

	// switch must perform one of the following actions:
	//   1. return 0
	//   2. setup ev->* and return 1
	//   3. set the value in keycode and break
	switch (state) {
	case state_normal:
		// escape sequence:  <esc> ('[' | 'O') <digits> <letter-or-tilde>
		if (27 == keycode) {
			state = state_escape;
			numeric_prefix = 0;
			return 0;
		} else if (1 == keycode) {
			state = state_touch;
			return 0;
		}
		break;

	case state_escape: // skip initial '[' or 'O'
		state = state_escape_parameter;
		return 0;

	case state_escape_parameter: // sequence of digits ending with letter or '~'
		if (isdigit(keycode)) {
			numeric_prefix *= 10;
			numeric_prefix += keycode - '0';
			return 0;
		}
		if ('~' == keycode || isalpha(keycode)) {
			state = state_normal;
		}
		// cursor up <esc>[A
		if ('A' == keycode) {
			ev->type = WL_INPUT_EV_TYPE_CURSOR;
			ev->key_event.keycode = WL_INPUT_KEY_CURSOR_UP;
			return 1;
		}
		// cursor down <esc>[B
		else if ('B' == keycode) {
			ev->type = WL_INPUT_EV_TYPE_CURSOR;
			ev->key_event.keycode = WL_INPUT_KEY_CURSOR_DOWN;
			return 1;
		}
		// home <esc>[H
		else if ('H' == keycode) {
			keycode = WL_INPUT_KEY_SEARCH;
			break;
		}
		// end <esc>[F
		else if ('F' == keycode) {
			keycode = WL_INPUT_KEY_HISTORY;
			break;
		}
		// page up <esc>[5~
		else if ('~' == keycode && 5 == numeric_prefix) {
			keycode = WL_INPUT_KEY_RANDOM;
			break;
		}
		return 0;

	case state_touch:
		touch = keycode & 0x01;  // touch flag
		state = state_x_high;
		return 0;

	case state_x_high:
		x = (keycode & 0x7f) << 7;
		state = state_x_low;
		return 0;

	case state_x_low:
		x |= keycode & 0x7f;
		state = state_y_high;
		return 0;

	case state_y_high:
		y = (keycode & 0x7f) << 8;
		state = state_y_low;
		return 0;

	case state_y_low:
		y |= keycode & 0x7f;
		//msg(MSG_INFO, "Touch: %d @ (%d, %d)\n", touch, x, y);
		state = state_normal;
		ev->type = WL_INPUT_EV_TYPE_TOUCH;
		ev->touch_event.x = x >> 1;
		ev->touch_event.y = y >> 1;
		ev->touch_event.ticks = Tick_get();
		ev->touch_event.value = touch
			? WL_INPUT_TOUCH_DOWN
			: WL_INPUT_TOUCH_UP;
		return 1;

	default:
		state = state_normal;
		return 0;
	}

	if ((0x80 & keycode) != 0) {
		return 0;
	}

	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	if (0x7f == keycode) {
		keycode = 0x08;
	}
	ev->key_event.keycode = keycode;
	ev->key_event.value = 1;
	//msg(MSG_INFO, "KEY: 0x%02x\n", ev->key_event.keycode);

	return 1;
}
