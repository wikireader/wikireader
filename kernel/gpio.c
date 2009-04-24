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
#include <msg.h>
#include <wikireader.h>

#define N_PINS 3

static unsigned char gpio_state;
static unsigned char last_state;

void gpio_irq(void)
{
	/* the current gpio state is our new comparison reference */
	gpio_state = get_key_state();
	REG_KINTCOMP_SCPK0 = gpio_state;
}

int gpio_get_event(struct wl_input_event *ev)
{
	unsigned int i, changed = gpio_state ^ last_state;

	if (!changed)
		return 0;

	for (i = 0; i < N_PINS; i++) {
		if (!(changed & (1 << i)))
			continue;

		ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
		ev->key_event.value = !!(gpio_state & (1 << i));
		ev->key_event.keycode = WL_BUTTON_BASE + (i * 2) + ev->key_event.value;
		last_state ^= (1 << i);
		return 1;
	}

	return 0;
}

void gpio_init(void)
{
	prepare_keys();
	gpio_state = 0;
	last_state = 0;
}

