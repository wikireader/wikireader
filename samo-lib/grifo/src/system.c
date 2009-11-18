/*
 * system - miscellaneous system routines
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

#include <string.h>
#include <ctype.h>

#include <regs.h>
#include <samo.h>

#include "CMU.h"
#include "elf32.h"
#include "event.h"
#include "file.h"
#include "interrupt.h"
#include "LCD.h"
#include "memory.h"
#include "serial.h"
#include "suspend.h"
#include "watchdog.h"
#include "system.h"


typedef int UserCode(int argc, const char *const argv[]);

static void ExecuteUserCode(UserCode *code, int argc, const char *const argv[]);


void System_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		CMU_initialise();
	}
}


void System_panic(const char *format, ...)
{
	va_list arguments;

	Event_flush();

	va_start(arguments, format);

	Serial_print("System Panic:\n");
	(void)Serial_vuprintf(format, arguments);
	Serial_print("\nWaiting to power off\n");

	LCD_clear(LCD_WHITE);
	LCD_print("System Panic:\n");
	(void)LCD_vuprintf(format, arguments);

	LCD_AtXY(0, LCD_MAX_ROWS - 1);
	LCD_print("Press any key to power off");

	va_end(arguments);

	Event_flush();
	Suspend(NULL, NULL);
	System_PowerOff();
}


void System_PowerOff(void)
{
	(void)Interrupt_disable();         // interrupts off

	for (;;) {
		power_off();
	}
}


void System_reboot(void)
{
	(void)Interrupt_disable();         // interrupts off

	CMU_enable1(WDT_CKE);              // ensure watchdog has clock

	REG_P6_03_CFP &= ~0xc0;            // select P63 as input

	REG_WD_WP = WD_WP_OFF;
	REG_WD_EN = 0;                     // disable watchdog
	REG_WD_COMP = MCLK_MHz * 100;      // delay in us
	REG_WD_CNTL = WDRESEN;             // reset watchdog
	REG_WD_EN =                        // enable as reset
		//CLKSEL |
		//CLKEN  |
		RUNSTP |
		//NMIEN  |
		RESEN  |
		0;
	REG_WD_CNTL = WDRESEN;             // reset watchdog
	REG_WD_WP = WD_WP_ON;

	for (;;) {                         // wait for reset
		asm volatile ("halt");
	}
}


void System_exit(System_ExitType result)
{
	Watchdog_KeepAlive(WATCHDOG_KEY);

	Serial_printf("application returned: %d\n", result);
	File_CloseAll();

	switch (result) {
	case EXIT_POWER_OFF:
		System_PowerOff();
		break;

	case EXIT_REBOOT:
		System_reboot();
		break;

	case EXIT_RESTART_INIT:
		System_chain("init.app restart grifo-kernel");
		break;
	}
	System_PowerOff();
}



void System_chain(const char *command)
{
	Watchdog_KeepAlive(WATCHDOG_KEY);

	// these must not be on the stack
	static char buffer[256];     // sets maximum command length
	static const char *ArgumentStrings[11]; // program name + N-1 arguments

	size_t ArgumentCount = 0;
	const char *source = command;
	char *destination = buffer;

	// parse something like: --option="isn't this easy"', '"it's ok"' and "quotes" can be used'
	while ('\0' != *source && ArgumentCount < SizeOfArray(ArgumentStrings)) {
		while (isspace(*source)) {
			++source;
		}
		if ('\0' == *source) {
			break;
		}

		char quote = '\0';
		*destination = '\0';

		ArgumentStrings[ArgumentCount++] = destination;

		while ('\0' != *source && destination < &buffer[sizeof(buffer) - 1]) {
			if ('\0' != quote) {
				while ('\0' != *source && quote != *source) {
					*destination++ = *source++;
					if (destination >= &buffer[sizeof(buffer) - 1]) {
						break;
					}
				}
				if ('\0' != *source) {
					quote = '\0';
					++source;
				}
			} else if (isspace(*source)) {
				break;
			} else if ('"' == *source || '\'' == *source) {
				quote = *source++;
			} else {
				*destination++ = *source++;
			}
		}
		*destination++ = '\0';
		if (destination >= &buffer[sizeof(buffer)]) {
			break;
		}
	}

	// ensure final terminator
	buffer[sizeof(buffer) - 1] = '\0';

	uint32_t ExecutionAddress;
	uint32_t FinalAddress;
	ELF32_ErrorType r = ELF32_load(&ExecutionAddress, &FinalAddress, ArgumentStrings[0]);


	if (ELF32_OK == r) {
		// need to reset everything here
		File_CloseAll();
		extern char __MAIN_STACK_LIMIT;  // the address of this give lowest sp value
		Memory_SetHeap(FinalAddress, (uint32_t)&__MAIN_STACK_LIMIT);

		Watchdog_KeepAlive(WATCHDOG_KEY);

		ExecuteUserCode((UserCode *)ExecutionAddress, ArgumentCount, ArgumentStrings);
		// above code will not return
	} else {
		Serial_printf("ELF32_load error=%d\n", r);
	}
	File_CloseAll();
	System_PowerOff();
}

// this will change SP, so only use register variables
static void ExecuteUserCode(UserCode *code, int argc, const char *const argv[])
{
	asm volatile (
		"psrclr\t4                  \n\t"  // disable interrupts
		"xld.w\t%r15, __MAIN_STACK  \n\t"
		"ld.w\t%sp, %r15            \n\t"
		"ld.w\t%r15, 0              \n\t"
		"ld.w\t%psr, %r15           \n\t"
		"xld.w\t%r15, __dp_user     \n\t"
		"psrset\t4                  "      // enable interrupts
		);
	register int result = code(argc, argv);

	asm volatile ("xld.w\t%r15, __dp");  // restore R15

	System_exit(result);
}
