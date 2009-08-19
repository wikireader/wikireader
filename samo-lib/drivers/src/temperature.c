/*
 * temperature controller driver
 * Copyright (c) 2009 Openmoko
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


static unsigned long LastTick;

void Temperature_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Analog_initialise();
		Contrast_initialise();
		Tick_initialise();
		LastTick = Tick_get();
	}
}

void Temperature_control(void)
{
	int ticks = Tick_get();
	if (ticks - LastTick > 75 * 1000 * Tick_TicksPerMicroSecond) {
		LastTick = ticks;

		Analog_scan();
		int CurrentTemperature = Analog_TemperatureCelcius();

		// compute demand milivolts from current temperature
		int DemandMilliVolts = 21079;
		if (CurrentTemperature > 25) {
			DemandMilliVolts -= 46 * (CurrentTemperature - 25);
		}

		// simple integral controller
		static int ControllerIntegral = 0;

		int ErrorMilliVolts = DemandMilliVolts - Analog_ContrastMilliVolts();

		// only update controller if error is too large
		// to avoid cycling the value up and down because of
		// limited resolution on the analog inputs
		if (abs(ErrorMilliVolts) >= 50) {
			ControllerIntegral += ErrorMilliVolts;
			// maintain integran with 4 binary fraction bits
			Contrast_set(Contrast_default + (ControllerIntegral / 16));
		}
#if 0
		extern void Debug_PutString(const char *s);
		extern void Debug_PutHex(int h);
		Debug_PutString("  t=");
		Debug_PutHex(CurrentTemperature);
		Debug_PutString("  v=");
		Debug_PutHex(DemandMilliVolts);
		Debug_PutString("  e=");
		Debug_PutHex(ErrorMilliVolts);
		Debug_PutString("  i=");
		Debug_PutHex(ControllerIntegral);
		Debug_PutString("\n");
#endif
	}
}

