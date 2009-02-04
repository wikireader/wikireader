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

static unsigned int gpio_state;
static unsigned int last_state;

void gpio_input(int pin, int state)
{
	if (pin >= 32)
		return;

	if (state)
		gpio_state |= 1 << pin;
	else
		gpio_state &= ~(1 << pin);
}

int gpio_get_event(struct wl_input_event *ev)
{
	unsigned int i, changed = gpio_state ^ last_state;

//	if (!changed)
		return 0;

	for (i = 0; i < 32; i++) {
		if (!(changed & (1 << i)))
			continue;

		ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
		ev->key_event.keycode = WL_INPUT_KEY_SEARCH + i;
		ev->key_event.value = 1;
		last_state ^= (1 << i);
		return 1;
	}

	return 0;
}

void gpio_init(void)
{
	/* wakeup sources, turn keyboard control 0 to wakeup */
	REG_KINTCOMP_SCPK0 = 0x1f;
	REG_KINTCOMP_SMPK0 = 0x10;
	REG_KINTSEL_SPPK01 = 0x40;
	REG_INT_EK01_EP0_3 = 0x10;

	gpio_state = 0;
	last_state = 0;
}

