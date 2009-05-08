/*
 * Copyright (C) 2009
 *
 * Marek Lindner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 *
 */



#ifndef MALLOC_H
#define MALLOC_H

/* page size should not be smaller than struct malloc_page */
#define PAGE_SIZE 	(256)
#define MEMORY_DEBUG	1

/* add your tag here */
enum {
	MEM_TAG_UNUSED,
	MEM_TAG_DECOMP_M1,
	MEM_TAG_DECOMP_M2,
	MEM_TAG_DECOMP_F1,
	MEM_TAG_DECOMP_F2,
	MEM_TAG_DECOMP_F3,
	MEM_TAG_ARTICLE_F1,
	MEM_TAG_INDEX_M1,
	MEM_TAG_WOMR_M1,
	MEM_TAG_WOMR_F1,
	MEM_TAG_WOMR_F2,
	MEM_TAG_PERF_M1,
	MEM_TAG_PERF_M2,
	MEM_TAG_PERF_M3,
	MEM_TAG_PERF_M4,
	MEM_TAG_PERF_M5,
	MEM_TAG_PERF_F1,
	MEM_TAG_PERF_F2,
	MEM_TAG_PERF_F3,
	MEM_TAG_PERF_F4,
	MEM_TAG_PERF_F5,
	MEM_TAG_PERF_F6,
	MEM_TAG_PERF_F7,
};

void malloc_init_simple(void);
void *malloc_simple(uint32_t size, uint32_t tag);
void free_simple(void *ptr, uint32_t tag);
void malloc_status_simple(void);

#endif /* MALLOC_H */

