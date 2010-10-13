/*
 * guilib - a minimal pixel framework
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
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

#include <grifo.h>

#include "string.h"
#include "guilib.h"
#include "glyph.h"
#include "lcd_buf_draw.h"


// just redfine, fix later
#define FRAMEBUFFER_WIDTH    LCD_WIDTH
#define FRAMEBUFFER_HEIGHT   LCD_HEIGHT
#define FRAMEBUFFER_SCANLINE LCD_BUFFER_WIDTH
#define FRAMEBUFFER_SIZE     LCD_BUFFER_SIZE_BYTES


#define EXTRACT_PIXEL(x, y)					\
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
			lcd_framebuffer_set_byte(byte, ~lcd_framebuffer_get_byte(byte));
		}
	}
}

/**
 * Invert the area specified on the screen.
 */
void guilib_invert_area(int start_x, int start_y, int end_x, int end_y)
{
	if (start_x > end_x || start_y > end_y ||
		(start_x < 0 && end_x < 0) || (start_x >= LCD_WIDTH && end_x >= LCD_WIDTH) ||
		(start_y < 0 && end_y < 0) || (start_y >= LCD_HEIGHT && end_y >= LCD_HEIGHT))
		return;

	if (start_x < 0)
		start_x = 0;
	if (end_x >= LCD_WIDTH)
		end_x = LCD_WIDTH - 1;
	if (start_y < 0)
		start_y = 0;
	if (end_y >= LCD_HEIGHT)
		end_y = LCD_HEIGHT - 1;
	guilib_buffer_invert_area(lcd_get_framebuffer(), start_x, start_y, end_x, end_y);
}

void guilib_buffer_invert_area(unsigned char *membuffer, int start_x, int start_y, int end_x, int end_y)
{
	int y, r1, r2;
	uint8_t byte_mask1 = 0;
	uint8_t byte_mask2 = 0;
	int byte_idx;
	int x_byte_idx;
	int nBits;
	int nBytes = 0;
	int i;

	if (start_x == 0 && end_x >= LCD_WIDTH - 1)
	{
		byte_idx = start_y * LCD_BUF_WIDTH_BYTES;
		for (i = byte_idx; i < byte_idx + (end_y - start_y + 1) * LCD_BUFFER_WIDTH_BYTES; i++)
			membuffer[i] = ~membuffer[i];
	}
	else
	{
		x_byte_idx = start_x / 8;
		r1 = start_x % 8;
		r2 = (end_x + 1) % 8;
		// calculate number of full bytes
		nBits = end_x - start_x + 1 - ((8 - r1) % 8) - r2;
		if (nBits > 0)
			nBytes = nBits / 8;

		if (r1 > 0)
		{
			byte_mask1 = 0xFF;
			byte_mask1 <<= 8 - r1;
		}
		if (r2 > 0)
		{
			byte_mask2 = 0xFF;
			byte_mask2 >>= r2;
		}
		if (r1 > 0 && end_x - start_x < 8 && r2 > 0)
		{
			byte_mask1 |= byte_mask2;
			r2 = 0;
		}

		for (y = start_y; y <= end_y; ++y) {
			byte_idx = y * LCD_BUF_WIDTH_BYTES + x_byte_idx;
			if (r1 > 0)
			{
				membuffer[byte_idx] = membuffer[byte_idx] ^ (~byte_mask1);
				byte_idx++;
			}
			if (nBytes > 0)
			{
				for (i = byte_idx; i < byte_idx + nBytes; i++)
					membuffer[i] = ~membuffer[i];
				byte_idx += nBytes;
			}
			if (r2 > 0)
			{
				membuffer[byte_idx] = membuffer[byte_idx] ^ (~byte_mask2);
			}
		}
	}
}

void guilib_buffer_set_pixel(unsigned char *membuffer, int x, int y)
{
	unsigned int byte = (x + LCD_BUFFER_WIDTH * y) / 8;
	unsigned int bit  = (x + LCD_BUFFER_WIDTH * y) % 8;


	membuffer[byte] |= (1 << (7 - bit));
}

