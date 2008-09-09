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
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "e07.h"
#include "misc.h"

#define ID_E07 0x060e0700
#define ID_L17 0x06151700
#define ID_L18 0x06151701

int sync_cpu(int fd)
{
	const unsigned char syncbytes[] = { 0x80, 0x80, 0x80, 0x80 };
	unsigned char buf[4];
	unsigned int id;

	/* clear garbage from serial port */
	flush_fd(fd);

	msg("sending sync bytes ... ");
	write(fd, syncbytes, sizeof(syncbytes));
	msg("done.\n");

	read(fd, buf, 4);
	id = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
	
	msg("reading CPU id: %08x\n", id);

	switch (id) {
		case ID_E07:
			msg("CPU id does match E07!\n");
			return 0;
		case ID_L17:
                	msg("CPU id does match L17!\n");
	                return 0;
		case ID_L18:
                	msg("CPU id does match L18!\n");
			return 0;
		default:
        		error("CPU id does not match! Bummer.\n");
			return -1;
	}
}

int bootstrap(int ttyfd, const char *bootstrap_file)
{
	char bootstrap_buf[512], verify_buf[512];
	int fd = open(bootstrap_file, O_RDONLY);
	
	memset(bootstrap_buf, 0, sizeof(bootstrap_buf));
	memset(verify_buf, 0, sizeof(verify_buf));
	
	if (fd < 0) {
		error("unable to open bootstrap file %s: %s\n", bootstrap_file, strerror(errno));
		return fd;
	}

	read(fd, bootstrap_buf, sizeof(bootstrap_buf));
	close(fd);

	msg("uploading %d bytes of bootstrap code from file >%s< ... ", sizeof(bootstrap_buf), bootstrap_file);
	write(ttyfd, bootstrap_buf, sizeof(bootstrap_buf));
	msg("done.\n");

	msg("reading back bootstrap code ... ");
	read_blocking(ttyfd, verify_buf, sizeof(verify_buf));
	if (memcmp(bootstrap_buf, verify_buf, sizeof(bootstrap_buf)) != 0) {
		error("FAILED to verify bootstrap code!\n");
		return -1;
	}

	msg("ok.\n");
	return 0;
}

