#include "regs.h"
#include "wikireader.h"
#include "spi.h"
#include "misc.h"

#define CMDREAD         17
#define CMDWRITE        24
#define CMDREADCSD       9

#define BYTES_PER_SECTOR 512

static unsigned char sdcard_spi_transmit(unsigned char out)
{
	unsigned char in;

	//SDCARD_CS_LO();
	in = spi_transmit(out);
	//SDCARD_CS_HI();
	return in;
}

static unsigned char sdcard_response(void)
{
	unsigned char ret, retry = 8;

	do {
		SDCARD_CS_LO();
		ret = sdcard_spi_transmit(0xff);
		SDCARD_CS_HI();
	} while (ret == 0xff && retry--);

	return ret;
}

static unsigned char sdcard_cmd(unsigned char cmd, unsigned short param_a, unsigned short param_b)
{
	SDCARD_CS_LO();
	sdcard_spi_transmit(0xff);
	sdcard_spi_transmit(0x40 | cmd);
	sdcard_spi_transmit(param_a >> 8);
	sdcard_spi_transmit(param_a & 0xff);
	sdcard_spi_transmit(param_b >> 8);
	sdcard_spi_transmit(param_b & 0xff);

	/* checksum */
	sdcard_spi_transmit(0x95);
	SDCARD_CS_HI();
	return sdcard_response();
}

static int sdcard_read_sector(unsigned int sector, unsigned char *buf)
{
	unsigned char ret;
	unsigned int i, retry = 0xffff;

	ret = sdcard_cmd(CMDREAD, sector >> 16, sector);

	if (ret != 0) {
		print("bad card response\n");
		return -1;
	}


	do {
		ret = sdcard_response();
	} while (ret == 0xff && retry--);

	if (ret != 0xfe) {
		print("unable to read first block of sector\n");
		return -2;
	}

	SDCARD_CS_LO();
	for (i = 0; i < BYTES_PER_SECTOR; i++)
		buf[i] = sdcard_spi_transmit(0xff);
	SDCARD_CS_HI();

	/* checksum, ignored */
	sdcard_spi_transmit(0xff);
	sdcard_spi_transmit(0xff);

	return 0;
}

int sdcard_init(void)
{
	unsigned char ret;
	unsigned char buf[BYTES_PER_SECTOR];
	unsigned int retry = 100;

	do {
		ret = sdcard_cmd(0, 0, 0);
	} while (ret != 1 && retry--);

	if (ret != 1) {
		print("SD card failed to init.\n");
		return -1;
	}

	retry = 32000;

	do {
		ret = sdcard_cmd(1, 0, 0);
	} while (ret == 1 && retry--);

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

