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

#include "keyboard.h"

static int keyboard_visible = 1;

int keyboard_is_visible(void)
{
	return keyboard_visible;
}

void keyboard_set_visible(int visible)
{
	keyboard_visible = visible;
}

/**
 * x,y is the point where the keyboard starts. Height
 * is coming from the implementation..
 */
void keyboard_paint(int x, int y)
{
}

/**
 * (0,0) is the start of the keyboard, coordinates
 * are normalized here.
 */
void keyboard_press(int x, int y)
{
}
