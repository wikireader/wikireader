/*
 * console serial port handling
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

#include "standard.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include <samo.h>
#include <regs.h>

#include "watchdog.h"
#include "vector.h"
#include "interrupt.h"
#include "event.h"
#include "serial.h"


void receive_interrupt(void) __attribute__((interrupt_handler));


void Serial_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Vector_initialise();
		Event_initialise();
		Watchdog_initialise();

		Interrupt_type state = Interrupt_disable();

		SET_BRTRD(0, CALC_BAUD(PLL_CLK, 1, SERIAL_DIVMD, CONSOLE_BPS));

		REG_INT_ESIF01 |= ESRX0 | ESERR0;

		REG_INT_PLCDC_PSI00 |= SERIAL_CH0_INT_PRI_7;
		int i;
		for (i = 0; i < 6; ++i) {
			// flush FIFO
			register uint32_t c = REG_EFSIF0_RXD;
			(void)c;
		}

		REG_INT_FSIF01 = FSTX0 | FSRX0 | FSERR0; // clear the interrupt

		Vector_set(VECTOR_Serial_interface_Ch_0_Receive_error, receive_interrupt);
		Vector_set(VECTOR_Serial_interface_Ch_0_Receive_buffer_full, receive_interrupt);
		Interrupt_enable(state);
	}
}


int Serial_PutChar(int c)
{
	c = (unsigned char)(c);
	if ('\n' == c) {
		Serial_PutChar('\r');
	}
	while (!Serial_PutReady()) {
	}
	Watchdog_KeepAlive(WATCHDOG_KEY);
	REG_EFSIF0_TXD = c;
	return c;
}


bool Serial_PutReady(void)
{
	return 0 != (REG_EFSIF0_STATUS & TDBEx);
}


char Serial_GetChar(void)
{
	while (!Serial_InputAvailable()) {
		Watchdog_KeepAlive(WATCHDOG_KEY);
	}
	return REG_EFSIF0_RXD;
}


bool Serial_InputAvailable(void)
{
	return 0 != (REG_EFSIF0_STATUS & RDBFx);
}



// simple line entry only handles backspace
void Serial_GetLine(char *buffer, size_t length)
{
	size_t cursor = 0;

	buffer[cursor] = '\0';
	for (;;) {
		char c = Serial_GetChar();
		if ('\010' == c || 127 == c) {
			if (cursor > 0) {
				buffer[--cursor] = '\0';
				Serial_print("\010 \010");
			} else {
				Serial_print("\007");
			}

		} else if (c >= ' ' && c < 127) {
			if (cursor < length - 1) {  // remember space for the '\0'
				buffer[cursor++] = c;
				buffer[cursor] = '\0';
				Serial_PutChar(c);
			} else {
				Serial_print("\007");
			}
		} else if ('\r' == c || '\n' == c) {
			return;
		}
	}
}


void Serial_print(const char *message)
{
	while ('\0' != *message) {
		Serial_PutChar(*message++);
	}
}


int Serial_printf(const char *format, ...)
{
	va_list arguments;

	va_start(arguments, format);

	int rc = vuprintf(Serial_PutChar, format, arguments);

	va_end(arguments);

	return rc;
}


int Serial_vuprintf(const char *format, va_list arguments)
{
	return vuprintf(Serial_PutChar, format, arguments);
}



void Serial_HexDump(const void *buffer, size_t size)
{
	uint32_t start = (uint32_t)buffer & 0x0f;
	uint32_t address = (uint32_t)buffer & ~0x0f;
	const uint8_t *bytes = (const uint8_t *)address;

	size_t offset;
	for (offset = 0; offset < start + size; offset += 16) {

		Serial_printf("%08lx: ", address + offset);

		int i;
		for (i = 0; i < 16; ++i) {
			if (8 == i) {
				Serial_PutChar(' ');
			}
			if (offset + i < start || offset + i >= start + size) {
				Serial_print(" --");
			} else {
				Serial_printf(" %02x", bytes[offset + i]);
			}
		}

		Serial_print("  |");

		for (i = 0; i < 16; i++) {
			if (8 == i) {
				Serial_PutChar(' ');
			}
			if (offset + i < start || offset + i >= start + size) {
				Serial_PutChar(' ');
			} else if (isprint(bytes[offset + i])) {
				Serial_PutChar(bytes[offset + i]);
			} else {
				Serial_PutChar('.');
			}
		}

		Serial_print("|\n");
	}
}


static int numeric_prefix;
static enum {
	state_normal = 0,
	state_escape,
	state_escape_parameter,
} state;

void receive_interrupt(void)
{
	Interrupt_SaveR15();  // must be first

	REG_INT_FSIF01 = FSRX0 | FSERR0; // clear the interrupt
	REG_EFSIF0_STATUS = 0; // clear errors

	while (0 != (REG_EFSIF0_STATUS & RDBFx)) {
		register uint32_t keycode = REG_EFSIF0_RXD;

		event_t e;

		// switch must perform one of the following actions:
		//   1. continue
		//   2. setup e.*
		switch (state) {
		case state_normal:
			// escape sequence:  <esc> ('[' | 'O') <digits> <letter-or-tilde>
			if (27 == keycode) {
				state = state_escape;
				numeric_prefix = 0;
			} else {
				e.item_type = EVENT_KEY;
				e.key.code = keycode;
				Event_put(&e);
			}
			break;

		case state_escape: // skip initial '[' or 'O'
			state = state_escape_parameter;
			break;

		case state_escape_parameter: // sequence of digits ending with letter or '~'
			if (isdigit(keycode)) {
				numeric_prefix *= 10;
				numeric_prefix += keycode - '0';
				break;
			} else {
				state = state_normal;
			}
			e.item_type = EVENT_NONE;
#if 0
			// cursor up <esc>[A
			if ('A' == keycode) {
				//CURSOR_UP
			}
			// cursor down <esc>[B
			else if ('B' == keycode) {
				//CURSOR_DOWN
			}
			// cursor right <esc>[C
			else if ('C' == keycode) {
				//CURSOR_RIGHT
			}
			// cursor left <esc>[D
			else if ('D' == keycode) {
				//CURSOR_LEFT
			}
			// home <esc>OH
			else
#endif
			if ('H' == keycode) {
				e.item_type = EVENT_BUTTON_DOWN;
				e.button.code = BUTTON_SEARCH;
			}
			// end <esc>OF
			else if ('F' == keycode) {
				e.item_type = EVENT_BUTTON_DOWN;
				e.button.code = BUTTON_HISTORY;
			}
			// page up <esc>[5~
			else if ('~' == keycode && 5 == numeric_prefix) {
				e.item_type = EVENT_BUTTON_DOWN;
				e.button.code = BUTTON_RANDOM;
			}
			// send button down/up
			if (EVENT_BUTTON_DOWN == e.item_type) {
				Event_put(&e);
				e.item_type = EVENT_BUTTON_UP;
				Event_put(&e);
			}
			break;

		default:
			state = state_normal;
			break;
		}
	}
	REG_EFSIF0_STATUS = 0; // clear errors

	Interrupt_RestoreR15();  // must be last
}
