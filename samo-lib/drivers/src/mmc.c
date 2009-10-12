/*
 * Implementation taken from
 *	http://vector06cc.googlecode.com/svn/trunk/firmware/floppysrc/
 * GPL and so on.
 */

//-----------------------------------------------------------------------
// MMC/SDSC/SDHC (in SPI mode) control module  (C)ChaN, 2007
//-----------------------------------------------------------------------

#include <regs.h>
#include <samo.h>
#include <diskio.h>
#include <mmc.h>
#include <delay.h>
#include "ff_config.h"


// Definitions for MMC/SDC command
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


// Port Controls  (Platform dependent)
#define SELECT()   SDCARD_CS_LO()
#define DESELECT() SDCARD_CS_HI()


//==========================================================================
//  Private Functions
//==========================================================================


static volatile
DSTATUS Stat = STA_NOINIT;	// Disk status

static
BYTE CardType;			// b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing


//--------------------------------------------------------------------------
// Exchange a byte via SPI  (Platform dependent)
//--------------------------------------------------------------------------

static uint8_t spi_transmit(uint8_t out)
{
	REG_SPI_TXD = out;
	do {} while (~REG_SPI_STAT & RDFF);
	return REG_SPI_RXD;
}

static inline BYTE spi_receive(void)
{
	return spi_transmit(0xff);
}


//--------------------------------------------------------------------------
// Wait for card ready
//--------------------------------------------------------------------------

static BYTE wait_ready(void)
{
	int timeout = 2000000;
	BYTE res = spi_receive();

	while ((res != 0xff) && timeout--) {
		//delay_us(5);
		res = spi_receive();
	}

	return res;
}


//--------------------------------------------------------------------------
// Deselect the card and release SPI bus
//--------------------------------------------------------------------------

static void release_spi(void)
{
	DESELECT();
	(void)spi_receive();
}


//--------------------------------------------------------------------------
// Power Control  (Platform dependent)
//--------------------------------------------------------------------------

static void turn_on_power(void)
{
	release_spi();
	enable_card_power();
	delay_us(10000);
}

static void turn_off_power(void)
{
	SELECT();
	wait_ready();
	release_spi();

	disable_card_power();
	Stat |= STA_NOINIT;
}

static inline int chk_power(void)
{
	return check_card_power();
}


//--------------------------------------------------------------------------
// Receive a data packet from MMC
//--------------------------------------------------------------------------

static BOOL rcvr_datablock(BYTE *buff, UINT byte_count)
{
	BYTE token;
	DWORD timeout = 2000000;

	token = spi_receive();
	while (token == 0xff && timeout > 0) {		// Wait for data packet in timeout of max 10ms
		//delay_us(5);
		token = spi_receive();
		--timeout;
	}

	if (token != 0xFE) {
		return FALSE;				// If not valid data token, return with error
	}

	do {						// Receive the data block into buffer
		*buff++ = spi_receive();
		*buff++ = spi_receive();
	} while ((byte_count -= 2) != 0);
	spi_receive();					// Discard CRC
	spi_receive();

	return TRUE;					// Return with success
}


//--------------------------------------------------------------------------
// Send a data packet to MMC
//--------------------------------------------------------------------------

#if _READONLY == 0
static BOOL xmit_datablock(const BYTE *buff, BYTE token)
{
	BYTE resp, wc;

	if (wait_ready() != 0xff) {
		return FALSE;
	}

	spi_transmit(token);				// Xmit data token
	if (token != 0xFD) {				// Is data token
		wc = 0;
		do {					// Xmit the 512 byte data block to MMC
			spi_transmit(*buff++);
			spi_transmit(*buff++);
		} while (--wc);
		spi_transmit(0xff);			// CRC (Dummy)
		spi_transmit(0xff);
		resp = spi_receive();			// Reveive data response
		if ((resp & 0x1f) != 0x05) {		// If not accepted, return with error
			return FALSE;
		}
	}
	return TRUE;
}
#endif // _READONLY


//--------------------------------------------------------------------------
// Send a command packet to MMC
//--------------------------------------------------------------------------

