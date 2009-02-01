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
#include "traps.h"
#include "suspend.h"
#include "msg.h"
#include "touchscreen.h"
#include "regs.h"

#define VERSION "0.1"

int wl_input_wait(struct wl_input_event *ev)
{
	/* wl_input_wait() is called from the wikilib mainloop and we will
	 * get here regularily when the system has no other duty. Hence,
	 * the only thing we want to do here is go to sleep - the interrupt
	 * sources are set up and will bring us back to life at some point
	 */
	
	while (1) {
		system_suspend();

		/* check whether there was any event in the system. If not,
		 * just go back to halt mode */
		if (serial_get_event(ev))
			break;

		if (touchscreen_get_event(ev))
			break;
	}

	return 0;
}

int main(void)
{
	/* set the default data pointer */
	//asm("xld.w   %r15, __dp");
	asm("xld.w   %r15, 0x10000000");
	REG_EFSIF0_TXD = '?';

	/* machine-specific init */
//	traps_init();
	serial_init();

	/* generic init */
	malloc_init();
	wikilib_init();
	guilib_init();

	msg(MSG_INFO, "Mahatma super slim kernel v%s booting.", VERSION);

	/* the next function will loop forever and call wl_input_wait() */
	wikilib_run();

	/* never reached */
	return 0;
}

