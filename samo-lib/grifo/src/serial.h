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

#if !defined(_SERIAL_H_)
#define _SERIAL_H_ 1

#include <stdbool.h> // bool
#include <stdlib.h>  // size_t

void Serial_initialise(void);

int Serial_PutChar(int c);
bool Serial_PutReady(void);

char Serial_GetChar(void);
bool Serial_InputAvailable(void);
void Serial_GetLine(char *buffer, size_t length);

void Serial_print(const char *message);
int Serial_printf(const char *format, ...) __attribute__((format (printf, 1, 2)));

#endif
