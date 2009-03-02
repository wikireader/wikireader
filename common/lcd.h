#ifndef LCD_H
#define LCD_H

#include "config.h"

void init_lcd(void);

#ifdef BOARD_S1C33E07
#define LCD_WIDTH	(320)
#define LCD_HEIGHT	(240)
#else
#define LCD_WIDTH	(240)
#define LCD_HEIGHT	(208)
#endif

#if LCD_MONOCHROME
#define LCD_VRAM	(0x80000)
#define LCD_VRAM_SIZE	((LCD_WIDTH) * (LCD_HEIGHT) / 8)
#else
#define LCD_VRAM	(0x10100000)
#define LCD_VRAM_SIZE	((LCD_WIDTH) * (LCD_HEIGHT) / 2)
#endif

#endif /* LCD_H */
