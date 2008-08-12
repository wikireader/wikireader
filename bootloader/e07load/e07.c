#include <stdio.h>
#include <unistd.h>
#include "e07.h"
#include "misc.h"

int sync_cpu(int fd)
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

