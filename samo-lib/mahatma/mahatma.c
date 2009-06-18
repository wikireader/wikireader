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
#include <stdlib.h>
#include <malloc-simple.h>
#include <tff.h>
#include <profile.h>

/* local includes */
#include "msg-output.h"
#include "serial.h"
#include "traps.h"
#include "gui.h"
#include "msg.h"
#include "touchscreen.h"
#include "regs.h"
#include "gpio.h"
#include "gui.h"

#define VERSION "0.1"

static FATFS fatfs;


int main(void)
{
	/* set the initial stack and data pointers */
	asm volatile (
		"\txld.w\t%r15, __MAIN_STACK\n"
		"\tld.w\t%sp, %r15\n"
		"\txld.w\t%r15, __dp\n"
		"\tld.w\t%r4, 0\n"
		"\tld.w\t%psr, %r4\n"
		);

	/* machine-specific init */
	gpio_init();
	traps_init();
	msg_init();

	touchscreen_init();
	msg(MSG_INFO, "Starting\n");

	fb_init();

	/* generic init */
	if (f_mount(0, &fatfs) != FR_OK)
		msg(MSG_INFO, "unable to mount FAT filesystem!\n");

	malloc_init_simple();
	wikilib_init();
	guilib_init();
	profile_init();

	msg(MSG_INFO, "Mahatma super slim kernel v%s\n", VERSION);

	/* the next function will loop forever and call wl_input_wait() */
	wikilib_run();

	/* never reached */
	return 0;
}
