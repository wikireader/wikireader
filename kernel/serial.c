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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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
#include "touchscreen.h"
#include "serial.h"
#include "irq.h"


static u8 console_buffer[16];
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
		DISABLE_IRQ();
		REG_INT_ESIF01 = ESRX0;

		REG_INT_PLCDC_PSIO0 = 0x70;
		console_read = 0;
		console_write = 0;
		send_queue_head = NULL;
		send_queue_tail = NULL;
		linefeed = false;

		initialised = true;
		ENABLE_IRQ();
	}
}


bool serial_output_pending(void)
{
	return 0 != (REG_EFSIF0_STATUS & TENDx);
}


void serial_put(serial_buffer_type *buffer)
{
	if (NULL != buffer) {
		// critical code - disable uart tx interrupts
		DISABLE_IRQ();
		REG_INT_ESIF01 &= ~ESTX0;
		ENABLE_IRQ();
		buffer->link = NULL;
		if (NULL != send_queue_tail) {
			send_queue_tail->link = buffer;
		}
		send_queue_tail = buffer;
		if (NULL == send_queue_head) {
			send_queue_head = send_queue_tail;
			transmit = send_queue_head->text;
			DISABLE_IRQ();
			REG_INT_FSIF01 = FSTX0;
			ENABLE_IRQ();
			{
				u8 c = *transmit++;
				if ('\n' == c) {
					c = '\r';
					linefeed = true;
				}
				REG_EFSIF0_TXD = c;
			}

		}
		DISABLE_IRQ();
		REG_INT_ESIF01 |= ESTX0;
		ENABLE_IRQ();
	}
}


// in interrupt state
void serial_drained_0(void)
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
		REG_INT_FSIF01 |= FSTX0;
		if (linefeed) {
			linefeed = false;
			REG_EFSIF0_TXD = '\n';
		}
		else {
			u8 c = *transmit++;
			if ('\n' == c) {
				c = '\r';
				linefeed = true;
			}
			REG_EFSIF0_TXD = c;
		}
	}
}


// in interrupt state
void serial_filled_0(void)
{
	while (REG_EFSIF0_STATUS & RDBFx) {
		u8 c = REG_EFSIF0_RXD;
		if (c == 0)
			continue;

		console_buffer[console_write] = c;
		BUFFER_NEXT(console_write, console_buffer);
	}
}

void serial_out(int port, char c)
{
	if (port != 0)
		return;

	DISABLE_IRQ();
	REG_EFSIF0_TXD = c;
	ENABLE_IRQ();
}

int serial_get_event(struct wl_input_event *ev)
{
	if (BUFFER_EMPTY(console_write, console_read, console_buffer))
		return 0;

//	msg(MSG_INFO, " OUT. %d %d    %p %p\n", console_read, console_write,
//		ev, &ev->type);

	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	if (0x7f == console_buffer[console_read]) {
		console_buffer[console_read] = 0x08;
	}
	ev->key_event.keycode = console_buffer[console_read];
	ev->key_event.value = 1;
	BUFFER_NEXT(console_read, console_buffer);

	/* Override for scrolling... */
	if ((ev->key_event.keycode == WL_KEY_PLUS) || (ev->key_event.keycode == WL_KEY_DOWN)) {
		ev->type = WL_INPUT_EV_TYPE_CURSOR;
		ev->key_event.keycode = WL_INPUT_KEY_CURSOR_DOWN;
	}
	else if ((ev->key_event.keycode == WL_KEY_MINUS) || (ev->key_event.keycode == WL_KEY_UP)) {
		ev->type = WL_INPUT_EV_TYPE_CURSOR;
		ev->key_event.keycode = WL_INPUT_KEY_CURSOR_UP;
	}

	return 1;
}
