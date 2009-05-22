/*
    Copyright (c) 2009 Openmoko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "delay.h"

void delay_us(unsigned int microsec)
{
	while (microsec--) {
		// at 48 MHz this should take 1 micro second
		asm volatile (
			"\tld.w\t%r4, 6\n"
			"delay_loop:\n"
			"\tnop\n"
			"\tnop\n"
			"\tsub\t%r4, 1\n"
			"\tjrne\tdelay_loop"
			);
	}
}
