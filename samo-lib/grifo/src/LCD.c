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

#include "standard.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <regs.h>
//#include <samo.h>

#include "graphics.h"
#include "LCD.h"


// text cursor
static int TextRow;
static int TextColumn;

// graphic cursor
static int GraphicX;
static int GraphicY;

// drawing colour
static int ForegroundColour;

// window information
static bool WindowAvailable;
static int WindowWidth;
static int WindowHeight;
static int WindowByteWidth;
static int WindowBufferSize;

// window text cursor
static int WindowTextRow;
static int WindowTextColumn;

// window graphic cursor
static int WindowGraphicX;
static int WindowGraphicY;

// window drawing colour
static int WindowForegroundColour;


void LCD_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		LCD_ResetFrameBuffer();
		LCD_Window_disable();
		initialised = true;
		GraphicX = 0;
		GraphicY = 0;
		TextRow = 0;
		TextColumn = 0;
		ForegroundColour = LCD_BLACK;
	}
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


// basic operations
// ----------------

void LCD_clear(LCD_ColourType colour)
{
	size_t i;
	uint32_t fill = colour == LCD_WHITE ? 0 : ~0;
	for (i = 0; i < LCD_BUFFER_SIZE_WORDS; ++i) {
		((uint32_t *)REG_LCDC_MADD)[i] = fill;
	}
	GraphicX = 0;
	GraphicY = 0;
	TextRow = 0;
	TextColumn = 0;
	ForegroundColour = colour == LCD_WHITE ? LCD_BLACK : LCD_WHITE;
}


// graphic operations
// ------------------

static inline int pos(int x, int y)
{
	return y * LCD_BUFFER_WIDTH_BYTES + (x >> 3);
}


LCD_ColourType LCD_GetPixel(int x, int y)
{
	if (x < 0 || x >= LCD_WIDTH ||
	    y < 0 || y >= LCD_HEIGHT) {
		return LCD_BLACK;
	}
	return 0 != (((uint8_t *)REG_LCDC_MADD)[pos(x, y)] & (0x80 >> (x & 0x07))) ? LCD_BLACK : LCD_WHITE;
}


static void SetPixel(int x, int y, uint32_t colour)
{
	if (x < 0 || x >= LCD_WIDTH ||
	    y < 0 || y >= LCD_HEIGHT) {
		return;
	}
	if (LCD_BLACK == colour) {
		((uint8_t *)REG_LCDC_MADD)[pos(x, y)] |= (0x80 >> (x & 0x07));
	} else {
		((uint8_t *)REG_LCDC_MADD)[pos(x, y)] &= ~(0x80 >> (x & 0x07));
	}
}


void LCD_SetPixel(int x, int y, LCD_ColourType colour)
{
	SetPixel(x, y, (uint32_t)colour);
}


LCD_ColourType LCD_SetColour(LCD_ColourType colour)
{
	register LCD_ColourType previous = ForegroundColour;
	ForegroundColour = colour;
	return previous;
}


LCD_ColourType LCD_GetColour(void)
{
	return ForegroundColour;
}


void LCD_point(int x, int y)
{
	LCD_MoveTo(x, y);
	LCD_SetPixel(x, y, ForegroundColour);
}


void LCD_MoveTo(int x, int y)
{
	GraphicX = Standard_ClipValue(x, 0, LCD_WIDTH - 1);
	GraphicY = Standard_ClipValue(y, 0, LCD_HEIGHT - 1);
}


void LCD_LineTo(int x, int y)
{
	register int x0 = GraphicX;
	register int y0 = GraphicY;
	LCD_MoveTo(x, y);
	Graphics_DrawLine(x0, y0, GraphicX, GraphicY, ForegroundColour, SetPixel);
}


// text output to frame buffer
// ---------------------------


int LCD_MaxColumns(void)
{
	return LCD_WIDTH / Graphics_FontWidth();
}


int LCD_MaxRows(void)
{
	return LCD_HEIGHT / Graphics_FontHeight();
}


