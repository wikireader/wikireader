/*
 * simple busy wait delay
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

#include <regs.h>

#include "timer.h"
#include "watchdog.h"
#include "delay.h"


void Delay_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Timer_initialise();
		Watchdog_initialise();
	}
}


void Delay_microseconds(unsigned long microseconds)
{
	unsigned long start = Timer_get();
	unsigned long delay = microseconds * TIMER_CountsPerMicroSecond;

	while (Timer_get() - start < delay) {
		Watchdog_KeepAlive(WATCHDOG_KEY);
	}
}
