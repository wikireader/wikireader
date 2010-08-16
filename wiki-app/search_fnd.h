/*
 * Copyright (c) 2010 Openmoko Inc.
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

#ifndef SEARCH_FND_H
#define SEARCH_FND_H

#include <inttypes.h>

#include "bigram.h"

#define MAX_SEARCH_STRING_HASHED_LEN 15
#define MAX_SEARCH_STRING_ALL_HASHED_LEN 5
#define SEARCH_FND_SEQUENTIAL_SEARCH_THRESHOLD 64

void init_search_fnd(void);
int copy_fnd_to_buf(long offset, char *buf, int len);
int copy_str_to_buf(long offset, char *buf, int len);
long get_search_offset_fnd(char *sSearchString, int len);
void retrieve_titles_from_fnd(long offset_fnd, unsigned char *sTitleSearch, unsigned char *sTitleActual);

#endif
