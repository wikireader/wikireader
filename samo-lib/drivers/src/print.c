/*
 * print - some debugging print functions
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

#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include <regs.h>
#include <samo.h>

#include "print.h"


int print_char(int c)
{
	if (c == '\n') {
		print_char('\r');
	}

	while (0 == (REG_EFSIF0_STATUS & TDBEx)) {
	}

	REG_EFSIF0_TXD = c;
	return c;
}

void print(const char *text)
{
	while (text && *text) {
		print_char(*text++);
	}
}

void print_int(int value)
{
	if (0 > value) {
		print("-");
		value = - value;
	}
	print_uint((unsigned int)value);
}


static void print_uint_1(unsigned int value)
{
	if (0 != value) {
		print_uint_1(value / 10);
		print_char(value % 10 + '0');
	}
}


void print_uint(unsigned int value)
{
	if (0 == value) {
		print_char('0');
	} else {
		print_uint_1(value);
	}
}


void print_hex_digit(unsigned int value)
{
	value &= 0x0f;
	if (value < 10) {
		print_char(value + '0');
	} else {
		print_char(value + ('a' - 10));
	}
}


void print_byte(unsigned int value)
{
	print_hex_digit(value >> 4);
	print_hex_digit(value);
}


void print_hex(unsigned int value)
{
	print("0x");
	int i;
	for (i = sizeof(value) - 1; i >= 0; --i) {
		print_byte(value >> (i * 8));
	}
}
