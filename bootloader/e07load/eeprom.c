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

static void eeprom_write_enable (int fd)
{
	unsigned char cmdbuf[3];

	spi_cs_lo(fd);
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 1;
	cmdbuf[2] = 0x06;
	write(fd, cmdbuf, 3);
	spi_cs_hi(fd);
}

static void eeprom_write_page(int fd, unsigned int addr)
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

static void eeprom_read_page(int fd, unsigned int addr)
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

#define CHUNKSIZE 128

int write_eeprom(int fd, unsigned char *buf, ssize_t len, ssize_t offset)
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

		eeprom_write_enable(fd);

		spi_cs_lo(fd);
		eeprom_write_page(fd, a + offset);
		
		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = xlen;
		memcpy(cmdbuf + 2, buf + a, xlen);
		write(fd, cmdbuf, xlen + 2);
		spi_cs_hi(fd);
		
		usleep(100 * 1000);
		printf(".");
		fflush(0);

		a += xlen;
	}

	msg("\n");
	return 0;
}

int verify_eeprom(int fd, unsigned char *buf, ssize_t len, ssize_t offset)
{
	int i, a;
	char *verify_buf = malloc(len);
	unsigned char cmdbuf[256 + 4];

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

		printf(".");
		fflush(0);
	
		if (memcmp(buf + a, verify_buf + a, i) != 0) {
			msg("\n");
			error("EEPROM verify failed in chunk @offset 0x%x, len = %d!\n", a, i);
			hex_dump((unsigned char *) verify_buf + a, 0, i);
			error("expected:\n");
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

