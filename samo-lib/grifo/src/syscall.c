/*
 * system call setup
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

#include "standard.h"

#include "vector.h"
#include "syscall.h"

// for individual syscalls
#include "delay.h"
#include "event.h"
#include "file.h"
#include "LCD.h"
#include "serial.h"
#include "system.h"
#include "timer.h"
#include "watchdog.h"


const void *const SysCallTable1[] = {
#include "syscall.table"
};

// NOTE: by saving these all hope of re-entrancy is lost
uint32_t saved_r15;
uint32_t saved_pc;

// NOTE: this does not have the interrupt attribute
static void handler(void);


void SystemCall_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;

		Vector_set(VECTOR_Software_exception_1, handler);
	}
}


// this is all assembler code
static void handler(void)
{
	// r15 is a real problem :-( need to save and restore
	asm volatile (
		"pushn\t%r1                \n\t"  // save a reg            ( return pc psr r1 r0 )
		"ld.w\t%r0, [%sp + 3]      \n\t"  // address of SYSCALL_x  [note: offset = 3 * 4]
		"ld.uh\t%r0, [%r0]         \n\t"  // SYSCALL_x value
		"sla\t%r0, 2               \n\t"  // convert to word offset
		"xld.w\t%r1, SysCallTable1 \n\t"  // vector table start
		"add\t%r0, %r1             \n\t"  // vector table start
		"ld.w\t%r0, [%r0]          \n\t"  // SYSCALL_x vector
		"or\t%r0, %r0              \n\t"  // check for NULL value
		"xjreq\tpanic_syscall      \n\t"  // to code in vector.c with other panic routines
		"ld.w\t[%sp + 3], %r0      \n\t"  // replace pc value      ( return vect psr r1 r0 )
		"xld.w\t%r0, saved_r15     \n\t"  // save r15
		"ld.w\t[%r0], %r15         \n\t"  // ...
		"xld.w\t%r15, __dp         \n\t"  // set up local data pointer in r15
		"xld.w\t%r0, saved_pc      \n\t"  // save return point
		"ld.w\t%r1, [%sp + 4]      \n\t"  // original xcall to the "int 1" address
		"ld.w\t[%r0], %r1          \n\t"  // ...                   ( restore_r15 vect psr r1 r0 )
		"xld.w\t%r0, restore_r15   \n\t"  // set exit point
		"ld.w\t[%sp + 4], %r0      \n\t"  // ...
		"popn\t%r1                 \n\t"  // restore reg           (  restore_r15 vect psr )
		"reti                      \n"    // jump to syscall routine with normal C stack
		"restore_r15:              \n\t"  // exit of syscall
		"pushn\t%r0                \n\t"  // create space on stack ( r0 )
		"pushn\t%r0                \n\t"  // save r0               ( r0 r0 )
		"xld.w\t%r0, saved_r15     \n\t"  // address of variable
		"ld.w\t%r15, [%r0]         \n\t"  // restore r15
		"xld.w\t%r0, saved_pc      \n\t"  // address of variable
		"ld.w\t%r0, [%r0]          \n\t"  // return address
		"ld.w\t[%sp + 1], %r0      \n\t"  // .. to stack           ( return r0 )
		"popn\t%r0                 \n\t"  // restore r0            ( return )
		);
}
