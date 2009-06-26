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
#include "samo.h"
#include "irq.h"
#include <diskio.h>
#include "suspend.h"


void suspend(void) __attribute__ ((section (".suspend_text")));
void suspend(void)
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

	DISABLE_IRQ();

	// no more function calls after this point
	// all code must be in-line

	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_OPT |= WAKEUPWT;
	REG_CMU_PROTECT = CMU_PROTECT_ON;

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
		MISC_HCKE |

		IVRAMARB_CKE |
		//TM5_CKE |
		//TM4_CKE |
		//TM3_CKE |
		//TM2_CKE |
		//TM1_CKE |
		//TM0_CKE |
		//EGPIO_MISC_CK |
		//I2S_CKE |
		//DCSIO_CKE |
		//WDT_CKE |
		//GPIO_CKE |
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

	// adjust baud rate for lower clock frequency
	SET_BRTRD(1, CALC_BAUD(MCLK / 32, DIV, CTP_BPS));
	SET_BRTRD(0, CALC_BAUD(MCLK / 32, DIV, CONSOLE_BPS));

	// end of suspend, wait for interrupt
	asm volatile ("halt");
	// interrupt is on hold until end of resume

	// restore baud rate
	SET_BRTRD(0, CALC_BAUD(MCLK, DIV, CONSOLE_BPS));
	SET_BRTRD(1, CALC_BAUD(MCLK, DIV, CTP_BPS));

	REG_CMU_PROTECT = CMU_PROTECT_OFF;

	REG_CMU_CLKCNTL =
		//CMU_CLK_SEL_OSC3_DIV_32 |
		//CMU_CLK_SEL_OSC3_DIV_16 |
		//CMU_CLK_SEL_OSC3_DIV_8 |
		//CMU_CLK_SEL_OSC3_DIV_4 |
		//CMU_CLK_SEL_OSC3_DIV_2 |
		//CMU_CLK_SEL_OSC3_DIV_1 |
		//CMU_CLK_SEL_LCDC_CLK |
		//CMU_CLK_SEL_MCLK |
		//CMU_CLK_SEL_PLL |
		//CMU_CLK_SEL_OSC1 |
		CMU_CLK_SEL_OSC3 |

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
		//LCDCDIV_12 |
		//LCDCDIV_11 |
		//LCDCDIV_10 |
		//LCDCDIV_9 |
		LCDCDIV_8 |
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
		//TM5_CKE |
		//TM4_CKE |
		//TM3_CKE |
		//TM2_CKE |
		//TM1_CKE |
		//TM0_CKE |
		EGPIO_MISC_CK |
		//I2S_CKE |
		DCSIO_CKE |
		WDT_CKE |
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

	ENABLE_IRQ();
	// it is now possible to call other functions
	// as SDRAM is operational again

	// resume SD card
	if (card_state) {
		disk_initialize(0);
	}
}
