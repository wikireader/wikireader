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

#define KEYBOARD_WIDTH	240
#define KEYBOARD_HEIGHT	82

enum {
	KEYBOARD_NONE,
	KEYBOARD_CHAR,
	KEYBOARD_NUM
};

enum {
	KEYBOARD_RESET_INVERT_DELAY,
	KEYBOARD_RESET_INVERT_NOW,
	KEYBOARD_RESET_INVERT_CHECK
};

struct keyboard_key {
	/*
	 * a rect described by top left and
	 * bottom right point.
	 */
	int left_x, right_x;
	int left_y, right_y;
	char key;
};

void keyboard_set_mode(int mode);
int keyboard_get_mode();
unsigned int keyboard_height();
void keyboard_paint();
struct keyboard_key * keyboard_get_data(int x, int y);
void keyboard_key_invert(struct keyboard_key *key);
int keyboard_key_reset_invert(int bFlag);

#endif
