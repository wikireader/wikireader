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
	int fd = open(tty, O_RDWR|O_NOCTTY|O_SYNC);

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
	
	if (tcsetattr(fd, TCSANOW, &options) < 0) {
		close(fd);
		error("unable to set serial tty configuration.\n");
		return -1;
	}

	debug("port >%s< opened.\n", tty);
	return fd;
}

