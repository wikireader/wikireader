#include <efs.h>
#include "regs.h"
#include "wikireader.h"

#define MEMSIZE (1024 * 1024 * 4)
#define MEMSTART 0x40000


static void boot_from_sdcard(void);
static void print(const char *txt);

__attribute__((noreturn))
int main(void) 
{

	const char *bla1 = "gagagagaga\n";
	const char *bla2 = "12323452345\n";

	INIT_PINS();
	EEPROM_CS_HI();
	SDCARD_CS_HI();
	INIT_RS232();

//	asm("xld.w   %r15,0x0800");
//	asm("ld.w    %sp,%r15"); //        ; set SP

	asm("xld.w   %r15, 0x10005000");
//	asm("ld.w    %dp,%r15");

	print("Bootloader alive and kicking.\n");
	print(bla2);
	print(bla1);
	print(bla2);

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
#if 0
        EmbeddedFileSystem efs;
        EmbeddedFile file;
        char *buf = (char *) MEMSTART;

//        debug_init();
print("cp1\n");
        if (efs_init(&efs,0))
		return;
print("cp2\n");

        if (file_fopen(&file, &efs.myFs, "kernel", 'r'))
		return;
print("cp3\n");

	file_read(&file, MEMSIZE, buf);

        file_fclose (&file);
        fs_umount (&efs.myFs);

	/* jump, just let go! :) */
        ((void (*) (void)) buf) ();
#endif
}

static void print(const char *txt)
{
	while (txt && *txt) {
		int delay = 0xff;

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

