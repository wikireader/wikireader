/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Christopher Hall <hsw@openmoko.com>
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

#if !defined(_LCD_H_)
#define _LCD_H_ 1

#include <config.h>

#if BOARD_SAMO_Ax || BOARD_SAMO_Vx

#define LCD_WIDTH_PIXELS  240
#define LCD_HEIGHT_LINES  208

#elif BOARD_S1C33E07

#define LCD_USES_SPI      1
#define LCD_WIDTH_PIXELS  320
#define LCD_HEIGHT_LINES  240

#else

#define LCD_WIDTH_PIXELS  240
#define LCD_HEIGHT_LINES  208

#endif


#define LCD_VRAM_WIDTH_PIXELS  (((LCD_WIDTH_PIXELS + 31) / 32) * 32)

#if !defined(LCD_VRAM_HEIGH_LINES)
#define LCD_VRAM_HEIGHT_LINES LCD_HEIGHT_LINES
#endif
#if !defined(LCD_VRAM_WIDTH_PIXELS)
#define LCD_VRAM_WIDTH_PIXELS LCD_WIDTH_PIXELS
#endif


#if 0 != (VRAM_WIDTH_PIXELS % 32)
#error "LCD_VRAM_WIDTH_PIXELS must be a multiple of 32 bits"
#endif


#define LCD_VRAM_WIDTH_BYTES (LCD_VRAM_WIDTH_PIXELS / 8)
#define LCD_WIDTH_BYTES      (LCD_WIDTH_PIXELS / 8)


#if LCD_MONOCHROME
#define LCD_VRAM	0x80000
#define LCD_VRAM_SIZE	((LCD_VRAM_WIDTH_PIXELS) * (LCD_VRAM_HEIGHT_LINES) / 8)
#else
#define LCD_VRAM	0x10100000
#define LCD_VRAM_SIZE	((LCD_VRAM_WIDTH_PIXELS) * (LCD_VRAM_HEIGHT_LINES) / 2)
#endif

typedef enum {
	LCD_PositionTop,
	LCD_PositionBottom,
	LCD_PositionCentre,
} LCD_PositionType;

typedef struct {
	uint32_t width;
	uint32_t height;
	uint8_t bytes[];
} LCD_ImageType;

void LCD_initialise(void);
void LCD_DisplayImage(LCD_PositionType position, bool clear_screen, const LCD_ImageType *image);


#endif /* LCD_H */
