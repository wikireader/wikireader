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
#include "wikireader.h"
#include "misc.h"

void print(const char *txt)
{
	while (txt && *txt) {
		int delay = 0xf;

		REG_EFSIF0_TXD = *txt;
		do {} while (REG_EFSIF0_STATUS & (1 << 5));

		if (*txt == '\n') {
			REG_EFSIF0_TXD = '\r';
			do {} while (REG_EFSIF0_STATUS & (1 << 5));
		}

		while (delay--)
			asm("nop");

		txt++;
	}
}

static void printnibble(char nib)
{
	char a[2] = { '\0' };

	nib &= 0xf;

	if (nib >= 10)
		a[0] = nib - 10 + 'a';
	else
		a[0] = nib + '0';

	print(a);
}

static void printbyte(char byte)
{
	printnibble(byte >> 4);
	printnibble(byte);
}

void hex_dump(const char *buf, int size)
{
	int i, l;
	char a[2] = { '\0' };

	for (l = 0; l < size; l += 16) {
		printbyte(l >> 24);
		printbyte(l >> 16);
		printbyte(l >> 8);
		printbyte(l);
		print("  ");

		for (i = 0; i < 16; i++) {
			if (l + i < size) {
				printbyte(buf[l + i]);
				print(" ");
			} else
				print("   ");
		}

		print("  |");
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

void delay(int nops)
{
	while (nops--)
		asm("nop");
}
