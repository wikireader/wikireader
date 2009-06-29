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

#include <wikilib.h>
#include <input.h>

#include "regs.h"
#include "traps.h"
#include "serial.h"
#include "irq.h"
#include "misc.h"
#include "gpio.h"
#include "touchscreen.h"

#if 1
#define CLEAR_IRQ(reg,val)                      \
	do {                                    \
		(reg) = (val);                  \
	} while (0)
#else
#error "this consumes too much stack"
#define CLEAR_IRQ(reg,val)			\
	asm("pushn %r12");			\
	asm("pushn %r13");			\
	asm("ld.w %%r12, %0" :: "r"((val)));	\
	asm("xld.w %%r13, %0" :: "g"(&(reg))); 	\
	asm("ld.b [%r13], %r12");		\
	asm("popn %r13");			\
	asm("popn %r12");
#endif


static void undef_irq_handler(void) __attribute__((interrupt_handler));
static void illegal_instruction(void) __attribute__((interrupt_handler));
static void illegal_irq_handler(void) __attribute__((interrupt_handler));
static void serial0_err_irq(void) __attribute__((interrupt_handler));
static void serial0_in_irq(void) __attribute__((interrupt_handler));
static void serial0_out_irq(void) __attribute__((interrupt_handler));
static void serial1_err_irq(void) __attribute__((interrupt_handler));
static void serial1_in_irq(void) __attribute__((interrupt_handler));
static void serial1_out_irq(void) __attribute__((interrupt_handler));
static void kint_irq(void) __attribute__((interrupt_handler));
static void unaligned_data_access(void) __attribute__((interrupt_handler));


void panic(const char *s, const uint32_t *stack)
{
	uint32_t *sp = (uint32_t *)((uint32_t)stack & ~3);
	print_char('\n');
	print(s);
	print("\nSP  = ");
	print_u32((uint32_t)stack);
	print_char('\n');

	{
		register unsigned int i = 0;
		register unsigned int j = 0;
		register unsigned int p = 0;
		for (i = 0; i <= 15; ++i) {
			print("R");
			print_dec32(i);
			if ( i < 10) {
				print_char(' ');
			}
			print(" = ");
			print_u32(sp[p]);
			print("  [");
			print_dec32(sp[p++]);
			print("]\n");
		}
		print("PS");
		print("  = ");
		print_u32(sp[p++]);
		print_char('\n');
		print("IP");
		print("  = ");
		i = sp[p++];
		print_u32(i);
		print(" [-1..]: ");
		for (j = 0; j < 8; j += 2) {
			print_char(' ');
			print_byte(((uint8_t *)i)[j - 2]);
			print_byte(((uint8_t *)i)[j - 1]);
		}
		print_char('\n');
		for (i = 0; i < 8; ++i) {
			print("sp[");
			print_dec32(p);
			print("] = ");
			for (j = 0; j < 4; ++j) {
				print_u32(sp[p++]);
				print_char(' ');
			}
			print_char('\n');
		}
	}

	hex_dump(stack, 256);

	for (;;) {
	}
}

#define PANIC(str)                                              \
	do {                                                    \
		asm volatile(                                   \
			"\tpushn\t%%r15\n"                      \
			"\txld.w\t%%r6, %0\n"                   \
			"\tld.w\t%%r7, %%sp\n"                  \
			"\txcall\tpanic\n"                      \
			:                                       \
			: "i"((str))                            \
			);                                      \
	} while (0)

static void undef_irq_handler(void)
{
	PANIC("Undefined interrupt");
}

static void nmi_handler(void)
{
	PANIC("Non-maskable interrupt");
}

static void illegal_irq_handler(void)
{
	PANIC("Illegal interrupt");
}

static void illegal_instruction(void)
{
	PANIC("Illegal instruction");
}

static void unaligned_data_access(void)
{
	PANIC("Unaligned data access");
}

static void serial0_err_irq(void)
{
	CLEAR_IRQ(REG_INT_FSIF01, 1 << 0);
}

