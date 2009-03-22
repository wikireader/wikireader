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

/*
 * The secret of the position and size of the keyboard
 * is shared between search.c and this file.
 */

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
 *
 */
void keyboard_paint()
{
	if (!keyboard_visible)
		return;
}

/**
 * Coordinates are screen absolute ones
 */
char keyboard_release(int x, int y)
{
	return -1;
}
