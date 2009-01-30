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
}

void system_resume(void)
{
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

