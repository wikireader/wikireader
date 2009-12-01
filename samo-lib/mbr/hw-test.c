/*
 * hw-test - test some of the hardware inputs
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

#define	APPLICATION_TITLE "Key Test"
#define	APPLICATION_TITLE2 "LCD Test"

#include <stdbool.h>
#include "application.h"
#include "lcd.h"


#define DELAY 1000


void key_test(void);
void lcd_test(void);
static bool lcd_items(void);


// this must be the first executable code as the loader executes from the first program address
ReturnType hardware_test(int block, int status)
{
	APPLICATION_INITIALISE();
	{
		switch (status) {
		case 0:
			key_test();
			break;
		case 1:
			lcd_test();
			break;
		}

	}
	APPLICATION_FINALISE(0, 0);
}



void key_test(void)
{
	unsigned int state = 0xffff; // impossible key state
	print("press buttons on keypad. [any serial input to exit]\n");

	while (!console_input_available()) {
		unsigned int keys = REG_P6_P6D & 0x07;
		if (keys != state) {
			state = keys;
			print("keys = 0x");
			print_byte(REG_P6_P6D & 0x07);
			if (0 != (keys & 0x02)) {
				print(" random");
			} else {
				print("       ");
			}
			if (0 != (keys & 0x04)) {
				print(" history");
			} else {
				print("        ");
			}
			if (0 != (keys & 0x01)) {
				print(" search");
			} else {
				print("       ");
			}
			print("\n");
		}
	}
	(void)console_input_char();
}


void lcd_test(void)
{
	print("lcd test\n");

	print("LCD_VRAM          = ");
	print_hex(LCD_VRAM);
	print_char('\n');

	print("LCD_HEIGHT_LINES  = ");
	print_uint(LCD_HEIGHT_LINES);
	print_char('\n');

	print("LCD_WIDTH_PIXELS  = ");
	print_uint(LCD_WIDTH_PIXELS);
	print_char('\n');

	print("LCD_WIDTH_BYTES   = ");
	print_uint(LCD_WIDTH_BYTES);
	print_char('\n');


	print("VRAM_HEIGHT_LINES = ");
	print_uint(LCD_VRAM_HEIGHT_LINES);
	print_char('\n');

	print("VRAM_WIDTH_PIXELS = ");
	print_uint(LCD_VRAM_WIDTH_PIXELS);
	print_char('\n');

	print("VRAM_WIDTH_BYTES  = ");
	print_uint(LCD_VRAM_WIDTH_BYTES);
	print_char('\n');

	LCD_initialise();

	do {
		print("loop (space->pause/resume, enter -> exit)\n");
	} while (lcd_items());

}

static bool pause(const char *prompt, int millisec)
{
	int i = 0;
	print(prompt);
	for (i = 0; i < millisec; ++i) {
		delay_us(1000);
		if (console_input_available()) {
			char c = 0;
			c = console_input_char();
			if ('\r' == c || '\n' == c) {
				return true;
			}
			c = console_input_char();
			if ('\r' == c || '\n' == c) {
				return true;
			}
			break;
		}
	}
	return false;
}


static void fill(uint8_t value)
{
	int x = 0;
	int y = 0;
	uint8_t *fb = (uint8_t*)LCD_VRAM;
	for (y = 0; y < LCD_HEIGHT_LINES; ++y) {
		for (x = 0; x < LCD_VRAM_WIDTH_BYTES; ++x) {
			*fb++ = value;
		}
	}
}

static void stripe(uint8_t value, uint8_t fill)
{
	int x = 0;
	int y = 0;
	uint8_t *fb = (uint8_t*)LCD_VRAM;
	for (y = 0; y < LCD_HEIGHT_LINES; ++y) {
		for (x = 0; x < LCD_VRAM_WIDTH_BYTES; ++x) {
			*fb++ = (0 == (y & (1 << 5))) ? value : fill;
		}
	}
}

#if 0
static void display_image(uint8_t toggle)
{
	int x = 0;
	const uint8_t *src = image_data;
	uint8_t *fb = (uint8_t*)LCD_VRAM;
	int l = 0;

	fill(toggle);
	for (l = 0; l < sizeof(image_data); ++l) {
		fb[x] = toggle ^ *src++;
		++x;
		if (LCD_WIDTH_BYTES <= x) {
			x = 0;
			fb += LCD_VRAM_WIDTH_BYTES;
		}
	}
}
#endif

static bool lcd_items(void)
{

	fill(0);
	if (pause("00 ", DELAY)) {
		return false;
	}

	fill(0xff);
	if (pause("ff ", DELAY)) {
		return false;
	}

	fill(0x55);
	if (pause("55 ", DELAY)) {
		return false;
	}

	fill(0x44);
	if (pause("44 ", DELAY)) {
		return false;
	}

	fill(0xf7);
	if (pause("f7 ", DELAY)) {
		return false;
	}
	print_char('\n');

	print("stripe(00) ");

	stripe(0xff, 0x00);
	if (pause("ff ", DELAY)) {
		return false;
	}

	stripe(0x01, 0x00);
	if (pause("01 ", DELAY)) {
		return false;
	}
	print_char('\n');

	print("stripe(ff) ");

	stripe(0x00, 0xff);
	if (pause("00 ", DELAY)) {
		return false;
	}

	stripe(0xee, 0xff);
	if (pause("7f ", DELAY)) {
		return false;
	}
	print_char('\n');

#if 0
	display_image(0x00);
	if (pause("image(00)", DELAY)) {
		return false;
	}
	print_char('\n');

	display_image(0xff);
	if (pause("image(ff)", DELAY)) {
		return false;
	}
	print_char('\n');
#endif
	return true;
}
