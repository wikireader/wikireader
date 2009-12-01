//
// Authors:	Epson Shanghai
//		Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//


#include "sd_spi.h"
#include <regs.h>
#include <samo.h>

#define IDMA_BASE_ADDR (unsigned long)(&__START_idma)

#define SPI_RXD_ADDR ((volatile unsigned long*)0x301700)
#define SPI_TXD_ADDR ((volatile unsigned long*)0x301704)
#define SPI_STATUS (*(volatile unsigned long*)0x301714)

static unsigned char ucDMASrc;


void SPI_SDXmitByte(unsigned char ucData)
{
	REG_SPI_TXD = ucData;
	do {} while (REG_SPI_STAT & BSYF);
}

void SPI_SDSendByte(unsigned char ucData)
{
	unsigned char ucTemp;

	while((SPI_STATUS&0x40)!=0)
		asm("nop"); // busy

	*SPI_TXD_ADDR = (ucData&0x000000FF);

	while((SPI_STATUS&0x40)!=0)
		asm("nop"); // busy

	while((SPI_STATUS&0x04)==0)
		asm("nop"); // Empty

	// Clear RDFF
	ucTemp = (unsigned char)((*SPI_RXD_ADDR)&0x000000FF);
}

void SPI_SDSendBlock(const unsigned char *pBuf)
{
	unsigned short usTemp;

	// ADV mode
	(*(volatile unsigned short*)0x30119C) = 0x0001;

	////////////////////////////////////////////////////
	// Ch. 2 SPI transfer setting
	// ADV Control
	(*(volatile unsigned short*)0x301182) = 0x0000;

	// Set counter 511 bytes, Dual Address mode
	(*(volatile unsigned short*)0x301140) = 511;
	(*(volatile unsigned short*)0x301142) = 0x8000;

	// Source: Byte, inc with init
	(*(volatile unsigned short*)0x301144) = 0x0000;
	(*(volatile unsigned short*)0x301146) = 0x2000;

	// Src Addr
	(*(volatile unsigned long*)0x301184) = (unsigned long)(pBuf+1);

	// Destination: Single, fixed
	(*(volatile unsigned short*)0x301148) = 0x0000;
	(*(volatile unsigned short*)0x30114A) = 0x0000;

	// Dest Addr
	(*(volatile unsigned long*)0x301188) = 0x301704;

	// Trigger Source SPI
	(*(volatile unsigned char*)0x300299) &= 0xF0;
	(*(volatile unsigned char*)0x300299) |= 0x09;

	// Reset flag
	(*(volatile unsigned char*)0x300289) = 0x20;
	(*(volatile unsigned short*)0x30114E) = 0x0001;

	// Enable
	(*(volatile unsigned short*)0x30114C) = 0x0001;

	// Invoke transfer
	(*(volatile unsigned long*)0x301704) = (0x000000FF&(pBuf[0]));

	do
	{
		usTemp = ((*(volatile unsigned short*)0x30114C)&0x01);
	}
	while(usTemp!=0);


}

void SPI_SDReadByte(unsigned char *pData)
{
	unsigned char ucTemp;

	while((SPI_STATUS&0x40)!=0)
		asm("nop"); // busy

	*SPI_TXD_ADDR = (0x000000FF);

	while((SPI_STATUS&0x40)!=0)
		asm("nop"); // busy

	while((SPI_STATUS&0x04)==0)
		asm("nop"); // Empty

	ucTemp = (unsigned char)((*SPI_RXD_ADDR)&0x000000FF);
	*pData = ucTemp;

}

void SPI_SDReadBlock(unsigned char *pBuf, unsigned short ulDataSize)
{
	unsigned short usTemp;

	ucDMASrc = 0xFF;

	/* enable DMA advanced mode */
	REG_HS_CNTLMODE = HSDMAADV;

	/* disable channel 3 DMA */
// 	(*(volatile unsigned short*)0x30115C) = 0x0000;
	REG_HS3_EN = DMA_DISABLED;

	(*(volatile unsigned short*)0x301192) = 0x0000;

	// Set counter 512 bytes, Dual Address mode
	(*(volatile unsigned short*)0x301150) = ulDataSize;
	(*(volatile unsigned short*)0x301152) = 0x8000;
	// Source: Byte, fixed
	(*(volatile unsigned short*)0x301154) = 0x0000;
	(*(volatile unsigned short*)0x301156) = 0x0000;
	// Destination: Single, inc with init
	(*(volatile unsigned short*)0x301158) = 0x0000;
	(*(volatile unsigned short*)0x30115A) = 0x2000;
	// Src Addr
	(*(volatile unsigned long*)0x301194) = 0x301700;
	// Dest Addr
	(*(volatile unsigned long*)0x301198) = (unsigned long)pBuf;
	// Trigger Source SPI
	(*(volatile unsigned char*)0x300299) &= 0x0F;
	(*(volatile unsigned char*)0x300299) |= 0x90; // SPI RX
	// Reset flag
	(*(volatile unsigned char*)0x300289) = 0x30;
	(*(volatile unsigned short*)0x30115E) = 0x0001;
	// Enable
	(*(volatile unsigned short*)0x30115C) = 0x0001;

	// IDMA Disable
	(*(volatile unsigned char*)0x301105) = 0x00;
	// Set base address
	(*(volatile unsigned long *)0x301100) = IDMA_BASE_ADDR;
	// IDMA info
	// No Link
	// 8 bit
	// Src Addr fixed; Dest Addr fixed
	// Single transfer; INT disable
	(*(volatile unsigned long*)(IDMA_BASE_ADDR + 0x240)) = 0x00000000;
	// length minus one trigger byte
	(*(volatile unsigned long*)(IDMA_BASE_ADDR + 0x244)) = ulDataSize - 1;
	// Source address
	(*(volatile unsigned long*)(IDMA_BASE_ADDR + 0x248)) = (unsigned long)(&ucDMASrc);
	// Destination address
	(*(volatile unsigned long*)(IDMA_BASE_ADDR + 0x24C)) = 0x301704;
	// SPI RX IDMA request, Enable
	(*(volatile unsigned char*)0x30029B) |= 0x10;
	(*(volatile unsigned char*)0x30029C) |= 0x10;
	// IDMA Enable
	//(*(volatile unsigned char*)0x301105) = 0x01;

	// Invoke transfer
	(*(volatile unsigned long*)0x301704) = (0x000000FF);
	do {
		usTemp = (*(volatile unsigned short*)0x30115C)&0x01;
	}
	while (usTemp != 0);
}

