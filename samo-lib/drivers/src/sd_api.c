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
#include <inttypes.h>
#include <sd_api.h>
#include <diskio.h>	// return definitions
#include <string.h>

////////////////////////////////////////////////////
// Command Supported by SPI Mode
#define CMD0	0x40	/* GO_IDLE_STATE */
#define CMD1	0x41	/* SEND_OP_COND (MMC) */
#define CMD5	0x45
#define CMD8	0x48	/* SEND_IF_COND */
#define CMD9	0x49	/* SEND_CSD */
#define CMD10	0x4A	/* SEND_CID */
#define CMD12	0x4C	/* STOP_TRANSMISSION */
#define CMD13	0x4D
#define CMD16	0x50	/* SET_BLOCKLEN */
#define CMD17	0x51	/* READ_SINGLE_BLOCK */
#define CMD18	0x52	/* READ_MULTIPLE_BLOCK */
#define CMD23	0x57	/* SET_BLOCK_COUNT (MMC) */
#define CMD24	0x58	/* WRITE_BLOCK */
#define CMD25	0x59	/* WRITE_MULTIPLE_BLOCK */
#define CMD27	0x5B
#define CMD28	0x5C
#define CMD29	0x5D
#define CMD30	0x5E
#define CMD32	0x60
#define CMD33	0x61
#define CMD55  	0x77	/* APP_CMD */
#define CMD58	0x7A	/* READ_OCR */
#define ACMD13	0xCD	/* SD_STATUS (SDC) */
#define ACMD41 	0x69	/* SEND_OP_COND (SDC) */
#define ACMD13	0xCD	/* SD_STATUS (SDC) */
#define	ACMD23	0xD7	/* SET_WR_BLK_ERASE_COUNT (SDC) */


#define SD_CMD_TIMEOUT 10
#define SD_CMD0_TIMEOUT 21


////////////////////////////////////////////////
// Command parameter
static unsigned char ucParameters[4];
// Response 1
// MSB should be 0
// 1 for CMD0 success
// 0 for success of other command
static unsigned char ucResponse;
// CSD Register
static unsigned char ucCSD[16];

////////////////////////////////////////////////
// The capacity of SD Card;
static unsigned long ulCapacity;
// The block length of SD
// static unsigned long ulBlockLen;
#define BLOCK_LENGTH 512	// retrieving the block length is not reliable

/////////////////////////////////////////////////////////////////////////////////////////////
// common functions
static unsigned char disk_type = SD_TYPE_NONE;

#if 0
static unsigned long GetIntFromBuf(unsigned char* pucBuf)
{
	unsigned long ulValue;

	if (pucBuf == NULL)
		return 0;

	ulValue = ((pucBuf[3]<<24)|(pucBuf[2]<<16)|(pucBuf[1]<<8)|(pucBuf[0]));

	return ulValue;
}
#endif

static void PutIntIntoBuf(unsigned char* pucBuf, unsigned long ulValue)
{
	if (pucBuf == NULL)
		return;

	*(pucBuf+0) = (unsigned char)ulValue;
	*(pucBuf+1) = (unsigned char)(ulValue>>8);
	*(pucBuf+2) = (unsigned char)(ulValue>>16);
	*(pucBuf+3) = (unsigned char)(ulValue>>24);
}

/////////////////////////////////////////////////////////////////////////////////////////////

void delay_us(unsigned int microsec)
{
	while (microsec--) {
		// at 48 MHz this should take 1 micro second
		asm volatile (
			"\tld.w\t%r4,12\n"
			"delay_loop:\n"
			"\tnop\n"
			"\tsub\t%r4,1\n"
			"\tjrne\tdelay_loop"
			);
	}
}

static void turn_on_power(void)
{
	int i;

	SDCARD_CS_HI();
	SPI_SDReadByte(&ucResponse);

	enable_card_power();
	delay_us(10000);

	SDCARD_CS_LO();
	/* 10 dummy cycles */
	for (i = 0; i < 10; i++)
		SPI_SDSendByte(0xFF);
	SDCARD_CS_HI();
}

