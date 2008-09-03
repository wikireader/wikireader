/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "regs.h"
#include "types.h"
#include "wikireader.h"
#include "spi.h"
#include "sdcard.h"
#include "eeprom.h"
#include "misc.h"
#include "fat.h"
#include "elf32.h"

#define MEMSIZE (1024 * 1024 * 4)
#define MEMSTART 0x10000000


static void boot_from_sdcard(void);

__attribute__((noreturn))
int main(void)
{
	init_pins();
	init_rs232();
	init_ram();

	EEPROM_CS_HI();
	SDCARD_CS_HI();

//	asm("xld.w   %r15,0x0800");
//	asm("ld.w    %sp,%r15"); //	; set SP

	asm("xld.w   %r15, 0x1500");
//	asm("ld.w    %dp,%r15");

	print("Bootloader starting\n");

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x03 | (7 << 10);

	boot_from_sdcard();

	/* we we get here, boot_from_sdcard() failed to find a kernel on the
	 * inserted media or there is no media. Thus, we register an
	 * interrupt handler for the SD card insert switch and try again as
	 * soon as a media switch is detected. */

	/* TODO */

	for(;;);
}

static void boot_from_sdcard(void)
{
	u8 *buf = (char *) MEMSTART;

	if (sdcard_init() < 0)
		return;

	if (fat_init(0) < 0)
		return;

	elf_read("KERNEL");

	for (;;);

	print("JUMP!\n");

	/* jump, just let go! :) */
	((void (*) (void)) buf) ();
}

