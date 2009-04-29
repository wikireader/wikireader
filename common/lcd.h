#ifndef LCD_H
#define LCD_H

#include "wikireader.h"

void init_lcd(void);

#if BOARD_SAMO_A1

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

#endif /* LCD_H */
