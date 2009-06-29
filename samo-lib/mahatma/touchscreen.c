/*
 * mahatma - a simple kernel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
 * Copyright (c) 2008, 2009 Matt Hsu <matt_hsu@openmoko.org>
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

#include <input.h>
#include <msg.h>
#include <wikilib.h>
#include <regs.h>
#include <delay.h>

#include "samo.h"
#include "touchscreen.h"
#include "irq.h"

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
	bool t;
} ItemType;

static volatile int queue_head;
static volatile int queue_tail;

static volatile ItemType queue[32];


#define BUFFER_FULL(w, r, b) ((((w) + 1) % ARRAY_SIZE(b)) == (r))
#define BUFFER_EMPTY(w, r, b) ((w) == (r))
#define BUFFER_NEXT(p, b) (p) = (((p) + 1) % ARRAY_SIZE(b))



void touchscreen_handler(void)
{
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
				if (!BUFFER_FULL(queue_head, queue_tail, queue)) {
					if (0x01 == c) {
						queue[queue_head].x = x;
						queue[queue_head].y = y;
						queue[queue_head].t = WL_INPUT_TOUCH_DOWN;
						BUFFER_NEXT(queue_head, queue);
					} else if (0x00 == c) {
						queue[queue_head].x = x;
						queue[queue_head].y = y;
						queue[queue_head].t = WL_INPUT_TOUCH_UP;
						BUFFER_NEXT(queue_head, queue);
					}
				}
				touch_state = STATE_WAITING;
				break;
			}

		}
	}
	REG_EFSIF1_STATUS = 0; // clear errors
}

int touchscreen_get_event(struct wl_input_event *ev)
{
	if (!BUFFER_EMPTY(queue_head, queue_tail, queue)) {
		ev->type = WL_INPUT_EV_TYPE_TOUCH;
		ev->touch_event.x = queue[queue_tail].x >> 1;
		ev->touch_event.y = queue[queue_tail].y >> 1;
		ev->touch_event.value = queue[queue_tail].t
			? WL_INPUT_TOUCH_DOWN
			: WL_INPUT_TOUCH_UP;
		BUFFER_NEXT(queue_tail, queue);
		return 1;
	}
	return 0;
}

void touchscreen_init(void)
{
	touch_state = STATE_WAITING;
	x = 0;
	y = 0;

	queue_head = 0;
	queue_tail = 0;

	init_rs232_ch1();

	DISABLE_IRQ();

	// CTP_INIT_Reset_function
	REG_P0_IOC0 |= 0x80;
	REG_P0_P0D  |= 0x80;
	delay_us(20);
	REG_P0_P0D  &= ~0x80;

	REG_INT_ESIF01 |= ESRX1 | ESERR1;

	REG_INT_PSI01_PAD |= SERIAL_CH1_INT_PRI_7;
	ENABLE_IRQ();
}