static BYTE send_cmd(BYTE cmd, DWORD arg)
{
	BYTE res;
	DWORD n;

	if (cmd & 0x80) {				// ACMD<n> is the command sequense of CMD55-CMD<n>
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) {
			return res;
		}
	}

	// Select the card and wait for ready
	release_spi();
	delay_us(10);
	SELECT();
	if (cmd != CMD0 && wait_ready() != 0xff) {
		return 0xff;
	}

	// Send command packet
	spi_transmit(cmd);				// Command
	spi_transmit((BYTE)(arg >> 24));		// Argument[31..24]
	spi_transmit((BYTE)(arg >> 16));		// Argument[23..16]
	spi_transmit((BYTE)(arg >> 8));			// Argument[15..8]
	spi_transmit((BYTE)arg);			// Argument[7..0]
	n = 0xFF;					// CRC
	if (cmd == CMD0) {
		n = 0x95;				// CRC for CMD0(0)
	}
	if (cmd == CMD8) {
		n = 0x87;				// CRC for CMD8(0x1AA)
	}
	spi_transmit(n);

	// Receive command response
	if (cmd == CMD12) {
		spi_receive();				// Skip a stuff byte when stop reading
	}

	// wait for a valid response in timeout of 10 attempts or
	// in case of the init command read 21 bytes from the card
	// in order to make sure the card was initialized properly
	n = (cmd == CMD0 ? 21 : 10);
	res = spi_receive();
	while ((n > 0) && ((cmd == CMD0) || (0 != (res & 0x80)))) {
		delay_us(5);
		res = spi_receive();
		--n;
	}
	return res;					// Return with the response value
}


//==========================================================================
// Public Functions
//==========================================================================


//--------------------------------------------------------------------------
// Poll the card
//--------------------------------------------------------------------------

DSTATUS disk_poll(BYTE drv)
{
	BYTE res, n;

	if (drv) {
		return STA_NOINIT;			// Supports only single drive
	}

	// use Receive OCR as a dummy command: we just need to see if there's any proper response
	// Receive OCR as an R3 resp (4 bytes)
	if (send_cmd(CMD58, 0) == 0) {			// READ_OCR
		for (n = 4; n; n--) {
			spi_receive();
		}
		res = RES_OK;
	} else {
		res = RES_NOTRDY;
	}

	release_spi();

	return res;
}


//-----------------------------------------------------------------------
// Initialize Disk Drive
//-----------------------------------------------------------------------

DSTATUS mmc_disk_initialize(BYTE drv)
{
	BYTE n, cmd, ty, ocr[4];
	DWORD timeout = 1000000;

	if (drv) {
		return STA_NOINIT;			// Supports only single drive
	}

	if (chk_power()) {
		turn_off_power();			// Power off
		delay_us(25000);
	}

//	if (Stat & STA_NODISK) return Stat;		// No card in the socket

	REG_SPI_CTL1 =
		BPT_8_BITS |
		//CPHA |
		//CPOL |
		MCBR_MCLK_DIV_4 |
		//TXDE |
		//RXDE |
		MODE_MASTER |
		ENA |
		0;
	REG_SPI_CTL2 = 0;                               // must be zero for master mode
	REG_SPI_WAIT = 0;

	turn_on_power();				// Force socket power on
	SELECT();
	for (n = 10; n; n--) {
		spi_receive();				// 80 dummy clocks
	}
	release_spi();

	ty = 0;

	if (send_cmd(CMD0, 0) == 0) {			// Enter Idle state
		goto switch_off_power;
	}

	if (send_cmd(CMD8, 0x1AA) == 1) {		// SDHC
		for (n = 0; n < 4; n++) {
			ocr[n] = spi_receive();		// Get trailing return value of R7 resp
		}
		if (ocr[2] == 0x01 && ocr[3] == 0xAA) {	// The card can work at vdd range of 2.7-3.6V
			while (timeout-- && send_cmd(ACMD41, 1UL << 30)) {
			}				// Wait for leaving idle state (ACMD41 with HCS bit)

			if (timeout && send_cmd(CMD58, 0) == 0) {	// Check CCS bit in the OCR
				for (n = 0; n < 4; n++) {
					ocr[n] = spi_receive();
				}
				ty = (ocr[0] & 0x40) ? 12 : 4;
			}
		}
	} else {					// SDSC or MMC
		if (send_cmd(ACMD41, 0) <= 1)	{
			ty = 2; cmd = ACMD41;		// SDSC
		} else {
			ty = 1; cmd = CMD1;		// MMC
		}
		while (timeout-- && send_cmd(cmd, 0));	// Wait for leaving idle state
		if (!timeout || send_cmd(CMD16, 512) != 0) {		// Set R/W block length to 512
			ty = 0;
		}
	}
	CardType = ty;
	release_spi();

	if (ty) {					// Initialization succeded
		Stat &= ~STA_NOINIT;			// Clear STA_NOINIT
		goto out;
	}

switch_off_power: 					// Initialization failed
	turn_off_power();

out:
	return Stat;
}


//--------------------------------------------------------------------------
// Get Disk Status
//--------------------------------------------------------------------------

DSTATUS mmc_disk_status (BYTE drv)
{
	if (drv) {
		return STA_NOINIT;			// Supports only single drive
	}
	return Stat;
}


//--------------------------------------------------------------------------
// Read Sector(s)
//--------------------------------------------------------------------------

