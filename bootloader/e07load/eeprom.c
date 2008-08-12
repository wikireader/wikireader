#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "eeprom.h"
#include "misc.h"

enum {
	SPI_WRITE = 0,
	SPI_READ = 1,
	SPI_CS_HI = 2,
	SPI_CS_LO = 3
};

int write_eeprom(int fd, char *buf, ssize_t len, ssize_t offset)
{
	int i, a;
	unsigned char cmdbuf[256 + 4];

	cmdbuf[0] = SPI_CS_LO;
	write(fd, buf, 1);

#if 0
	cmdbuf[0] = SPI_WRITE;
	cmdbuf[1] = 1;
	cmdbuf[2] = 0x9f;
	write(fd, cmdbuf, 3);

	cmdbuf[0] = SPI_READ;
	cmdbuf[1] = 3;
	write(fd, cmdbuf, 2);
	read(fd, cmdbuf, 3);

	msg("EEPROM identifier: %02x %02x %02x\n", cmdbuf[0], cmdbuf[1], cmdbuf[2]);
#endif

	cmdbuf[0] = SPI_CS_HI;
	write(fd, cmdbuf, 1);

	msg("writing %d bytes to EEPROM, offset 0x%x ", len, offset);

	/* write data */
	for (a = 0; a < len;) {
		/* set EEPROM's write enable */
		cmdbuf[0] = SPI_CS_LO;
		write(fd, cmdbuf, 1);

		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = 1;
		cmdbuf[2] = 0x06;
		write(fd, cmdbuf, 3);
		
		cmdbuf[0] = SPI_CS_HI;
		write(fd, cmdbuf, 1);

		/* enter page write mode */
		cmdbuf[0] = SPI_CS_LO;
		write(fd, cmdbuf, 1);

		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = 4;
		cmdbuf[2] = 0x0a;
		cmdbuf[3] = (a + offset) >> 16;
		cmdbuf[4] = (a + offset) >> 8;
		cmdbuf[5] = (a + offset) & 0xff;
		write(fd, cmdbuf, 4 + 2);

		cmdbuf[0] = SPI_WRITE;

		for (i = 0; i < 0x80 && a + i < len; i++)
			cmdbuf[i + 2] = buf[i + a];

		cmdbuf[1] = i;
		write(fd, cmdbuf, i + 2);

		cmdbuf[0] = SPI_CS_HI;
		write(fd, cmdbuf, 1);
		usleep(100 * 1000);

		msg(".");
		fflush(0);

		a += i;
	}

	msg("\n");
	return 0;
}

int verify_eeprom(int fd, char *buf, ssize_t len, ssize_t offset)
{
	int i, a;
	char *verify_buf = malloc(len);
	char cmdbuf[256 + 4];

	msg("verifying %d bytes of EEPROM, offset 0x%x ", len, offset);
	memset(verify_buf, 0, len);

	for (a = 0; a < len;) {
		i = ((len - a) < 0x80) ? (len - a) : 0x80;

		/* READ command */
		cmdbuf[0] = SPI_CS_LO;
		write(fd, cmdbuf, 1);

		cmdbuf[0] = SPI_WRITE;
		cmdbuf[1] = 4;
		cmdbuf[2] = 0x03;
		cmdbuf[3] = (a + offset) >> 16;
		cmdbuf[4] = (a + offset) >> 8;
		cmdbuf[5] = (a + offset) & 0xff;
		write(fd, cmdbuf, 4 + 2);

		cmdbuf[0] = SPI_READ;
		cmdbuf[1] = i;
		write(fd, cmdbuf, 2);
		read_blocking(fd, verify_buf + a, i);

		cmdbuf[0] = SPI_CS_HI;
		write(fd, cmdbuf, 1);
		msg (".");
		fflush(0);
	
		if (memcmp(buf + a, verify_buf + a, i) != 0) {
			msg("\n");
			error("EEPROM verify failed in chunk @offset 0x%x!\n", a);
			hex_dump((unsigned char *) verify_buf + a, 0, i);
			error("expected:\n");
			hex_dump((unsigned char *) buf + a, 0, i);
			free(verify_buf);
			return -1;
		}

		a += i;
	}
	
	msg("\n");
	free(verify_buf);
	return 0;
}

