#include "regs.h"
#include "wikireader.h"
#include "spi.h"
#include "sdcard.h"
#include "eeprom.h"
#include "misc.h"
#include "fat.h"

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

	/* CARDPWR on */
	REG_SRAMC_A0_BSL |= 1 << 1;
	*(volatile unsigned int *) 0x200000 |= 0x1;

//	asm("xld.w   %r15,0x0800");
//	asm("ld.w    %sp,%r15"); //        ; set SP

	asm("xld.w   %r15, 0x1000");
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
        char *buf = (char *) MEMSTART;

	if (sdcard_init() < 0)
		return;

	if (fat_init(0) < 0)
		return;

	if (fat_read_file("TEST", buf, MEMSIZE) < 0)
		return;

	print("DUMP:\n");
	hex_dump(buf, 1024);
	//print(buf);

	print("JUMP!\n");

	/* jump, just let go! :) */
        ((void (*) (void)) buf) ();
}

