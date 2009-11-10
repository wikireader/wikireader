/*
 * watchdog - driver for the watchdog timer
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
#include "vector.h"
#include "event.h"
#include "interrupt.h"
#include "watchdog.h"


// N seconds timeout at normal speed
#define NORMAL_TIMEOUT_VALUE (MCLK * 20)
#if NORMAL_TIMEOUT_VALUE > 0x3fffffff
#error "NORMAL_AUTO_POWER_OFF_SECONDS is too large"
#endif

void Watchdog_interrupt(void) __attribute__((interrupt_handler));


void Watchdog_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Vector_initialise();
		Event_initialise();
		CMU_initialise();

		Vector_set(VECTOR_NMI, Watchdog_interrupt);

		// enable watchdog
		CMU_enable1(WDT_CKE);
		Watchdog_SetTimeout(WATCHDOG_KEY, NORMAL_TIMEOUT_VALUE);
	}
}


void Watchdog_KeepAlive(Watchdog_type key)
{
	if (WATCHDOG_KEY == key) {
		//REG_WD_WP = WD_WP_OFF;
		REG_WD_CNTL = WDRESEN;
		//REG_WD_WP = WD_WP_ON;
	}
}


void Watchdog_SetTimeout(Watchdog_type key, uint32_t WatchdogTimeout)
{
	if (WATCHDOG_KEY == key) {

		REG_WD_WP = WD_WP_OFF;
		REG_WD_COMP = WatchdogTimeout;
		REG_WD_CNTL = WDRESEN;
		REG_WD_EN =
			//CLKSEL |
			//CLKEN  |
			RUNSTP |
			NMIEN  |     // so that low pulse is output
			//RESEN  |     // reset takes priority
			0;
		REG_WD_WP = WD_WP_ON;

		REG_P6_03_CFP &= ~0xc0; // select P63 as input
		//REG_P6_03_CFP |= 0xc0; // select P63 as #WDT_NMI
	}
}


void Watchdog_PowerOffMode(Watchdog_type key, uint32_t WatchdogTimeout)
{
	if (WATCHDOG_KEY == key) {

		REG_WD_WP = WD_WP_OFF;
		REG_WD_COMP = WatchdogTimeout;
		REG_WD_CNTL = WDRESEN;
		REG_WD_EN =
			//CLKSEL |
			//CLKEN  |
			RUNSTP |
			NMIEN  |     // so that low pulse is output
			RESEN  |     // reset takes priority
			0;
		REG_WD_WP = WD_WP_ON;

		REG_P6_03_CFP |= 0xc0; // select P63 as #WDT_NMI
	}
}


void Watchdog_interrupt(void)
{
	Interrupt_SaveR15();  // must be first

	// next watchdog timeout will be power off
	Watchdog_PowerOffMode(WATCHDOG_KEY, NORMAL_TIMEOUT_VALUE);

	// simulate this a power off event
	event_t event;
	event.item_type = EVENT_BUTTON_DOWN;
	event.button.code = BUTTON_POWER;
	Event_put(&event);

	Watchdog_KeepAlive(WATCHDOG_KEY);

	Interrupt_RestoreR15();  // must be last
}
