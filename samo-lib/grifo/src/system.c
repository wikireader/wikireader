/*
 * system - miscellaneous system routines
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

#include "standard.h"

#include <regs.h>
#include <samo.h>

#include "CMU.h"
#include "interrupt.h"
#include "system.h"


void System_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		CMU_initialise();
		CMU_enable1(WDT_CKE);
	}
}


void System_PowerOff(void)
{
	(void)Interrupt_disable();         // interrupts off

	power_off();
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
