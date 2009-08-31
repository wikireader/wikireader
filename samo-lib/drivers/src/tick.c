/*
 * continuously incrementing 32 bit timer
 * Copyright (c) 2009 Openmoko
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

#include "interrupt.h"
#include "tick.h"


void Tick_initialise()
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		InterruptType s = Interrupt_disable();
		// enable clocks - for all timers that will be used
		// even if internal clocking is not used.
		// modify drivers/src/suspend.c to match this.
		REG_CMU_PROTECT = CMU_PROTECT_OFF;
		REG_CMU_GATEDCLK1 |=
			//CPUAHB_HCKE |
			//LCDCAHB_HCKE |
			//GPIONSTP_HCKE |
			//SRAMC_HCKE |
			//EFSIOBR_HCKE |
			//MISC_HCKE |
			//IVRAMARB_CKE |
#if !BOARD_SAMO_A3
			TM5_CKE |
#endif
			//TM4_CKE |
			//TM3_CKE |
#if BOARD_SAMO_A3
			TM2_CKE |
#endif
			//TM1_CKE |
			TM0_CKE |
			//EGPIO_MISC_CK |
			//I2S_CKE |
			//DCSIO_CKE |
			//WDT_CKE |
			//GPIO_CKE |
			//SRAMSAPB_CKE |
			//SPI_CKE |
			//EFSIOSAPB_CKE |
			//CARD_CKE |
			//ADC_CKE |
			//ITC_CKE |
			//DMA_CKE |
			//RTCSAPB_CKE |
			0;
		REG_CMU_PROTECT = CMU_PROTECT_ON;

#if BOARD_SAMO_A3
		// enable EXCL2
		REG_P6_47_CFP = (REG_P6_47_CFP & ~0x03) | 0x02;
#else
		// enable EXCL5
		REG_P7_4_CFP = (REG_P7_4_CFP & ~0x03) | 0x02;
#endif

		// enable TM0
		REG_P1_03_CFP = (REG_P1_03_CFP & ~0x03) | 0x01;

		// Advanced Mode
		REG_T16_ADVMODE = T16ADV;

		// 16 bit Timer 0
		// Stop timer
		REG_T16_CTL0 =
			//INITOLx |
			//SELFMx |
			//SELCRBx |
			//OUTINVx |
			//CKSLx |
			PTMx |
			//PRESETx |
			//PRUNx |
			0;

		// Set prescale
		REG_T16_CLKCTL_0 =
			P16TONx |
			//P16TSx_MCLK_DIV_4096 |
			//P16TSx_MCLK_DIV_1024 |
			//P16TSx_MCLK_DIV_256 |
			//P16TSx_MCLK_DIV_64 |
			//P16TSx_MCLK_DIV_16 |
			//P16TSx_MCLK_DIV_4 |
			//P16TSx_MCLK_DIV_2 |
			P16TSx_MCLK_DIV_1 |
			0;

		// Set count
		REG_T16_CR0A = 0;
		REG_T16_CR0B = 65535;

		// Reset
		REG_T16_CTL0 |= PRESETx;


#if BOARD_SAMO_A3
		// 16 bit Timer 2
		// Stop timer
		REG_T16_CTL2 =
			//INITOLx |
			//SELFMx |
			//SELCRBx |
			//OUTINVx |
			CKSLx |
			//PTMx |
			//PRESETx |
			//PRUNx |
			0;

		// Set prescale
		REG_T16_CLKCTL_2 =
			P16TONx |
			//P16TSx_MCLK_DIV_4096 |
			//P16TSx_MCLK_DIV_1024 |
			//P16TSx_MCLK_DIV_256 |
			//P16TSx_MCLK_DIV_64 |
			//P16TSx_MCLK_DIV_16 |
			//P16TSx_MCLK_DIV_4 |
			//P16TSx_MCLK_DIV_2 |
			P16TSx_MCLK_DIV_1 |
			0;

		// Set count
		REG_T16_CR2A = 0;
		REG_T16_CR2B = 65535;

		// Reset
		REG_T16_CTL2 |= PRESETx;
#else
		// 16 bit Timer 5
		// Stop timer
		REG_T16_CTL5 =
			//INITOLx |
			//SELFMx |
			//SELCRBx |
			//OUTINVx |
			CKSLx |
			//PTMx |
			//PRESETx |
			//PRUNx |
			0;

		// Set prescale
		REG_T16_CLKCTL_5 =
			P16TONx |
			//P16TSx_MCLK_DIV_4096 |
			//P16TSx_MCLK_DIV_1024 |
			//P16TSx_MCLK_DIV_256 |
			//P16TSx_MCLK_DIV_64 |
			//P16TSx_MCLK_DIV_16 |
			//P16TSx_MCLK_DIV_4 |
			//P16TSx_MCLK_DIV_2 |
			P16TSx_MCLK_DIV_1 |
			0;

		// Set count
		REG_T16_CR5A = 0;
		REG_T16_CR5B = 65535;

		// Reset
		REG_T16_CTL5 |= PRESETx;
#endif

		// Set PAUSE On
		REG_T16_CNT_PAUSE =
#if !BOARD_SAMO_A3
			PAUSE5 |
#endif
			//PAUSE4 |
			//PAUSE3 |
#if BOARD_SAMO_A3
			PAUSE2 |
#endif
			//PAUSE1 |
			PAUSE0 |
			0;
		// Run
		REG_T16_CTL0 |= PRUNx;
#if BOARD_SAMO_A3
		REG_T16_CTL2 |= PRUNx;
#else
		REG_T16_CTL5 |= PRUNx;
#endif

		// Set PAUSE Off
		REG_T16_CNT_PAUSE =
			//PAUSE5 |
			//PAUSE4 |
			//PAUSE3 |
			//PAUSE2 |
			//PAUSE1 |
			//PAUSE0 |
			0;
		Interrupt_enable(s);
	}
}


unsigned long Tick_get(void)
{
	register unsigned long count;

	InterruptType s = Interrupt_disable();

	// Set PAUSE On
	REG_T16_CNT_PAUSE =
#if !BOARD_SAMO_A3
		PAUSE5 |
#endif
		//PAUSE4 |
		//PAUSE3 |
#if BOARD_SAMO_A3
		PAUSE2 |
#endif
		//PAUSE1 |
		PAUSE0 |
		0;

#if BOARD_SAMO_A3
	count = (REG_T16_TC2 << 16) | REG_T16_TC0;
#else
	count = (REG_T16_TC5 << 16) | REG_T16_TC0;
#endif

	// Set PAUSE Off
	REG_T16_CNT_PAUSE =
		//PAUSE5 |
		//PAUSE4 |
		//PAUSE3 |
		//PAUSE2 |
		//PAUSE1 |
		//PAUSE0 |
		0;

	Interrupt_enable(s);
	return count;
}
