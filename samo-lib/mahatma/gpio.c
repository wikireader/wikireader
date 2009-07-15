/*
 * mahatma - a simple kernel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
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
#include <input.h>
#include <regs.h>
#include <irq.h>
#include <samo.h>
#include <wikilib.h>

#include "gpio.h"


#if BOARD_SAMO_Ax
static const int keymap[] = {
	WL_INPUT_KEY_RANDOM,
	WL_INPUT_KEY_SEARCH,
	WL_INPUT_KEY_HISTORY,
};
#else
static const int keymap[] = {
	WL_INPUT_KEY_SEARCH,
	WL_INPUT_KEY_HISTORY,
	WL_INPUT_KEY_RANDOM,
};
#endif

static volatile uint8_t gpio_state;
static volatile uint8_t last_state;

void gpio_irq(void)
{
	gpio_state =  REG_P6_P6D & 0x7;
	REG_KINTCOMP_SCPK0 = gpio_state;
}

int gpio_get_event(struct wl_input_event *ev)
{
	unsigned int i;
	uint8_t keys = gpio_state;
	uint8_t changed = keys ^ last_state;

	if (0 != changed) {
		for (i = 0; i < ARRAY_SIZE(keymap); i++) {
			uint8_t bit = 1 << i;
			if (0 != (changed & bit)) {
				ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
				ev->key_event.keycode = keymap[i];
				ev->key_event.value = 0 != (keys & bit);
				last_state ^= bit;
				return 1;
			}
		}
	}

	return 0;
}

void gpio_init(void)
{
	DISABLE_IRQ();

	// initial comparison is all buttons open
	REG_KINTCOMP_SCPK0 = 0x00;

	// enable mask for three buttons
	REG_KINTCOMP_SMPK0 = 0x07;

	// select P60/P61/P62
	REG_KINTSEL_SPPK01 = 0x04;

	// only interested in KINT0 source
	REG_INT_EK01_EP0_3 = 0x10;

	// assume all keys are initially up
	gpio_state = 0;
	last_state = 0;

	ENABLE_IRQ();
}
