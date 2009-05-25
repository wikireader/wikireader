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
#include "wom_reader.h"

#define TITLE_LEN 256

#define DBG_RANDOM 0


int random_article(void)
{
	uint32_t offset;
	int32_t fd, res;
	uint8_t buff[TITLE_LEN], *eol_ptr;
	const wom_article_index_t* idx;
	char target_buf[32];
	unsigned int file_size = 0;

	fd = wl_open(RAND_OFFSET_PATH, WL_O_RDONLY);
	offset = 0;

	if (fd >= 0) {
		res = wl_read(fd, &offset, sizeof(uint32_t));

		if (res != sizeof(uint32_t))
			offset = 0;

		wl_close(fd);
	}

read_src:
	fd = wl_open(RAND_SRC_PATH, WL_O_RDONLY);
	if (fd < 0) {
		DP(1, ("X rand src not found\n"));
		return -1;
	}

	/* old saved offset must be discarded if it is too large */
	wl_fsize(fd, &file_size);
	if (offset > file_size)
		offset = 0;

	/* seek in our random article source */
	if (offset > 0)
		res = wl_seek(fd, offset);

	if (TITLE_LEN > file_size - offset)
		res = file_size - offset;
	else
		res = TITLE_LEN - 1;

	res = wl_read(fd, buff, res);
	wl_close(fd);

	/* find end of line */
	buff[res] = '\0';
	eol_ptr = strchr(buff, '\n');

	/* no end of line found - retry at the beginning of the file */
	if (eol_ptr == NULL) {
		if (offset != 0) {
			DP(DBG_RANDOM, ("O no article index found: resetting offset\n"));
			offset = 0;
			goto read_src;
		}

		DP(1, ("X no article index found: %s\n", buff));
		return -2;
	}

	*eol_ptr = '\0';
	DP(DBG_RANDOM, ("O opening (%i): %s\n", strlen(buff), buff));

	idx = wom_find_article(g_womh, buff, strlen(buff));
	if (idx) {
		sprintf(target_buf, "%.6x", (unsigned int) idx->offset_into_articles);
		open_article(target_buf, ARTICLE_NEW);
	}

	/* save new offset */
	offset += strlen(buff) + 1;

	fd = wl_open(RAND_OFFSET_PATH, WL_O_CREATE);

	if (fd >= 0) {
		DP(DBG_RANDOM, ("O saving: %u\n", (unsigned int)offset));
		res = wl_write(fd, &offset, sizeof(uint32_t));
		wl_close(fd);
	} else {
		DP(DBG_RANDOM, ("O can't save: %u %i\n", (unsigned int)offset, fd));
	}

	return 1;
}

