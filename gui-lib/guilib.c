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
#include <regs.h>
#include <lcd.h>
#include <wikireader.h>


// just redfine, fix later
#define FRAMEBUFFER_WIDTH    LCD_WIDTH_PIXELS
#define FRAMEBUFFER_HEIGHT   LCD_HEIGHT_LINES
#define FRAMEBUFFER_SCANLINE LCD_VRAM_WIDTH_PIXELS
#define FRAMEBUFFER_SIZE     LCD_VRAM_SIZE


#define EXTRACT_PIXEL(x, y) \
	unsigned int byte = (x + FRAMEBUFFER_SCANLINE * y) / 8; \
	unsigned int bit  = (x + FRAMEBUFFER_SCANLINE * y) % 8; \
								\
	if (byte >= FRAMEBUFFER_SIZE)				\
		return;


unsigned int guilib_framebuffer_width(void)
{
	return FRAMEBUFFER_WIDTH;
}

unsigned int guilib_framebuffer_height(void)
{
	return FRAMEBUFFER_HEIGHT;
}

unsigned int guilib_framebuffer_size(void)
{
	return FRAMEBUFFER_SIZE;
}


/*
 * Special version for really setting the pixel as it
 * should be... This does not take DISPLAY_INVERTED into
 * account.
 */
static void guilib_set_pixel_plain(int x, int y, int v)
{
	EXTRACT_PIXEL(x, y)

	if (v)
		framebuffer[byte] |= (1 << (7 - bit));
	else
		framebuffer[byte] &= ~(1 << (7 - bit));
}

void guilib_set_pixel(int x, int y, int v)
{
	EXTRACT_PIXEL(x, y)

	if (v)
#ifdef DISPLAY_INVERTED
		framebuffer[byte] &= ~(1 << (7 - bit));
	else
		framebuffer[byte] |= (1 << (7 - bit));
#else
		framebuffer[byte] |= (1 << (7 - bit));
	else
		framebuffer[byte] &= ~(1 << (7 - bit));
#endif
}

int guilib_get_pixel(int x, int y)
{
	unsigned int byte = (x + FRAMEBUFFER_SCANLINE * y) / 8;
	unsigned int bit  = (x + FRAMEBUFFER_SCANLINE * y) % 8;
	int bit_set = (framebuffer[byte] & 1<<(7-bit)) != 0;
#ifdef DISPLAY_INVERTED
	bit_set = !bit_set;
#endif
	return bit_set;
}

/**
 * Invert the pixels of lines starting at @param start_line
 * and the following @param height lines.
 */
void guilib_invert(int start_line, int height)
{
	int x, y;

	for (y = 0; y < height; ++y) {
		for (x = 0; x < FRAMEBUFFER_SCANLINE; x += 8) {
			unsigned int byte = (x + FRAMEBUFFER_SCANLINE * (start_line + y)) / 8;
			framebuffer[byte] = ~framebuffer[byte];
		}
	}
}

/**
 * Clear the content of the screen.
 */
void guilib_clear(void)
{
#ifdef DISPLAY_INVERTED
	memset(framebuffer, ~0, FRAMEBUFFER_SIZE);
#else
	memset(framebuffer, 0, FRAMEBUFFER_SIZE);
#endif
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
	unsigned int xx, yy;

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
	 * fiddling. We can copy it line by line*/

	if ((x & 7) == 0 && img->width == FRAMEBUFFER_WIDTH) {
		unsigned int i;
		for (i = 0; i < img->height; ++i) {
			unsigned char *d = framebuffer + (x + FRAMEBUFFER_SCANLINE * (y+i)) / 8;
			memcpy(d, &img->data[(i*img->width) / 8], img->width / 8);
		}

		return;
	}

	/* hardest case - go for bit fiddling */
	for (xx = 0; xx < img->width; xx++)
		for (yy = 0; yy < img->height; yy++)
			guilib_set_pixel_plain(x + xx, y + yy,
				IMG_GET_PIXEL(img, xx, yy));
}

#define FONTFILE "/fontfile.gen"

void guilib_init(void)
{
	msg(MSG_INFO, "guilib_init:: clear\n");
	guilib_clear();
	msg(MSG_INFO, "guilib_init:: font file %s\n", FONTFILE);

	/* just some tests ... */
	if (read_font_file(FONTFILE) != 0) {
		msg(MSG_INFO, "unable to load font file %s\n", FONTFILE);
		return;
	}

	msg(MSG_INFO, "loaded font file %s\n", FONTFILE);
}

