/*
 * enter low power suspend state
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

#include <stdbool.h>

#include <regs.h>
#include <samo.h>
#include <interrupt.h>
#include <diskio.h>

#include "suspend.h"


void suspend(int WatchdogTimeout) __attribute__ ((section (".suspend_text")));
void suspend2(int WatchdogTimeout) __attribute__ ((section (".suspend_text")));


// if this fails to be in-line, the program will crash
// select PLL and adjust clock enables
static inline void restore_clocks(void)
{

	REG_CMU_PROTECT = CMU_PROTECT_OFF;
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

		//PLLINDIV_10 |
		//PLLINDIV_9 |
		PLLINDIV_8 |
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


		OSCSEL_PLL |
		//OSCSEL_OSC3 |
		//OSCSEL_OSC1 |
		//OSCSEL_OSC3 |

		SOSC3 |
		//SOSC1 |
		0;

	// enable various clocks
	REG_CMU_GATEDCLK0 =
		//USBSAPB_CKE |
		//USB_CKE |
		//SDAPCPU_HCKE |
		SDAPCPU_CKE |
		SDAPLCDC_CKE |
		SDSAPB_CKE |
		DSTRAM_CKE |
		LCDCAHBIF_CKE |
		LCDCSAPB_CKE |
		LCDC_CKE |
		0;
	REG_CMU_GATEDCLK1 =
		CPUAHB_HCKE |
		LCDCAHB_HCKE |
		GPIONSTP_HCKE |
		//SRAMC_HCKE |
		EFSIOBR_HCKE |
		MISC_HCKE |
		IVRAMARB_CKE |
#if !BOARD_SAMO_A3
		TM5_CKE |      // for tick.c
#endif
		//TM4_CKE |
		//TM3_CKE |
#if BOARD_SAMO_A3
		TM2_CKE |      // for tick.c
#endif
		TM1_CKE |      // for contrast.c
		TM0_CKE |      // for tick.c
		EGPIO_MISC_CK |
		//I2S_CKE |
		//DCSIO_CKE |
		//WDT_CKE |
		GPIO_CKE |
		//SRAMSAPB_CKE |
		SPI_CKE |
		EFSIOSAPB_CKE |
		//CARD_CKE |
		ADC_CKE |
		ITC_CKE |
		//DMA_CKE |
		//RTCSAPB_CKE |
		0;

	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// switch clocks
	asm volatile ("slp\n\tnop\n\tslp");

#if 1
	// restore baud rate (using PLL)
	SET_BRTRD(0, CALC_BAUD(PLL_CLK, 1, SERIAL_DIVMD, CONSOLE_BPS));
	SET_BRTRD(1, CALC_BAUD(PLL_CLK, 1, SERIAL_DIVMD, CTP_BPS));
#else
	// restore baud rate (Crystal Oscillator)
	SET_BRTRD(0, CALC_BAUD(MCLK, 1, SERIAL_DIVMD, CONSOLE_BPS));
	SET_BRTRD(1, CALC_BAUD(MCLK, 1, SERIAL_DIVMD, CTP_BPS));
#endif

}



// if this fails to be in-line, the program will crash
static inline void initialise_clocks(void)
{
	// disable watchdog
	REG_WD_WP = WD_WP_OFF;
	REG_WD_EN = 0;
	REG_WD_WP = WD_WP_ON;

	// restore clocks
	REG_CMU_PROTECT = CMU_PROTECT_OFF;

	// set up so "slp" instruction can be used to switch clocks
	REG_CMU_OPT =
		(0 << OSCTM_SHIFT) |
		//OSC3OFF |
		//TMHSP |
		//WAKEUPWT |
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

		//PLLINDIV_10 |
		//PLLINDIV_9 |
		PLLINDIV_8 |
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

		//OSC3DIV_32 |
		//OSC3DIV_16 |
		//OSC3DIV_8 |
		//OSC3DIV_4 |
		//OSC3DIV_2 |
		OSC3DIV_1 |


		//OSCSEL_PLL |
		//OSCSEL_OSC3 |
		//OSCSEL_OSC1 |
		OSCSEL_OSC3 |

		SOSC3 |
		//SOSC1 |
		0;

	// set up PLL for 48 MHz / 8 input -> 60 MHz output
	REG_CMU_PLL =

		PLLCS |
		PLLBYP |
		PLLCP |

		//PLLVC_360MHz_400MHz |
		//PLLVC_320MHz_360MHz |
		//PLLVC_280MHz_320MHz |
		//PLLVC_240MHz_280MHz |
		//PLLVC_200MHz_240MHz |
		//PLLVC_160MHz_200MHz |
		//PLLVC_120MHz_160MHz |
		PLLVC_100MHz_120MHz |

		PLLRS_5MHz_20MHz |
		//PLLRS_20MHz_150MHz |

		//PLLN_X16 |
		//PLLN_X15 |
		//PLLN_X14 |
		//PLLN_X13 |
		//PLLN_X12 |
		//PLLN_X11 |
		PLLN_X10 |
		//PLLN_X9 |
		//PLLN_X8 |
		//PLLN_X7 |
		//PLLN_X6 |
		//PLLN_X5 |
		//PLLN_X4 |
		//PLLN_X3 |
		//PLLN_X2 |
		//PLLN_X1 |

		//PLLV_DIV_8 |
		//PLLV_DIV_4 |
		PLLV_DIV_2 |

		PLLPOWR |
		0;

	// disable spread spectrum
	REG_CMU_SSCG = 0;

	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// a minimum 200us delay required
	// for the PLL to stabilise
	asm volatile (
		"\txld.w\t%%r4, 6*250\n"
		"1:\n"
		"\tnop\n"
		"\tnop\n"
		"\tsub\t%%r4, 1\n"
		"\tjrne\t1b"
		: : : "r4");

	// set clock configuration
	restore_clocks();

#if 0
	// for debugging
	// enable CMU_CLK on P52
	REG_P5_03_CFP &= 0xcf;
	REG_P5_03_CFP |= 0x30; // enable clock output function
	// dynamic stop
	asm volatile (
		"9:\tjp\t9b"
		);
#endif
}


void Suspend_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		initialise_clocks();
		restore_clocks();
	}
}


void suspend(int WatchdogTimeout)
{
	// if in CTP receive sequence
	//REG_MISC_PUP6 |= 0x10;
	if (0 == (REG_P6_P6D & 0x10)) {
		return;
	}

	register int card_state = check_card_power();
	// cannot suspend if serial port is running
	if (0 != (REG_EFSIF0_STATUS & TENDx)) {
		return;
	}

	disable_card_power();
	SDCARD_CS_HI();
	EEPROM_CS_HI();

	InterruptType s = Interrupt_disable();

	suspend2(WatchdogTimeout);

	Interrupt_enable(s);
	// it is now possible to call other functions
	// as SDRAM is operational again

	// resume SD card
	if (card_state) {
		disk_initialize(0);
	}
}

void suspend2(int WatchdogTimeout)
{
	// no more function calls after this point
	// all code must be in-line

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

	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_GATEDCLK0 &= ~(
		//USBSAPB_CKE |
		//USB_CKE |
		SDAPCPU_HCKE |
		SDAPCPU_CKE |
		SDAPLCDC_CKE |
		SDSAPB_CKE |
		//DSTRAM_CKE |
		//LCDCAHBIF_CKE |
		//LCDCSAPB_CKE |
		//LCDC_CKE |
		0);
	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// release the SDRAMC pin functions
	REG_P2_P2D = ~0;
	REG_P2_03_CFP = 0x01;
	REG_P2_47_CFP = 0x00;

	// adjust baud rate for lower clock frequency
	SET_BRTRD(1, CALC_BAUD(MCLK, 32, SERIAL_DIVMD, CTP_BPS));
	SET_BRTRD(0, CALC_BAUD(MCLK, 32, SERIAL_DIVMD, CONSOLE_BPS));

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
		//TM2_CKE |
		TM1_CKE |
		//TM0_CKE |
		//EGPIO_MISC_CK |
		//I2S_CKE |
		//DCSIO_CKE |
		WDT_CKE |
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

		//PLLINDIV_10 |
		//PLLINDIV_9 |
		PLLINDIV_8 |
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

	// switch clocks
	asm volatile ("slp\n\tnop\n\tslp");

	// turn off PLL to save power
	// unfortunately switch back on is too long and disrupts CTP serial port
	// so cannot do this
	//REG_CMU_PLL &= ~PLLPOWR;


	if (0 < WatchdogTimeout) {
		// enable watchdog
		REG_WD_WP = WD_WP_OFF;
		REG_WD_COMP = WatchdogTimeout;
		REG_WD_CNTL = WDRESEN;
		REG_WD_EN =
			//CLKSEL |
			//CLKEN  |
			RUNSTP |
			NMIEN  |     // so that low pulse is output
			RESEN  |     // reset takes priority
			0;
		REG_WD_WP = WD_WP_ON;

		REG_P6_03_CFP |= 0xc0; // select P63 as #WDT_NMI
	}

	// end of suspend, wait for interrupt
	asm volatile ("halt");
	// interrupt is on hold until end of resume

	// disable watchdog
	REG_WD_WP = WD_WP_OFF;
	REG_WD_CNTL = WDRESEN;
	REG_WD_EN = 0;
	REG_WD_WP = WD_WP_ON;
	REG_P6_03_CFP &= ~0xc0; // select P63 as GPIO

	// clocks back to normal
	restore_clocks();

	// re-enable the SDRAMC pin functions
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;

	// enable SDRAM
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
}