DRESULT mmc_disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
	if (drv || !count) {
		return RES_PARERR;
	}
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}

	if (!(CardType & 8)) sector *= 512;		// Convert to byte address if needed

	if (count == 1) {				// Single block read
		if ((send_cmd(CMD17, sector) == 0)	// READ_SINGLE_BLOCK
		    && rcvr_datablock(buff, 512)) {
			count = 0;
		}
	}
	else {						// Multiple block read
		if (send_cmd(CMD18, sector) == 0) {	// READ_MULTIPLE_BLOCK
			do {
				if (!rcvr_datablock(buff, 512)) {
					break;
				}
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);		// STOP_TRANSMISSION
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}


//--------------------------------------------------------------------------
// Write Sector(s)
//--------------------------------------------------------------------------

#if _READONLY == 0
DRESULT mmc_disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
	if (drv || !count) {
		return RES_PARERR;
	}
	if (Stat & STA_NOINIT) {
		return RES_NOTRDY;
	}
	if (Stat & STA_PROTECT) {
		return RES_WRPRT;
	}

	if (!(CardType & 8)) {
		sector *= 512;				// Convert to byte address if needed
	}

	if (count == 1) {				// Single block write
		if ((send_cmd(CMD24, sector) == 0)	// WRITE_BLOCK
		    && xmit_datablock(buff, 0xFE)) {
			count = 0;
		}
	}
	else {						// Multiple block write
		if (CardType & 6) {
			send_cmd(ACMD23, count);
		}
		if (send_cmd(CMD25, sector) == 0) {	// WRITE_MULTIPLE_BLOCK
			do {
				if (!xmit_datablock(buff, 0xFC)) {
					break;
				}
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD)) {	// STOP_TRAN token
				count = 1;
			}
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}
#endif // _READONLY == 0


//--------------------------------------------------------------------------
// Control Function
//--------------------------------------------------------------------------

#if _USE_IOCTL != 0
DRESULT mmc_disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	WORD csize;


	if (drv) {
		return RES_PARERR;
	}

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		switch (*ptr) {
		case 0:					// Sub control code == 0 (POWER_OFF)
			if (chk_power()) {
				turn_off_power();	// Power off
			}
			res = RES_OK;
			break;
		case 1:					// Sub control code == 1 (POWER_ON)
			turn_on_power();		// Power on
			res = RES_OK;
			break;
		case 2:					// Sub control code == 2 (POWER_GET)
			*(ptr+1) = (BYTE)chk_power();
			res = RES_OK;
			break;
		default :
			res = RES_PARERR;
		}
	}
	else {
		if (Stat & STA_NOINIT) {
			return RES_NOTRDY;
		}

		switch (ctrl) {
		case CTRL_SYNC :			// Make sure that no pending write process
			SELECT();
			if (wait_ready() == 0xFF) {
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT :			// Get number of sectors on the disk (DWORD)
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	// SDC ver 2.00
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} else {		// SDC ver 1.XX or MMC
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :			// Get R/W sector size (WORD)
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :			// Get erase block size in unit of sector (DWORD)
			if (CardType & 4) {		// SDC ver 2.00
				if (send_cmd(ACMD13, 0) == 0) {					// Read SD status
					spi_receive();
					if (rcvr_datablock(csd, 16)) {				// Read partial block
						for (n = 64 - 16; n; n--) spi_receive();	// Purge trailing data
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {			// SDC ver 1.XX or MMC
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	// Read CSD
					if (CardType & 2) {					// SDC ver 1.XX
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {	// MMC
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :			// Get card type flags (1 byte)
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :			// Receive CSD as a data block (16 bytes)
			if (send_cmd(CMD9, 0) == 0	// READ_CSD
			    && rcvr_datablock(ptr, 16)) {
				res = RES_OK;
			}
			break;

		case MMC_GET_CID :			// Receive CID as a data block (16 bytes)
			if (send_cmd(CMD10, 0) == 0	// READ_CID
			    && rcvr_datablock(ptr, 16)) {
				res = RES_OK;
			}
			break;

		case MMC_GET_OCR :			// Receive OCR as an R3 resp (4 bytes)
			if (send_cmd(CMD58, 0) == 0) {	// READ_OCR
				for (n = 4; n; n--) {
					*ptr++ = spi_receive();
				}
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :			// Receive SD status as a data block (64 bytes)
			if (send_cmd(ACMD13, 0) == 0) {	// SD_STATUS
				spi_receive();
				if (rcvr_datablock(ptr, 64)) {
					res = RES_OK;
				}
			}
			break;

		default:
			res = RES_PARERR;
		}

		release_spi();
	}

	return res;
}
#endif // _USE_IOCTL != 0
