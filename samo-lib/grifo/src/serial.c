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


unsigned char Serial_GetChar(void)
{
	while (!Serial_InputAvailable()) {
	}
	return REG_EFSIF0_RXD;
}


bool Serial_InputAvailable(void)
{
	return 0 != (REG_EFSIF0_STATUS & RDBFx);
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

