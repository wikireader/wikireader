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

#include "wikireader.h"
#include "touchscreen.h"
#include "msg-output.h"

#define ASCII_A 				65
#define ASCII_ALPHA_OFFSET 		55
#define ASCII_DIGIT_OFFSET 		48

enum parsing_state {

	PARSING_INIT = 0,
	PARSING_A = 1,
	PARSING_X = 2,
	PARSING_Y = 3,
	PARSING_PRESS = 4,
	PARSING_LAST = 5
};

enum parsing_state ctp_event = PARSING_INIT;

static inline void parsing_init_handler(u8 c);
static inline void parsing_a_handler(u8 c);
static inline void parsing_x_handler(u8 c);
static inline void parsing_y_handler(u8 c);
static inline void parsing_press_handler(u8 c);

static void (*parsing_touch_handling[PARSING_LAST])(u8 c) = {

	&parsing_init_handler,
	&parsing_a_handler,
	&parsing_x_handler,
	&parsing_y_handler,
	&parsing_press_handler,
};

u8 x_buffer[4];
u8 y_buffer[4];
u8 p_buffer[2];

static volatile int touch_state = WL_INPUT_TOUCH_NONE;

static inline void parsing_init_handler(u8 c)
{
	if (c == ASCII_A)
		ctp_event = PARSING_A;
}

static inline void parsing_a_handler(u8 c)
{
	if (c == ASCII_A)
		ctp_event = PARSING_X;
}

static inline void parsing_x_handler(u8 c)
{
	static u8 count = 0;

	if (c < ASCII_A)
		x_buffer[count] = (c - ASCII_DIGIT_OFFSET);
	else
		x_buffer[count] = (c - ASCII_ALPHA_OFFSET);

	if (count == 3){
		ctp_event = PARSING_Y;
		count = 0;
	}
	else
		count++;
}

static inline void parsing_y_handler(u8 c)
 {
	static u8 count = 0;

	if (c < ASCII_A)
		y_buffer[count] = (c - ASCII_DIGIT_OFFSET);
	else
		y_buffer[count] = (c - ASCII_ALPHA_OFFSET);

	if (count == 3){
		ctp_event = PARSING_PRESS;
		count = 0;
	}
	else
		count++;
}

static inline void parsing_press_handler(u8 c)
{
	static u8 count = 0;

	p_buffer[count] = (c - ASCII_DIGIT_OFFSET);

	if (count == 2){
		touch_state  = p_buffer[1];
		ctp_event = PARSING_INIT;
		count = 0;
	}
	else
		count++;
}

static inline int calc_coord(u8 *axis_array)
{
	return ((axis_array[0] << 12) + (axis_array[1] << 8)
						+ (axis_array[2] << 4) + axis_array[3]);
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
		ev->touch_event.x = calc_coord(x_buffer);
		ev->touch_event.y = calc_coord(y_buffer);
		ev->touch_event.value = touch_state;
		msg(MSG_DEBUG, "coord: %d, %d, %d\n\r",
					ev->touch_event.x, ev->touch_event.y, ev->touch_event.value);

		touch_state = WL_INPUT_TOUCH_NONE;
	}

	return 0;
}

void touchscreen_init(void)
{
	init_rs232_ch1();

	REG_INT_ESIF01 |= ESRX1;

	REG_INT_PSI01_PAD |= SERIAL_CH1_INT_PRI_7;
}
