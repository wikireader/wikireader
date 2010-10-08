/*
 * sample - an simple example program
 *
 * Copyright (c) 2010 Openmoko Inc.
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

#include "grifo.h"
#include <regs.h>

int grifo_main(int argc, char *argv[])
{
	// verify r15 is ok

	uint32_t r15_value = 0;

	asm volatile ("ld.w\t%0, %%r15" : "=r" (r15_value));

	debug_printf("sample start: r15 = 0x%08lx\n", r15_value);


	// some simple prints

	int x[] = {
		[0] 10,
		[6] 33,
		[3] 12,
	};

	debug_printf("x[%d]\n", sizeof(x)/sizeof(x[0]));
	int i;
	for (i = 0; i <  sizeof(x)/sizeof(x[0]); ++i) {
		debug_printf("x[%d] = %d\n", i, x[i]);
	}

	debug_printf("argc = %d\n", argc);
	for (i = 0; i < argc; ++i) {
		debug_printf("argv[%d] = '%s'\n", i, argv[i]);
	}

	debug_printf("This is debug_printf number %d / %d\n", 1, 2);
	debug_printf("This is debug_printf number %d / %d\n", 2, 2);

	debug_printf("battery      = %ld mV\n", analog_input(ANALOG_BATTERY_MILLIVOLTS));
	debug_printf("LCD contrast = %ld mV\n", analog_input(ANALOG_LCD_MILLIVOLTS));
	debug_printf("Temperature  = %ld C\n", analog_input(ANALOG_TEMPERATURE_CELCIUS));

	// test delay

	unsigned long delay_time = 1000;
	debug_printf("Use scope to check H-TP_GPIO1 high=low= %ld us\n", delay_time);
	REG_P0_IOC0 |= 0x04; // P02 = output
	REG_P0_P0D &= ~0x04; // P02 = low

	debug_printf("Check now -> running for about 60 s\n");
	for (i = 0; i < 30000; ++i) {
		REG_P0_P0D |= 0x04; // P02 = high
		delay_us(delay_time);
		REG_P0_P0D &= ~0x04; // P02 = low
		delay_us(delay_time);
	}

	debug_printf("Disconnect scope\n");

	// check that timer_get works
	{
		unsigned long delay_time = 10000;
		unsigned long start = timer_get();
		delay_us(delay_time);
		unsigned long end = timer_get();

		unsigned long difference = end - start;

		debug_printf("delay = %lu us\n", delay_time);
		debug_printf("start = %lu\n", start);
		debug_printf("end   = %lu\n", end);
		debug_printf("e - s = %lu => %lu us\n", difference, difference / TIMER_CountsPerMicroSecond);
	}

	// verify r15 is still ok
	asm volatile ("ld.w\t%0, %%r15" : "=r" (r15_value));

	debug_printf("sample finish: r15 = 0x%08lx\n", r15_value);

	return 42;
}