static void turn_off_power(void)
{
	int i;

	SDCARD_CS_LO();
	/* 10 dummy cycles */
	for (i = 0; i < 10; i++)
		SPI_SDSendByte(0xFF);
	SDCARD_CS_HI();

	SPI_SDSendByte(0xFF);

	disable_card_power();
	disk_type = SD_TYPE_NONE;
}

static unsigned int rcv_cmd_res(unsigned int timeout, unsigned char break_cond)
{
	int i;

	for (i = 0; i < timeout; i++) {
		delay_us(50);
		SPI_SDReadByte(&ucResponse);
		if (ucResponse == break_cond)
			break;
	}

	return i;
}

static void SdSendCommand(unsigned char ucCmd, unsigned long ulPara)
{
	int i;
	unsigned char ucCRC = 0;

	SDCARD_CS_HI();
	SPI_SDSendByte(0xFF);
	delay_us(100);
	SDCARD_CS_LO();

	switch (ucCmd) {
	case CMD0:
		ucCRC = 0x95;
		break;

	case CMD8:
		ucCRC = 0x87;
		break;

	default:
		ucCRC = 0;
		break;
	}

	if (ulPara == 0)
		memset(ucParameters, 0, 4);
	else
		PutIntIntoBuf(ucParameters, ulPara);

	SPI_SDXmitByte(ucCmd);
	for (i = 3; i >= 0; i--)
		SPI_SDXmitByte(ucParameters[i]);
	SPI_SDXmitByte(ucCRC);
}

static void wait_for_sdinit(unsigned char cmd, unsigned long arg)
{
	int i;

	for (i = 0; i < SD_CMD_TIMEOUT * SD_CMD_TIMEOUT; i++) {
		if (cmd == ACMD41) {
			SdSendCommand(CMD55, 0);
			rcv_cmd_res(SD_CMD_TIMEOUT, 0x01);
		}

		SdSendCommand(cmd, arg);
		rcv_cmd_res(SD_CMD_TIMEOUT * SD_CMD_TIMEOUT, 0x00);

		if (ucResponse == 0x00)
			return;
	}
}

#if 0
static int SdGetR1(unsigned char* pResponse)
{
	int i = 0;
	int nResult;
	unsigned char ucTemp;

	do {
		SPI_SDReadByte(pResponse);
		if (((*pResponse) & 0x80) == 0)
			break;
		i++;
	} while (i < SD_CMD_TIMEOUT);

	if (i >= SD_CMD_TIMEOUT)
		nResult = SD_FAILURE;
	else
		nResult = SD_SUCCESS;

	// Output 8 dummy clocks
	SPI_SDReadByte(&ucTemp);

	return nResult;
}
#endif


static int SdGetCSD(unsigned char* pCSD)
{
	int i = 0;
	unsigned char ucTemp;
	int nCount = 15;

	do {
		SPI_SDReadByte(&ucTemp);
		if (ucTemp != 0xFF)
			break;
		i++;
	} while (i < SD_CMD_TIMEOUT);

	if (i >= SD_CMD_TIMEOUT)
		return SD_FAILURE;

	if (ucTemp != 0xFE) {
		nCount = 14;
		pCSD[15] = ucTemp;
	}

	for (i = nCount; i >= 0; i--) {
		SPI_SDReadByte(&ucTemp);
		pCSD[i] = ucTemp;
	}

	// CRC Read
	SPI_SDReadByte(&ucTemp);
	SPI_SDReadByte(&ucTemp);

	// Output 8 dummy clocks
	SPI_SDSendByte(0xFF);

	return STA_SUCCESS;
}

static unsigned long SdGetCapacity(unsigned char* pCSD)
{
	unsigned short usDeviceSize;
	unsigned short usMulti;
	unsigned long ulBlockNumber;

	usDeviceSize = (pCSD[7] & 0xC0) >> 6;
	usDeviceSize = (usDeviceSize | (pCSD[8] << 2));
	usDeviceSize = (usDeviceSize | ((pCSD[9] & 0x03) << 10));

	usMulti = (pCSD[5] & 0x80) >> 7;
	usMulti = (usMulti | ((pCSD[6] & 0x03) << 1));
	usMulti = (0x01 << (usMulti + 2));

	ulBlockNumber = usDeviceSize * usMulti;
	return ulBlockNumber;
}

