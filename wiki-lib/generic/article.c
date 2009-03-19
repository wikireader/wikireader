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
#include <msg.h>

struct page_offset {
	int page;
	unsigned int offset;
};

#define BUFSIZE	(10)
static struct page_offset pages[BUFSIZE];
static unsigned int bufpos = 0;

static int article_fd = -1;
static int current_page = -1;
static unsigned int current_page_offset = -1;


#undef PRINT_PAGE_BUFFER
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
#endif


int article_open(const char *article)
{
	article_close();
	article_fd = wl_open(article, WL_O_RDONLY);
	current_page = -1;
	current_page_offset = -1;
	/* reset ring buffer */
	memset(pages, 0, sizeof(struct page_offset) * BUFSIZE);
	return article_fd;
}

#define READ_UINT(var, fd) \
	wl_read(fd, &var, 4);

void article_display(int page)
{
	unsigned int font, x, y, glyph_index, len, i;
	unsigned int page_start;
	unsigned int page_end;
	const struct glyph *glyph;

	if (page < 0 || page == current_page || article_fd < 0)
		return;

	/* optimized for one page at a time scrolling */
	if (page == 0) {
		wl_seek(article_fd, 0);
	} else if (page == current_page + 1) {
		/* push into buffer */
		pages[bufpos].page = current_page;
		pages[bufpos].offset = current_page_offset;
		bufpos = (bufpos + 1) % BUFSIZE;
	} else if (page == current_page - 1) {
		/* pop from buffer and seek to previous page.  if
		 * buffer is empty, start looking from the
		 * beginning. (stupid) */
		bufpos = (bufpos + BUFSIZE - 1) % BUFSIZE;
		if (pages[bufpos].page <= page)
			wl_seek(article_fd, pages[bufpos].offset);
		else
			wl_seek(article_fd, 0);
	} else {
		wl_seek(article_fd, 0);
	}
#ifdef PRINT_PAGE_BUFFER
	print_page_buffer();
#endif
	page_start = page * FRAMEBUFFER_HEIGHT;
	page_end = page_start + FRAMEBUFFER_HEIGHT;

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

			if (y < page_start || x > FRAMEBUFFER_WIDTH)
				continue;
			if (y >= page_end)
				continue;

			if (font >= guilib_nr_fonts())
				continue;

			glyph = get_glyph(font, glyph_index);
			if (!glyph)
				continue;

			render_glyph(x, y % FRAMEBUFFER_HEIGHT, glyph);
		}
		if (y >= page_end)
			break;
	} while(1);

	guilib_fb_unlock();
	current_page = page;
}

void article_close(void)
{
	if (article_fd >= 0) {
		wl_close(article_fd);
		article_fd = -1;
	}
}
