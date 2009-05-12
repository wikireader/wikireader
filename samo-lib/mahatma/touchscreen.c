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

#include "samo.h"
#include "touchscreen.h"
#include "irq.h"

#define AA_PREFIX  		0xaa

enum parsing_state {

	PARSING_INIT = 0,
	PARSING_X = 1,
	PARSING_Y = 2,
	PARSING_PRESS = 3,
	PARSING_LAST = 4
};

enum parsing_state ctp_event = PARSING_INIT;

static inline void parsing_init_handler(u8 c);
static inline void parsing_x_handler(u8 c);
static inline void parsing_y_handler(u8 c);
static inline void parsing_press_handler(u8 c);

static void (*parsing_touch_handling[PARSING_LAST])(u8 c) = {

	&parsing_init_handler,
	&parsing_x_handler,
	&parsing_y_handler,
	&parsing_press_handler,
};

u8 x_buffer[2];
u8 y_buffer[2];

static volatile int touch_state = WL_INPUT_TOUCH_NONE;

static inline void parsing_init_handler(u8 c)
{
	if (c == AA_PREFIX)
		ctp_event = PARSING_X;
}

static inline void parsing_x_handler(u8 c)
{
	static u8 count = 0;

	x_buffer[count] = c;

	if (count == 1){
		ctp_event = PARSING_Y;
		count = 0;
	}
	else
		count++;
}

static inline void parsing_y_handler(u8 c)
 {
	static u8 count = 0;

	y_buffer[count] = c;

	if (count == 1){
		ctp_event = PARSING_PRESS;
		count = 0;
	}
	else
		count++;
}

static inline void parsing_press_handler(u8 c)
{
	touch_state  = c;
	ctp_event = PARSING_INIT;
}

static inline int calc_coord(u8 *axis_array)
{
	/*
	 * the output resolution is 480 * 416,
	 * here we divide 2 as the real resolution
	 */
    return (((axis_array[0] << 8) + axis_array[1]) >> 1);
}

void touchscreen_parsing_packets()
{
	while (REG_EFSIF1_STATUS & RDBFx) {
		u8 c = REG_EFSIF1_RXD;
		(*parsing_touch_handling[ctp_event])(c);
	}
}

int touchscreen_get_event(struct wl_input_event *ev)
{
	if (touch_state != WL_INPUT_TOUCH_NONE){
		ev->type = WL_INPUT_EV_TYPE_TOUCH;

		if (touch_state) {
			ev->touch_event.x = calc_coord(x_buffer);
			ev->touch_event.y = calc_coord(y_buffer);
		}

		ev->touch_event.value = touch_state;
		touch_state = WL_INPUT_TOUCH_NONE;
		return 1;
	}

	return 0;
}

void touchscreen_init(void)
{
	init_rs232_ch1();

	DISABLE_IRQ();
	REG_INT_ESIF01 |= ESRX1;

	REG_INT_PSI01_PAD |= SERIAL_CH1_INT_PRI_7;
	ENABLE_IRQ();
}
