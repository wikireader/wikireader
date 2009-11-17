/*
 * suspend - put the CPU into low power mode, retain LCD controller
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Christopher Hall <hsw@openmoko.com>
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

#include <string.h>

#include <regs.h>
#include <samo.h>

#include "file.h"
#include "interrupt.h"
#include "syscall.h"
#include "system.h"
#include "vector.h"
#include "watchdog.h"
#include "suspend.h"


// The / 32 is because the suspend routine operates with MCLK=CLK/32
// other factor is timer prescaler value
#define SUSPEND_TIMEOUT_VALUE ((MCLK / 32 / 4096) * SUSPEND_AUTO_POWER_OFF_SECONDS)
#if SUSPEND_TIMEOUT_VALUE > 0xffff
#error "SUSPEND_AUTO_POWER_OFF_SECONDS is too large"
#endif


int SuspendCode(void) __attribute__ ((section (".suspend_text")));


void Suspend_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		System_initialise();
		Vector_initialise();
		File_initialise();
		Watchdog_initialise();

		// copy the suspend code to A0RAM
		uint8_t *DestinationAddress;
		const uint8_t *SourceAddress;
		size_t ByteCount;

		asm volatile ("xld.w\t%[d], __START_SuspendCode  \n\t"
			      "xld.w\t%[s], __START_suspend      \n\t"
			      "xld.w\t%[c], __SIZE_suspend"
			      : [d] "=r" (DestinationAddress),
				[s] "=r" (SourceAddress),
				[c] "=r" (ByteCount));
		memcpy(DestinationAddress, SourceAddress, ByteCount);
		Vector_set(VECTOR_Software_exception_0, (void *)DestinationAddress);

	}
}


// this callback is for user code doo not use in kernel
void Suspend(Standard_BoolCallBackType *callback, void *arg)
{
	Watchdog_KeepAlive(WATCHDOG_KEY);

	// if in CTP receive sequence
	//REG_MISC_PUP6 |= 0x10;
	if (0 == (REG_P6_P6D & 0x10)) {
		return;
	}

	// cannot suspend if serial port is running
	if (0 != (REG_EFSIF0_STATUS & TENDx)) {
		return;
	}

	File_PowerDown();

	Interrupt_type state = Interrupt_disable();

	Watchdog_KeepAlive(WATCHDOG_KEY);

	register bool timeout_flag;
	asm volatile ("xcall\t__START_SuspendCode\n\t"
		      "ld.w\t%[result], %%r4       "
		      : [result] "=r" (timeout_flag));

	Watchdog_KeepAlive(WATCHDOG_KEY);

	Interrupt_enable(state);

	if (timeout_flag) {
		register bool continue_flag = false;

		if (NULL != callback) {
			continue_flag = SystemCall_BoolUserCallback(callback, arg);
		}

		if (!continue_flag) {
			Watchdog_KeepAlive(WATCHDOG_KEY);
			File_CloseAll();
			System_PowerOff();
		}
	}
}


// This must not use the stack (apart from entry/exit)
// Check the .asm33 file for any use of %sp
// (to make sure all local variables are really in registers)
// Also this needs to be position independant code
// since it is copied to A0RAM by initialise above.
int SuspendCode(void)
{
	// no function calls allowed (i.e. call/xcall/int assembler op-codes)
	// no non-register local variables allowed (.e. [%sp+n] operands)
	// no access to static or global variables allowed (i.e. .bss/.data)
	// no access to global constants or strings allowed (i.e. .rodata/.text)
	// all code must be in-lined (must be in .suspend_text)
	// access to A0RAM and device registers is allowed

	// SDRAM to self-refresh mode (disables clock)
	REG_SDRAMC_REF =
		//SELDO |
		//SCKON |
		SELEN |
		(0x7f << SELCO_SHIFT) |
		(0 << AURCO_SHIFT) |
		0;

	BUSY_WAIT_FOR(REG_SDRAMC_REF & SELDO);

	// SDRAMC controller off
	REG_SDRAMC_APP &= ~(
		ARBON |
		//DBF |
		//INCR |
		//CAS1 |
		//CAS0 |
		APPON |
		//IQB |
		0);

	// release the SDRAMC pin functions
	REG_P2_P2D = ~0;
	REG_P2_03_CFP = 0x01;
	REG_P2_47_CFP = 0x00;

	// adjust baud rate for lower clock frequency
	SET_BRTRD(1, CALC_BAUD(MCLK, 32, DIV, CTP_BPS));
	SET_BRTRD(0, CALC_BAUD(MCLK, 32, DIV, CONSOLE_BPS));

	// preserve current clock enable state
	register uint32_t save_g0 = REG_CMU_GATEDCLK0;
	register uint32_t save_g1 = REG_CMU_GATEDCLK1;
	register uint32_t save_clkcntl = REG_CMU_CLKCNTL;

	// turn off un necessary clocks
	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_GATEDCLK0 &= ~(
		USBSAPB_CKE |
		USB_CKE |
		SDAPCPU_HCKE |
		SDAPCPU_CKE |
		SDAPLCDC_CKE |
		SDSAPB_CKE |
		//DSTRAM_CKE |
		//LCDCAHBIF_CKE |
		//LCDCSAPB_CKE |
		//LCDC_CKE |
		0);
	REG_CMU_GATEDCLK1 =
		CPUAHB_HCKE |
		LCDCAHB_HCKE |
		GPIONSTP_HCKE |
		//SRAMC_HCKE |
		EFSIOBR_HCKE |
		//MISC_HCKE |

		IVRAMARB_CKE |
		//TM5_CKE |
		//TM4_CKE |
		//TM3_CKE |
		TM2_CKE |    // to use for suspend timeout
		TM1_CKE |
		//TM0_CKE |
		//EGPIO_MISC_CK |
		//I2S_CKE |
		//DCSIO_CKE |
		//WDT_CKE |
		GPIO_CKE |
		//SRAMSAPB_CKE |
		//SPI_CKE |
		EFSIOSAPB_CKE |
		//CARD_CKE |
		//ADC_CKE |
		ITC_CKE |
		//DMA_CKE |
		//RTCSAPB_CKE |
		0;

	REG_CMU_CLKCNTL =
		CMU_CLK_SEL_OSC3_DIV_32 |
		//CMU_CLK_SEL_OSC3_DIV_16 |
		//CMU_CLK_SEL_OSC3_DIV_8 |
		//CMU_CLK_SEL_OSC3_DIV_4 |
		//CMU_CLK_SEL_OSC3_DIV_2 |
		//CMU_CLK_SEL_OSC3_DIV_1 |
		//CMU_CLK_SEL_LCDC_CLK |
		//CMU_CLK_SEL_MCLK |
		//CMU_CLK_SEL_PLL |
		//CMU_CLK_SEL_OSC1 |
		//CMU_CLK_SEL_OSC3 |

		PLLINDIV_10 |
		//PLLINDIV_9 |
		//PLLINDIV_8 |
		//PLLINDIV_7 |
		//PLLINDIV_6 |
		//PLLINDIV_5 |
		//PLLINDIV_4 |
		//PLLINDIV_3 |
		//PLLINDIV_2 |
		//PLLINDIV_1 |

		//LCDCDIV_16 |
		//LCDCDIV_15 |
		//LCDCDIV_14 |
		//LCDCDIV_13 |
		LCDCDIV_12 |
		//LCDCDIV_11 |
		//LCDCDIV_10 |
		//LCDCDIV_9 |
		//LCDCDIV_8 |
		//LCDCDIV_7 |
		//LCDCDIV_6 |
		//LCDCDIV_5 |
		//LCDCDIV_4 |
		//LCDCDIV_3 |
		//LCDCDIV_2 |
		//LCDCDIV_1 |

		//MCLKDIV |

		OSC3DIV_32 |
		//OSC3DIV_16 |
		//OSC3DIV_8 |
		//OSC3DIV_4 |
		//OSC3DIV_2 |
		//OSC3DIV_1 |


		//OSCSEL_PLL |
		//OSCSEL_OSC3 |
		//OSCSEL_OSC1 |
		OSCSEL_OSC3 |

		SOSC3 |
		//SOSC1 |
		0;
	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// configure 16 bit timer2 to resume after a given timeout
	// Stop timer
	REG_T16_CTL2 =
		//INITOLx |
		//SELFMx |
		//SELCRBx |
		//OUTINVx |
		//CKSLx |
		//PTMx |
		PRESETx |
		//PRUNx |
		0;

	// Set prescale
	REG_T16_CLKCTL_2 =
		P16TONx |
		P16TSx_MCLK_DIV_4096 |
		//P16TSx_MCLK_DIV_1024 |
		//P16TSx_MCLK_DIV_256 |
		//P16TSx_MCLK_DIV_64 |
		//P16TSx_MCLK_DIV_16 |
		//P16TSx_MCLK_DIV_4 |
		//P16TSx_MCLK_DIV_2 |
		//P16TSx_MCLK_DIV_1 |
		0;

	// Set count
	REG_T16_CR2A = SUSPEND_TIMEOUT_VALUE;
	REG_T16_CR2B = SUSPEND_TIMEOUT_VALUE;

	// set priority, clear flag and enable interrupt
	REG_INT_P16T23 = (REG_INT_P16T23 & 0xf0) | 0x07;  // priority T2 = 7
	REG_INT_F16T23 |= (F16TC2 | F16TU2);              // 1 => reset flag bit
	REG_INT_E16T23 |= E16TU2;

	// start timer
	REG_T16_CTL2 |= PRUNx;

	// end of suspend, wait for interrupt
	asm volatile ("halt");
	// interrupt is on hold until end of resume

	// stop the timer
	REG_T16_CTL2 = 0;

	// restore clocks
	REG_CMU_PROTECT = CMU_PROTECT_OFF;

	REG_CMU_CLKCNTL = save_clkcntl;
	REG_CMU_GATEDCLK0 = save_g0;
	REG_CMU_GATEDCLK1 = save_g1;

	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// restore baud rate
	SET_BRTRD(0, CALC_BAUD(MCLK, 1, DIV, CONSOLE_BPS));
	SET_BRTRD(1, CALC_BAUD(MCLK, 1, DIV, CTP_BPS));

	// re-enable the SDRAMC pin functions
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;

	// enable RAM and self-refresh
	REG_SDRAMC_APP |=
		ARBON |
		//DBF |
		//INCR |
		//CAS1 |
		//CAS0 |
		APPON |
		IQB |
		0;
	REG_SDRAMC_REF =
		//SELDO |
		SCKON |
		SELEN |
		(0x7f << SELCO_SHIFT) |
		(0x8c << AURCO_SHIFT) |
		0;

	// determine if timeout
	register int timed_out = 0 != (REG_INT_F16T23 & (E16TC2 | F16TU2));

	// suppress timer2 interrupt
	REG_INT_E16T23 &= ~(E16TC2 | E16TU2);
	REG_INT_F16T23 |= (F16TC2 | F16TU2);

	return timed_out;
}
