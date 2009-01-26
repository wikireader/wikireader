/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>
    Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>

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
#include "eeprom.h"
#include "misc.h"
#include "elf32.h"
#include "lcd.h"

#define KERNEL "/KERNEL"

static void power_tests();

__attribute__((noreturn))
int main(void)
{
	int i;
	init_pins();
	init_rs232();
	init_ram();

	EEPROM_CS_HI();
	SDCARD_CS_HI();
	
	/* value of default data area is hard-coded in this case */
	asm("xld.w   %r15, __dp");

	//print("Bootloader starting\n");
	/* set FPT1 to another gpio, make it falling edge trieggered */
	REG_PINTSEL_SPT03 |= 0xC;
	REG_PINTEL_SEPT07 |= 0x2;
	REG_PINTPOL_SPP07 &= ~0x2;

	/* some debug helper... P64 as output */
	/* set P64 as output */
	REG_P6_IOC6 |= 0x10;

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x03 | (7 << 10) | (1 << 4);
	
	/* attempt to boot */
	print_u32(elf_exec(KERNEL) * -1);


	/* load the 'could not boot from SD card' image */
	init_lcd();
	eeprom_load(0x10000, (u8 *) LCD_VRAM, LCD_VRAM_SIZE);
#if BOARD_PROTO1
	{
		int i;
		for (i = LCD_VRAM; i < LCD_VRAM + LCD_VRAM_SIZE; ++i)
			*(char *) i ^= 0xff;
	}
#endif
        print("\n");
        print("bootloader 4\n");

	/* if we get here, boot_from_sdcard() failed to find a kernel on the
	 * inserted media or there is no media. Thus, we register an
	 * interrupt handler for the SD card insert switch and try again as
	 * soon as a media switch is detected. */

	/* TODO */
#ifdef POWER_MANAGEMENT
	power_tests();
#endif
}

#if POWER_MANAGEMENT
#define READ_AND_CLEAR_CAUSE(REG) \
    REG = 0xff;

static void interrupt_handler() {
	READ_AND_CLEAR_CAUSE(REG_INT_FK01_FP03);
	REG_P6_P6D ^= 0x10;
	asm("reti");
}

static void power_tests() {
	int i;
        unsigned int *interrupt_vector = 0x0;
        for (i = 0; i < 107; ++i)
            interrupt_vector[i] = interrupt_handler;
        asm("ld.w %%ttbr, %0" :: "r"(interrupt_vector));

	/* WAKEUP=1 */
	REG_CMU_PROTECT = 0x96;
	REG_CMU_OPT |= 0x1;

	/* wakeup sources, turn keyboard control 0 to wakeup */
	REG_KINTCOMP_SCPK0 = 0x1f;
	REG_KINTCOMP_SMPK0 = 0x10;
	REG_KINTSEL_SPPK01 = 0x40;
	REG_INT_EK01_EP0_3 = 0x10;

	REG_P6_P6D &= ~0x10;

	for(;;) {
		unsigned char data;
		READ_AND_CLEAR_CAUSE(REG_INT_FSIF01);
		READ_AND_CLEAR_CAUSE(REG_INT_F16T01);
                READ_AND_CLEAR_CAUSE(REG_INT_FK01_FP03);
		READ_AND_CLEAR_CAUSE(REG_INT_FDMA);
		READ_AND_CLEAR_CAUSE(REG_INT_F16T23);
		READ_AND_CLEAR_CAUSE(REG_INT_F16T45);
		READ_AND_CLEAR_CAUSE(REG_INT_FP47_FRTC_FAD);
		READ_AND_CLEAR_CAUSE(REG_INT_FLCDC);
		READ_AND_CLEAR_CAUSE(REG_INT_FSIF2_FSPI);

		/* set SDCLKE enable.. */
		REG_P2_03_CFP = 0x01;
		REG_P2_47_CFP = 0;

		/* for HALT D24 needs to be set even if it is R only.
		 * disable the SDRAM Controller and disable the SDRAM clock
		 * but keep the internal RAM clock on */
		REG_SDRAMC_REF |= (1<<25);
		REG_SDRAMC_APP = 0;
		REG_CMU_GATEDCLK0 &= ~0x70;

		asm("slp");
                delay(10000);
                print("woke up\n");

	
		/* restore */
		REG_CMU_GATEDCLK0 |= 0x70;
		REG_SDRAMC_APP |= 0x2;
        }
}
#endif