// static unsigned long SdGetBlockLen(unsigned char* pCSD)
// {
// 	unsigned char ucRdBlkLen;
// 	unsigned long ulBlockLen;
//
// 	ucRdBlkLen = pCSD[10] & 0x0F;
// 	ulBlockLen = 0x01 << ucRdBlkLen;
//
// 	return ulBlockLen;
// }


/////////////////////////////////////////////////////////////////////////////
//  Read Data Blocks                                                       //

static int SdGetDataBlock(unsigned char *Data, unsigned long ulDataLen)
{
	int i;
	unsigned char ucTemp;

	for (i = 0; i < 20000; i++) {
		delay_us(1000);
		SPI_SDReadByte(&ucTemp);
		if (ucTemp != 0xFF)
			break;
	}

	if (i >= SD_CMD_TIMEOUT) {
		return SD_FAILURE;
	}

	if (ucTemp != 0xFE) {
		SPI_SDSendByte(0xFF);
		return SD_FAILURE;
	}

	SPI_SDReadBlock(Data, ulDataLen);

	// CRC Read
	SPI_SDReadByte(&ucTemp);
	SPI_SDReadByte(&ucTemp);

	// Output 8 dummy clocks
	SPI_SDSendByte(0xFF);
	return SD_SUCCESS;
}

static int SdSingleBlockRead(unsigned long BlkAddr, unsigned char *Data, unsigned long ulDataLen)
{
	int nResult;
	int i;

	nResult = SdCardDetect();
	if (nResult != STA_SUCCESS)
		return nResult;

	SdSendCommand(CMD17, BlkAddr);
	i = rcv_cmd_res(SD_CMD_TIMEOUT, 0x00);

	if (i >= SD_CMD_TIMEOUT) {
		return SD_FAILURE;
	}

	return SdGetDataBlock(Data, ulDataLen);
}

static int SdMultiBlockRead(unsigned long BlkAddr, unsigned char *Data, unsigned long ulDataLen)
{
	int nResult;
	int i;
	unsigned char ucTemp;

	nResult = SdCardDetect();
	if (nResult != STA_SUCCESS)
		return nResult;

	/* start transmission */
	SdSendCommand(CMD18, BlkAddr);

	for (i = 0; i < SD_CMD_TIMEOUT; i++) {
		SPI_SDReadByte(&ucTemp);
		if (ucTemp == 0x00)
			break;
	}

	if (i >= SD_CMD_TIMEOUT)
		return SD_FAILURE;

	nResult = SdGetDataBlock(Data, ulDataLen);

	/* stop transmission */
	SdSendCommand(CMD12, 0);
	SPI_SDReadByte(&ucTemp);

	if (nResult != SD_SUCCESS)
		return SD_FAILURE;
	else
		return SD_SUCCESS;
}



/////////////////////////////////////////////////////////////////////////////
//  Send Data Blocks                                                       //

static int SdSendDataBlock(const unsigned char* pData)
{
	int nResult;
	unsigned char ucTemp;
	int i = 0;

	SPI_SDSendByte(0xFE);

#if 0
	for(i=0; i<ulBlockLen; i++)
	{
		SPI_SDSendByte(pData[i]);
	}
#else
	SPI_SDSendBlock(pData);
#endif

	// CRC
	SPI_SDSendByte(0);
	SPI_SDSendByte(0);

	do {
		SPI_SDReadByte(&ucTemp);
		if ((ucTemp&0x11) == 0x01)
			break;
		i++;
	} while (i < SD_CMD_TIMEOUT);

	if (i >= SD_CMD_TIMEOUT)
		return SD_FAILURE;

	if ((ucTemp & 0x1F) != 0x05)
		nResult = SD_FAILURE;
	else
		nResult = SD_SUCCESS;

	while(1) {
		// Output 8 dummy clocks
		SPI_SDReadByte(&ucTemp);
		if (ucTemp == 0xFF)
			break;
	}
	return nResult;
}

