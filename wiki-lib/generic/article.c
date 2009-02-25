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
#include <file-io.h>
#include <guilib.h>
#include <glyph.h>
#include <fontfile.h>

static int article_fd = -1;
static int current_page = -1;

int article_open(const char *article)
{
	article_close();
	article_fd = wl_open(article, WL_O_RDONLY);
	current_page = -1;
	return article_fd;
}

static unsigned char buf[512];
static unsigned int *buf_ptr;
static int available = 0;
#define READ_UINT(var, fd) \
	if (available == 0) { \
	    available = wl_read(fd, buf, 512); \
	    buf_ptr = (unsigned int*)&buf[0]; \
	    if (available < 4) break; \
	} \
	var = *buf_ptr; \
	buf_ptr++; \
	available -= 4;

void article_display(int page)
{
	unsigned int font, x, y, glyph_index, len, i;
	unsigned int page_start;
	unsigned int page_end;
	const struct glyph *glyph;

	if (page == current_page || article_fd < 0)
		return;

	/* TODO it is not the most efficient thing to do */
	wl_seek(article_fd, 0);


	page_start = page * FRAMEBUFFER_HEIGHT;
	page_end = page_start + FRAMEBUFFER_HEIGHT;

	guilib_fb_lock();
	guilib_clear();

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
				break;

			if (font >= guilib_nr_fonts())
				continue;

			glyph = get_glyph(font, glyph_index);
			if (!glyph)
				continue;

			render_glyph(x, y % FRAMEBUFFER_HEIGHT, glyph);
		}
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
