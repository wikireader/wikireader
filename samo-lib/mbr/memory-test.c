// taken from Qi boot loader

/*
 * (C) Copyright 2007 Openmoko, Inc.
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


#define APPLICATION_TITLE "Memory Test";
#define APPLICATION_TITLE2 "Memory Check";
#include "application.h"


#define RAM_START ((uint8_t *)0x10000000)
#define RAM_SIZE  (32 * 1024 * 1024)

// redirect Qi routines to the correct EEPROM routines
#define puts print
#define print32 print_hex
#define printdec print_uint


void memory_test(void *start, unsigned int length);
void memory_check(void *start, unsigned int length);

// this must be the first executable code as the loader executes from the first program address
ReturnType mem(int block, int status)
{
	APPLICATION_INITIALISE();

	if (0 == status) {
		memory_test(RAM_START, RAM_SIZE);
	} else {
		memory_check(RAM_START, RAM_SIZE);
	}
	APPLICATION_FINALISE(0, 0);
}


int memory_test_const32(void * start, unsigned int length, uint32_t value)
{
	int errors = 0;
	uint32_t * p = (uint32_t *)start;
	uint32_t * pend = (uint32_t *)(start + length);
	int count = length >> 2;

	puts(".");

	while (p < pend)
		*p++ = value;

	p = (uint32_t *)start;
	count = length >> 2;

	while (count--)
		if (*p++ != value) {
			puts("*A*");
			print32((long)p - 4);
			puts("=");
			print32((uint32_t)p[-4]);
			puts("/");
			print32((uint32_t)value);
			errors++;
		}

	return errors;
}

int memory_test_ads(void * start, unsigned int length, uint32_t mask)
{
	int errors = 0;
	uint32_t * p = (uint32_t *)start;
	uint32_t * pend = (uint32_t *)(start + length);

	puts(".");

	while (p < pend)
		if ((uint32_t)p & mask)
			*p++ = 0xffffffff;
		else
			*p++ = 0;

	p = (uint32_t *)start;

	while (p < pend) {
		if ((uint32_t)p & mask) {
			if (*p++ != 0xffffffff) {
				puts("*B:");
				print32((long)p - 4);
				puts("/");
				print32((uint32_t)mask);
				errors++;
			}
		} else {
			if (*p++) {
				puts("*C:");
				print32((long)p - 4);
				puts("/");
				print32((uint32_t)mask);
				errors++;
			}
		}
	}
	return errors;
}

int memory_test_walking1(void * start, unsigned int length)
{
	int errors = 0;
	uint32_t value = 1;

	while (value) {
		errors += memory_test_const32(start, length, value);
		value <<= 1;
	}

	return errors;
}

/* negative runs == run forever */

#define INTERRUPT_HERE()			\
	do {					\
		if (console_input_available()) { \
			console_input_char();	\
			return;			\
		}				\
	} while (0)

void memory_test(void *start, unsigned int length)
{
	int errors = 0;
	int series = 0;
	int mask;

	puts("\nMemory: ");
	print32((uint32_t)start);
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


void memory_check(void *start, unsigned int length)
{
	int i;
	int pass = 1;
	uint32_t mega = length >> 20;
	volatile uint32_t *memory = (uint32_t *)start;

	puts("\nMemory: ");
	print32((uint32_t)start);
	puts(" length ");
	printdec(mega);
	puts(" MB  [");

	for (i = 0; i < mega; ++i) {
		int j;
		int flag = 1;
		for (j = 0; j < (1 << 20) ; j += 256) {
			volatile uint32_t *p = (volatile uint32_t *)&memory[(i << 20) + j];
			uint32_t s = *p;
			*p = ~s;
			if (*p != ~s) {
				flag = 0;
				pass = 0;
				break;
			}
			*p = 0x5aa55aa5;
			if (*p != 0x5aa55aa5) {
				flag = 0;
				pass = 0;
				break;
			}
			*p = 0xa55aa55a;
			if (*p != 0xa55aa55a) {
				flag = 0;
				pass = 0;
				break;
			}
			*p = s;
			if (*p != s) {
				flag = 0;
				pass = 0;
				break;
			}
		}
		print_char(flag ? '.' : 'F');
	}
	print("]\n");
	print(pass ? "PASS" : "FAIL");
	print(": Memory Check\n");
}
