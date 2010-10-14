/*
 * Generic graphics routines
 *
 * Copyright (c) 2010 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#if  !defined(_GRAPHICS_H_)
#define _GRAPHICS_H_ 1

#include "standard.h"

#ifdef __cplusplus
extern "C" {
#endif

void Graphics_DrawLine(int x0, int y0, int x1, int y1, uint32_t colour,
		       void (*SetPixel)(int x, int y, uint32_t colour));

//*[bitmap]: transfer a bitmap ro framebuffer
void Graphics_PutBitMap(void *framebuffer, size_t BufferWidth,
			int x, int y, size_t width, size_t height,
			bool reverse,
			const uint8_t *bits);

const uint8_t *Graphics_GetFont(unsigned char c);
int Graphics_FontWidth(void);
int Graphics_FontHeight(void);

#ifdef __cplusplus
}
#endif

#endif
