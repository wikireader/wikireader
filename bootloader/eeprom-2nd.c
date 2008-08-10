#include "regs.h"
#include "wikireader.h"
#include "spi.h"
#include "sdcard.h"
#include "misc.h"
#include "fat.h"

#define MEMSIZE (1024 * 1024 * 4)
#define MEMSTART 0x40000


static void boot_from_sdcard(void);

__attribute__((noreturn))
int main(void) 
{
	INIT_PINS();
	EEPROM_CS_HI();
	SDCARD_CS_HI();
	INIT_RS232();

	/* CARDPWR on */
	REG_SRAMC_A0_BSL |= 1 << 1;
	*(volatile unsigned int *) 0x200000 |= 0x1;

//	asm("xld.w   %r15,0x0800");
//	asm("ld.w    %sp,%r15"); //        ; set SP

	asm("xld.w   %r15, 0x10005000");
//	asm("ld.w    %dp,%r15");

	print("Bootloader starting\n");

	/* enable SPI: master mode, no DMA, 8 bit transfers, slowest clock */
	REG_SPI_CTL1 = 0x03 | (7 << 10) | (7 << 4);

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
	int ret;
	char tmp[100];

	if (sdcard_init() < 0)
		return;
	
	if (fat_init(0) < 0)
		return;

	/* jump, just let go! :) */
        ((void (*) (void)) buf) ();
}

