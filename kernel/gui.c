/*
 * mahatma - a simple kernel framework
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

#include <wikilib.h>
#include <guilib.h>
#include <malloc.h>
#include <string.h>

/* FIXME: don't use bootloader includes */
#include <regs.h>

unsigned char *framebuffer;

void fb_init(void)
{
	framebuffer = (unsigned char *) malloc((FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT) / 2);
	REG_LCDC_MADD = (unsigned int) framebuffer;
}

void fb_clear(void)
{
	memset(framebuffer, 0, (FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT) / 2);
}

void fb_refresh(void)
{
	/* is there a pragma to omit the frame pointer? */
	return;
}

/* 1bpp implementation */

void fb_set_pixel(int x, int y, int val)
{
	char mask = 1 << (x % 8);
	unsigned int byte = (y * FRAMEBUFFER_WIDTH + x) / 8;

	framebuffer[byte] &= ~mask;

	if (val)
		framebuffer[byte] |= mask;
}

