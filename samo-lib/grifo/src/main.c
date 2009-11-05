/*
 * grifo - a small kernel for WikiReader
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

#include <inttypes.h>
#include <stdbool.h>

#include "elf32.h"

#include "CMU.h"
#include "vector.h"
#include "interrupt.h"
#include "serial.h"
#include "delay.h"
#include "timer.h"
#include "syscall.h"


__attribute ((noreturn))
int main(void)
{
	// set the initial stack and data pointers
	// how to handle different SDRAM sizes
	asm volatile (
		"\txld.w\t%r15, __MAIN_STACK\n"
		"\tld.w\t%sp, %r15\n"
		"\txld.w\t%r15, __dp\n"
		"\tld.w\t%r4, 0\n"
		"\tld.w\t%psr, %r4\n"
		);
	// interrupts will be disabled at this point
	Interrupt_initialise();

	// ensure MCU clocks are set up
	CMU_initialise();

	// critical initialisations
	Vector_initialise();
	//*Suspend_initialise();
	Serial_initialise();

	// enable interrupts
	Interrupt_enable(Interrupt_enabled);

	Serial_print("Grifo starting\n");

	// system initialisation
	Delay_initialise();
	Timer_initialise();

#if 0
	// secondary
	SPI_initialise();
	CTP_initialise();
	Buttons_initialise();
	Memory_initialise();
	Analog_initialise();
	//Contrast_initialise();  // needs to be shared with boot loader and read FLASH
	Files_Initialise();
	Menu_initialise();
#endif

	// final initialisation before running the application
	SystemCall_initialise();

#if 0
	Menu_run();
#endif

	// quick test to run sample.elf
	const char *args[] = {
		"sample.elf", "--option", "thing"
	};

	uint32_t exec;
	ELF32_ErrorType r = ELF32_load(&exec, "sample.elf");

	if (ELF32_OK == r) {
		asm volatile ("xld.w\t%r15, __dp_user");
		int result = ((int (*) (int, const char **)) exec) (3, args);
		asm volatile ("xld.w\t%r15, __dp");
		Serial_printf("application returned: %d\n", result);
	} else {
		Serial_printf("ELF32_load error=%d\n", r);
	}

	// cannot exit this so power off or reboot
	for (;;) {
//*		Serial_print("Looping...\n");
//*		System_PowerOff();
//*		System_reboot();

		// Just Panic fo the moment
		Serial_print("Execute int 3, should panic\n");
		asm volatile(
			"xld.w\t%r14, 0xcafe1a7e\n\t"
			"xld.w\t%r0, 0xcafedeca\n\t"
			"int\t3\n\t"
			);
	}
}
