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

#include "history.h"
#include <guilib.h>
#include <lsearcher.h>

#include <stdlib.h>

struct history_item {
	char title[MAXSTR];
	char target[6];
};

static struct history_item history_items[100];
static unsigned int items = 0;

void history_display(void)
{
	guilib_fb_lock();

	guilib_clear();
	render_string(0, 1, 14, "History", 7);

	guilib_fb_unlock();
}

void history_select_down(void)
{
}

void history_select_up(void)
{
}

const char *history_current_target(void)
{
    return NULL;
}

void history_add(const char *text, const char *target)
{
}

void history_move_current_to_top(void)
{
}
