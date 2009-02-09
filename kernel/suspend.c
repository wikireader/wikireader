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

#include "regs.h"
#include "suspend.h"

#define WAIT_FOR_CONDITION(cond)        \
        do { asm("nop"); } while(!(cond))

void system_suspend(void)
{
#if 0
        /* WAKEUP=1 */
	REG_CMU_PROTECT = 0x96;
	REG_CMU_GATEDCLK1 = 0xffffffff;
	REG_CMU_PROTECT = 0;
#endif
#if 0
	REG_CMU_OPT |= 0x1;
	/* send the SDRAM to its self-refresh mode (which disables the clock) */
	REG_SDRAMC_REF = (1 << 23) | (0x7f << 16);

	/* wait for the SELDO bit to be asserted */
	WAIT_FOR_CONDITION(REG_SDRAMC_REF & (1 << 25));

	/* switch off the SDRAMC application core */
	REG_SDRAMC_APP = 0;
	REG_CMU_GATEDCLK0 &= ~0x70;

	/* release the SDRAMC pin functions */
	REG_P2_03_CFP = 0x01;
	REG_P2_47_CFP = 0x00;
#endif

	REG_EFSIF0_BRTRDL = 12 & 0xff;
	REG_EFSIF0_BRTRDM = 12 >> 8;


#if 1
	//REG_CMU_GATEDCLK1 = (1 << 29) | (1 << 28) | (1 << 27) | (1 << 19) | (1 << 8);
	REG_CMU_GATEDCLK1 = 0x3f08002f;
	REG_CMU_GATEDCLK0 &= ~(1 << 1);

	/* disable clocks we don't need in HALT mode */
	//REG_CMU_CLKCNTL = (0x5 << 24) | (8 << 16) | (1 << 12) | (1 << 1);
	REG_CMU_CLKCNTL = 0x05970a02;
#endif
	
	asm("xld.w %r10, 0xdeadbeef");
	asm("halt");
REG_P1_P1D = 0;
	system_resume();
}

void system_resume(void)
{
	int i, system_halted;

	/* check whether we awoke from halt mode freshly */
	asm("ld.w %0, %%r10" : "=r"(system_halted));
	if (system_halted != 0xdeadbeef)
		return;

REG_P1_P1D = 0;

	REG_EFSIF0_BRTRDL = 51 & 0xff;
	REG_EFSIF0_BRTRDM = 51 >> 8;

	/* restore clock setup */
	REG_CMU_CLKCNTL = 0x00770003;

	for (i = 0; i < 10000; i++)
		asm("nop");

#if 0
	/* re-enable the SDRAMC pin functions */
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;
#endif

	/* re-enable all the clocks */
	REG_CMU_GATEDCLK1 = 0x3f0fffff;
	REG_CMU_GATEDCLK0 |= (1 << 1);

	asm("ld.w %r10, 0");
}

