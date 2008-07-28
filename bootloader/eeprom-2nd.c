#include <efs.h>
#include "regs.h"
#include "wikireader.h"

#define MEMSIZE (1024 * 1024 * 4)
#define MEMSTART 0x40000

void boot_from_sdcard(void)
{
        EmbeddedFileSystem efs;
        EmbeddedFile file;
        char *buf = (char *) MEMSTART;

        debug_init();

        if (efs_init(&efs,0))
		return;

        if (file_fopen(&file, &efs.myFs, "kernel",'r'))
		return;

	file_read(&file, MEMSIZE, buf);

        file_fclose (&file);
        fs_umount (&efs.myFs);

	/* jump, just let go! :) */
        ((void (*) (void)) buf) ();
}

int main(void)
{
	INIT_PINS();

bla:
	REG_P6_P6D |=  (1 << 4);
	REG_P6_P6D &= ~(1 << 4);
goto bla;


	boot_from_sdcard();

	/* we we get here, boot_from_sdcard() failed to find a kernel on the
	 * inserted media or there is no media. Thus, we register an 
	 * interrupt handler for the SD card insert switch and try again as
	 * soon as a media switch is detected. */

	/* TODO */

	return 0;
}

