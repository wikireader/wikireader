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
	/* enable write access to clock control registers */
	REG_CMU_PROTECT = 0x96;

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

	//REG_CMU_GATEDCLK1 = (1 << 29) | (1 << 28) | (1 << 27) | (1 << 19) | (1 << 8);
	REG_CMU_GATEDCLK1 = 0x3f08002f;
	REG_CMU_GATEDCLK0 &= ~(1 << 1);

	/* disable clocks we don't need in HALT mode */
	REG_CMU_CLKCNTL = (0xa << 24) | (8 << 16) | (1 << 12) | (1 << 1);

	/* write protect CMU registers */
	REG_CMU_PROTECT = 0;	
		
	asm("halt");

	/* resume procedure */
	REG_CMU_PROTECT = 0x96;

	/* restore clock setup */
	REG_CMU_CLKCNTL = 0x00770002;

	/* re-enable the SDRAMC pin functions */
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;

	/* re-enable all the clocks */
	REG_CMU_GATEDCLK1 = 0x3f0fffff;
	REG_CMU_GATEDCLK0 |= (1 << 1);
	REG_CMU_PROTECT = 0;
}

