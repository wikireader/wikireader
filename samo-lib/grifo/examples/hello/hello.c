/*
 * hello - a sample application for the grifo kernel
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

#include <grifo.h>


int grifo_main(int argc, char *argv[])
{
	lcd_clear(LCD_WHITE);
//	lcd_printf("Hello LCD world\n");
	debug_printf("Hello serial world\n");
	int i;
	for (i = 0; i < argc; ++i) {
//		lcd_printf("argv[%d] = '%s'\n", i, argv[i]);
		debug_printf("argv[%d] = '%s'\n", i, argv[i]);
	}

	delay_us(1000000);
	return EXIT_RESTART_INIT;
}