void LCD_AtXY(int x, int y)
{
	TextRow = Standard_ClipValue(y, 0, LCD_MaxRows() - 1);
	TextColumn = Standard_ClipValue(x, 0, LCD_MaxColumns() - 1);
}


static void scroll(void)
{
	TextColumn = 0;
	++TextRow;
	if (TextRow >= LCD_MaxRows()) {
		TextRow = LCD_MaxRows() - 1;

		uint8_t *p = LCD_GetFrameBuffer();
		const uint32_t one_line =  Graphics_FontHeight() * LCD_BUFFER_WIDTH_BYTES;

		memcpy(p, p + one_line, LCD_BUFFER_SIZE_BYTES - one_line);
		memset(p + LCD_BUFFER_SIZE_BYTES - one_line, ForegroundColour == LCD_BLACK ? 0 : 0xff, one_line);
	}
}


int LCD_PutChar(int c)
{
	if ('\r' == c) {
		TextColumn = 0;
		return c;
	}
	if ('\n' == c) {
		scroll();
		return c;
	}

	if (TextColumn >= LCD_MaxColumns()) {
		scroll();
	}
	const uint8_t *font = Graphics_GetFont(c);
	uint8_t *p = LCD_GetFrameBuffer();

	int h = Graphics_FontHeight();
	int w = Graphics_FontWidth();
	p += w / 8 * TextColumn + h * LCD_BUFFER_WIDTH_BYTES * TextRow;

	int i;
	if (LCD_BLACK == ForegroundColour) {
		for (i = 0; i < h; ++i, p += LCD_BUFFER_WIDTH_BYTES) {
			*p = *font++;
		}
	} else {
		for (i = 0; i < h; ++i, p += LCD_BUFFER_WIDTH_BYTES) {
			*p = ~*font++;
		}
	}
	++TextColumn;
	return c;
}


void LCD_print(const char *message)
{
	while ('\0' != *message) {
		LCD_PutChar(*message++);
	}
}


int LCD_printf(const char *format, ...)
{
	va_list arguments;

	va_start(arguments, format);

	int rc = vuprintf(LCD_PutChar, format, arguments);

	va_end(arguments);

	return rc;
}


int LCD_vuprintf(const char *format, va_list arguments)
{
	return vuprintf(LCD_PutChar, format, arguments);
}


// window buffer
// -------------


size_t LCD_Window(int x, int y, int width, int height)
{
	WindowAvailable = false;
	if (x < 0 || y < 0 || width <= 0 || height <= 0) {
		return 0;
	}

	// limitations of controller
	x = (x + 31) & ~31;
	width = (width + 31) & ~31;

	if (x + width >= LCD_BUFFER_WIDTH ||
	    y + height>= LCD_BUFFER_HEIGHT) {
		return 0;
	}

	register uint32_t value;
	asm volatile ("xld.w\r%[v], __START_WindowBuffer"
		      : [v] "=r" (value));
	REG_LCDC_SADD = value;

	REG_LCDC_SSP =
		((y << PIPYST_SHIFT) & PIPYST_MASK) |
		((x / 32 << PIPXST_SHIFT) & PIPXST_MASK);

	REG_LCDC_SEP =
		(((y + height - 1) << PIPYEND_SHIFT) & PIPYEND_MASK) |
		((((x + width) / 32 - 1) << PIPXEND_SHIFT) & PIPXEND_MASK);

	REG_LCDC_MLADD = LCD_BUFFER_WIDTH_WORDS;

	WindowWidth = width;
	WindowHeight = height;
	WindowByteWidth = ((width + 31) >> 5) * sizeof(uint32_t);
	WindowBufferSize = WindowByteWidth * height;
	WindowGraphicX = 0;
	WindowGraphicY = 0;
	WindowTextRow = 0;
	WindowTextColumn = 0;
	WindowForegroundColour = LCD_BLACK;

	WindowAvailable = WindowBufferSize != 0;

	return WindowBufferSize;
}


size_t LCD_Window_GetBufferSize(void)
{
	return WindowBufferSize;
}


