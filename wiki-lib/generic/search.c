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
static struct search_state last_first_hit;

static int search_index;
static char search_string[MAXSTR];

static char first_hit = 0;
static char trigram_loaded = 0;


void search_init()
{
	int result = load_index(&global_search, "/pedia.idx", "/pedia.jmp");
	if (!result) {
        msg(MSG_ERROR, "Failed to initialize search.\n");
		/* XXX, FIXME, handle the error */
	}
}

int search_load_trigram(void)
{
    if (!trigram_loaded) {
	trigram_loaded = !load_trigram_chunk(&global_search);
    }

    return trigram_loaded;
}

void search_reset()
{
	search_index = 0;
	first_hit = 0;
}

void search_add(char c)
{
	if (search_index >= MAXSTR - 2)
		return;

	search_string[search_index++] = c;
	search_string[search_index] = '\0';
	if (search_index >= 4) {
		state.pattern_len = search_index;
		if (first_hit) {
			reset_state(&global_search, &state, &last_first_hit);
		}

		first_hit = 0;
		return;
	}

	first_hit = 0;
	prepare_search(&global_search, search_string, &state);
}

/*
 * TODO: Optimize going back... For the first three entries
 * one could remember the &state and then seek back to the
 * position and continue the search from there... For now do it
 * the easy way and wait for user testing.
 */
void search_remove_char(void)
{
	if (search_index == 0)
		return;

	search_index -= 1;

	search_string[search_index] = '\0';
	memset(&state, 0, sizeof(state));
	first_hit = 0;
	prepare_search(&global_search, search_string, &state);
}

char* search_fetch_result()
{
	char *result;

	if (search_index == 0)
		return NULL;

	result = search_fast(&global_search, search_string, &state);
	if (!first_hit) {
		store_state(&global_search, &state, &last_first_hit);
		first_hit = 1;
	}

	return result;
}

extern unsigned int lsesrch_consume_block_stat();
void search_print_stats()
{
    msg(MSG_INFO, "Block read for search: %d\n", lsesrch_consume_block_stat());
}