static void serial0_in_irq(void)
{
	serial_filled_0();
	CLEAR_IRQ(REG_INT_FSIF01, 1 << 1);
}

static void serial0_out_irq(void)
{
	serial_drained_0();
}

static void serial1_err_irq(void)
{
	touchscreen_handler();
	CLEAR_IRQ(REG_INT_FSIF01, 1 << 3);
}

static void serial1_in_irq(void)
{
	touchscreen_handler();
	CLEAR_IRQ(REG_INT_FSIF01, 1 << 4);
}

static void serial1_out_irq(void)
{
	CLEAR_IRQ(REG_INT_FSIF01, 1 << 5);
}

static void kint_irq(void)
{
	gpio_irq();
	CLEAR_IRQ(REG_INT_FK01_FP03, 0x3f);
}


#define N_TRAPS 108
typedef void (*irq_callback)(void);

irq_callback trap_table[N_TRAPS] = {
	undef_irq_handler,	//   0 Reset
	undef_irq_handler,	//   1 *reserved*
	undef_irq_handler,	//   2 ext exception
	illegal_instruction,	//   3 Undefined instruction exception
	undef_irq_handler,	//   4 *reserved*
	undef_irq_handler,	//   5 *reserved*
	unaligned_data_access,	//   6 Address misaligned exception
	nmi_handler,		//   7 NMI
	undef_irq_handler,	//   8 *reserved*
	undef_irq_handler,	//   9 *reserved*
	undef_irq_handler,	//  10 *reserved*
	illegal_irq_handler,	//  11 Illegal interrupt exception
	undef_irq_handler,	//  12 Software exception 0
	undef_irq_handler,	//  13 Software exception 1
	undef_irq_handler,	//  14 Software exception 2
	undef_irq_handler,	//  15 Software exception 3
	undef_irq_handler,	//  16 Port input interrupt 0
	undef_irq_handler,	//  17 Port input interrupt 1
	undef_irq_handler,	//  18 Port input interrupt 2
	undef_irq_handler,	//  19 Port input interrupt 3
	kint_irq,		//  20 Key input interrupt 0
	kint_irq,		//  21 Key input interrupt 1
	undef_irq_handler,	//  22 High-speed DMA Ch.0
	undef_irq_handler,	//  23 High-speed DMA Ch.1
	undef_irq_handler,	//  24 High-speed DMA Ch.2
	undef_irq_handler,	//  25 High-speed DMA Ch.3
	undef_irq_handler,	//  26 Intelligent DMA
	undef_irq_handler,	//  27 *reserved*
	undef_irq_handler,	//  28 *reserved*
	undef_irq_handler,	//  29 *reserved*
	undef_irq_handler,	//  30 16-bit timer 0 - compare-match B
	undef_irq_handler,	//  31 16-bit timer 0 - compare-match A
	undef_irq_handler,	//  32 *reserved*
	undef_irq_handler,	//  33 *reserved*
	undef_irq_handler,	//  34 16-bit timer 1 - compare-match B
	undef_irq_handler,	//  35 16-bit timer 1 - compare-match A
	undef_irq_handler,	//  36 *reserved*
	undef_irq_handler,	//  37 *reserved*
	undef_irq_handler,	//  38 16-bit timer 2 - compare-match B
	undef_irq_handler,	//  39 16-bit timer 2 - compare-match A
	undef_irq_handler,	//  40 *reserved*
	undef_irq_handler,	//  41 *reserved*
	undef_irq_handler,	//  42 16-bit timer 3 - compare-match B
	undef_irq_handler,	//  43 16-bit timer 3 - compare-match A
	undef_irq_handler,	//  44 *reserved*
	undef_irq_handler,	//  45 *reserved*
	undef_irq_handler,	//  46 16-bit timer 4 - compare-match B
	undef_irq_handler,	//  47 16-bit timer 4 - compare-match A
	undef_irq_handler,	//  48 *reserved*
	undef_irq_handler,	//  49 *reserved*
	undef_irq_handler,	//  50 16-bit timer 5 - compare-match B
	undef_irq_handler,	//  51 16-bit timer 5 - compare-match A
	undef_irq_handler,	//  52 *reserved*
	undef_irq_handler,	//  53 *reserved*
	undef_irq_handler,	//  54 *reserved*
	undef_irq_handler,	//  55 *reserved*
	serial0_err_irq,	//  56 Serial interface Ch.0 - Receive error
	serial0_in_irq,		//  57 Serial interface Ch.0 - Receive buffer full
	serial0_out_irq,	//  58 Serial interface Ch.0 - Transmit buffer empty
	undef_irq_handler,	//  59 *reserved*
	serial1_err_irq,	//  60 Serial interface Ch.1 - Receive error
	serial1_in_irq,		//  61 Serial interface Ch.1 - Receive buffer full
	serial1_out_irq,	//  62 Serial interface Ch.1 - Transmit buffer empty
	undef_irq_handler,	//  63 A/D converter - Result out of range
	undef_irq_handler,	//  64 A/D converter - End of conversion
	undef_irq_handler,	//  65 RTC
	undef_irq_handler,	//  66 *reserved*
	undef_irq_handler,	//  67 *reserved*
	undef_irq_handler,	//  68 Port input interrupt 4
	undef_irq_handler,	//  69 Port input interrupt 5
	undef_irq_handler,	//  70 Port input interrupt 6
	undef_irq_handler,	//  71 Port input interrupt 7
	undef_irq_handler,	//  72 *reserved*
	undef_irq_handler,	//  73 LCDC
	undef_irq_handler,	//  74 *reserved*
	undef_irq_handler,	//  75 *reserved*
	undef_irq_handler,	//  76 Serial interface Ch.2 - Receive error
	undef_irq_handler,	//  77 Serial interface Ch.2 - Receive buffer full
	undef_irq_handler,	//  78 Serial interface Ch.2 - Transmit buffer empty
	undef_irq_handler,	//  79 *reserved*
	undef_irq_handler,	//  80 *reserved*
	undef_irq_handler,	//  81 SPI - Receive DMA request
	undef_irq_handler,	//  82 SPI - Transmit DMA request
	undef_irq_handler,	//  83 *reserved*
	undef_irq_handler,	//  84 Port input interrupt 8  / SPI       - SPI interrupt
	undef_irq_handler,	//  85 Port input interrupt 9  / USB PDREQ - USB DMA request
	undef_irq_handler,	//  86 Port input interrupt 10 / USB       - USBinterrupt
	undef_irq_handler,	//  87 Port input interrupt 11 / DCSIO     - DCSIO interrupt
	undef_irq_handler,	//  88 Port input interrupt 12
	undef_irq_handler,	//  89 Port input interrupt 13
	undef_irq_handler,	//  90 Port input interrupt 14
	undef_irq_handler,	//  91 Port input interrupt 15
	undef_irq_handler,	//  92 *reserved*
	undef_irq_handler,	//  93 *reserved*
	undef_irq_handler,	//  94 I2S interface - I2S FIFO empty
	undef_irq_handler,	//  95 *reserved*
	undef_irq_handler,	//  96 *reserved*
	undef_irq_handler,	//  97 *reserved*
	undef_irq_handler,	//  98 *reserved*
	undef_irq_handler,	//  99 *reserved*
	undef_irq_handler,	// 100 *reserved*
	undef_irq_handler,	// 101 *reserved*
	undef_irq_handler,	// 102 *reserved*
	undef_irq_handler,	// 103 *reserved*
	undef_irq_handler,	// 104 *reserved*
	undef_irq_handler,	// 105 *reserved*
	undef_irq_handler,	// 106 *reserved*
	undef_irq_handler,	// 107 *reserved*
};

void traps_init(void)
{
	DISABLE_IRQ();

	/* WAKEUP=1 */
	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_OPT |= 0x1;
	REG_CMU_PROTECT = CMU_PROTECT_ON;

	/* relocate the trap table */
	asm("ld.w %%ttbr, %0" :: "r"(0x84000));
	ENABLE_IRQ();
}
