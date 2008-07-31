#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEFAULT_BAUD 		B38400
#define DEFAULT_TTY		"/dev/ttyUSB0"
#define DEFAULT_RS232_FILE	"rs232"
#define DEFAULT_EEPROM_1ST_FILE "eeprom-1st"
#define DEFAULT_EEPROM_2ND_FILE "eeprom-2nd"
#define EEPROM_OFFSET 		0

#define msg(x...)                                                       \
do {                                                                    \
        fprintf(stderr, x);                                             \
        fflush(stderr);                                                 \
} while (0)


#define error(x...)                                                     \
do {                                                                    \
        fprintf(stderr, "\033[31m");            /* red */               \
        fprintf(stderr, "%s (%s: %d): ",                                \
                __FUNCTION__, __FILE__, __LINE__);                      \
        fprintf(stderr, x);                                             \
        fprintf(stderr, "\033[0m");                                     \
        fflush(stderr);                                                 \
} while (0)


void hex_dump (const unsigned char *buf, unsigned int addr, unsigned int len)
{
        unsigned int start;
        int i, j;
        char c;

        start = addr & ~0xf;

        for (j=0; j<len; j+=16) {
                printf("%08x:", start+j);

                for (i=0; i<16; i++) {
                        if (start+i+j >= addr && start+i+j < addr+len)
                                printf(" %02x", buf[start+i+j]);
                        else
                                printf("   ");
                }
                printf("  |");
                for (i=0; i<16; i++) {
                        if (start+i+j >= addr && start+i+j < addr+len) {
                                c = buf[start+i+j];
                                if (c >= ' ' && c < 127)
                                        printf("%c", c);
                                else
                                        printf(".");
                        } else
                                printf(" ");
                }
                printf("|\n");
        }
}

void read_blocking(int fd, unsigned char *dest, size_t size)
{
	int ret;

	do {
		ret = read(fd, dest, size);
		if (ret < 0) {
			perror("read");
			return;
		}

		size -= ret;
		dest += ret;
	} while (size);
}

static int open_tty(const char *tty)
{
	struct termios options;
	int fd = open(tty, O_RDWR | O_SYNC);

	if (fd < 0) {
		perror("open");
		return -1;
	}

	tcgetattr(fd, &options);
	cfsetispeed(&options, DEFAULT_BAUD);
	cfsetospeed(&options, DEFAULT_BAUD);
	options.c_cflag |= CLOCAL | CREAD;
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CS8;
	options.c_cflag |= CRTSCTS;
	cfmakeraw(&options);
	tcsetattr(fd, TCSANOW, &options);
	msg("port >%s< opened.\n", tty);

	return fd;
}

static unsigned char *read_file(const char *file, unsigned char *buf, ssize_t len, ssize_t *retlen)
{
	int file_fd, ret;
	struct stat statbuf;

	
	if (stat(file, &statbuf) < 0) {
		perror("stat");
		return NULL;
	}

	if (len == -1)
		len = statbuf.st_size;

	if (len < statbuf.st_size) {
		error("file image %s is too big (>%d bytes)\n", file, len);
		return NULL;
	}

	if (!buf)
		buf = (unsigned char *) malloc(len);
	
	file_fd = open(file, O_RDONLY);
	if (file_fd < 0) {
		perror("open");
		return NULL;
	}

	ret = read(file_fd, buf, len);
	close(file_fd);

	if (retlen)
		*retlen = ret;

	msg("file >%s< read, %d bytes\n", file, ret);
	return buf;
}

static int sync_cpu(int fd)
{
	const unsigned char syncbytes[] = { 0x80, 0x80, 0x80, 0x80 };
	unsigned char buf[4];

	msg("sending sync bytes ... ");
	write(fd, syncbytes, sizeof(syncbytes));
	msg("done.\n");

	read(fd, buf, 4);
	msg("reading CPU id: %02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);

	if (buf[0] != 0x06 || buf[1] != 0x0e || buf[2] != 0x07 || buf[3] != 0x00) {
		error("invalid  CPU id! Bummer.\n");
		return -1;
	}

	return 0;
}

enum {
	SPI_WRITE = 0,
	SPI_READ = 1,
	SPI_CS_HI = 2,
	SPI_CS_LO = 3
};

static int write_eeprom(int fd, unsigned char *buf, ssize_t len, ssize_t offset)
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

static int verify_eeprom(int fd, unsigned char *buf, ssize_t len, ssize_t offset)
{
	int i, a;
	unsigned char *verify_buf = malloc(len);
	unsigned char cmdbuf[256 + 4];

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
			hex_dump(verify_buf + a, 0, i);
			free(verify_buf);
			return -1;
		}

		a += i;
	}
	
	msg("\n");
	free(verify_buf);
	return 0;
}

int main(int argc, char **argv)
{
	char *tty = DEFAULT_TTY;
	char *rs232_file = DEFAULT_RS232_FILE;
	char *eeprom_1st_file = DEFAULT_EEPROM_1ST_FILE;
	char *eeprom_2nd_file = DEFAULT_EEPROM_2ND_FILE;
	unsigned char buf[512], buf2[512], shifted_buf[512 + EEPROM_OFFSET], *tmp;
	int ret, fd, len;

	memset(buf, 0, sizeof(buf));
	memset(buf2, 0, sizeof(buf2));

	fd = open_tty(tty);
	if (fd < 0)
		return fd;

	ret = sync_cpu(fd);
	if (ret < 0)
		return ret;

	if (!read_file(rs232_file, buf, sizeof(buf), &len))
		return -1;

	msg("uploading %d bytes of bootstrap code from file >%s< ... ", sizeof(buf), rs232_file);
	write(fd, buf, sizeof(buf));
	msg("done.\n");

	msg("reading back bootstrap code ... ");
	read_blocking(fd, buf2, sizeof(buf2));
	if (memcmp(buf, buf2, sizeof(buf)) != 0) {
		error("FAILED!\n");
		perror("read");
		return -1;
	}
	msg("ok.\n");

//return 0;

	if (!read_file(eeprom_1st_file, buf, sizeof(buf), &len))
		return -1;

	memmove(shifted_buf + EEPROM_OFFSET, buf, len + EEPROM_OFFSET);

	write_eeprom(fd, shifted_buf, len, 0);
	if (verify_eeprom(fd, shifted_buf, len, 0) < 0)
		return -1;
	

	tmp = read_file(eeprom_2nd_file, NULL, -1, &len);
	if (!tmp)
		return -1;

	write_eeprom(fd, tmp, len, 0x400);
	if (verify_eeprom(fd, tmp, len, 0x400) < 0)
		return -1;

	msg("YEAH.\n");
	return 0;
}

