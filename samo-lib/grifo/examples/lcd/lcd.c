/*
 * lcd - an simple example program
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
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

int main(int argc, char **argv)
{
	debug_printf("lcd start\n");

	debug_printf("black screen\n");
	lcd_clear_reverse();
	delay_us(1000000);

	debug_printf("clear some pixels\n");
	int x;
	int y;
	for (x = 10, y = 10; x < 100; x += 5, y += 10) {
		lcd_set_pixel(x, y, LCD_WHITE);
	}
	delay_us(1000000);

	debug_printf("white screen\n");
	lcd_clear();
	delay_us(1000000);

	debug_printf("set some pixels\n");
	for (x = 150, y = 140; x < 200; x += 10, y += 5) {
		lcd_set_pixel(x, y, LCD_BLACK);
	}
	delay_us(1000000);

	debug_printf("draw some lines\n");
	lcd_line( 10,  10, 120,  10, LCD_BLACK);
	lcd_line(120,  10, 120, 100, LCD_BLACK);
	lcd_line(120, 100,  10, 100, LCD_BLACK);
	lcd_line( 10, 100,  10,  10, LCD_BLACK);

	lcd_line( 10,  10, 120, 100, LCD_BLACK);
	lcd_line(120,  10,  10, 100, LCD_BLACK);
	delay_us(1000000);

	debug_printf("eye pattern\n");
	lcd_clear();

	for (x = 0; x < 76; x += 5) {
		y = 75 - x;
		lcd_line(120 - y, 100, 120, y + 25, LCD_BLACK);
		lcd_line(120, y + 25, 120 + y, 100, LCD_BLACK);
		lcd_line(120 + y, 100, 120, x + 100, LCD_BLACK);
		lcd_line(120, x + 100, 120 - y, 100, LCD_BLACK);
		watchdog(WATCHDOG_KEY);
	}
	delay_us(2000000);

	return 0;
}
