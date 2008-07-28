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

#define DEFAULT_BAUD B38400
#define DEFAULT_TTY "/dev/ttyUSB0"
#define DEFAULT_BOOTSTRAP_FILE "rs232"
#define DEFAULT_EEPROM_FILE "eeprom.img"

static const char syncbytes[] = { 0x80, 0x80, 0x80, 0x80 };
static unsigned char bootstrap_buf[512], bootstrap_buf2[512], buf[4], *eeprom_image;
static int eeprom_image_size = 0;

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
	printf("port >%s< opened.\n", tty);

	return fd;
}

static int read_files(const char *bootstrap_file, const char *eeprom_image_name)
{
	int file_fd;
	struct stat statbuf;

	printf("reading bootstrap code from file >%s<\n", bootstrap_file);
	file_fd = open(bootstrap_file, O_RDONLY);
	if (file_fd < 0) {
		perror("open");
		return -1;
	}
	read(file_fd, bootstrap_buf, sizeof(bootstrap_buf));
	close(file_fd);

	printf("reading EEPROM image from file >%s<\n", eeprom_image_name);
	if (stat(eeprom_image_name, &statbuf) < 0) {
		perror("stat");
		return -1;
	}

	file_fd = open(eeprom_image_name, O_RDONLY);
	if (file_fd < 0) {
		perror("open");
		return -1;
	}

	eeprom_image_size = statbuf.st_size;
	eeprom_image = malloc(eeprom_image_size);
	if (!eeprom_image) {
		perror("malloc");
		return -1;
	}

	read(file_fd, eeprom_image, eeprom_image_size);
	close(file_fd);
	printf(" ... %d bytes loaded\n", eeprom_image_size);

	return 0;
}

static int sync_cpu(int fd)
{
	const unsigned char syncbytes[] = { 0x80, 0x80, 0x80, 0x80 };

	printf("sending sync bytes ... ");
	write(fd, syncbytes, sizeof(syncbytes));
	printf("done.\n");

	read(fd, buf, 4);
	printf("reading CPU id: %02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3]);

	if (buf[0] != 0x06 || buf[1] != 0x0e || buf[2] != 0x07 || buf[3] != 0x00) {
		printf("invalid  CPU id! Bummer.\n");
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

static int write_eeprom(int fd)
{
	int i, a;
	unsigned char buf[256 + 4];
	unsigned char *verify_buf = malloc(eeprom_image_size);

	buf[0] = SPI_CS_LO;
	write(fd, buf, 1);

	buf[0] = SPI_WRITE;
	buf[1] = 1;
	buf[2] = 0x9f;
	write(fd, buf, 3);

	buf[0] = SPI_READ;
	buf[1] = 3;
	write(fd, buf, 2);
	read(fd, buf, 3);

	printf("EEPROM identifier: %02x %02x %02x\n", buf[0], buf[1], buf[2]);

	buf[0] = SPI_CS_HI;
	write(fd, buf, 1);

	printf("writing eeprom ");

	/* write data */
	for (a = 0; a < eeprom_image_size;) {
		/* set EEPROM's write enable */
		buf[0] = SPI_CS_LO;
		write(fd, buf, 1);

		buf[0] = SPI_WRITE;
		buf[1] = 1;
		buf[2] = 0x06;
		write(fd, buf, 3);
		
		buf[0] = SPI_CS_HI;
		write(fd, buf, 1);

		/* enter page write mode */
		buf[0] = SPI_CS_LO;
		write(fd, buf, 1);

		buf[0] = SPI_WRITE;
		buf[1] = 4;
		buf[2] = 0x0a;
		buf[3] = (a >> 16);
		buf[4] = (a >> 8);
		buf[5] = a & 0xff;
		write(fd, buf, 4 + 2);

		buf[0] = SPI_WRITE;

		for (i = 0; i < 0x80 && a + i < eeprom_image_size; i++)
			buf[i + 2] = eeprom_image[i + a];

		buf[1] = i;
		write(fd, buf, i + 2);

		buf[0] = SPI_CS_HI;
		write(fd, buf, 1);
		usleep(100 * 1000);

		printf(".");
		fflush(0);

		a += i;
	}

	printf("\n");
	
	printf("verifying EEPROM contents \n");
	usleep(100 * 1000);
	memset(verify_buf, 0, eeprom_image_size);

	for (a = 0; a < eeprom_image_size;) {
		i = ((eeprom_image_size - a) < 0xff) ?
			(eeprom_image_size - a) : 0xff;

		/* READ command */
		buf[0] = SPI_CS_LO;
		write(fd, buf, 1);

		buf[0] = SPI_WRITE;
		buf[1] = 4;
		buf[2] = 0x03;
		buf[3] = (a >> 16);
		buf[4] = (a >> 8);
		buf[5] = a & 0xff;
		write(fd, buf, 4 + 2);

		buf[0] = SPI_READ;
		buf[1] = i;
		write(fd, buf, 2);
		read_blocking(fd, verify_buf + a, i);

		buf[0] = SPI_CS_HI;
		write(fd, buf, 1);
		usleep(100 * 1000);
		printf (".");
		fflush(0);

		a += i;
	}

	if (memcmp(eeprom_image, verify_buf, eeprom_image_size) != 0) {
		printf("EEPROM verify failed!\n");
		printf("written:\n");
		hex_dump(eeprom_image, 0, eeprom_image_size);
		printf("read back:\n");
		hex_dump(verify_buf, 0, eeprom_image_size);
		free(verify_buf);
		return -1;
	}

	free(verify_buf);
	return 0;
}

int main(int argc, char **argv)
{
	char *tty = DEFAULT_TTY;
	char *bootstrap_file = DEFAULT_BOOTSTRAP_FILE;
	char *eeprom_image_name = DEFAULT_EEPROM_FILE;
	int ret, fd;

	memset(bootstrap_buf, 0, sizeof(bootstrap_buf));
	memset(bootstrap_buf2, 0, sizeof(bootstrap_buf2));

	fd = open_tty(tty);
	if (fd < 0)
		return fd;

	ret = read_files(bootstrap_file, eeprom_image_name);
	if (ret < 0)
		return ret;

	ret = sync_cpu(fd);
	if (ret < 0)
		return ret;

	printf("uploading 512 bytes of bootstrap code from file >%s< ... ", bootstrap_file);
	write(fd, bootstrap_buf, sizeof(bootstrap_buf));
	printf("done.\n");

	printf("reading back bootstrap code ... ");
	read_blocking(fd, bootstrap_buf2, sizeof(bootstrap_buf2));
	if (memcmp(bootstrap_buf, bootstrap_buf2, sizeof(bootstrap_buf)) != 0) {
		printf("FAILED!\n");
		perror("read");

		hex_dump(bootstrap_buf, 0, sizeof(bootstrap_buf));
		hex_dump(bootstrap_buf2, 0, sizeof(bootstrap_buf2));

		return -7;
	}
	printf("ok.\n");

	ret = write_eeprom(fd);
	if (ret < 0)
		return ret;

	printf("YEAH.\n");
	return 0;
}

