#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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

void strchomp(char *s)
{
        while(s[strlen(s) - 1] == '\n')
                s[strlen(s) - 1] = '\0';
}

void flush_fd(int fd)
{
	char buf;
	long arg = 1;
	fcntl(fd, F_SETFL, O_NONBLOCK, arg);

	while (read(fd, &buf, 1));
}

