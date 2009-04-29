/*
 * Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "article.h"
#include <string.h>
#include <file-io.h>
#include <guilib.h>
#include <glyph.h>
#include <fontfile.h>
#include <search.h>
#include <limits.h>
#include <msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <decompress.h>

/* decompressed article */
static char *article_data = NULL;
static unsigned int article_data_length = 0;

static int current_page = -1;
static unsigned int current_page_offset = -1;
static char filename_buf[8 + 1 + 3 + 1];


int article_open(const char *target)
{
	unsigned int file, offset, lzo_len;
	int r, article_fd;
	article_close();

	/* find out which file */
	article_extract_file_and_offset(target, &file, &offset);
	if (file == UINT_MAX && offset == UINT_MAX)
		return -1;

	/* open it */
	snprintf(&filename_buf[0], sizeof(filename_buf),
		"wikiset%d.set", file % 10);
	article_fd = wl_open(filename_buf, WL_O_RDONLY);
	if (article_fd < 0)
		return -2;

	/* seek in it */
	if (wl_seek(article_fd, offset) < 0)
		goto exit_1;

	/* read size */
	/* FIXME: endian... */
	r = wl_read(article_fd, &lzo_len, sizeof(u_int32_t));
	if (r != sizeof(u_int32_t))
		goto exit_1;

	/* decompress it */
	article_data = decompress_block(article_fd,
					lzo_len + sizeof(u_int32_t),
					&article_data_length);
	if (!article_data)
		goto exit_1;

	/* decompress */
	wl_close(article_fd);
	return 1;

exit_1:
	wl_close(article_fd);
	return -3;
}

#define READ_UINT(var, fd) \
	res = wl_read(fd, &var, 4); \
	if (res < 4) { \
		msg(MSG_INFO, "Could not read article file - wl_read() returned: %i\n", res); \
		current_page_offset = article_data_length; \
		goto out; \
	}

void article_display(enum article_nav nav)
{
	guilib_fb_lock();
	guilib_clear();

#warning BROKEN...Only paint one page
	//current_page_offset = 0;

	// TODO: Decoding routines...

	guilib_fb_unlock();
}

void article_close(void)
{
	current_page = -1;
	current_page_offset = -1;

	if (article_data) {
		free(article_data);
		article_data = NULL;
		article_data_length = 0;
	}
}

/**
 * From a decimal representation like:
 *     400000002342
 * extract
 *     4 ------------ The file to use wikiped4.set
 *      00000002342-- The offset within the file
 *
 * In case of error file and offset will contain UINT_MAX
 */
void article_extract_file_and_offset(const char *target,
				    unsigned int *file, unsigned int *offset)
{
	int len, i;
	unsigned int out_offset = 0;
	unsigned int accu = 1;
	/* naughty... spank... */
	if (!target || (len = strlen(target)) != TARGET_SIZE)
		goto error_out;

	for (i = 0; i < len; ++i) {
		if (target[i] < 0x30 || target[i] > 0x39)
			goto error_out;
	}


	for (i = len-1; i >= 1; --i) {
		out_offset += (target[i] - 0x30) * accu;
		accu *= 10;
	}

	*offset = out_offset;
	*file = target[0] - 0x30;
	return;

error_out:
	*file = UINT_MAX;
	*offset = UINT_MAX;
	return;
}
