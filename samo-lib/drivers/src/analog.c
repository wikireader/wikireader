/*
 * analog - driver for analog inputs
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

#include <stdio.h>
#include <stdbool.h>
#include <regs.h>
#include <samo.h>

#include "analog.h"


// Battery

#define ADC_FULL_SCALE	1024
#define VADC_DIVISOR     128

#define VADC_MULTIPLIER (VADC_DIVISOR * (ADC_SERIES_RESISTOR_K + ADC_SHUNT_RESISTOR_K) / ADC_SHUNT_RESISTOR_K)


// Thermistor

#define THERMISTOR_K0     766386
#define THERMISTOR_K1      -1129
#define THERMISTOR_DIVISOR 10000


// Contrast voltage

#define CONTRAST_MULTIPLIER 45826
#define CONTRAST_DIVISOR     1000


static int adc[3];


static void StartADC(void)
{
	// configure adc
	REG_AD_CLKCTL = 0x000f;
	REG_AD_ADVMODE = 0x0100;
	REG_AD_TRIG_CHNL = 0x1000;
	REG_AD_EN_SMPL_STAT = 0x0304;

	// enable pins adc0..adc2
	REG_P7_03_CFP = 0x15;

	// start the adc
	REG_AD_EN_SMPL_STAT |=  0x02;

	// wait for conversion complete
	while (0 == (REG_AD_END & 0x04)) {
		asm volatile ("nop");
	}

}

static void StopADC(void)
{
	// turn adc off
	REG_P7_03_CFP  = 0;
	REG_AD_EN_SMPL_STAT= 0;
	REG_AD_CLKCTL = 0;
}

void Analog_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		StartADC();
		adc[0] = REG_AD_CH0_BUF;
		adc[1] = REG_AD_CH1_BUF;
		adc[2] = REG_AD_CH2_BUF;
		StopADC();
	}
	int i;
	for (i = 0; i < 12; ++i) {
		Analog_scan();
	}
}

void Analog_scan(void)
{
	StartADC();
	adc[0] += (REG_AD_CH0_BUF - adc[0] + 4) / 8;
	adc[1] += (REG_AD_CH1_BUF - adc[1] + 4) / 8;
	adc[2] += (REG_AD_CH2_BUF - adc[2] + 4) / 8;
	StopADC();
}

int Analog_BatteryMilliVolts(void)
{
	return (adc[0] * (VADC_MULTIPLIER * AVDD_MILLIVOLTS)) / (ADC_FULL_SCALE * VADC_DIVISOR);
}

int Analog_TemperatureCelcius(void)
{
	return (adc[1] * THERMISTOR_K1 + THERMISTOR_K0) / THERMISTOR_DIVISOR;
}

int Analog_ContrastMilliVolts(void)
{
	return (adc[2] * CONTRAST_MULTIPLIER) / CONTRAST_DIVISOR;
}
