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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "eeprom.h"
#include "misc.h"
#include "../config.h"

enum {
	SPI_WRITE = 0,
	SPI_READ  = 1,
	SPI_CS_HI = 2,
	SPI_CS_LO = 3
};

static void spi_cs_lo (int fd)
{
	unsigned char cmdbuf = SPI_CS_LO;
	write(fd, &cmdbuf, 1);
}

static void spi_cs_hi (int fd)
{
	unsigned char cmdbuf = SPI_CS_HI;
	write(fd, &cmdbuf, 1);
}

static void eeprom_write_enable (int fd, int enable)
{
	unsigned char cmdbuf[3];

	spi_cs_lo(fd);
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 1;
	cmdbuf[2] = enable ? 0x06 : 0x04;
	write(fd, cmdbuf, 3);
	spi_cs_hi(fd);
}

#ifdef EEPROM_MP45PE80
static void eeprom_write_page (int fd, unsigned int addr)
{
	unsigned char cmdbuf[6];

	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 4;
	cmdbuf[2] = 0x0a;
	cmdbuf[3] = addr >> 16;
	cmdbuf[4] = addr >> 8;
	cmdbuf[5] = addr & 0xff;
	write(fd, cmdbuf, sizeof(cmdbuf));
}

#define CHUNKSIZE 128
int write_eeprom (int fd, unsigned char *buf, ssize_t len, ssize_t offset)
{
	int a, rest = offset % CHUNKSIZE;
	unsigned char cmdbuf[CHUNKSIZE + 4];

	spi_cs_hi(fd);
	msg("writing %d bytes to EEPROM, offset 0x%x ", len, offset);

	for (a = 0; a < len;) {
		int xlen = len - a;

		if (xlen > CHUNKSIZE)
			xlen = CHUNKSIZE;

		if (rest) {
			xlen -= rest;
			rest = 0;
		}

		eeprom_write_enable(fd, 1);

		spi_cs_lo(fd);
		eeprom_write_page(fd, a + offset);
		
		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = xlen;
		memcpy(cmdbuf + 2, buf + a, xlen);
		write(fd, cmdbuf, xlen + 2);
		spi_cs_hi(fd);

		eeprom_write_enable(fd, 0);

		usleep(100 * 1000);
		printf(".");
		fflush(stdout);
		fflush(stderr);

		a += xlen;
	}

	msg("\n");
	return 0;
}

#elif EEPROM_SST25VF040

static void eeprom_erase_block (int fd, int block)
{
	unsigned char cmdbuf[6];
	static unsigned char block_erased[1 << 12] = { 0 };
	int a = block << 12;

	block &= (1 << 12) - 1;

	if (block_erased[block])
		return;

	msg("erasing 4k block @addr 0x%08x\n", a);
	eeprom_write_enable(fd, 1);
	spi_cs_lo(fd);
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 4;
	cmdbuf[2] = 0x20;
	cmdbuf[3] = a >> 16;
	cmdbuf[4] = a >> 8;
	cmdbuf[5] = a & 0xff;
	write(fd, cmdbuf, sizeof(cmdbuf));
	spi_cs_hi(fd);
	usleep(100 * 1000);
	block_erased[block] = 1;
}

static void eeprom_wait_ready (int fd)
{
	//char status;
	//unsigned char cmdbuf[3];

	usleep(8000);
	return;
/*
	do {
		spi_cs_lo(fd);
		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = 1;
		cmdbuf[2] = 0x05;
		write(fd, cmdbuf, 3);

		cmdbuf[0] = SPI_READ;
		cmdbuf[1] = 1;
		write(fd, cmdbuf, 2);
		read_blocking(fd, &status, 1);
		spi_cs_hi(fd);
	} while (status & 1);
*/
}

static void eeprom_set_block_protection (int fd)
{
	unsigned char cmdbuf[4];

	/* enable write status register */
	spi_cs_lo(fd);
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 1;
	cmdbuf[2] = 0x50;
	write(fd, cmdbuf, 3);
	spi_cs_hi(fd);

	/* write status register */
	spi_cs_lo(fd);
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 2;
	cmdbuf[2] = 0x01;
	cmdbuf[3] = 0x00;
	write(fd, cmdbuf, 4);
	spi_cs_hi(fd);
}

