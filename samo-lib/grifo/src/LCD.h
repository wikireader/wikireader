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

	LCD_FONT_WIDTH  = 8,
	LCD_FONT_HEIGHT = 13,
	LCD_MAX_COLUMNS = LCD_WIDTH / LCD_FONT_WIDTH,
	LCD_MAX_ROWS    = LCD_HEIGHT / LCD_FONT_HEIGHT,

};
//-MakeSystemCalls: types

typedef enum {
//+MakeSystemCalls: colours
	LCD_WHITE = 0,
	LCD_BLACK = 1,
//-MakeSystemCalls: colours
} LCD_ColourType;

void LCD_initialise(void);

uint8_t *LCD_GetFrameBuffer(void);

// frame buffer must be word aligned
uint32_t *LCD_SetFrameBuffer(uint32_t *address);

void LCD_ResetFrameBuffer(void);


// clear the screen and home the graphic and text cursors
// sets drawing colour to the opposite value
void LCD_clear(LCD_ColourType colour);

// absolute pixel access (does not change any cursor)
LCD_ColourType LCD_GetPixel(int x, int y);
void LCD_SetPixel(int x, int y, LCD_ColourType colour);

// drawing colour
LCD_ColourType LCD_SetColour(LCD_ColourType value);
LCD_ColourType LCD_GetColour(void);

// move graphic gursor and plot a pixel
void LCD_point(int x, int y);

// line drawing (only affects graphic cursor)
void LCD_MoveTo(int x, int y);
void LCD_LineTo(int x, int y);

// text output (only affects text cursor)
void LCD_AtXY(int column, int row);
int LCD_PutChar(int c);
void LCD_print(const char *message);
int LCD_printf(const char *format, ...) __attribute__((format (printf, 1, 2)));


#endif
