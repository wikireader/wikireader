/*
 * grifo - a small kernel for WikiReader
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

#include "standard.h"

#include <ctype.h>

#include "standard.h"
#include "elf32.h"

// modules to initialise
#include "button.h"
#include "CMU.h"
#include "CTP.h"
#include "delay.h"
#include "event.h"
#include "file.h"
#include "interrupt.h"
#include "memory.h"
#include "serial.h"
#include "suspend.h"
#include "syscall.h"
#include "system.h"
#include "timer.h"
#include "vector.h"
#include "watchdog.h"

void process(void);


// Note: the cross compiler will generate bad code
// if local variables are used here.
// The initial assembler code must affect it in some way.
// Therefore most of the code goes in process()
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

	// main part of system
	process();

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


void process(void)
{
	// critical initialisations
	Vector_initialise();
	//*Suspend_initialise();
	Watchdog_initialise();
	Serial_initialise();

	// enable interrupts
	Interrupt_enable(Interrupt_enabled);

	Serial_print("Grifo starting\n");

	// system initialisation
	System_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Timer_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Delay_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Event_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	CTP_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Button_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	File_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Suspend_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Memory_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

#if 0
	// secondary
	SPI_initialise();
	Analog_initialise();
	//Contrast_initialise();  // needs to be shared with boot loader and read FLASH
	Menu_initialise();
#endif

	// final initialisation before running the application
	SystemCall_initialise();
	Watchdog_KeepAlive(WATCHDOG_KEY);

#if 0
	Menu_run();
#endif

	// program to run
	for (;;) {
		Watchdog_KeepAlive(WATCHDOG_KEY);

		char buffer[81]; // remember the '\0'

		Serial_print("command: ");
		Serial_GetLine(buffer, sizeof(buffer));
		Serial_print("\n");

		const char *args[11]; // program name + N-1 arguments

		size_t args_index = 0;
		char *p = buffer;
		while ('\0' != *p && args_index < SizeOfArray(args)) {
			while (isspace(*p)) {
				++p;
			}
			if ('\0' == *p) {
				break;
			}
			//asm volatile("nop");
			args[args_index++] = p;
			while ('\0' != *p && !isspace(*p)) {
				++p;
			}
			if ('\0' == *p) {
				break;
			}
			*p++ = '\0';
		}

		uint32_t ExecutionAddress;
		uint32_t FinalAddress;
		ELF32_ErrorType r = ELF32_load(&ExecutionAddress, &FinalAddress, args[0]);


		if (ELF32_OK == r) {
			extern char __MAIN_STACK_LIMIT;  // the address of this give lowest sp value
			Memory_SetHeap(FinalAddress, (uint32_t)&__MAIN_STACK_LIMIT);
			asm volatile ("xld.w\t%r15, __dp_user");
			int result = ((int (*) (int, const char **)) ExecutionAddress) (args_index, args);
			asm volatile ("xld.w\t%r15, __dp");
			Serial_printf("application returned: %d\n", result);
		} else {
			Serial_printf("ELF32_load error=%d\n", r);
		}
		File_CloseAll();
	}

}
