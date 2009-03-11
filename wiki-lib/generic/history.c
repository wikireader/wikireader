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
#include <glyph.h>
#include <lsearcher.h>
#include <wikilib.h>

#include <stdlib.h>


#define RESULT_START 20
#define RESULT_HEIGHT 10
#define NUMBER_OF_RESULTS 19

struct history_item {
	char title[MAXSTR];
	char target[6];
};

static struct history_item history_items[100];
static unsigned int current_start = 0;
static unsigned int used_items = 0;

void history_display(void)
{
	unsigned int i;
	int y_pos;

	guilib_fb_lock();

	guilib_clear();
	render_string(0, 1, 14, "History", 7);

	if (used_items == 0) {
		render_string(0, 1, 100, "No history.", 11);
	} else {
		for (i = current_start; i < used_items; ++i) {
			render_string(0, 1, y_pos, history_items[0].title, strlen(history_items[0].title));
			y_pos += RESULT_HEIGHT;
		}
	}

	guilib_fb_unlock();
}

void history_reset(void)
{
	current_start = 0;
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

void history_add(const char *title, const char *target)
{
	/* make room ... */
	static const unsigned int items = ARRAY_SIZE(history_items);

	memmove(&history_items[1], &history_items[0], (items - 1) * sizeof(history_items[0]));
	if (title)
		strcpy(history_items[0].title, title);

	if (target)
		strcpy(history_items[0].target, target);

	used_items = MIN(used_items + 1, ARRAY_SIZE(history_items));
}

void history_move_current_to_top(void)
{
}
