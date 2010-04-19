/*
 * control of the MCU clock configuration
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

#include <regs.h>

#include "interrupt.h"
#include "CMU.h"


void CMU_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Interrupt_type state = Interrupt_disable();

		REG_CMU_PROTECT = CMU_PROTECT_OFF;

		// set up so "slp" instruction can be used to switch clocks
		REG_CMU_OPT =
			(0 << OSCTM_SHIFT) |
			//OSC3OFF |
			TMHSP |
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
			//TM5_CKE |      // for tick.c
			//TM4_CKE |
			//TM3_CKE |
			//TM2_CKE |
			TM1_CKE |        // for contrast.c
			//TM0_CKE |      // for tick.c
			EGPIO_MISC_CK |
			//I2S_CKE |
			//DCSIO_CKE |
			//WDT_CKE |
			GPIO_CKE |
			//SRAMSAPB_CKE |
			SPI_CKE |        // turn off in suspend?
			EFSIOSAPB_CKE |
			//CARD_CKE |
			ADC_CKE |        // turn off initially? (in suspend?)
			ITC_CKE |
			//DMA_CKE |
			//RTCSAPB_CKE |
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

		// block further CMU access
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

		// allow access to CMU
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

		// prevent access to CMU
		REG_CMU_PROTECT = CMU_PROTECT_ON;

		// switch clocks
		asm volatile ("slp\n\tnop\n\tslp");

		// restore interrupts
		Interrupt_enable(state);
	}
}


void CMU_enable1(uint32_t mask)
{
	Interrupt_type state = Interrupt_disable();
	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_GATEDCLK1 |= mask;
	REG_CMU_PROTECT = CMU_PROTECT_ON;
	Interrupt_enable(state);
}
