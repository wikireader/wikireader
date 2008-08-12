#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "misc.h"
#include "tty.h"

#define DEFAULT_BAUD B57600

int open_tty(const char *tty)
{
	struct termios options;
	int fd = open(tty, O_RDWR | O_SYNC);

	if (fd < 0) {
		error("unable to open tty %s: %s\n", tty, strerror(errno));
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
	debug("port >%s< opened.\n", tty);

	return fd;
}

