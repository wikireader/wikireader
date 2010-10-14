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
} LCD_ColourType;

void LCD_initialise(void);

//*[gfb]: returns pointer to frame buffer
//*[gfb]: byte for convenience, but it is guaranteed to be word aligned
uint8_t *LCD_GetFrameBuffer(void);

//*[sfb]: frame buffer must be word aligned
uint32_t *LCD_SetFrameBuffer(uint32_t *address);

//*[rfb]: reset to default buffer
void LCD_ResetFrameBuffer(void);

//*[clr]: clear the screen and home both the graphic and text cursors
//*[clr]: sets drawing colour to the opposite of the colour value
void LCD_clear(LCD_ColourType colour);

//*[pix]: absolute pixel access (does not change any cursor)
LCD_ColourType LCD_GetPixel(int x, int y);
void LCD_SetPixel(int x, int y, LCD_ColourType colour);

//*[dc]: drawing colour
LCD_ColourType LCD_SetColour(LCD_ColourType value);
LCD_ColourType LCD_GetColour(void);

//*[pt]: move graphic gursor and plot a pixel
void LCD_point(int x, int y);

//*[line]: line drawing (only affects graphic cursor)
void LCD_MoveTo(int x, int y);
void LCD_LineTo(int x, int y);

//*[text]: text output (only affects text cursor)
int LCD_MaxColumns(void);
int LCD_MaxRows(void);
void LCD_AtXY(int column, int row);
int LCD_PutChar(int c);
void LCD_print(const char *message);
int LCD_printf(const char *format, ...) __attribute__((format (printf, 1, 2)));
int LCD_vuprintf(const char *format, va_list arguments);


// Picture in picture

//*[win]: returns buffer size of window (0 => invalid window)
//*[win]: default buffer is already allocated
size_t LCD_Window(int x, int y, int width, int height);

//*[wint]: total bytes used by window
size_t LCD_Window_GetBufferSize(void);

//*[winb]: bytes for 1 row of pixels (rounded up as required by the hardware)
//*[winb]: (the rounding up does not cause extra pixels to be displayed)
size_t LCD_Window_GetByteWidth(void);

///*[wina]: get buffer address of window
uint8_t *LCD_Window_GetBuffer(void);

//*[winsb]: window buffer must be word aligned
//*[winsb]: this changes the buffer from the default
//*[winsb]: use NULL to set back to default buffer
//*[winsb]: returns previous buffer value
uint32_t *LCD_Window_SetBuffer(uint32_t *address);

//*[windis]: turn off the window
void LCD_Window_disable(void);

//*[winen]: turn on the window
void LCD_Window_enable(void);

//*[wingr]: as the LCD graphic operations but in the window
void LCD_Window_clear(LCD_ColourType colour);
LCD_ColourType LCD_Window_GetPixel(int x, int y);
void LCD_Window_SetPixel(int x, int y, LCD_ColourType colour);
LCD_ColourType LCD_Window_SetColour(LCD_ColourType colour);
LCD_ColourType LCD_Window_GetColour(void);
void LCD_Window_Point(int x, int y);
void LCD_Window_MoveTo(int x, int y);
void LCD_Window_LineTo(int x, int y);
void LCD_framebuffer_set_byte(int byte_idx, uint8_t value);
uint8_t LCD_framebuffer_get_byte(int byte_idx);


void LCD_PutBitMap(void *framebuffer, size_t BufferWidth,
		   int x, int y, size_t width, size_t height,
		   bool reverse,
		   const uint8_t *bits);

#endif
