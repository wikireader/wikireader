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

/*
 * To enable debugging:
 * #define PRINT_PAGE_BUFFER
 */

struct page_offset {
	int page;
	unsigned int offset;
};

#define BUFSIZE	(10)
static struct page_offset pages[BUFSIZE];
static unsigned int bufpos = 0;

static int article_fd = -1;
static int current_page = -1;
static unsigned int article_file_size = -1;
static unsigned int current_page_offset = -1;


#ifdef PRINT_PAGE_BUFFER
static void print_page_buffer(void)
{
	unsigned int i;
	for (i = 0; i < BUFSIZE; ++i) {
		msg(MSG_DEBUG, "%c ", i == bufpos ? '*' : ' ');
		msg(MSG_DEBUG, "%d page %d offset %u\n",
		    i, pages[i].page, pages[i].offset);
	}
	msg(MSG_DEBUG, "--------------------------\n");
}
#else
static inline void print_page_buffer(void)
{
}
#endif


int article_open(const char *article)
{
	article_close();
	article_fd = wl_open(article, WL_O_RDONLY);
	current_page = -1;
	article_file_size = -1;
	current_page_offset = -1;
	/* reset ring buffer */
	memset(pages, 0, sizeof(struct page_offset) * BUFSIZE);
	return article_fd;
}

#define READ_UINT(var, fd) \
	res = wl_read(fd, &var, 4); \
	if (res < 4) { \
		msg(MSG_INFO, "Could not read article file - wl_read() returned: %i\n", res); \
		current_page_offset = article_file_size; \
		goto out; \
	}

void article_display(enum article_nav nav)
{
	unsigned int font, x, y, glyph_index, len, i;
	unsigned int page_start;
	unsigned int page_end;
	int res;
	const struct glyph *glyph;

	if (article_fd < 0)
		return;

	/* scroll one page at a time */
	switch(nav) {
	case ARTICLE_PAGE_0:
		wl_seek(article_fd, 0);
		wl_fsize(article_fd, &article_file_size);
		current_page = 0;
		break;

	case ARTICLE_PAGE_NEXT:
		/* reached end of file */
		if (current_page_offset == article_file_size)
			return;

		/* push into buffer */
		pages[bufpos].page = current_page;
		pages[bufpos].offset = current_page_offset;
		bufpos = (bufpos + 1) % BUFSIZE;
		current_page++;
		break;

	case ARTICLE_PAGE_PREV:
		/* displaying first page */
		if (current_page == 0)
			return;

		/* pop from buffer and seek to previous page.  if
		 * buffer is empty, start looking from the
		 * beginning. (stupid) */
		bufpos = (bufpos + BUFSIZE - 1) % BUFSIZE;
		if (pages[bufpos].page < current_page)
			wl_seek(article_fd, pages[bufpos].offset);
		else
			wl_seek(article_fd, 0);
		current_page--;
		break;

	default:
		break;
	}

	print_page_buffer();

	page_start = current_page * guilib_framebuffer_height();
	page_end = page_start + guilib_framebuffer_height();

	guilib_fb_lock();
	guilib_clear();

	current_page_offset = wl_tell(article_fd);

	do {
		READ_UINT(font, article_fd)
		READ_UINT(len, article_fd)

		for (i = 0; i < len; ++i) {
			READ_UINT(x, article_fd)
			READ_UINT(y, article_fd)
			READ_UINT(glyph_index, article_fd)

			if (y < page_start || x > guilib_framebuffer_width())
				continue;
			if (y >= page_end)
				continue;

			if (font >= guilib_nr_fonts())
				continue;

			glyph = get_glyph(font, glyph_index);
			if (!glyph)
				continue;

			render_glyph(x, y % guilib_framebuffer_height(), glyph);
		}
		if (y >= page_end)
			break;
	} while(1);

out:
	guilib_fb_unlock();
}

void article_close(void)
{
	if (article_fd >= 0) {
		wl_close(article_fd);
		article_fd = -1;
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
