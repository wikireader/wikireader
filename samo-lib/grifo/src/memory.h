/*
 * memory - memory driver for heap based memory allocator
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#if  !defined(_MEMORY_H_)
#define _MEMORY_H_ 1

#include "standard.h"

void Memory_initialise(void);

// reconfigure the memory region available to the allocator
void Memory_SetHeap(uint32_t FirstFreeAddress, uint32_t LastFreeAddress);

//*[alloc]: tag is a short string to help debug memory allocation failures
//*[alloc]: choose unique tag strings for each allocate/free
void *Memory_allocate(size_t size, const char *tag);
void Memory_free(void *address, const char *tag);

//*[debug]: display message on the seriala console
//*[debug]: then dump the heap headers followed by a short summary
//*[debug]: each header contains the allcate/free tags to show
//*[debug]: where the memory was allocated or freed
void Memory_debug(const char *message);

#endif
