/*
 * lcd - an simple example program
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

#include "grifo.h"

int grifo_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int x;
	int y;

	debug_printf("lcd start\n");

	debug_printf("black screen\n");
	lcd_clear(LCD_BLACK);
	delay_us(1000000);

	debug_printf("plot white pixels\n");
	for (x = 10, y = 10; x < 100; x += 5, y += 10) {
		lcd_point(x, y);
	}
	delay_us(1000000);

	debug_printf("draw white lines\n");
	lcd_move_to( 10,  10);
	lcd_line_to(120,  10);
	lcd_line_to(120, 100);
	lcd_line_to( 10, 100);
	lcd_line_to( 10,  10);

	lcd_move_to( 10,  10);
	lcd_line_to(120, 100);
	lcd_move_to(120,  10);
	lcd_line_to( 10, 100);
	delay_us(1000000);

	debug_printf("white screen\n");
	lcd_clear(LCD_WHITE);
	delay_us(1000000);

	debug_printf("plot black pixels\n");
	for (x = 150, y = 140; x < 200; x += 10, y += 5) {
		lcd_point(x, y);
	}
	delay_us(1000000);

	debug_printf("draw black lines\n");
	lcd_move_to( 10,  10);
	lcd_line_to(120,  10);
	lcd_line_to(120, 100);
	lcd_line_to( 10, 100);
	lcd_line_to( 10,  10);

	lcd_move_to( 10,  10);
	lcd_line_to(120, 100);
	lcd_move_to(120,  10);
	lcd_line_to( 10, 100);
	delay_us(1000000);

	debug_printf("eye pattern\n");
	lcd_clear(LCD_WHITE);

	for (x = 0; x < 76; x += 5) {
		y = 75 - x;
		lcd_move_to(120 - y, 100);
		lcd_line_to(120, y + 25);
		lcd_line_to(120 + y, 100);
		lcd_line_to(120, x + 100);
		lcd_line_to(120 - y, 100);
		watchdog(WATCHDOG_KEY);
	}
	delay_us(2000000);

	debug_printf("positioned text\n");
	lcd_clear(LCD_WHITE);
	lcd_at_xy(20, 8);
	lcd_print("one");
	delay_us(500000);

	lcd_at_xy(3, 1);
	lcd_print("two");
	delay_us(500000);

	lcd_at_xy(0, 0);
	lcd_print("A");
	delay_us(500000);

	lcd_at_xy(LCD_MAX_COLUMNS - 1, LCD_MAX_ROWS - 1);
	lcd_print("Z");
	delay_us(2000000);

	debug_printf("black text\n");
	lcd_clear(LCD_WHITE);
	lcd_print("This the first is a line of text\n");
	lcd_print("This the second is a line of text\n");
	lcd_print("This the third is a line of text\n");
	lcd_print("This the fourth is a line of text\n");
	lcd_print("This the fifth is a line of text\n");
	lcd_printf("some numbers: %d 0x%08x\n", 12345, 349599327);
	delay_us(2000000);

	debug_printf("white text\n");
	lcd_clear(LCD_BLACK);
	lcd_print("This the first is a line of text\n");
	lcd_print("This the second is a line of text\n");
	lcd_print("This the third is a line of text\n");
	lcd_print("This the fourth is a line of text\n");
	lcd_print("This the fifth is a line of text\n");
	lcd_printf("some numbers: %d 0x%08x\n", 12345, 349599327);
	delay_us(2000000);

	debug_printf("mixed text\n");
	lcd_clear(LCD_WHITE);
	lcd_print("This the first is a line of text\n");
	(void)lcd_set_colour(LCD_WHITE);
	lcd_print("This the second is a line of text\n");
	(void)lcd_set_colour(LCD_BLACK);
	lcd_print("This the third is a line of text\n");
	(void)lcd_set_colour(LCD_WHITE);
	lcd_print("This the fourth is a line of text\n");
	(void)lcd_set_colour(LCD_BLACK);
	lcd_print("This the fifth is a line of text\n");
	lcd_printf("some numbers: %d 0x%08x\n", 12345, 349599327);
	delay_us(2000000);


	debug_printf("scrolling text\n");
	lcd_clear(LCD_WHITE);
	for (x = 1; x <= 30; ++x) {
		lcd_printf("line number: %d\n", x);
	}
	delay_us(2000000);


	debug_printf("picture-in-picture\n");
	lcd_clear(LCD_WHITE);
	for (y = 0; y < LCD_MAX_ROWS; ++y) {
		lcd_at_xy(0, y);
		lcd_printf("background line: %d", y);
	}

	lcd_window(15, 19, 17, 21);
	lcd_window_clear(LCD_WHITE);
	lcd_window_move_to(0,0);
	lcd_window_line_to(16,0);
	lcd_window_line_to(16,20);
	lcd_window_line_to(0,20);
	lcd_window_line_to(0,0);

	for (x = 0; x < 10; ++x) {
		debug_printf("picture-in-picture enable\n");
		lcd_window_enable();
		delay_us(1000000);
		debug_printf("picture-in-picture disable\n");
		lcd_window_disable();
		delay_us(1000000);
	}

	return 0;
}
