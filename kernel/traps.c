/*
 * mahatma - a simple kernel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
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

#include <input.h>

#include "regs.h"
#include "traps.h"
#include "serial.h"
#include "suspend.h"
#include "irq.h"

static void undef_irq_handler(void)
{
	asm("reti");
}

static void serial0_err_irq(void)
{
	/* clear interrupt */
	REG_INT_FSIF01 = 1 << 0;
	asm("reti");
}

static void serial0_in_irq(void)
{
	serial_filled(0);
	serial_out(0,  ".");
	/* clear interrupt */
	REG_INT_FSIF01 = 1 << 1;
	asm("reti");
}

static void serial0_out_irq(void)
{
	serial_drained(0);
	/* clear interrupt */
	REG_INT_FSIF01 = 1 << 2;
	asm("reti");
}

static void serial1_err_irq(void)
{
	/* clear interrupt */
	REG_INT_FSIF01 = 1 << 3;
	asm("reti");
}

static void serial1_in_irq(void)
{
	serial_filled(1);
	/* clear interrupt */
	REG_INT_FSIF01 = 1 << 4;
	asm("reti");
}

static void serial1_out_irq(void)
{
	serial_drained(1);
	/* clear interrupt */
	REG_INT_FSIF01 = 1 << 5;
	asm("reti");
}

#define N_TRAPS 108
typedef void (*irq_callback)(void);
irq_callback trap_table[N_TRAPS] = {
	undef_irq_handler,	/* offset 0	*/
	undef_irq_handler,	/* offset 1	*/
	undef_irq_handler,	/* offset 2	*/
	undef_irq_handler,	/* offset 3	*/
	undef_irq_handler,	/* offset 4	*/
	undef_irq_handler,	/* offset 5	*/
	undef_irq_handler,	/* offset 6	*/
	undef_irq_handler,	/* offset 7	*/
	undef_irq_handler,	/* offset 8	*/
	undef_irq_handler,	/* offset 9	*/
	undef_irq_handler,	/* offset 10	*/
	undef_irq_handler,	/* offset 11	*/
	undef_irq_handler,	/* offset 12	*/
	undef_irq_handler,	/* offset 13	*/
	undef_irq_handler,	/* offset 14	*/
	undef_irq_handler,	/* offset 15	*/
	undef_irq_handler,	/* offset 16	*/
	undef_irq_handler,	/* offset 17	*/
	undef_irq_handler,	/* offset 18	*/
	undef_irq_handler,	/* offset 19	*/
	undef_irq_handler,	/* offset 20	*/
	undef_irq_handler,	/* offset 21	*/
	undef_irq_handler,	/* offset 22	*/
	undef_irq_handler,	/* offset 23	*/
	undef_irq_handler,	/* offset 24	*/
	undef_irq_handler,	/* offset 25	*/
	undef_irq_handler,	/* offset 26	*/
	undef_irq_handler,	/* offset 27	*/
	undef_irq_handler,	/* offset 28	*/
	undef_irq_handler,	/* offset 29	*/
	undef_irq_handler,	/* offset 30	*/
	undef_irq_handler,	/* offset 31	*/
	undef_irq_handler,	/* offset 32	*/
	undef_irq_handler,	/* offset 33	*/
	undef_irq_handler,	/* offset 34	*/
	undef_irq_handler,	/* offset 35	*/
	undef_irq_handler,	/* offset 36	*/
	undef_irq_handler,	/* offset 37	*/
	undef_irq_handler,	/* offset 38	*/
	undef_irq_handler,	/* offset 39	*/
	undef_irq_handler,	/* offset 40	*/
	undef_irq_handler,	/* offset 41	*/
	undef_irq_handler,	/* offset 42	*/
	undef_irq_handler,	/* offset 43	*/
	undef_irq_handler,	/* offset 44	*/
	undef_irq_handler,	/* offset 45	*/
	undef_irq_handler,	/* offset 46	*/
	undef_irq_handler,	/* offset 47	*/
	undef_irq_handler,	/* offset 48	*/
	undef_irq_handler,	/* offset 49	*/
	undef_irq_handler,	/* offset 50	*/
	undef_irq_handler,	/* offset 51	*/
	undef_irq_handler,	/* offset 52	*/
	undef_irq_handler,	/* offset 53	*/
	undef_irq_handler,	/* offset 54	*/
	undef_irq_handler,	/* offset 55	*/
	serial0_err_irq,	/* offset 56	serial 0 - receive error		*/
	serial0_in_irq,		/* offset 57	serial 0 - receive buffer full		*/
	serial0_out_irq,	/* offset 58	serial 0 - transmit buffer empty	*/
	undef_irq_handler,	/* offset 59	*/
	serial1_err_irq,	/* offset 60	serial 1 - receive error		*/
	serial1_in_irq,		/* offset 61	serial 1 - recevice buffer full		*/
	serial1_out_irq,	/* offset 62	serial 1 - transmit buffer empty	*/
	undef_irq_handler,	/* offset 63	*/
	undef_irq_handler,	/* offset 64	*/
	undef_irq_handler,	/* offset 65	*/
	undef_irq_handler,	/* offset 66	*/
	undef_irq_handler,	/* offset 67	*/
	undef_irq_handler,	/* offset 68	*/
	undef_irq_handler,	/* offset 69	*/
	undef_irq_handler,	/* offset 70	*/
	undef_irq_handler,	/* offset 71	*/
	undef_irq_handler,	/* offset 72	*/
	undef_irq_handler,	/* offset 73	*/
	undef_irq_handler,	/* offset 74	*/
	undef_irq_handler,	/* offset 75	*/
	undef_irq_handler,	/* offset 76	*/
	undef_irq_handler,	/* offset 77	*/
	undef_irq_handler,	/* offset 78	*/
	undef_irq_handler,	/* offset 79	*/
	undef_irq_handler,	/* offset 80	*/
	undef_irq_handler,	/* offset 81	*/
	undef_irq_handler,	/* offset 82	*/
	undef_irq_handler,	/* offset 83	*/
	undef_irq_handler,	/* offset 84	*/
	undef_irq_handler,	/* offset 85	*/
	undef_irq_handler,	/* offset 86	*/
	undef_irq_handler,	/* offset 87	*/
	undef_irq_handler,	/* offset 88	*/
	undef_irq_handler,	/* offset 89	*/
	undef_irq_handler,	/* offset 90	*/
	undef_irq_handler,	/* offset 91	*/
	undef_irq_handler,	/* offset 92	*/
	undef_irq_handler,	/* offset 93	*/
	undef_irq_handler,	/* offset 94	*/
	undef_irq_handler,	/* offset 95	*/
	undef_irq_handler,	/* offset 96	*/
	undef_irq_handler,	/* offset 97	*/
	undef_irq_handler,	/* offset 98	*/
	undef_irq_handler,	/* offset 99	*/
	undef_irq_handler,	/* offset 100	*/
	undef_irq_handler,	/* offset 101	*/
	undef_irq_handler,	/* offset 102	*/
	undef_irq_handler,	/* offset 103	*/
	undef_irq_handler,	/* offset 104	*/
	undef_irq_handler,	/* offset 105	*/
	undef_irq_handler,	/* offset 106	*/
	undef_irq_handler,	/* offset 107	*/
};

void traps_init(void)
{
	/* relocate the trap table */
	asm("ld.w %%ttbr, %0" :: "r"(trap_table));
	ENABLE_IRQ();
}

