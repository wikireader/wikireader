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
#include "types.h"
#include "wikireader.h"
#include "misc.h"

#define WAIT_FOR_EFSIF0_RDY()	\
	do {			\
		asm("nop");	\
	} while (REG_EFSIF0_STATUS & (1 << 5));

void print(const u8 *txt)
{
	while (txt && *txt) {
		int delay = 0xff;

		REG_EFSIF0_TXD = *txt;
		WAIT_FOR_EFSIF0_RDY();

		if (*txt == '\n') {
			REG_EFSIF0_TXD = '\r';
			WAIT_FOR_EFSIF0_RDY();
		}

		while (delay--)
			asm("nop");

		txt++;
	}
}

static void print_nibble(u8 nib)
{
	char a[2] = "X";

	nib &= 0xf;

	if (nib >= 10)
		a[0] = nib - 10 + 'a';
	else
		a[0] = nib + '0';

	print(a);
}

void print_byte(u8 byte)
{
	print_nibble(byte >> 4);
	print_nibble(byte);
}

void hex_dump(const u8 *buf, u32 size)
{
	int i, l;
	char a[2] = "X";

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

void print_u32(u32 val)
{
	print("0x");
	print_byte(val >> 24);
	print_byte(val >> 16);
	print_byte(val >> 8);
	print_byte(val);
}

void delay(u32 nops)
{
	while (nops--)
		asm("nop");
}

#if 0
void printf(const char *fmt, ...)       /* format to be printed */
{
  int c;                                        /* next character in fmt */
  int d;
  unsigned long u;                              /* hold number argument */
  int base;                                     /* base of number arg */
  int negative;                                 /* print minus sign */
  static char x2c[] = "0123456789ABCDEF";       /* nr conversion table */
  char ascii[8 * sizeof(long) / 3 + 2];         /* string for ascii number */
  char *s;                                      /* string to be printed */
  va_list argp;                                 /* optional arguments */

  va_start(argp, fmt);                          /* init variable arguments */

  while((c=*fmt++) != 0) {

      if (c == '%') {                           /* expect format '%key' */
          negative = 0;                         /* (re)initialize */
          s = NULL;                             /* (re)initialize */
          switch(c = *fmt++) {                  /* determine what to do */

          /* Known keys are %d, %u, %x, %s, and %%. This is easily extended 
           * with number types like %b and %o by providing a different base.
           * Number type keys don't set a string to 's', but use the general
           * conversion after the switch statement.
           */
          case 'd':                             /* output decimal */
              d = va_arg(argp, signed int);
              if (d < 0) { negative = 1; u = -d; }  else { u = d; }
              base = 10;
              break;
          case 'u':                             /* output unsigned long */
              u = va_arg(argp, unsigned long);
              base = 10;
              break;
          case 'x':                             /* output hexadecimal */
              u = va_arg(argp, unsigned long);
              base = 0x10;
              break;
          case 's':                             /* output string */
              s = va_arg(argp, char *);
              if (s == NULL) s = "(null)";
              break;
          case '%':                             /* output percent */
              s = "%";
              break;

          /* Unrecognized key. */
          default:                              /* echo back %key */
              s = "%?";
              s[1] = c;                         /* set unknown key */
          }

          /* Assume a number if no string is set. Convert to ascii. */
          if (s == NULL) {
              s = ascii + sizeof(ascii)-1;
              *s = 0;
              do {  *--s = x2c[(u % base)]; }   /* work backwards */
              while ((u /= base) > 0);
          }

          /* This is where the actual output for format "%key" is done. */
          if (negative) kputc('-');             /* print sign if negative */
          while(*s != 0) { kputc(*s++); }       /* print string/ number */
      }
      else {
          kputc(c);                             /* print and continue */
      }
  }
  va_end(argp);                                 /* end variable arguments */
}
#endif

