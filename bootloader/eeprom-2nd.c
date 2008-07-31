#include <efs.h>
#include "regs.h"
#include "wikireader.h"

#define MEMSIZE (1024 * 1024 * 4)
#define MEMSTART 0x40000


static void boot_from_sdcard(void);
static void print(const char *txt);

int main(void)
{
	INIT_PINS();
	EEPROM_CS_HI();
	SDCARD_CS_HI();

	/* serial line 0: 8-bit async, no parity, internal clock, 1 stop bit */
	REG_EFSIF0_CTL = 0xc3;

	/* DIVMD = 1/8, General I/F mode */
	REG_EFSIF0_IRDA = 0x10;

	/* by default MCLKDIV = 0 which means that the internal MCLK is OSC/1,
	 * where OSC = OSC3 as OSCSEL[1:0] = 00b
	 * Hence, MCLK is 48MHz */

	/* set up baud rate timer reload data */
	/* 
	 * BRTRD = ((F[brclk] * DIVMD) / (2 * bps)) - 1;
	 * where
	 * 	F[brclk] = 48MHz
	 * 	DIVMD = 1/8
	 *	bps = 38400
	 *
	 *   = 77
	 */

	REG_EFSIF0_BRTRDL = 77 & 0xff;
	REG_EFSIF0_BRTRDM = 77 >> 8;

	/* baud rate timer: run! */
	REG_EFSIF0_BRTRUN = 0x01;

	print("Bootloader alive and kicking.\n");

	boot_from_sdcard();

	/* we we get here, boot_from_sdcard() failed to find a kernel on the
	 * inserted media or there is no media. Thus, we register an 
	 * interrupt handler for the SD card insert switch and try again as
	 * soon as a media switch is detected. */

	/* TODO */

	return 0;
}

static void boot_from_sdcard(void)
{
#if 0
        EmbeddedFileSystem efs;
        EmbeddedFile file;
        char *buf = (char *) MEMSTART;

        debug_init();

        if (efs_init(&efs,0))
		return;

        if (file_fopen(&file, &efs.myFs, "kernel", 'r'))
		return;

	file_read(&file, MEMSIZE, buf);

        file_fclose (&file);
        fs_umount (&efs.myFs);

	/* jump, just let go! :) */
        ((void (*) (void)) buf) ();
#endif
}

static void print(const char *txt)
{
	int delay = 0xff;
	
	while (txt && *txt) {
		REG_EFSIF0_TXD = *txt;
		do {} while (REG_EFSIF0_STATUS & (1 << 5));

		if (*txt == '\n') {
			REG_EFSIF0_TXD = '\r';
			do {} while (REG_EFSIF0_STATUS & (1 << 5));
		}

		while (delay--)
			asm("nop");

		txt++;
	}
}

