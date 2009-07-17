/*
 * button - driver for keypad buttons
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

#include <stdio.h>
#include <stdbool.h>
#include <regs.h>
#include <irq.h>
#include <samo.h>

#include "button.h"


#define BUTTON_MASK 0x07


static struct {
	ButtonType button;
	bool pressed;
} ButtonBuffer[16];

static unsigned int ButtonRead;
static unsigned int ButtonWrite;

#define BUFFER_FULL(w, r, b) ((((w) + 1) % ARRAY_SIZE(b)) == (r))
#define BUFFER_EMPTY(w, r, b) ((w) == (r))
#define BUFFER_NEXT(p, b) (p) = (((p) + 1) % ARRAY_SIZE(b))


void Button_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {

		DISABLE_IRQ();

		ButtonRead = 0;
		ButtonWrite = 0;

		REG_INT_PK01L = 0x66;  // key priority

		REG_INT_FK01_FP03 =    // clear key interrupts
			FK1 |
			FK0 |
			FP3 |
			FP2 |
			FP1 |
			FP0 |
			0;

		REG_KINTCOMP_SCPK0 = 0x00;         // comparison = all buttons off

		REG_KINTCOMP_SMPK0 = BUTTON_MASK;  // enable 3 buttons

		REG_KINTSEL_SPPK01 = 0x04;         // use P60..P62

		REG_INT_PP01L = 0x06;              // P0 prority (for power button)

		REG_PINTSEL_SPT03 = 0x00;          // select P03 interrupt (the power button

		REG_PINTPOL_SPP07 = SPPT3;         // P03 active high

		REG_PINTEL_SEPT07 = SEPT3;         // P03 edge trigered

		REG_INT_EK01_EP03 = EK0 | EP3;     // enable KINT0 and Port0

		ENABLE_IRQ();
	}
}


void Button_KeyInterrupt(void)
{
	REG_INT_FK01_FP03 = FK1 | FK0; // clear interrupt flags

	uint8_t bits = REG_P6_P6D & BUTTON_MASK;
	REG_KINTCOMP_SCPK0 = bits;

	static bool state[3] = {false, false, false};

	int i;
	for (i = 0; i < ARRAY_SIZE(state); ++i) {

		bool pressed = 0 != (bits & (1 << i));
		if (pressed != state[i]) {

			ButtonBuffer[ButtonWrite].button = i;
			ButtonBuffer[ButtonWrite].pressed = pressed;
			state[i] = pressed;

			if (!BUFFER_FULL(ButtonWrite, ButtonRead, ButtonBuffer)) {
				BUFFER_NEXT(ButtonWrite, ButtonBuffer);
			}
		}
	}
}

void Button_PowerInterrupt(void)
{
	REG_INT_FK01_FP03 = FP3; // clear interrupt flag

	ButtonBuffer[ButtonWrite].button = Button_PowerKey;
	ButtonBuffer[ButtonWrite].pressed = true;

	if (!BUFFER_FULL(ButtonWrite, ButtonRead, ButtonBuffer)) {
		BUFFER_NEXT(ButtonWrite, ButtonBuffer);
	}
}

bool Button_available(void)
{
	return !BUFFER_EMPTY(ButtonWrite, ButtonRead, ButtonBuffer);
}

bool Button_get(ButtonType *button, bool *pressed)
{
	if (BUFFER_EMPTY(ButtonWrite, ButtonRead, ButtonBuffer)) {
		return false;
	}

	*button = ButtonBuffer[ButtonRead].button;
	*pressed = ButtonBuffer[ButtonRead].pressed;
	BUFFER_NEXT(ButtonRead, ButtonBuffer);
	return true;
}
