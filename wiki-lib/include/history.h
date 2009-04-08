/*
 *  Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
 *  Copyright (c) 2009 Matt Hsu <matt_hsu@openmoko.org>
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

#ifndef WL_HISTORY_H
#define WL_HISTORY_H

/*
 * Interface for the History feature
 */


void history_display(void);
void history_reset(void);
void history_select_down(void);
void history_select_up(void);
const char *history_current_target(void);

void history_add(const char *text, const char *target);
void history_move_current_to_top(const char *target);

const char *history_get_item_title(unsigned int index);
const char *history_get_item_target(unsigned int index);

unsigned int history_item_size(void);
unsigned int history_free_item_size(void);

const char *history_get_top_target(void);

void history_list_init(void);
struct history_item *history_find_item_title(const char *title);
struct history_item *history_find_item_target(const char *target);

const char *history_release(int y);
#endif
