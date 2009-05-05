#ifndef WIKIREADER_H
#define WIKIREADER_H

#include "config.h"


#define DEBUGLED1_ON()  do { REG_P1_P1D &= ~(1 << 4); } while (0)
#define DEBUGLED1_OFF() do { REG_P1_P1D |=  (1 << 4); } while (0)

#define DEBUGLED2_ON()  do { REG_P1_P1D &= ~(1 << 3); } while (0)
#define DEBUGLED2_OFF() do { REG_P1_P1D |=  (1 << 3); } while (0)

#define EEPROM_CS_LO()  do { REG_P5_P5D &= ~(1 << 2); } while (0)
#define EEPROM_CS_HI()  do { REG_P5_P5D |=  (1 << 2); } while (0)

#define LCD_CS_LO()     do { REG_P8_P8D &= ~(1 << 5); } while (0)
#define LCD_CS_HI()     do { REG_P8_P8D |=  (1 << 5); } while (0)

#define LCD_DISPLAY_ON()   do { REG_P3_P3D |= (1 << 0); } while (0)
#define LCD_DISPLAY_OFF()  do { REG_P3_P3D &= ~(1 << 0); } while (0)

// remove this later - this pin is used by LCD controller!
//#define TFT_CTL1_LO()   do { REG_P8_P8D &= ~(1 << 3); } while (0)
//#define TFT_CTL1_HI()   do { REG_P8_P8D |=  (1 << 3); } while (0)

// macro to busy wait
#define BUSY_WAIT_FOR(cond)                     \
	do {                                    \
		asm volatile ("\tnop\n");       \
	} while (0 == (cond))


#define MCLK 		48000000
#define MCLK_MHz	(MCLK / 1000000)
#define DIV 		8

#define CALC_BAUD(fbrclk, divmd, bps)  ((fbrclk / divmd)/(2 * bps) - 1)

#define SET_BRTRD(ch, value) do {                       \
		REG_EFSIF##ch##_BRTRDM = value >> 8;    \
		REG_EFSIF##ch##_BRTRDL = value & 0xff;  \
	} while(0)

#define SERIAL_8N1 (NO_PARx | ONE_STPBx | INT_CLKx | EIGHT_BIT_ASYNx)


// include the board specific macros and functions

#if BOARD_S1C33E07 || SIMULATOR
#include "boards/s1c33e07.h"
#elif BOARD_PROTO1
#include "boards/proto1.h"
#elif BOARD_PROTO2
#include "boards/proto2.h"
#elif BOARD_SAMO_A1
#include "boards/samo_a1.h"
#elif BOARD_PRT33L17LCD
#include "boards/prt33l17lcd.h"
#else
#error "unsupported board type - see common/config.h"
#endif


// common functions

static inline void init_rs232_ch0(void)
{
	/* serial line 0: 8-bit async, no parity, internal clock, 1 stop bit */
	REG_EFSIF0_CTL = TXENx | RXENx | SERIAL_8N1;

	/* DIVMD = 1/8, General I/F mode */
	REG_EFSIF0_IRDA = DIVMD_8x | IRMD_GEN_IFx;

	/* set up baud rate timer reload data */
	SET_BRTRD(0, CALC_BAUD(MCLK, DIV, 57600));

	/* baud rate timer: run! */
	REG_EFSIF0_BRTRUN |= BRTRUN_STARx;
}

static inline void init_rs232_ch1(void)
{
	REG_EFSIF1_CTL = RXENx | SERIAL_8N1;

	REG_EFSIF1_IRDA = DIVMD_8x | IRMD_GEN_IFx;

	SET_BRTRD(1, CALC_BAUD(MCLK, DIV, 38400));

	REG_EFSIF1_BRTRUN |= BRTRUN_STARx;
}

#if      1 //def INCLUDED_FROM_KERNEL
#if BOARD_PROTO2 || BOARD_SAMO_A1

#define ADC_FULL_SCALE	1024
#define VADC_DIVISOR 128

#define VADC_MULTIPLIER (VADC_DIVISOR * (ADC_SERIES_RESISTOR_K + ADC_SHUNT_RESISTOR_K) / ADC_SHUNT_RESISTOR_K)

/* returns the battery voltage, in mV */
static inline int get_battery_voltage(void)
{
	unsigned int val;

	/* switch on A/D converter clock and select MCLK/256 */
	REG_AD_CLKCTL = 0xf;

	/* A/D Trigger/Channel Select Register: channel 0,
	 * one-shot, software triggered */
	REG_AD_TRIG_CHNL = 0;

	/* select P70 pin function for AIN0 */
	REG_P7_03_CFP = 0x01;

	/* A/D Control/Status Register: start conversion (9 clock cycles) */
	REG_AD_EN_SMPL_STAT = 0x304;

	/* A/D Control/Status Register: trigger ADST */
	REG_AD_EN_SMPL_STAT |= (1 << 1);

	/* wait for the conversion to complete */
	do { asm volatile ("nop"); } while (!(REG_AD_EN_SMPL_STAT & (1 << 3)));

	/* read the value */
	val = REG_AD_ADD;

	/* select P70 pin function for P70 */
	REG_P7_03_CFP = 0;

	/* A/D Control/Status Register: disable controller */
	REG_AD_EN_SMPL_STAT = 0;

	/* switch off A/D converter clock */
	REG_AD_CLKCTL = 0;

	/* The circuit divides the battery voltage by
	 * ADC_SERIES_RESISTOR_K kOhm in series and
	 * ADC_SHUNT_RESISTOR_K kOhm to GND. Measurement is done
	 * rail-to-rail in respect of AVDD_MILLIVOLTS.	This formula
	 * converts the captured value to mV.
	 */
	return (val * (VADC_MULTIPLIER * AVDD_MILLIVOLTS)) / (ADC_FULL_SCALE * VADC_DIVISOR);
}
#else
static inline int get_battery_voltage(void)
{
	/* return some sane value for platforms with no hardware support
	 * for voltage measurement so the logic using this function will
	 * not bail. */
	return 3000;
}
#endif

#endif /* INCLUDED_FROM_KERNEL */

#endif /* WIKIREADER_H */

