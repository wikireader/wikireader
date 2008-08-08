#include "regs.h"
#include "wikireader.h"
#include "spi.h"
#include "misc.h"
#include "crc.h"

#define CMDREAD		0x11
#define CMDREADCSD	0x09

#define BYTES_PER_SECTOR 512

static unsigned char sdcard_response(void)
{
	unsigned char ret, retry = 8;

	while (retry--) {
		SDCARD_CS_LO();
		ret = spi_transmit(0xff);
		SDCARD_CS_HI();

		if (ret != 0xff)
			break;
	}

	return ret;
}

static unsigned char sdcard_cmd(unsigned char cmd, unsigned int param)
{
	unsigned char i, c[] = {
		0x40 | cmd,
		param >> 24,
		param >> 16,
		param >> 8,
		param
	};

	SDCARD_CS_LO();
	for (i = 0; i < sizeof(c); i++)
		spi_transmit(c[i]);

	/* checksum */
	spi_transmit(crc7(0, c, sizeof(c)) | 1);
	SDCARD_CS_HI();
	return sdcard_response();
}

static int sdcard_read_sector(unsigned int sector, unsigned char *buf)
{
	unsigned char ret;
	unsigned int i, retry = 0xffff;

	/* only valid for non-SDHC cards! */
	sector *= BYTES_PER_SECTOR;


	for (i = 0; i < BYTES_PER_SECTOR; i++)
		buf[i] = 0;

	ret = sdcard_cmd(CMDREAD, sector);

	if (ret != 0) {
		print("bad card response\n");
		return -1;
	}

	while (retry--) {
		ret = sdcard_response();
		if (ret != 0xff)
			break;
	}

	if (ret != 0xfe) {
		print("unable to read first block of sector\n");
		return -2;
	}

	SDCARD_CS_LO();
	for (i = 0; i < BYTES_PER_SECTOR; i++)
		buf[i] = spi_transmit(0xff);
	SDCARD_CS_HI();

	/* checksum, ignored */
	spi_transmit(0xff);
	spi_transmit(0xff);

	return 0;
}

int sdcard_init(void)
{
	unsigned char ret;
	unsigned char buf[BYTES_PER_SECTOR];
	unsigned int retry = 100;

	while (retry--) {
		ret = sdcard_cmd(0, 0);
		if (ret == 1)
			break;
	}

	if (ret != 1) {
		print("SD card failed to init.\n");
		return -1;
	}

	retry = 32000;
	while (retry--) {
		ret = sdcard_cmd(1, 0);
		if (ret != 1)
			break;
	}

	if (ret != 0) {
		print("SD card not responding\n");
		return -1;
	}

	sdcard_read_sector(0, buf);
	hex_dump(buf, sizeof(buf));
	sdcard_read_sector(1, buf);
	hex_dump(buf, sizeof(buf));

	return 0;
}

