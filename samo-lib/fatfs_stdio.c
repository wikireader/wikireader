//
// Authors:	Wolfgang Spraul <wolfgang@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//           

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tff.h"

FRESULT f_open(FIL *fp, const char *path, BYTE mode)
{
(void) mode; // unused

	memset(fp, 0, sizeof(*fp));
	// We use the FATFS* pointer to 'park' our FILE* pointer :-)
	fp->fs = (FATFS*) fopen(path, "r");
	return fp->fs ? FR_OK : FR_NO_FILE;
}

FRESULT f_read(FIL *fp, void *buff, UINT btr, UINT *br)
{
	size_t num_read;
	num_read = fread(buff, 1 /* size */, btr, (FILE*) fp->fs);
	*br = num_read;
	if (*br != btr) return FR_RW_ERROR;
	return FR_OK;
}

FRESULT f_lseek(FIL *fp, DWORD ofs)
{
	if (fseek((FILE*) fp->fs, ofs, SEEK_SET))
		return FR_INVALID_OBJECT;
	return FR_OK;
}

FRESULT f_close(FIL *fp)
{
	if (fp->fs) fclose((FILE*) fp->fs);
	return FR_OK;
}
