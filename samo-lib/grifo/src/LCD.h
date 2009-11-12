/*
 * LCD - driver for the LCD Module
 *
 * Copyright (c) 2009 Openmoko Inc.
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

#if  !defined(_LCD_H_)
#define _LCD_H_ 1

#include "standard.h"

//+MakeSystemCalls: types
enum {
	LCD_WIDTH  = 240,
	LCD_HEIGHT = 208,

	LCD_BUFFER_WIDTH = 256,
	LCD_BUFFER_WIDTH_BYTES = LCD_BUFFER_WIDTH / 8,
	LCD_BUFFER_WIDTH_WORDS = LCD_BUFFER_WIDTH_BYTES / sizeof(uint32_t),
	LCD_BUFFER_HEIGHT = 208,

	LCD_BUFFER_SIZE_BYTES = LCD_BUFFER_WIDTH_BYTES * LCD_BUFFER_HEIGHT,
	LCD_BUFFER_SIZE_WORDS = LCD_BUFFER_WIDTH_WORDS * LCD_BUFFER_HEIGHT,

};
//-MakeSystemCalls: types

typedef enum {
//+MakeSystemCalls: colours
	LCD_WHITE = 0,
	LCD_BLACK = 1,
//-MakeSystemCalls: colours
} LCD_colour_t;

void LCD_initialise(void);

uint8_t *LCD_GetFrameBuffer(void);

// frame buffer must be word aligned
uint32_t *LCD_SetFrameBuffer(uint32_t *address);

void LCD_ResetFrameBuffer(void);


void LCD_AllWhite(void);
void LCD_AllBlack(void);

LCD_colour_t LCD_GetPixel(int x, int y);
void LCD_SetPixel(int x, int y, LCD_colour_t value);

void LCD_line(int x0, int y0, int x1, int y1, LCD_colour_t value);

#endif
