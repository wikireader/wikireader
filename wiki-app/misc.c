/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

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

#include "regs.h"
#include "types.h"
#include "samo.h"
#include "misc.h"

int serial_input_available(void) {
	return (0 != (REG_EFSIF0_STATUS	& RDBFx));
}


int serial_input_char(void)
{
	while (!serial_input_available()) {
	}

	return(REG_EFSIF0_RXD);
}

#define WAIT_FOR_EFSIF0_RDY()				\
	do {						\
	} while (0 == (REG_EFSIF0_STATUS & TDBEx))

int print_char(int c)
{
	if (c == '\n') {
		WAIT_FOR_EFSIF0_RDY();
		REG_EFSIF0_TXD = '\r';
	}
	WAIT_FOR_EFSIF0_RDY();
	REG_EFSIF0_TXD = c;
	return 0;
}

void print(const char *txt)
{
	while (txt && *txt) {
		print_char(*txt++);
	}
}

static void print_nibble(uint8_t nib)
{
	nib &= 0x0f;
	if (nib >= 10)
		print_char(nib - 10 + 'a');
	else
		print_char(nib + '0');
}

void print_byte(uint8_t byte)
{
	print_nibble(byte >> 4);
	print_nibble(byte);
}

void hex_dump(const void *buffer, uint32_t size)
{
	int i, l;
	char a[2] = "X";
	const uint8_t *buf = (const uint8_t *)buffer;

	for (l = 0; l < size; l += 16) {
		print_byte(l >> 24);
		print_byte(l >> 16);
		print_byte(l >> 8);
		print_byte(l);
		print("	 ");

		for (i = 0; i < 16; i++) {
			if (l + i < size) {
				print_byte(buf[l + i]);
				print(" ");
			} else
				print("	  ");
		}

		print("	 |");
		for (i = 0; i < 16; i++) {
			if (l + i < size) {
				if (buf[l + i] >= ' ' && buf[l + i] <= '~')
					a[0] = buf[l + i];
				else
					a[0] = '.';
			} else
				a[0] = ' ';

			print(a);
		}

		print("|\n");
	}
}

void print_u32(uint32_t val)
{
	print("0x");
	print_byte(val >> 24);
	print_byte(val >> 16);
	print_byte(val >> 8);
	print_byte(val);
}

void print_dec32(uint32_t value)
{
	char c[33];
	int i;

	c[sizeof(c) - 1] = '\0';
	for (i = sizeof(c) - 2; i >= 0; --i) {
		c[i] = value % 10 + '0';
		value /= 10;
	}
	for (i = 0; '0' == c[i] && i < sizeof(c) - 2; ++i) {
	}
	print(&c[i]);
}

void delay(unsigned int nops)
{
	while (nops--)
		asm volatile ("nop");
}

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
