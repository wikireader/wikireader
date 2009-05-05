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



#include <profile.h>
#include <tick.h>
#include <types.h>
#include <irq.h>
#include <string.h>
#include <msg.h>
#include <regs.h>
#include <wikireader.h>
#include <misc.h>

#if PROFILER_ON

static struct prof_container prof_container[PROF_COUNT];
static const char *prof_to_string[] = { PROF_TO_STRING };


void profile_init(void)
{
	tick_initialise();

	memset(&prof_container, 0, sizeof(prof_container));
}

void prof_start(unsigned long index)
{
	DISABLE_IRQ();
	prof_container[index].start_time = tick_get();
}

void prof_stop(unsigned long index)
{
	unsigned long tmp_tick = tick_get();

	ENABLE_IRQ();

	prof_container[index].calls++;

	prof_container[index].total_time += tmp_tick - prof_container[index].start_time;

	msg(MSG_INFO, "debug: %x %x %x\n", prof_container[index].start_time, tmp_tick, tmp_tick - prof_container[index].start_time);
	msg(MSG_INFO, "debug: %u %u %u\n", prof_container[index].start_time, tmp_tick, tmp_tick - prof_container[index].start_time);
}

void prof_print(void) {

	unsigned long index;

	msg(MSG_INFO, "Profile data:\n");

	for (index = 0; index < PROF_COUNT; index++) {

		msg(MSG_INFO, "   %s: cpu time = %u, calls: %u, avg time per call = %u\n",
			prof_to_string[index],
			prof_container[index].total_time / MCLK_MHz, prof_container[index].calls,
			(prof_container[index].calls == 0 ? 0 :
			((prof_container[index].total_time / MCLK_MHz) / prof_container[index].calls)));

	}

}

void prof_demo(void)
{
	volatile unsigned int x = 2323;

	prof_start(PROF_add);
	x = 50000 + x;
	prof_stop(PROF_add);

	prof_start(PROF_subst);
	x = 50000 - x;
	prof_stop(PROF_subst);

	prof_start(PROF_mult);
	x = 50000 * x;
	prof_stop(PROF_mult);

	prof_start(PROF_div);
	x = 50000 / x;
	prof_stop(PROF_div);

	prof_start(PROF_mod);
	x = 50000 % x;
	prof_stop(PROF_mod);

	prof_start(PROF_delay);
	delay_us(1000);
	prof_stop(PROF_delay);

	prof_print();
}

#else

void profile_init(void)
{
}

void prof_start(unsigned long index)
{
}

void prof_stop(unsigned long index)
{
}

void prof_print(void)
{
}

void prof_demo(void)
{
}

#endif