static int SdSingleBlockWrite(unsigned long BlkAddr, const unsigned char *Data)
{
	int nResult;
	unsigned char ucTemp;
	int i;

	nResult = SdCardDetect();
	if (nResult != STA_SUCCESS)
		return nResult;

	if (SdCardWriteProtected())
		return SD_FAILURE;

	SdSendCommand(CMD24, (BlkAddr * BLOCK_LENGTH));

	for (i = 0; i < SD_CMD_TIMEOUT; i++) {
		SPI_SDReadByte(&ucTemp);
		if (ucTemp == 0x00)
			break;
	}

	if (i >= SD_CMD_TIMEOUT)
		return SD_FAILURE;

	nResult = SdSendDataBlock(Data);

	if (nResult != SD_SUCCESS)
		return SD_FAILURE;
	else
		return SD_SUCCESS;
}


////////////////////////////////////////////////////////////////////////

int SdMediaReset()
{
	return SD_SUCCESS;
}

int SdInitialize(void)
{
	int i, res = STA_SUCCESS;

	turn_on_power();

	SdSendCommand(CMD0, 0);
	i = rcv_cmd_res(SD_CMD0_TIMEOUT, 0x01);

	if (i >= SD_CMD0_TIMEOUT) {
		res = STA_NODISK;
		goto error;
	}

	SdSendCommand(CMD8, 0x1AA);
	i = rcv_cmd_res(SD_CMD_TIMEOUT, 0x01);

	/* SDHC card */
	if (i < SD_CMD_TIMEOUT) {
		/* retrieve R7 data */
		for (i = 0; i < 4; i++)
			SPI_SDReadByte(&ucParameters[i]);

		/* voltage is not within allowed range (2.7 - 3.6V) */
		if ((ucParameters[2] != 0x01) || (ucParameters[3] != 0xAA)) {
			res = STA_UNKNOWN;
			goto error;
		}

		wait_for_sdinit(ACMD41, 1UL << 30);

		if (ucResponse != 0) {
			res = STA_NOINIT;
			goto error;
		}

		SdSendCommand(CMD58, 0);
		rcv_cmd_res(SD_CMD_TIMEOUT, 0x00);

		if (ucResponse != 0) {
			res = STA_NOINIT;
			goto error;
		}

		/* Check CCS bit in the OCR */
		for (i = 0; i < 4; i++)
			SPI_SDReadByte(&ucParameters[i]);

		if (ucParameters[0] & 0x40)
			disk_type = SD_TYPE_SDHC;
		else
			disk_type = SD_TYPE_SDSC;

		goto get_size;
	}

	/* MMC or SDSC */
	SdSendCommand(CMD55, 0);
	i = rcv_cmd_res(SD_CMD_TIMEOUT, 0x01);

	if (i >= SD_CMD_TIMEOUT) {
		res = STA_UNKNOWN;
		goto error;
	}

	SdSendCommand(ACMD41, 0);
	rcv_cmd_res(SD_CMD_TIMEOUT, 0x01);

	if (ucResponse <= 1) {
		disk_type = SD_TYPE_SDSC;
		wait_for_sdinit(ACMD41, 0);
	} else {
		disk_type = SD_TYPE_MMC;
		wait_for_sdinit(CMD1, 0);
	}

	if (ucResponse != 0) {
		res = STA_NOINIT;
		goto error;
	}

	// change block size
	SdSendCommand(CMD16, 512);
	rcv_cmd_res(SD_CMD_TIMEOUT, 0x00);

	if (ucResponse != 0) {
		res = STA_BAD_BLCK;
		goto error;
	}

get_size:
	// Get capacity
	SdSendCommand(CMD9, 0);
	rcv_cmd_res(SD_CMD_TIMEOUT, 0x00);

	if (ucResponse != 0) {
		res = STA_PROTECT;
		goto error;
	}

	i = SdGetCSD(ucCSD);
	if (i != STA_SUCCESS) {
		res = STA_CAPACITY;
		goto error;
	}

	ulCapacity = SdGetCapacity(ucCSD);

	/* retrieving the block length does not work for all cards */
	/* ulBlockLen = SdGetBlockLen(ucCSD);

	if (ulBlockLen != 512) {
		res = STA_BAD_BLCK;
		goto error;
	} */

	// enable SPI to DMA
	REG_SPI_CTL1 =
		BPT_8_BITS |
		//CPHA |
		//CPOL |
		MCBR_MCLK_DIV_4 |
		TXDE |
		RXDE |
		MODE_MASTER |
		ENA |
		0;

	return STA_SUCCESS;

error:
	turn_off_power();
	return res;
}

