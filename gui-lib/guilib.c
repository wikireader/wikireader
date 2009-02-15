/*
 * guilib - a minimal pixel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
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

#include <file-io.h>
#include <msg.h>
#include <string.h>
#include "guilib.h"
#include "glyph.h"
#include "fontfile.h"

void guilib_set_pixel(int x, int y, int v)
{
	unsigned int byte = (x + FRAMEBUFFER_SCANLINE * y) / 8;
	unsigned int bit  = (x + FRAMEBUFFER_SCANLINE * y) % 8;

	if (byte >= FRAMEBUFFER_SIZE)
		return;

	if (v)
		framebuffer[byte] |= (1 << (7 - bit));
	else
		framebuffer[byte] &= ~(1 << (7 - bit));
}

int guilib_get_pixel(int x, int y)
{
	unsigned int byte = (x + FRAMEBUFFER_SCANLINE * y) / 8;
	unsigned int bit  = (x + FRAMEBUFFER_SCANLINE * y) % 8;

	return (framebuffer[byte] >> (7 - bit)) & 1;
}

/**
 * Clear the content of the screen.
 */
void guilib_clear(void)
{
	memset(framebuffer, 0, FRAMEBUFFER_SIZE);
}

/* The idea is that every function which calls painting routines calls
 * guilib_fb_lock() before any operation and guilib_fb_unlock() after
 * it. This way, only the last of these functions in the calling stack
 * will actually execute fb_refresh(). */
static int fb_ref = 0;

void guilib_fb_lock(void)
{
	fb_ref++;
}

void guilib_fb_unlock(void)
{
	if (fb_ref == 0)
		return;
	
	if (--fb_ref == 0)
		fb_refresh();
}

#define IMG_GET_PIXEL(img,x,y) \
	(img->data[(x + img->width * y) / 8] >> (7 - (x + img->width * y) % 8) & 1)

void guilib_blit_image(const struct guilib_image *img, int x, int y)
{
	int xx, yy;

	/* special case: the image has the same width than the 
	 * height and is rendered at y=0. Then we can go for a
	 * simple memcpy() */

	if (y == 0 && img->width == FRAMEBUFFER_SCANLINE) {
		memcpy(framebuffer + (x + FRAMEBUFFER_SCANLINE * y) / 8,
			img->data, (img->width * img->height) / 8);
		return;
	}

	/* special case: the image will be blitted byte aligned.
	 * we can simply copy over all the bytes, without bit
	 * fiddling. */

	if ((x & 7) == 0) {
		int i;
		char *d = framebuffer + (x + FRAMEBUFFER_SCANLINE * y) / 8;

		for (i = 0; i < (img->width * img->height) / 8; i++)
			*d++ = img->data[i];

		return;
	}

	/* hardest case - go for bit fiddling */
	for (xx = 0; xx < img->width; xx++)
		for (yy = 0; yy < img->height; yy++)
			guilib_set_pixel(x + xx, y + yy,
				IMG_GET_PIXEL(img, xx, yy));
}

#define FONTFILE "/fontfile.gen"

void guilib_init(void)
{
	memset(framebuffer, 0, FRAMEBUFFER_SIZE);

	/* just some tests ... */
	if (read_font_file(FONTFILE) != 0) {
		msg(MSG_INFO, "unable to load font file %s\n", FONTFILE);
		return;
	}

	msg(MSG_INFO, "loaded font file %s\n", FONTFILE);
}

