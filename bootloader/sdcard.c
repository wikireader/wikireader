/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "regs.h"
#include "types.h"
#include "wikireader.h"
#include "spi.h"
#include "misc.h"
#include "crc.h"
#include "sdcard.h"

#define CMD_GO_IDLE_STATE	0x00
#define CMD_8			0x08
#define CMD_READ_CSD		0x09
#define CMD_READ_SECTOR		0x11
#define CMD_SEND_OP_COND	0x29
#define CMD_APP			0x37
#define CMD_41			41
#define CMD_58			58

#define MODE_SDSC 0
#define MODE_SDHC 1
static u8 mode;

static u8 csd[16];

static u8 sdcard_wait_for_ready(void)
{
	u8 ret, retry = 10;

	spi_transmit(0xff);
return 1;

	while (retry--) {
		ret = spi_transmit(0xff);
		if (ret == 0xff);
			break;
	}

	return ret == 0xff;
}

static u8 sdcard_response(void)
{
	u8 ret, retry = 100;

	do {
		SDCARD_CS_LO();
		ret = spi_transmit(0xff);
		SDCARD_CS_HI();
	} while (--retry && (ret & 0x80));

	if (retry == 0)
		print("TIMEOUT RESPONSE\n");

	return ret;
}

static void sdcard_cmd(u8 cmd, u32 param)
{
	u8 crc, i, c[] = {
		0x40 | cmd,
		param >> 24,
		param >> 16,
		param >> 8,
		param
	};

	SDCARD_CS_LO();

	if (cmd != CMD_GO_IDLE_STATE && !sdcard_wait_for_ready()) {
		print("timeout waiting for card ready\n");
		SDCARD_CS_HI();
		return;
	}

	for (i = 0; i < sizeof(c); i++)
		spi_transmit(c[i]);

	/* checksum */
	//spi_transmit((crc7(c, sizeof(c)) << 1) | 1);
	switch (cmd) {
		case 0:
			crc = 0x95;
			break;
		case 8:
			crc = 0x87;
			break;
		default:
			crc = (crc7(c, sizeof(c)) << 1) | 1;
			break;
	}

	spi_transmit(crc);
	SDCARD_CS_HI();
}

int sdcard_read_sector(u32 sector, u8 *buf)
{
	u8 ret;
	u32 i, retry;

	//print("read sector "); print_u32(sector); print("\n");

	if (mode == MODE_SDSC)
		sector *= BYTES_PER_SECTOR;

	for (i = 0; i < BYTES_PER_SECTOR; i++)
		buf[i] = 0;

	sdcard_cmd(CMD_READ_SECTOR, sector);
	ret = sdcard_response();

	if (ret != 0) {
		print("bad card response: ");
		print_u32(ret); print("\n");
		return -1;
	}

	for (retry = 0; retry < 1000; retry++) {
		SDCARD_CS_LO();
		ret = spi_transmit(0xff);
		SDCARD_CS_HI();

		if (ret == 0xfe)
			break;
	}

	if (ret != 0xfe) {
		print("read timeout\n");
		return -1;
	}

	SDCARD_CS_LO();
	for (i = 0; i < BYTES_PER_SECTOR; i++)
		buf[i] = spi_transmit(0xff);

	/* swallow checksum */
	spi_transmit(0xff);
	spi_transmit(0xff);
	SDCARD_CS_HI();


	/* stop reading */
//	sdcard_cmd(12, 0);



	return 0;
}

static int sdcard_read_csd(void)
{
	u32 retry, ret, i;

	sdcard_cmd(CMD_READ_CSD, 0);

	for (retry = 0; retry < 10; retry++) {
		ret = sdcard_response();
		if (ret == 0xfe)
			break;
	}

	if (ret != 0xfe)
		return -1;

	SDCARD_CS_LO();
	for (i = 0; i < sizeof(csd); i++)
		csd[i] = spi_transmit(0xff);
	SDCARD_CS_HI();

	print("dumping CSD:\n");
	hex_dump(csd, sizeof(csd));

	return 0;
}

int sdcard_init(void)
{
	u8 ret;
	u32 retry;

	/* 80 dummy clocks */
	SDCARD_CS_LO();
	for (retry = 0; retry < 10; retry++)
		spi_transmit(0xff);

	SDCARD_CS_LO();
		
	/* set card to IDLE state */
	for (retry = 100; retry; retry--) {
		sdcard_cmd(CMD_GO_IDLE_STATE, 0);
		ret = sdcard_response();

		if (ret == 1)
			break;
	}

	if (ret != 1) {
		print("unable to set SD card to IDLE state\n");
		return -1;
	}

	/* check for SDHC card type */
	for (retry = 100; retry; retry--) {
		sdcard_cmd(CMD_8, 0x1aa);
		ret = sdcard_response();
		if (ret == 0x01)
			break;

		delay(10000);
	}

	if (ret != 0x01) {
		/* SDSC? */
		for (retry = 100; retry; retry--) {
			sdcard_cmd(CMD_APP, 0);
			ret = sdcard_response();

			sdcard_cmd(CMD_SEND_OP_COND, 0);
			ret = sdcard_response();

			if (ret == 0)
				break;

			delay(10000);
		}

		if (ret != 0x00) {
			print("unable to init SDSC card!\n");
			print_u32(ret);
			return -1;
		}

		print("detected SDSC card\n");

/*
		if (sdcard_read_csd() < 0) {
			print("unable to read CSD\n");
			return -1;
		}
*/		
		mode = MODE_SDSC;
	} else {
		u8 i, ocr[4];
		/* SDHC? */

		/* get the rest of R7 response */
		SDCARD_CS_LO();
		for (i = 0; i < sizeof(ocr); i++)
			ocr[i] = spi_transmit(0xff);

		SDCARD_CS_HI();

		if (ocr[2] != 0x01 || ocr[3] != 0xaa) {
			/* card can't operate @2.7-3.3V */
			print("SDHC card not usable\n");
			hex_dump(ocr, 4);
			return -1;
		}

		/* leave the IDLE state (CMD41 | HCS bit) */
		for (retry = 100; retry; retry--) {
			sdcard_cmd(CMD_APP, 0);
			ret = sdcard_response();

			sdcard_cmd(CMD_41, (1UL << 30));
			ret = sdcard_response();

			if (ret == 0x00)
				break;

			delay(10000);
		}

		if (ret == 0x00) {
			sdcard_cmd(CMD_58, 0);
			ret = sdcard_response();

			SDCARD_CS_LO();
			for (i = 0; i < sizeof(ocr); i++)
				ocr[i] = spi_transmit(0xff);

			SDCARD_CS_HI();
			hex_dump(ocr, 4);
		} else {
			print("SDHC: timeout\n");
			return -1;
		}

		print("detected SDHC card\n");
		mode = MODE_SDHC;
	}

	sdcard_cmd(16, 512);
	ret = sdcard_response();
	print("CMD16 response: "); print_u32(ret); print("\n");

	print("SD card initialized.\n");

	return 0;
}

