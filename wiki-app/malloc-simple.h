// Copyright (c) 2009 Openmoko Inc.
//
//
// Authors:	Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#ifndef MALLOC_H
#define MALLOC_H

#include <inttypes.h>

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
