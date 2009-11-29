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

#if !defined(_PRINT_H_)
#define _PRINT_H_ 1

int print_char(int c);
void print(const char *text);
void print_int(int value);
void print_uint(unsigned int value);

void print_hex_digit(unsigned int value);
void print_byte(unsigned int value);

// word with 0x prefix
void print_hex(unsigned int value);

#endif
