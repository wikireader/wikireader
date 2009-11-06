/*
 * button - driver for keypad buttons
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

#include "event.h"
#include "interrupt.h"
#include "vector.h"
#include "button.h"


#define BUTTON_MASK 0x07

void Button_KeyInterrupt(void) __attribute__((interrupt_handler));
void Button_PowerInterrupt(void) __attribute__((interrupt_handler));


void Button_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		Event_initialise();

		Interrupt_type state = Interrupt_disable();

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

		Vector_set(VECTOR_Port_input_interrupt_3, Button_PowerInterrupt);
		Vector_set(VECTOR_Key_input_interrupt_0, Button_KeyInterrupt);
		Vector_set(VECTOR_Key_input_interrupt_1, Button_KeyInterrupt);

		Interrupt_enable(state);
	}
}


void Button_KeyInterrupt(void)
{
	Interrupt_SaveR15();  // must be first

	REG_INT_FK01_FP03 = FK1 | FK0; // clear interrupt flags

	uint8_t bits = REG_P6_P6D & BUTTON_MASK;
	REG_KINTCOMP_SCPK0 = bits;

	static bool state[3] = {false, false, false};
	int i;
	for (i = 0; i < ARRAY_SIZE(state); ++i) {

		bool pressed = 0 != (bits & (1 << i));
		if (pressed != state[i]) {
			event_t event;
			event.item_type = pressed ? EVENT_BUTTON_DOWN : EVENT_BUTTON_UP;
			event.button.code = i;  // 0=random, 1=search, 2=history
			state[i] = pressed;
			Event_put(&event);
		}
	}
	Interrupt_RestoreR15();  // must be last
}


void Button_PowerInterrupt(void)
{
	Interrupt_SaveR15();  // must be first

	REG_INT_FK01_FP03 = FP3; // clear interrupt flag

	event_t event;
	event.item_type = EVENT_BUTTON_DOWN;
	event.button.code = BUTTON_POWER;
	Event_put(&event);

	Interrupt_RestoreR15();  // must be last
}