size_t LCD_Window_GetByteWidth(void)
{
	return WindowByteWidth;
}


uint8_t *LCD_Window_GetBuffer(void)
{
	return (uint8_t *)REG_LCDC_SADD;
}


uint32_t *LCD_Window_SetBuffer(uint32_t *address)
{
	register uint32_t *previous = (uint32_t *)REG_LCDC_SADD;
	REG_LCDC_SADD = (uint32_t)address;
	return previous;
}


void LCD_Window_disable(void)
{
	REG_LCDC_SSP &= ~PIPEN;
}


void LCD_Window_enable(void)
{
	if (!WindowAvailable) {
		return;
	}
	REG_LCDC_SSP |= PIPEN;
}


void LCD_Window_clear(LCD_ColourType colour)
{
	if (!WindowAvailable) {
		return;
	}
	size_t i;
	uint32_t fill = colour == LCD_WHITE ? 0 : ~0;
	for (i = 0; i < WindowBufferSize / sizeof(uint32_t); ++i) {
		((uint32_t *)REG_LCDC_SADD)[i] = fill;
	}
	WindowGraphicX = 0;
	WindowGraphicY = 0;
	WindowTextRow = 0;
	WindowTextColumn = 0;
	WindowForegroundColour = colour == LCD_WHITE ? LCD_BLACK : LCD_WHITE;
}


// window graphic operations
// -------------------------

static inline int WindowPos(int x, int y)
{
	return y * WindowByteWidth + (x >> 3);
}


LCD_ColourType LCD_Window_GetPixel(int x, int y)
{
	if (!WindowAvailable ||
	    x < 0 || x >= WindowWidth ||
	    y < 0 || y >= WindowHeight) {
		return LCD_BLACK;
	}
	return 0 != (((uint8_t *)REG_LCDC_SADD)[WindowPos(x, y)] & (0x80 >> (x & 0x07))) ? LCD_BLACK : LCD_WHITE;
}


static void Window_SetPixel(int x, int y, uint32_t colour)
{
	if (!WindowAvailable ||
	    x < 0 || x >= WindowWidth ||
	    y < 0 || y >= WindowHeight) {
		return;
	}
	if (LCD_BLACK == colour) {
		((uint8_t *)REG_LCDC_SADD)[WindowPos(x, y)] |= (0x80 >> (x & 0x07));
	} else {
		((uint8_t *)REG_LCDC_SADD)[WindowPos(x, y)] &= ~(0x80 >> (x & 0x07));
	}
}


void LCD_Window_SetPixel(int x, int y, LCD_ColourType colour)
{
	Window_SetPixel(x, y, (uint32_t)colour);
}


LCD_ColourType LCD_Window_SetColour(LCD_ColourType colour)
{
	register LCD_ColourType previous = WindowForegroundColour;
	WindowForegroundColour = colour;
	return previous;
}


LCD_ColourType LCD_Window_GetColour(void)
{
	return WindowForegroundColour;
}


void LCD_Window_Point(int x, int y)
{
	LCD_Window_MoveTo(x, y);
	LCD_Window_SetPixel(x, y, ForegroundColour);
}


void LCD_Window_MoveTo(int x, int y)
{
	WindowGraphicX = Standard_ClipValue(x, 0, WindowWidth - 1);
	WindowGraphicY = Standard_ClipValue(y, 0, WindowHeight - 1);
}


void LCD_Window_LineTo(int x, int y)
{
	register int x0 = WindowGraphicX;
	register int y0 = WindowGraphicY;
	LCD_Window_MoveTo(x, y);
	Graphics_DrawLine(x0, y0, WindowGraphicX, WindowGraphicY, WindowForegroundColour, Window_SetPixel);
}


// Generic Functions
// -----------------


void LCD_framebuffer_set_byte(int byte_idx, uint8_t value)
{
	((uint8_t *)REG_LCDC_MADD)[byte_idx] = value;
}

uint8_t LCD_framebuffer_get_byte(int byte_idx)
{
	return ((uint8_t *)REG_LCDC_MADD)[byte_idx];
}
