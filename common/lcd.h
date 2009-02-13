#ifndef LCD_H
#define LCD_H

void init_lcd(void);

#if LCD_MONOCHROME
#define LCD_VRAM	(0x80000)
#define LCD_VRAM_SIZE	(0x2FFF)
#else
#define LCD_VRAM	(0x10100000)
#define LCD_VRAM_SIZE	((320 * 240) / 2)
#endif

#endif /* LCD_H */

