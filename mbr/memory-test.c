// taken from Qi boot loader

/*
 * (C) Copyright 2007 OpenMoko, Inc.
 * Author: Andy Green <andy@openmoko.com>
 *
 * Memory test routines
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */


#define APPLICATION_TITLE "32MB memory test";
#include "application.h"


#define RAM_START ((u8 *)0x10000000)
#define RAM_SIZE  (32 * 1024 * 1024)

// redirect Qi routines to the correct EEPROM routines
#define puts print
#define print32 print_u32
#define printdec print_u32


void memory_test(void *start, unsigned int length);

// main() must be first as the loader executes from the first program address
int main(void)
{
	APPLICATION_INITIALISE();

	memory_test(RAM_START, RAM_SIZE);

	APPLICATION_FINALISE(0);
}


int memory_test_const32(void * start, unsigned int length, u32 value)
{
	int errors = 0;
	u32 * p = (u32 *)start;
	u32 * pend = (u32 *)(start + length);
	int count = length >> 2;

	puts(".");

	while (p < pend)
		*p++ = value;

	p = (u32 *)start;
	count = length >> 2;

	while (count--)
		if (*p++ != value) {
			puts("*A*");
			print32((long)p - 4);
			puts("=");
			print32((u32)p[-4]);
			puts("/");
			print32((u32)value);
			errors++;
		}

	return errors;
}

int memory_test_ads(void * start, unsigned int length, u32 mask)
{
	int errors = 0;
	u32 * p = (u32 *)start;
	u32 * pend = (u32 *)(start + length);

	puts(".");

	while (p < pend)
		if ((u32)p & mask)
			*p++ = 0xffffffff;
		else
			*p++ = 0;

	p = (u32 *)start;

	while (p < pend) {
		if ((u32)p & mask) {
			if (*p++ != 0xffffffff) {
				puts("*B:");
				print32((long)p - 4);
				puts("/");
				print32((u32)mask);
				errors++;
			}
		} else {
			if (*p++) {
				puts("*C:");
				print32((long)p - 4);
				puts("/");
				print32((u32)mask);
				errors++;
			}
		}
	}
	return errors;
}

int memory_test_walking1(void * start, unsigned int length)
{
	int errors = 0;
	u32 value = 1;

	while (value) {
		errors += memory_test_const32(start, length, value);
		value <<= 1;
	}

	return errors;
}

/* negative runs == run forever */

#define INTERRUPT_HERE()			\
	do {					\
		if (serial_input_available()) { \
			serial_input_char();	\
			return;			\
		}				\
	} while (0)

void memory_test(void *start, unsigned int length)
{
	int errors = 0;
	int series = 0;
	int mask;

	puts("\nMemory: ");
	print32((u32)start);
	puts(" length ");
	printdec(length >> 20);
	puts(" MB\n");

	for (;;) {
		puts("Test series ");
		printdec(series + 1);
		puts(" ");

		/* these are looking at data issues, they flood the whole
		 * array with the same data
		 */

		errors += memory_test_const32(start, length, 0x55555555);
		INTERRUPT_HERE();
		errors += memory_test_const32(start, length, 0xaaaaaaaa);
		INTERRUPT_HERE();
		errors += memory_test_const32(start, length, 0x55aa55aa);
		INTERRUPT_HERE();
		errors += memory_test_const32(start, length, 0xaa55aa55);
		INTERRUPT_HERE();
		errors += memory_test_const32(start, length, 0x00ff00ff);
		INTERRUPT_HERE();
		errors += memory_test_const32(start, length, 0xff00ff00);
		INTERRUPT_HERE();
		errors += memory_test_walking1(start, length);
		INTERRUPT_HERE();
		puts("+");

		/* this is looking at addressing issues, it floods only
		 * addresses meeting a walking mask with 0xffffffff (the rest
		 * is zeroed), and makes sure all the bits are only seen where
		 * they were placed
		 */

		mask = 1;
		while (! (length & mask)) {
			errors += memory_test_ads(start, length, mask);
			INTERRUPT_HERE();
			mask = mask << 1;
		}

		puts("Errors: ");
		printdec(errors);
		puts("\n");

		series++;
	}
}
