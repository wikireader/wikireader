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

#include <lsearcher.h>
#include "search.h"
#include "msg.h"

static lindex global_search;
static struct search_state state;
static int result;
static int search_index;
static char search_string[MAXSTR];



void search_init()
{
	result = load_index(&global_search, "/wikipedia.index.db", "/wikipedia.index.jump");
	if (!result) {
		/* XXX, FIXME, handle the error */
	}
}

void search_reset()
{
	search_index = 0;
}

void search_add(char c)
{
	if (search_index >= MAXSTR - 2)
		return;

	search_string[search_index++] = c;
	search_string[search_index] = '\0';
	prepare_search(&global_search, search_string, &state);
}

char* search_fetch_result()
{
    return search_fast(&global_search, search_string, &state);
}

extern unsigned int lsesrch_consume_block_stat();
void search_print_stats()
{
    msg(MSG_INFO, "Block read for search: %d", lsesrch_consume_block_stat());
}
