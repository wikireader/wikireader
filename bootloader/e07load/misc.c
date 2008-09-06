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
#include <sys/ioctl.h>

#include "misc.h"

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

void read_blocking(int fd, char *dest, size_t size)
{
	int ret;

	while (size) {
		ret = read(fd, dest, size);
		if (ret < 0) {
			perror("read");
			return;
		}

		size -= ret;
		dest += ret;
	}
}

void strchomp(char *s)
{
        while(s[strlen(s) - 1] == '\n')
                s[strlen(s) - 1] = '\0';
}

void flush_fd(const int fd)
{
	int bytes = 0, buf = -10;
	if (ioctl(fd, FIONREAD, &bytes) != -1){
		while (bytes-- > 0 && read(fd, &buf, 1) == 1);
	}
}