void guilib_buffer_clear_pixel(unsigned char *membuffer, int x, int y)
{
	unsigned int byte = (x + LCD_BUFFER_WIDTH * y) / 8;
	unsigned int bit  = (x + LCD_BUFFER_WIDTH * y) % 8;

	membuffer[byte] &= ~(1 << (7 - bit));
}

/**
 * Clear the content of the screen.
 */
void guilib_clear(void)
{
#ifdef DISPLAY_INVERTED
	lcd_clear(LCD_BLACK);
#else
	lcd_clear(LCD_WHITE);
#endif
}

/**
 * Clear the area specified on the screen.
 */
void guilib_clear_area(int start_x, int start_y, int end_x, int end_y)
{
	guilib_buffer_clear_area(lcd_get_framebuffer(),
		LCD_WIDTH, LCD_HEIGHT, LCD_BUF_WIDTH_BYTES,
		start_x, start_y, end_x, end_y);
}

void guilib_buffer_clear_area(unsigned char *membuffer,
	int width, int height, int buf_width_bytes,
	int start_x, int start_y, int end_x, int end_y)
{
	int y, r1, r2;
	uint8_t byte_mask1 = 0;
	uint8_t byte_mask2 = 0;
	int byte_idx;
	int x_byte_idx;
	int nBits;
	int nBytes = 0;

	if (start_x > end_x || start_y > end_y ||
		(start_x < 0 && end_x < 0) || (start_x >= width && end_x >= width) ||
		(start_y < 0 && end_y < 0) || (start_y >= height && end_y >= height))
		return;

	if (start_x < 0)
		start_x = 0;
	if (end_x >= width)
		end_x = width - 1;
	if (start_y < 0)
		start_y = 0;
	if (end_y >= height)
		end_y = height - 1;

	if (start_x == 0 && end_x >= width - 1)
	{
		byte_idx = start_y * buf_width_bytes;
		memset(&membuffer[byte_idx], 0, buf_width_bytes * (end_y - start_y + 1));
	}
	else
	{
		x_byte_idx = start_x / 8;
		r1 = start_x % 8;
		r2 = (end_x + 1) % 8;
		// calculate number of full bytes
		nBits = end_x - start_x + 1 - ((8 - r1) % 8) - r2;
		if (nBits > 0)
			nBytes = nBits / 8;

		if (r1 > 0)
		{
			byte_mask1 = 0xFF;
			byte_mask1 <<= 8 - r1;
		}
		if (r2 > 0)
		{
			byte_mask2 = 0xFF;
			byte_mask2 >>= r2;
		}
		if (r1 > 0 && end_x - start_x < 8 && r2 > 0)
		{
			byte_mask1 |= byte_mask2;
			r2 = 0;
		}

		for (y = start_y; y <= end_y; ++y) {
			byte_idx = y * buf_width_bytes + x_byte_idx;
			if (r1 > 0)
			{
				membuffer[byte_idx] &= byte_mask1;
				byte_idx++;
			}
			if (nBytes > 0)
			{
				memset(&membuffer[byte_idx], 0, nBytes);
				byte_idx += nBytes;
			}
			if (r2 > 0)
			{
				membuffer[byte_idx] &= byte_mask2;
			}
		}
	}
}

/* The idea is that every function which calls painting routines calls
 * guilib_fb_lock() before any operation and guilib_fb_unlock() after
 * it. This way, only the last of these functions in the calling stack
 * will actually execute fb_refresh(). */
//static int fb_ref = 0;

void guilib_fb_lock(void)
{
//	fb_ref++;
}

void guilib_fb_unlock(void)
{
//	if (fb_ref == 0)
//		return;
//
//	if (--fb_ref == 0)
//		fb_refresh();
}

#define IMG_GET_PIXEL(img,x,y)						\
	(img->data[(x + img->width * y) / 8] >> (7 - (x + img->width * y) % 8) & 1)

void guilib_blit_image(const struct guilib_image *img, int x, int y)
{
	unsigned int xx, yy;
	uint8_t *framebuffer = lcd_get_framebuffer();

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
			lcd_set_pixel(x + xx, y + yy,
					       IMG_GET_PIXEL(img, xx, yy));
}

void guilib_init(void)
{
	guilib_clear();
}