int write_eeprom (int fd, unsigned char *buf, ssize_t len, ssize_t offset)
{
	unsigned char cmdbuf[8];
	int i, first_block, last_block;

	if (len & 1)
		len++;

	spi_cs_hi(fd);
	eeprom_write_enable(fd, 1);
	eeprom_set_block_protection(fd);

	/* erase all 4k block we will touch */
	first_block = offset >> 12;
	last_block = (offset + len) >> 12;

	for (i = first_block; i <= last_block; i++)
		eeprom_erase_block(fd, i);
	
	msg("writing %d bytes to EEPROM, offset 0x%x ", len, offset);
	eeprom_write_enable(fd, 1);

	/* issue 1st AII command */
	spi_cs_lo(fd);
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 6;
	cmdbuf[2] = 0xad;
	
	if (offset & 1) {
		cmdbuf[3] = (offset - 1) >> 16;
		cmdbuf[4] = (offset - 1) >> 8;
		cmdbuf[5] = (offset - 1) & 0xff;
		cmdbuf[6] = 0;
		cmdbuf[7] = *buf++;
		len--;
	} else {
		cmdbuf[3] = offset >> 16;
		cmdbuf[4] = offset >> 8;
		cmdbuf[5] = offset & 0xff;
		cmdbuf[6] = *buf++;
		cmdbuf[7] = *buf++;
		len -= 2;
	}

	write(fd, cmdbuf, sizeof(cmdbuf));
	spi_cs_hi(fd);

	while (len > 0) {
		eeprom_wait_ready(fd);
		
		/* write next 2 bytes */
		spi_cs_lo(fd);
		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = 3;
		cmdbuf[2] = 0xad;
		cmdbuf[3] = *buf++;
		cmdbuf[4] = *buf++;
		write(fd, cmdbuf, 5);
		spi_cs_hi(fd);
		if ((len & 0x7f) == 0) {
			printf(".");
			fflush(stdout);
			fflush(stderr);
		}

		len -= 2;
	}

	eeprom_write_enable(fd, 0);

	msg("\n");
	return 0;
}

#else
#error EEPROM write implementation missing.
#endif

static void eeprom_read_page (int fd, unsigned int addr)
{
	unsigned char cmdbuf[6];

	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 4;
	cmdbuf[2] = 0x03;
	cmdbuf[3] = addr >> 16;
	cmdbuf[4] = addr >> 8;
	cmdbuf[5] = addr & 0xff;
	write(fd, cmdbuf, sizeof(cmdbuf));
}

int verify_eeprom (int fd, unsigned char *buf, ssize_t len, ssize_t offset)
{
	int i, a;
	char *verify_buf = malloc(len);
	char cmdbuf[256 + 4];

	spi_cs_hi(fd);
	msg("verifying %d bytes of EEPROM, offset 0x%x ", len, offset);
	memset(verify_buf, 0, len);

	for (a = 0; a < len;) {
		i = ((len - a) < 0x80) ? (len - a) : 0x80;

		spi_cs_lo(fd);
		eeprom_read_page(fd, a + offset);

		cmdbuf[0] = SPI_READ;
		cmdbuf[1] = i;
		write(fd, cmdbuf, 2);
		read_blocking(fd, verify_buf + a, i);
		spi_cs_hi(fd);

		msg(".");
		fflush(stdout);
		fflush(stderr);
	
		if (memcmp(buf + a, verify_buf + a, i) != 0) {
			msg("\n");
			error("EEPROM verify failed in chunk @offset 0x%x, len = %d!\n", a, i);
			hex_dump((unsigned char *) verify_buf + a, 0, i);
			msg("expected:\n");
			hex_dump((unsigned char *) buf + a, 0, i);
			free(verify_buf);
			return -1;
		}

		a += i;
	}
	
	msg(" ok.\n");
	free(verify_buf);
	return 0;
}

