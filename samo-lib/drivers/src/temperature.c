/*
 * temperature controller driver
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

#include <stdlib.h>
#include <stdbool.h>
#include <regs.h>

#include "analog.h"
#include "contrast.h"
#include "temperature.h"
#include "tick.h"


#define POLL_TIME_MICROSECONDS    (100 * 1000 * Tick_TicksPerMicroSecond)
#define CONTRAST_HIGH_SLOPE       (- 66)
#define CONTRAST_LOW_SLOPE        (- 66)
#define CUTOFF_LOW_TEMPERATURE     5
#define CUTOFF_HIGH_TEMPERATURE   35


static unsigned long LastTick;
static int InitialContrast;

void Temperature_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Analog_initialise();
		//Contrast_initialise(); // **already done by the boot loader**
		Tick_initialise();
		LastTick = Tick_get();
		InitialContrast = Contrast_get();
	}
}

void Temperature_control(void)
{
	unsigned long ticks = Tick_get();
	if (ticks - LastTick > POLL_TIME_MICROSECONDS) {
		LastTick = ticks;

		Analog_scan();
		int CurrentTemperature = Analog_TemperatureCelcius();

		if (CurrentTemperature < CUTOFF_LOW_TEMPERATURE) {
			int adjust = CONTRAST_LOW_SLOPE * (CurrentTemperature - CUTOFF_LOW_TEMPERATURE);
			Contrast_set_value(InitialContrast + adjust);
		} else if (CurrentTemperature > CUTOFF_HIGH_TEMPERATURE) {
			int adjust = CONTRAST_HIGH_SLOPE * (CurrentTemperature - CUTOFF_HIGH_TEMPERATURE);
			Contrast_set_value(InitialContrast + adjust);
		} else {
			Contrast_set_value(InitialContrast);
		}
	}
}
