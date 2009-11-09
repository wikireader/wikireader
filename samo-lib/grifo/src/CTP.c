/*
 * ctp - driver for the touch panel
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

#include "interrupt.h"
#include "vector.h"
#include "event.h"
#include "CTP.h"

typedef enum {
	STATE_WAITING,
	STATE_X_HIGH,
	STATE_X_LOW,
	STATE_Y_HIGH,
	STATE_Y_LOW,
	STATE_TOUCH,
} StateType;

static volatile StateType touch_state;

static volatile uint32_t x, y;

static volatile bool touch;


void CTP_interrupt(void) __attribute__((interrupt_handler));


void CTP_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Vector_initialise();
		Event_initialise();

		touch_state = STATE_WAITING;
		x = 0;
		y = 0;
		touch = false;

		init_rs232_ch1();

		Interrupt_type state = Interrupt_disable();

		// CTP_INIT_Reset_function
		REG_P0_IOC0 |= 0x80;
		REG_P0_P0D  |= 0x80;
		delay_us(20);
		REG_P0_P0D  &= ~0x80;

		REG_INT_ESIF01 |= ESRX1 | ESERR1;

		REG_INT_PSI01_PAD |= SERIAL_CH1_INT_PRI_7;
		int i;
		for (i = 0; i < 6; ++i) {
			// flush FIFO
			register uint32_t c = REG_EFSIF1_RXD;
			(void)c;
		}

		REG_INT_FSIF01 = FSTX1 | FSRX1 | FSERR1; // clear the interrupt

		Vector_set(VECTOR_Serial_interface_Ch_1_Receive_error, CTP_interrupt);
		Vector_set(VECTOR_Serial_interface_Ch_1_Receive_buffer_full, CTP_interrupt);
		Interrupt_enable(state);
	}
}


void CTP_interrupt(void)
{
	Interrupt_SaveR15();  // must be first

	REG_INT_FSIF01 = FSRX1 | FSERR1; // clear the interrupt

	while (0 != (REG_EFSIF1_STATUS & RDBFx)) {
		register uint32_t c = REG_EFSIF1_RXD;

		if (0xaa == c) {
			touch_state = STATE_X_HIGH;
		} else if (0 != (0x80 & c) && (0xff != c)) {
			touch_state = STATE_WAITING;
		} else {
			switch (touch_state) {
			case STATE_WAITING:
				break;
			case STATE_X_HIGH:
				if (0xff != c) {
					x = c << 7;
				}
				++touch_state;
				break;
			case STATE_X_LOW:
				if (0xff != c) {
					x |= c;
				}
				++touch_state;
				break;
			case STATE_Y_HIGH:
				if (0xff != c) {
					y = c << 7;
				}
				++touch_state;
				break;
			case STATE_Y_LOW:
				if (0xff != c) {
					y |= c;
				}
				++touch_state;
				break;
			case STATE_TOUCH:
			{
				event_t e;
				e.touch.x = x;
				e.touch.y = y;
				if (0x01 == c) {
					e.item_type = touch ? EVENT_TOUCH_MOTION : EVENT_TOUCH_DOWN;
					touch = true;
					Event_put(&e);
				} else if (0x00 == c) {
					e.item_type = EVENT_TOUCH_UP;
					touch = false;
					Event_put(&e);
				}
				touch_state = STATE_WAITING;
				break;
			}

			}
		}
	}
	REG_EFSIF1_STATUS = 0; // clear errors

	Interrupt_RestoreR15();  // must be last
}
