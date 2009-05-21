//
// Authors:	Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//



#include <inttypes.h>
#include <file-io.h>
#include <string.h>
#include "random.h"
#include "wikilib.h"
#include "msg.h"

#define TITLE_LEN 256


int random_article(void)
{
	uint32_t offset;
	int32_t fd, res;
	uint8_t buff[TITLE_LEN], *eol_ptr;

	//TODO: EOF
// 	memset(buff, 'A', TITLE_LEN);
// 	buff[TITLE_LEN - 1] = 0;

	debug_printf("random button pressed\n");

	fd = wl_open(RAND_OFFSET_PATH, WL_O_RDONLY);
	offset = 0;

	if (fd >= 0) {
		res = wl_read(fd, &offset, sizeof(uint32_t));

		if (res != sizeof(uint32_t))
			offset = 0;

		wl_close(fd);
	} else {
		debug_printf("offset file not found\n");
	}

	fd = wl_open(RAND_SRC_PATH, WL_O_RDONLY);
	if (fd < 0) {
		debug_printf("rand src not found\n");
		return -1;
	}

	/* seek in it */
	if (offset > 0)
		wl_seek(fd, offset);

	debug_printf("reading\n");
	res = wl_read(fd, buff, TITLE_LEN);

	if (res != TITLE_LEN)
		return -2;

	wl_close(fd);

	// find end of line
	eol_ptr = strchr(buff, '\n');
	*eol_ptr = '\0';
	debug_printf("opening (%i): %s\n", strlen(buff), buff);
	open_article(buff, ARTICLE_NEW);

	// save new offset
	offset += strlen(buff) + 1;

	fd = wl_open(RAND_OFFSET_PATH, WL_O_WRONLY);

	if (fd >= 0) {
		debug_printf("saving: %u\n", (unsigned int)offset);
		res = wl_write(fd, &offset, sizeof(uint32_t));
		wl_close(fd);
	}

	debug_printf("done\n");
	return 1;
}

