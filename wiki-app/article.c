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
#include <malloc-simple.h>

/* decompressed article */
static char *article_data = NULL;
static unsigned int article_data_length = 0;

static int current_page = -1;
static unsigned int current_page_offset = -1;
static char filename_buf[8 + 1 + 3 + 1];
static char byte_buffer[12];

enum ParseMode {
	Parse_Start,
	Parse_Font,
	Parse_Position,
	Parse_Glyph
};

static void article_close(void);
static void article_extract_file_and_offset(const char *target, unsigned int *file, unsigned int *offset);

static int decimal_to_int(const char *data, int len)
{
	int number = 0, i;
	int accu = 1;

	for (i = len-1; i >= 0; --i) {
		if (data[i] < 0x30 || data[i] > 0x39) {
			if (data[i] == '-') {
				return -1 * number;
			} else {
				return 0;
			}
		}

		number += (data[i] - 0x30) * accu;
		accu *= 10;
	}

	return number;
}

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
					lzo_len,
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

void article_display(enum article_nav nav)
{
	int last_x = 0, last_y = 0, last_font = 0;
	int has_x = 0;
	int mode = Parse_Start;
	unsigned int byte_offset = 0;
	//struct glyph *last_glyph = NULL;
	int i;

	guilib_fb_lock();
	guilib_clear();

/* Change the mode and reset state */
#define CHANGE_MODE(new_mode)	\
	mode = new_mode;	\
	byte_offset = 0;	\
	if (new_mode == Parse_Position) \
		has_x = 0;

/*
 * Consume a byte by putting into the byte structure. This
 * can then be converted into a integer...
 */
#define CONSUME_BYTE() \
	byte_buffer[byte_offset++ % sizeof(byte_buffer)] = article_data[i];
#define PARSE_INT() \
	decimal_to_int(byte_buffer, byte_offset % sizeof(byte_buffer));



//#warning BROKEN...Only paint one page
	//current_page_offset = 0;

	/* display one page of text... */

	// TODO: Decoding routines...
	for (i = 0; i < article_data_length; ++i) {
		if (last_y > guilib_framebuffer_height())
			break;

		if (mode == Parse_Start) {
			if (article_data[i] == 'f') {
				CHANGE_MODE(Parse_Font)
			} else {
				/* error */
			}
		} else if (mode == Parse_Font) {
			if (article_data[i] == ',') {
				last_font = PARSE_INT();
				CHANGE_MODE(Parse_Position)
			} else {
				CONSUME_BYTE()
			}
		} else if (mode == Parse_Position) {
			if (article_data[i] == ',') {
				CHANGE_MODE(Parse_Glyph)
			} else if (article_data[i] == '_') {
				if (has_x) {
					last_y += PARSE_INT();
				} else {
					last_x += PARSE_INT();
					has_x = 1;
				}
			} else {
				CONSUME_BYTE()
			}
		} else if (mode == Parse_Glyph) {
			if (article_data[i] == '-') {
			} else if (article_data[i] == 'f') {
				//printf("Switch back to fonts\n");
				CHANGE_MODE(Parse_Font)
			} else if (article_data[i] == ',') {
				//printf("Position change..\n");
				CHANGE_MODE(Parse_Position)
			} else {
				CONSUME_BYTE()
				continue;
			}

			/* time to paint here */
			int glyph = PARSE_INT();
			(void) glyph;
		}
	}

	guilib_fb_unlock();
}

static void article_close(void)
{
	current_page = -1;
	current_page_offset = -1;

	if (article_data) {
		free_simple(article_data, MEM_TAG_ARTICLE_F1);
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
static void article_extract_file_and_offset(const char *target,
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
