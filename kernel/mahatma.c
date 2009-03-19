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
#include <tff.h>

/* local includes */
#include "serial.h"
#include "traps.h"
#include "suspend.h"
#include "gui.h"
#include "msg.h"
#include "touchscreen.h"
#include "regs.h"
#include "gpio.h"

#define VERSION "0.1"

static FATFS fatfs;

int main(void)
{
	/* set the default data pointer */
	asm("xld.w   %r15, __dp");
//	asm("xld.w   %r4, 0x1000");
//	asm("ld.w   %sp, %r4");

	/* machine-specific init */
	gpio_init();
	traps_init();
	serial_init();
	fb_init();

	/* generic init */
	if (f_mount(0, &fatfs) != FR_OK)
		msg(MSG_INFO, "unable to mount FAT filesystem!\n");

	malloc_init();
	wikilib_init();
	guilib_init();

	REG_P1_IOC1 = 0x08;
	REG_P1_P1D = 0x08;

	msg(MSG_INFO, "Mahatma super slim kernel v%s booting.\n", VERSION);
	
	REG_CMU_PROTECT = 0x96;

	/* the next function will loop forever and call wl_input_wait() */
	wikilib_run();

	/* never reached */
	return 0;
}

DWORD get_fattime(void)
{
	/* FIXME: fill this function */
	return 0;
}

void delay(u32 nops)
{
    while (nops--)
        asm("nop");
}

