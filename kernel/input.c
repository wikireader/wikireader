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

/* wikilib and guilib includes */
#include <guilib.h>
#include <wikilib.h>
#include <input.h>
#include <malloc.h>

/* local includes */
#include "serial.h"
#include "suspend.h"
#include "touchscreen.h"
#include "gpio.h"
#include "msg-output.h"

int wl_input_wait(struct wl_input_event *ev)
{
	/* wl_input_wait() is called from the wikilib mainloop and we will
	 * get here regularily when the system has no other duty. Hence,
	 * the only thing we want to do here is go to sleep - the interrupt
	 * sources are set up and will bring us back to life at some point
	 */

	while (1) {
		if (serial_get_event(ev))
			break;

		if (touchscreen_get_event(ev))
			break;

		if (gpio_get_event(ev))
			break;

		/* we only go to a power saving halt mode if there is no
		 * messages pending */
		if (msg_output_pending())
			asm("halt");
		else
			system_suspend();
	}

	return 0;
}

