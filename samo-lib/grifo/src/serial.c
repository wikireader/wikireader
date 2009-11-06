/*
 * console serial port handling
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
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

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include <regs.h>

#include "serial.h"


void Serial_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
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

