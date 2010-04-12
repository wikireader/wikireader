/*
 * Event hanndling
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Christopher Hall <hsw@openmoko.com>
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

#include <stdlib.h>
#include <guilib.h>
#include <wikilib.h>
#include <input.h>

#include <regs.h>
#include <samo.h>
#include <suspend.h>
//#include <temperature.h>
#include <ctp.h>

#include "serial.h"
#include "touchscreen.h"
#include "gpio.h"

// the / 32 is because the suspend routine operates with MCLK=CLK/32
#define TIMEOUT_VALUE (MCLK / 32 * SUSPEND_AUTO_POWER_OFF_SECONDS)
#if TIMEOUT_VALUE > 0x3fffffff
#error "SUSPEND_AUTO_POWER_OFF_SECONDS is too large"
#endif


bool wl_input_event_pending(void)
{
	return serial_event_pending() || touchscreen_event_pending() || gpio_event_pending();
}

void wl_input_reset_random_key(void)
{
	struct wl_input_event ev;
	
	while (gpio_peek_event(&ev) && ev.key_event.keycode == WL_INPUT_KEY_RANDOM)
	{
		gpio_get_event(&ev);
		if (ev.key_event.value) {
			CTP_flush();  // flush and reset the CTP
		}
	}
}

void wl_input_wait(struct wl_input_event *ev, int sleep)
{
	/* wl_input_wait() is called from the wikilib mainloop and we will
	 * get here regularily when the system has no other duty. Hence,
	 * the only thing we want to do here is go to sleep - the interrupt
	 * sources are set up and will bring us back to life at some point
	 */

	while (1) {
		if (serial_get_event(ev)) {
			break;
		}

		if (touchscreen_get_event(ev)) {
			break;
		}

		if (gpio_get_event(ev)) {
			if (ev->key_event.value) {
				CTP_flush();  // flush and reset the CTP
			}
			break;
		}

		/* no power saving return */
		if (!sleep) {
			ev->type = -1;
			break;
		}

/* the timers needed for profiling don't work with suspend enabled */
#if !PROFILER_ON
		suspend(TIMEOUT_VALUE);
#endif
//		Temperature_control();

	}
}

