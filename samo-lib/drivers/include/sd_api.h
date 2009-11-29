//
// Authors:	Epson Shanghai
//		Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//


#if !defined_SD_API_H_
#define _SD_API_H_ 1

#ifdef __cplusplus
   extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

enum {
	SD_TYPE_NONE,
	SD_TYPE_MMC,
	SD_TYPE_SDSC,
	SD_TYPE_SDHC,
};

#define SD_SUCCESS	0
#define SD_FAILURE	-1

#include <inttypes.h>
#include <regs.h>
#include <samo.h>

#define SD_CS_L SDCARD_CS_LO()
#define SD_CS_H SDCARD_CS_HI()

// Functions
extern int SdMediaReset(void);
extern int SdInitialize(void);
extern int SdClose(void);

extern int SdGetDeviceInfo(void* pData);
extern void SdGetDevicePara(unsigned long *ulSectors, unsigned long *ulSectorSize);
extern int SdGetErrCode(void);
extern int SdPhyFormat(void);

extern int SdCardDetect(void);
extern int SdCardWriteProtected(void);

extern int SdRdSect(unsigned short uwDeviceNo, unsigned long ulSectAddr, unsigned long ulDataLen, unsigned char * pDataBuf);
extern int SdWtSect(unsigned short uwDeviceNo, unsigned long ulSectAddr, unsigned long ulDataLen, unsigned char *pDataBuf,unsigned char bVerify);

#ifdef __cplusplus
}
#endif

#endif

