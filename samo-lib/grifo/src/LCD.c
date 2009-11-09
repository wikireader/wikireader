/*
 * LCD - driver for the LCD Module
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
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

#include "standard.h"

#include <regs.h>
#include <samo.h>

#include "LCD.h"


void LCD_initialise(void)
{
#if 0
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
	}
#endif
}


uint8_t *LCD_GetFrameBuffer(void)
{
	return (uint8_t *)REG_LCDC_MADD;
}


uint32_t *LCD_SetFrameBuffer(uint32_t *address)
{
	register uint32_t *previous = (uint32_t *)REG_LCDC_MADD;
	REG_LCDC_MADD = (uint32_t)address;
	return previous;
}


void LCD_ResetFrameBuffer(void)
{
	register uint32_t value;
	asm volatile ("xld.w\r%[v], __START_FrameBuffer"
		      : [v] "=r" (value));

	REG_LCDC_MADD = value;
}


void LCD_AllWhite(void)
{
	size_t i;
	for (i = 0; i < LCD_BUFFER_SIZE_WORDS; ++i) {
		((uint32_t *)REG_LCDC_MADD)[i] = 0;
	}
}


void LCD_AllBlack(void)
{
	size_t i;
	for (i = 0; i < LCD_BUFFER_SIZE_WORDS; ++i) {
		((uint32_t *)REG_LCDC_MADD)[i] = ~0;
	}
}


static inline int pos(int x, int y)
{
	return y * LCD_BUFFER_WIDTH_BYTES + (x >> 3);
}

LCD_colour_t LCD_GetPixel(int x, int y)
{
	if (x < 0 || x > LCD_WIDTH ||
	    y < 0 || y > LCD_HEIGHT) {
		return 0;
	}
	return 0 != (((uint8_t *)REG_LCDC_MADD)[y * LCD_BUFFER_WIDTH_BYTES + (x >> 3)] & (0x80 >> (x & 0x07)));
}


void LCD_SetPixel(int x, int y, LCD_colour_t value)
{
	if (x < 0 || x > LCD_WIDTH ||
	    y < 0 || y > LCD_HEIGHT) {
		return;
	}
	if (0 != value) {
		(((uint8_t *)REG_LCDC_MADD)[y * LCD_BUFFER_WIDTH_BYTES + (x >> 3)] |= (0x80 >> (x & 0x07)));
	} else {
		(((uint8_t *)REG_LCDC_MADD)[y * LCD_BUFFER_WIDTH_BYTES + (x >> 3)] &= ~(0x80 >> (x & 0x07)));
	}
}


// Bresenham's line algorithm
void LCD_line(int x0, int y0, int x1, int y1, LCD_colour_t value)
{
	int stepx = 1;
	int dx = (x1 - x0) << 1;

	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	}

	int stepy = 1;
	int dy = (y1 - y0) << 1;

	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	}

	LCD_SetPixel(x0, y0, value);

	if (dx > dy) {
		int fraction = dy - (dx >> 1);

		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			LCD_SetPixel(x0, y0, value);
		}
	} else {
		int fraction = dx - (dy >> 1);

		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			LCD_SetPixel(x0, y0, value);
		}
	}
}
