/*
 * contrast pwm driver
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

#include <stdlib.h>
#include <stdbool.h>
#include <regs.h>

#include "contrast.h"

void Contrast_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
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
			//TM5_CKE |
			//TM4_CKE |
			//TM3_CKE |
			//TM2_CKE |
			TM1_CKE |
			//TM0_CKE |
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

		// enable TM1
		REG_P1_03_CFP = (REG_P1_03_CFP & ~0x0c) | 0x04;

		// Advanced Mode
		REG_T16_ADVMODE = T16ADV;


		// Set PAUSE On
		REG_T16_CNT_PAUSE =
			//PAUSE5 |
			//PAUSE4 |
			//PAUSE3 |
			//PAUSE2 |
			PAUSE1 |
			//PAUSE0 |
			0;

		// 16 bit Timer 1
		// Stop timer
		REG_T16_CTL1 =
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
		REG_T16_CLKCTL_1 =
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
		REG_T16_CR1A = Contrast_default;
		REG_T16_CR1B = Contrast_maximum;

		// Reset
		REG_T16_CTL1 |= PRESETx;

		// Run
		REG_T16_CTL1 |= PRUNx;

		// Set PAUSE Off
		REG_T16_CNT_PAUSE =
			//PAUSE5 |
			//PAUSE4 |
			//PAUSE3 |
			//PAUSE2 |
			//PAUSE1 |
			//PAUSE0 |
			0;
		initialised = true;
	} else {
		Contrast_set(Contrast_default);
	}
}

int Contrast_get(void)
{
	return REG_T16_CR1A;
}

void Contrast_set(int value)
{
	if (value > REG_T16_CR1B) {
		value = REG_T16_CR1B;
	} else if (0 > value) {
		value = 0;
	}
	REG_T16_CR1A = value;
}
