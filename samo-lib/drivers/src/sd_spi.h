//
// Authors:	Epson Shanghai
//		Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//


#ifndef _SD_SPI_
#define _SD_SPI_

#ifdef __cplusplus
   extern "C" {
#endif

//////////////////////////////////////////////////////////////////

extern unsigned char __START_idma;

// Functions

// I/O
extern void SPI_SDXmitByte(unsigned char ucData);
extern void SPI_SDSendByte(unsigned char ucData);
extern void SPI_SDSendBlock(const unsigned char* pBuf);
extern void SPI_SDReadByte(unsigned char* pData);
extern void SPI_SDReadBlock(unsigned char* pBuf, unsigned short ulDataSize);

// Card Detect
extern int SPI_IsCardEject();

// Write Protect
extern int SPI_IsWriteProtected();


#ifdef __cplusplus
}
#endif

#endif

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

