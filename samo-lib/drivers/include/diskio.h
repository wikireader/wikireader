//
// Authors:	Epson Shanghai
//		Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//


#if !defined(_DISKIO_)
#define _DISKIO_

#define _READONLY	0	/* 1: Read-only mode */
#define _USE_IOCTL	1

#include "sd_api.h"
#include <inttypes.h>

// uint8_t mmc_disk_initialize(uint8_t disk_idx);
// uint8_t mmc_disk_read(uint8_t disk_idx, uint8_t *buff, uint16_t sector, uint8_t count);


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR,		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

uint8_t disk_initialize(uint8_t disk_idx) {
// 	return mmc_disk_initialize(0);
	return SdInitialize();
}

uint8_t disk_status(uint8_t disk_idx)
{
	return 0;
}

uint8_t disk_read(uint8_t disk_idx, uint8_t *buff, uint16_t sect_addr, uint16_t size) {
// 	return mmc_disk_read(disk_idx, buff, sect_addr, size);
	return SdRdSect((unsigned short)disk_idx, sect_addr, (unsigned long)size, buff);
}

#if	_READONLY == 0
uint8_t disk_write(uint8_t disk_idx, const uint8_t *buff, uint16_t sect_addr, uint16_t size) {
	// TODO
	return 0;
}
#endif

uint8_t disk_ioctl(uint8_t disk_idx, uint8_t ioctl, void *buff) {
	return 0;
}



/* Disk Status Bits (DSTATUS) */

#define STA_SUCCESS		0x00	/* ready to be used */
#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */
#define STA_UNKNOWN		0x05	/* unknown type of card */
#define STA_BAD_BLCK		0x06	/* block len of 512 bytes not supported */
#define STA_CAPACITY		0x07	/* unable to retrieve capacity */


/* Command code for disk_ioctrl() */

/* Generic command */
#define CTRL_SYNC			0	/* Mandatory for read/write configuration */
#define GET_SECTOR_COUNT	1	/* Mandatory for only f_mkfs() */
#define GET_SECTOR_SIZE		2
#define GET_BLOCK_SIZE		3	/* Mandatory for only f_mkfs() */
#define CTRL_POWER			4
#define CTRL_LOCK			5
#define CTRL_EJECT			6
/* MMC/SDC command */
#define MMC_GET_TYPE		10
#define MMC_GET_CSD			11
#define MMC_GET_CID			12
#define MMC_GET_OCR			13
#define MMC_GET_SDSTAT		14
/* ATA/CF command */
#define ATA_GET_REV			20
#define ATA_GET_MODEL		21
#define ATA_GET_SN			22

#endif
