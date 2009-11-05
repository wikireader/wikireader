/*
 * interrupt vector handling
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
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


#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>

#include <regs.h>

#include "serial.h"
#include "interrupt.h"
#include "vector.h"


static void panic_dump(uint32_t *base, uint32_t r15_value, const char *message);

// this must be kept very simple:
//   restict use of local variables to registers (max 4)
//   only cal simple functions (0..4) simple arguments
// Otherwise the %sp value will not be as expected
// also save and restore kernel r15 so access to data works


#define MAKE_PANIC(name, string)                                      \
__attribute__((interrupt_handler))                                    \
void name(void)                                                       \
{                                                                     \
	register uint32_t *base;                                      \
	register uint32_t r15_value;                                  \
								      \
	asm volatile ("ld.w\t%0, %%sp    \n\t"                        \
		      "ld.w\t%1, %%r15   \n\t"                        \
		      "xld.w\t%%r15, __dp    "                        \
		      : "=r" (base), "=r" (r15_value));               \
								      \
	panic_dump(base, r15_value, string);                          \
								      \
	for (;;) {                                                    \
	}                                                             \
}


// the default interrupt handlers
MAKE_PANIC(panic, "undefined interrupt")
MAKE_PANIC(panic_syscall, "undefined syscall") // called directly by syscall.c
MAKE_PANIC(panic_ext_exception, "ext exception")
MAKE_PANIC(panic_Undefined_instruction_exception, "Undefined instruction exception")
MAKE_PANIC(panic_Address_misaligned_exception, "Address misaligned exception")
MAKE_PANIC(panic_NMI, "NMI")
MAKE_PANIC(panic_Illegal_interrupt_exception, "Illegal interrupt exception")
MAKE_PANIC(panic_Software_exception_0, "Software exception 0")
MAKE_PANIC(panic_Software_exception_1, "Software exception 1")
MAKE_PANIC(panic_Software_exception_2, "Software exception 2")
MAKE_PANIC(panic_Software_exception_3, "Software exception 3")


void Vector_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		register Vector_handler **table;
		register int table_size;

		// attempting to access linker defined values by "extern uint_t __START_VectorTable"
		// and using & operator as described in the GCC/LD documentation
		// results in linker error.  This is because the vector table is below the __dp
		// value, therefore it is necesssary to use assembler to get these values.

		asm volatile ("xld.w\t%0, __START_VectorTable\n\t"
			      "xld.w\t%1, __ENTRIES_VectorTable"
			      : "=r" (table), "=r" (table_size));

		int i = 0;

		Interrupt_type state = Interrupt_disable();

		for (i = 0; i < table_size; ++i) {
			table[i] = panic;
		}

		REG_CMU_PROTECT = CMU_PROTECT_OFF;
		REG_CMU_OPT |= WAKEUPWT;
		REG_CMU_PROTECT = CMU_PROTECT_ON;

		asm volatile ("ld.w %%ttbr, %0"
			      :
			      : "r" (table));

		(void)Vector_set(VECTOR_ext_exception, panic_ext_exception);
		(void)Vector_set(VECTOR_Undefined_instruction_exception, panic_Undefined_instruction_exception);
		(void)Vector_set(VECTOR_Address_misaligned_exception, panic_Address_misaligned_exception);
		(void)Vector_set(VECTOR_NMI, panic_NMI);
		(void)Vector_set(VECTOR_Illegal_interrupt_exception, panic_Illegal_interrupt_exception);
		(void)Vector_set(VECTOR_Software_exception_0, panic_Software_exception_0);
		(void)Vector_set(VECTOR_Software_exception_1, panic_Software_exception_1);
		(void)Vector_set(VECTOR_Software_exception_2, panic_Software_exception_2);
		(void)Vector_set(VECTOR_Software_exception_3, panic_Software_exception_3);

		Interrupt_enable(state);
	}
}


Vector_handler *Vector_get(Vector_type vector_number)
{
	register Vector_handler **table;
	register int table_size;

	asm volatile ("xld.w\t%0, __START_VectorTable\n\t"
		      "xld.w\t%1, __ENTRIES_VectorTable"
		      : "=r" (table), "=r" (table_size));

	if (vector_number < 0 || vector_number >= table_size) {
		return NULL;
	}

	return table[vector_number];
}


Vector_handler *Vector_set(Vector_type vector_number, Vector_handler *vector)
{
	register Vector_handler **table;
	register int table_size;

	asm volatile ("xld.w\t%0, __START_VectorTable\n\t"
		      "xld.w\t%1, __ENTRIES_VectorTable"
		      : "=r" (table), "=r" (table_size));

	if (vector_number < 0 || vector_number >= table_size) {
		return NULL;
	}

	Vector_handler *previous_entry = table[vector_number];
	table[vector_number] = vector;

	return previous_entry;
}


static void panic_dump(uint32_t *base, uint32_t r15_value, const char *message)
{
	Serial_printf("\nPanic: %s\n", message);
	Serial_printf("sp  = 0x%08lx [%11ld]\n"
		      "pc  = 0x%08lx [%11ld]\n"
		      "psr = 0x%08lx [%11ld]\n"
		      "ahr = 0x%08lx [%11ld]\n"
		      "alr = 0x%08lx [%11ld]\n",
		      (uint32_t)(base), (uint32_t)(base),
		      base[18], base[18],
		      base[17], base[17],
		      base[1], base[1],
		      base[0], base[0]);
	register int i = 0;
	for (i = 0; i < 15; ++i) {
		Serial_printf("r%02d = 0x%08lx [%11ld]\n", i, base[i + 2], base[i + 2]);
	}
	Serial_printf("r%02d = 0x%08lx [%11ld]\n", 15, r15_value, r15_value);
}


