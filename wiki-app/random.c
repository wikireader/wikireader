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
#include <tff.h>
#include <search.h>
#include <ctype.h>
#include "random.h"
#include "wikilib.h"
#include "msg.h"

#define TITLE_LEN 256


int random_article(void)
{
	uint32_t offset;
	int32_t fd, res, i;
	uint8_t buff[TITLE_LEN], *eol_ptr;
	const char *target;

	//TODO: EOF

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

	/* hackish solution to get the article displayed - no idea how can one make such a stupid API */
	for (i = 0; i < TITLE_LEN; i++)
		search_remove_char();

	for (i = 0; i < strlen(buff); i++)
		search_add_char(tolower(buff[i]));

	target = search_fetch_result();
	open_article(&target[strlen(target)-TARGET_SIZE], ARTICLE_NEW);

	// save new offset
	offset += strlen(buff) + 1;

	fd = wl_open(RAND_OFFSET_PATH, WL_O_CREATE);

	if (fd >= 0) {
		debug_printf("saving: %u\n", (unsigned int)offset);
		res = wl_write(fd, &offset, sizeof(uint32_t));
		wl_close(fd);
	} else {
		debug_printf("can't save: %u %i\n", (unsigned int)offset, fd);
	}

	return 1;
}

