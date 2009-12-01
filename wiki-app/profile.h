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



/**
 * to profile a new function add another enum constant
 * and an identification string in PROF_TO_STRING
 */

enum {
	PROF_add,
	PROF_subt,
	PROF_mult,
	PROF_div,
	PROF_mod,
	PROF_delay,
	PROF_memset,
	PROF_memcpy,
	PROF_memcmp,
	PROF_fread,
	PROF_sd_read,
	PROF_sd_cmd,
	PROF_sd_block,
	PROF_sd_data,
	PROF_sd_spi,
	PROF_sd_rcvr,
	PROF_COUNT, /* keep COUNT the last item */
};

#if PROFILER_ON
#define PROF_TO_STRING				\
	[PROF_add]	= "addition",		\
		[PROF_subt]	= "subtract",	\
		[PROF_mult]	= "multipl ",	\
		[PROF_div]      = "division",	\
		[PROF_mod]	= "modulo  ",	\
		[PROF_delay]	= "delay   ",	\
		[PROF_memset]	= "memset  ",	\
		[PROF_memcpy]	= "memcpy  ",	\
		[PROF_memcmp]	= "memcmp  ",	\
		[PROF_fread]	= "fread   ",	\
		[PROF_sd_read]	= "sd read ",	\
		[PROF_sd_cmd]	= "sd cmd  ",	\
		[PROF_sd_block]	= "sd block",	\
		[PROF_sd_data]	= "sd data ",	\
		[PROF_sd_spi]	= "sd spi  ",	\
		[PROF_sd_rcvr]	= "sd rcvr ",

extern struct prof_container prof_container[PROF_COUNT];
#endif

struct prof_container {
	unsigned long start_time;
	unsigned long total_time;
	unsigned long calls;
};

void profile_init(void);
void prof_start(unsigned long index);
void prof_stop(unsigned long index);
void prof_print(void);
void prof_demo(void);
