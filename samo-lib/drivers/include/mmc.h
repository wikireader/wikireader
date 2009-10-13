// Copyright (c) 2009 Openmoko Inc.
//
// Authors:	Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//


#if !defined(_MMC_)
#define _MMC_

#include <integer.h>


typedef unsigned char DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR,		/* 4: Invalid Parameter */
} DRESULT;


DSTATUS mmc_disk_initialize(BYTE drv);
DRESULT mmc_disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count);
DRESULT mmc_disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count);

#endif
