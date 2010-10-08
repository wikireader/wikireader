/*
 * memory - test memory allocate / free
 *
 * Copyright (c) 2010 Openmoko Inc.
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

#include <string.h>

#include "grifo.h"


int grifo_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	debug_printf("memory allocation test\n");

	memory_debug("start");

	uint8_t *m1 = memory_allocate(5242, "m1");
	uint8_t *m2 = memory_allocate(25242, "m2");
	uint8_t *m3 = memory_allocate(53242, "m3");
	uint8_t *m4 = memory_allocate(25142, "m4");
	uint8_t *m5 = memory_allocate(985242, "m5");

	memory_debug("Allocated: m1..m5");

	memory_free(m3, "f1");
	memory_free(m1, "f2");
	memory_free(m5, "f3");
	memory_debug("freed m3, m1, m5");

	memory_free(m4, "f4");
	memory_debug("free m4");

	memory_free(m2, "f5");
	memory_debug("free m2");

#if !defined(GRIFO_SIMULATOR)
	memory_free(m2, "f5*2");
	memory_debug("free f2 again");
#endif

	m1 = memory_allocate(75242, "new m1");
	memory_debug("allocated new m1");
	memory_free(m1, "f1b");
	memory_debug("free new m1");


	m1 = memory_allocate(12524, "m1c");
	m2 = memory_allocate(12524, "m2c");
	m3 = memory_allocate(12524, "m3c");
	m4 = memory_allocate(12524, "m4c");
	m5 = memory_allocate(12524, "m5c");
	memory_debug("allocated 5 equal size blocks");

	memory_free(m2, "f1c");
	memory_free(m3, "f2c");
	memory_debug("Freed blocks 2 and 3");

	m2 = memory_allocate(20000, "allocation larger than 1 original block");
	memory_debug("consolidation should have happened");


	memory_free(m2, "f1d");

	uint8_t *a1 = memory_allocate(30000, "a1");
	uint8_t *a2 = memory_allocate(30000, "a2");
	uint8_t *a3 = memory_allocate(30000, "a3");
	uint8_t *a4 = memory_allocate(30000, "a4");
	uint8_t *a5 = memory_allocate(30000, "a5");
	uint8_t *a6 = memory_allocate(30000, "a6");
	uint8_t *a7 = memory_allocate(30000, "a7");
	uint8_t *a8 = memory_allocate(30000, "a8");
	uint8_t *a9 = memory_allocate(30000, "a9");

	memory_free(a2, "f a2");
	memory_free(a3, "f a3");

	memory_free(a6, "f a6");
	memory_free(a7, "f a7");
	memory_free(a8, "f a8");

	memory_debug("now have various sized free spaces");

	uint8_t *x1 = memory_allocate(80000, "x1");

	memory_debug("consolidated");

	memory_free(a1, "z a1");
	memory_free(a4, "z a4");
	memory_free(a5, "z a5");
	memory_free(a9, "z a9");
	memory_free(x1, "z x1");
	memory_free(m1, "z m1");
	memory_free(m4, "z m4");
	memory_free(m5, "z m5");

	memory_debug("should all be free");

	m1 = memory_allocate(100,"last");
	memory_debug("final consolidation, just 1 allocated and 1 free");

	return 0;
}
