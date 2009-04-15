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

//#define POWER_MANAGEMENT 1

#ifdef POWER_MANAGEMENT
static void power_tests();
#endif

__attribute__((noreturn))
int main(void)
{
	init_pins();
	init_rs232_ch0();
	init_ram();

	EEPROM_CS_HI();
	SDCARD_CS_HI();

	/* value of default data area is hard-coded in this case */
	asm("xld.w   %r15, __dp");

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x03 | (7 << 10) | (1 << 4);

	/* attempt to boot */
#ifndef POWER_MANAGEMENT
	print_u32(elf_exec(KERNEL) * -1);
#endif

	/* load the 'could not boot from SD card' image */
	init_lcd();
	eeprom_load(0x10000, (u8 *) LCD_VRAM, LCD_VRAM_SIZE);

#if LCD_INVERTED
	{
		int i;
		for (i = LCD_VRAM; i < LCD_VRAM + LCD_VRAM_SIZE; ++i)
			*(char *) i ^= 0xff;
	}
#endif

	/* if we get here, boot_from_sdcard() failed to find a kernel on the
	 * inserted media or there is no media. Thus, we register an
	 * interrupt handler for the SD card insert switch and try again as
	 * soon as a media switch is detected. */

	/* TODO */
#ifdef POWER_MANAGEMENT
	power_tests();
#endif

	while(1)
		;
}

#if POWER_MANAGEMENT
#define READ_AND_CLEAR_CAUSE(REG) \
    REG = 0xff;

#include "ramtest.h"

#define WAIT_FOR_CONDITION(cond)	\
	do { asm("nop"); } while(!(cond))

static void power_tests()
{
	int i;

	/* WAKEUP=1 */
	REG_CMU_PROTECT = 0x96;
//	REG_CMU_OPT |= 0x1;
	REG_CMU_PROTECT = 0;

	/* wakeup sources, turn keyboard control 0 to wakeup */
	REG_KINTCOMP_SCPK0 = 0x1f;
	REG_KINTCOMP_SMPK0 = 0x10;
	REG_KINTSEL_SPPK01 = 0x40;
	REG_INT_EK01_EP0_3 = 0x10;

	REG_P6_P6D &= ~0x10;

	ram_write();
	ram_read();

	for(;;) {
		unsigned char data;
#if 1
		READ_AND_CLEAR_CAUSE(REG_INT_FSIF01);
		READ_AND_CLEAR_CAUSE(REG_INT_F16T01);
		READ_AND_CLEAR_CAUSE(REG_INT_FDMA);
		READ_AND_CLEAR_CAUSE(REG_INT_F16T23);
		READ_AND_CLEAR_CAUSE(REG_INT_F16T45);
		READ_AND_CLEAR_CAUSE(REG_INT_FP47_FRTC_FAD);
		READ_AND_CLEAR_CAUSE(REG_INT_FLCDC);
		READ_AND_CLEAR_CAUSE(REG_INT_FSIF2_FSPI);
#endif
		READ_AND_CLEAR_CAUSE(REG_INT_FK01_FP03);

		/* enable write access to clock control registers */
		REG_CMU_PROTECT = 0x96;

		/* send the SDRAM to its self-refresh mode (which disables the clock) */
		REG_SDRAMC_REF = (1 << 23) | (0x7f << 16);

		/* wait for the SELDO bit to be asserted */
		WAIT_FOR_CONDITION(REG_SDRAMC_REF & (1 << 25));

		/* switch off the SDRAMC application core */
		REG_SDRAMC_APP = 0;
		REG_CMU_GATEDCLK0 &= ~0x70;

		/* release the SDRAMC pin functions */
		REG_P2_03_CFP = 0x01;
		REG_P2_47_CFP = 0x00;

		//REG_CMU_GATEDCLK1 = (1 << 29) | (1 << 28) | (1 << 27) | (1 << 19) | (1 << 8);
		REG_CMU_GATEDCLK1 = 0x3f08002f;
		REG_CMU_GATEDCLK0 &= ~(1 << 1);

		/* disable clocks we don't need in HALT mode */
		REG_CMU_CLKCNTL = (0xa << 24) | (8 << 16) | (1 << 12) | (1 << 1);

		/* write protect CMU registers */
		REG_CMU_PROTECT = 0;

		/*********************************************************/
		asm("halt");
		/*********************************************************/

		REG_CMU_PROTECT = 0x96;

		/* restore clock setup */
		REG_CMU_CLKCNTL = 0x00770002;

		/* re-enable the SDRAMC pin functions */
		REG_P2_03_CFP = 0x55;
		REG_P2_47_CFP = 0x55;

		/* re-enable all the clocks */
		REG_CMU_GATEDCLK1 = 0x3f0fffff;
		REG_CMU_GATEDCLK0 |= (1 << 1);
		REG_CMU_PROTECT = 0;

		delay(100);
		print("woke up\n");
		init_ram();
		ram_read();
	}
}
#endif