int SdClose()
{
	turn_off_power();
	return STA_SUCCESS;
}

int SdCardDetect()
{
	if (disk_type == SD_TYPE_NONE)
		return STA_NOINIT;
#if 1
//	ucTemp = (*(volatile unsigned char *)0x300392);
	//if((ucTemp&0x40)!=0)
//		return SD_FAILURE;
//	else
		return STA_SUCCESS;
#else
// 	ucTemp = (*(volatile unsigned char *)0x30038E);
// 	if ((ucTemp & 0x01) != 0)
// 		return SD_FAILURE;
// 	else
// 		return SD_SUCCESS;
#endif
}

int SdCardWriteProtected()
{
#if 1
  //    ucTemp = (*(volatile unsigned char *)0x300392);
//	if((ucTemp&0x20)==0)
		return 0;
//	else
//		return 1;
#else
	ucTemp = (*(volatile unsigned char *)0x30038C);

	if ((ucTemp & 0x10) == 0)
		return 0;
	else
		return 1;
#endif
}

int SdGetErrCode()
{
	return SD_SUCCESS;
}

int SdPhyFormat()
{
	return SD_SUCCESS;
}

int SdGetDeviceInfo(void* pData)
{
	memcpy((pData+120), (unsigned char*)(&ulCapacity), 4);
	return SD_SUCCESS;
}

void SdGetDevicePara(unsigned long *ulSectors, unsigned long *ulSectorSize)
{
	*ulSectors = ulCapacity;
// 	*ulSectorSize = ulBlockLen;
	*ulSectorSize = BLOCK_LENGTH;

}

int SdRdSect(unsigned short uwDeviceNo, unsigned long ulSectAddr, unsigned long ulDataLen, unsigned char * pDataBuf)
{
	int nResult;
	unsigned long data_len;
	unsigned long SectNum;

	/**
	 * the fatfs driver gives us the number of sectors to read
	 * and assumes a sector size of 512 bytes - lets hope it is true ..
	 */
	/* data_len = ulDataLen;
	SectNum = ulDataLen / BLOCK_LENGTH; */
	data_len = ulDataLen * BLOCK_LENGTH;
	SectNum = ulDataLen;

	if (disk_type != SD_TYPE_SDHC)
		ulSectAddr *= BLOCK_LENGTH;

	/* some cards return a bogus capacity */
	/* if ((ulSectAddr + SectNum - 1) > (ulCapacity - 1))
		return SD_FAILURE; */

	/* nothing to read */
	if (SectNum == 0)
		return STA_SUCCESS;

	if (SectNum == 1)
		nResult = SdSingleBlockRead(ulSectAddr, pDataBuf, data_len);
	else
		nResult = SdMultiBlockRead(ulSectAddr, pDataBuf, data_len);

	if (nResult != STA_SUCCESS)
		return SD_FAILURE;

	return STA_SUCCESS;
}

int SdWtSect(unsigned short uwDeviceNo, unsigned long ulSectAddr, unsigned long ulDataLen, unsigned char *pDataBuf,unsigned char bVerify)
{
	int nResult;
	unsigned long ulSectIndex;
	unsigned long ulSectNum;

	unsigned long ulSectorAddr;
	unsigned char* pBuf;

	if (disk_type != SD_TYPE_SDHC)
		ulSectAddr *= BLOCK_LENGTH;

	ulSectNum = ulDataLen / BLOCK_LENGTH;

	if ((ulSectAddr + ulSectNum - 1) > (ulCapacity - 1))
		return SD_FAILURE;

	for (ulSectIndex = 0; ulSectIndex < ulSectNum; ulSectIndex++) {
		ulSectorAddr = ulSectAddr + ulSectIndex;
		pBuf = pDataBuf + ulSectIndex * BLOCK_LENGTH;
		nResult = SdSingleBlockWrite(ulSectorAddr, pBuf);
		if (nResult != SD_SUCCESS)
			return SD_FAILURE;
	}

	return SD_SUCCESS;
}



