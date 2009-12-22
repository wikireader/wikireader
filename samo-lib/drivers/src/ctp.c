/*
 * ctp - driver for the touch panel
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

#include <stdio.h>
#include <stdbool.h>
#include <regs.h>
#include <samo.h>
#include <delay.h>

#include "interrupt.h"
#include "tick.h"
#include "ctp.h"

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

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t ticks;
	bool pressed;
} ItemType;

static volatile int CTPwrite;
static volatile int CTPread;

static volatile ItemType CTPbuffer[256];


#define BUFFER_FULL(w, r, b) ((((w) + 1) % ARRAY_SIZE(b)) == (r))
#define BUFFER_EMPTY(w, r, b) ((w) == (r))
#define BUFFER_NEXT(p, b) (p) = (((p) + 1) % ARRAY_SIZE(b))


void CTP_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;

		Tick_initialise();

		touch_state = STATE_WAITING;
		x = 0;
		y = 0;

		CTPwrite = 0;
		CTPread = 0;

		init_rs232_ch1();

		InterruptType s = Interrupt_disable();

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

		Interrupt_enable(s);
	}
}


void CTP_interrupt(void)
{
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
				if (!BUFFER_FULL(CTPwrite, CTPread, CTPbuffer)) {
					if (0x01 == c) {
						CTPbuffer[CTPwrite].x = x;
						CTPbuffer[CTPwrite].y = y;
						CTPbuffer[CTPwrite].pressed = true;
						CTPbuffer[CTPwrite].ticks = Tick_get();
						BUFFER_NEXT(CTPwrite, CTPbuffer);
					} else if (0x00 == c) {
						CTPbuffer[CTPwrite].x = x;
						CTPbuffer[CTPwrite].y = y;
						CTPbuffer[CTPwrite].pressed = false;
						CTPbuffer[CTPwrite].ticks = Tick_get();
						BUFFER_NEXT(CTPwrite, CTPbuffer);
					}
				}
				touch_state = STATE_WAITING;
				break;
			}

		}
	}
	REG_EFSIF1_STATUS = 0; // clear errors
}


void CTP_flush(void)
{
	InterruptType s = Interrupt_disable();

	touch_state = STATE_WAITING;
	x = 0;
	y = 0;

	CTPwrite = 0;
	CTPread = 0;

	Interrupt_enable(s);
}


bool CTP_available(void)
{
	return !BUFFER_EMPTY(CTPwrite, CTPread, CTPbuffer);
}


bool CTP_get(int *x, int *y, bool *pressed, unsigned long *ticks)
{
	if (BUFFER_EMPTY(CTPwrite, CTPread, CTPbuffer)) {
		return false;
	}

	*x = CTPbuffer[CTPread].x;
	*y = CTPbuffer[CTPread].y;
	*pressed = CTPbuffer[CTPread].pressed;
	*ticks = CTPbuffer[CTPread].ticks;
	BUFFER_NEXT(CTPread, CTPbuffer);
	return true;
}
