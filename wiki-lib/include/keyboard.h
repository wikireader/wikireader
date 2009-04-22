/*
 *  Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WL_KEYBOARD_H
#define WL_KEYBOARD_H

/**
 * keyboard functionality
 */

typedef enum {
	KEYBOARD_NONE,
	KEYBOARD_CHAR,
	KEYBOARD_NUM
} keyboard_mode_e;

void keyboard_set_mode(keyboard_mode_e mode);
keyboard_mode_e keyboard_get_mode();
void keyboard_paint();
char keyboard_release(int x, int y);

#endif
