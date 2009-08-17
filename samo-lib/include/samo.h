#if !defined(_SAMO_H)
#define _SAMO_H 1

#include <config.h>
#include <regs.h>

// available range 10 .. 715 seconds (10 sec .. 11 min 55 sec)
#define SUSPEND_AUTO_POWER_OFF_SECONDS 120

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#if !defined(NULL)
#define NULL 0
#endif

#if !defined(SAMO_RESTRICTIONS)
#define SAMO_RESTRICTIONS 0
#endif

//#define DEBUGLED1_ON()  do { REG_P1_P1D &= ~(1 << 4); } while (0)
//#define DEBUGLED1_OFF() do { REG_P1_P1D |=  (1 << 4); } while (0)

//#define DEBUGLED2_ON()  do { REG_P1_P1D &= ~(1 << 3); } while (0)
//#define DEBUGLED2_OFF() do { REG_P1_P1D |=  (1 << 3); } while (0)

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


//#define CALC_BAUD(fbrclk, divmd, bps)  ((fbrclk / divmd)/(2 * bps) - 1)
#define CALC_BAUD(fbrclk, div, divmd, bps)  (((((10 * fbrclk) / div / divmd)/(2 * bps)) + 5) / 10 - 1)

#define SET_BRTRD(ch, value) do {                       \
		REG_EFSIF##ch##_BRTRUN = 0;             \
		REG_EFSIF##ch##_BRTRDM = value >> 8;    \
		REG_EFSIF##ch##_BRTRDL = value & 0xff;  \
		REG_EFSIF##ch##_BRTRUN = BRTRUNx;       \
	} while(0)

#define SERIAL_8N1 (NO_PARx | ONE_STPBx | INT_CLKx | EIGHT_BIT_ASYNx)


// include the board specific macros and functions

#if BOARD_S1C33E07 || SIMULATOR
#include <boards/s1c33e07.h>
#elif BOARD_PROTO1
#include <boards/proto1.h>
#elif BOARD_PROTO2
#include <boards/proto2.h>
#elif BOARD_SAMO_A1 || BOARD_SAMO_A3 || BOARD_SAMO_A5
#define BOARD_SAMO_Ax 1
#include <boards/samo_a1.h>
#elif BOARD_PRT33L17LCD
#include <boards/prt33l17lcd.h>
#else
#error "unsupported board type - see config.h"
#endif


#if !defined(CONSOLE_BPS)
#define CONSOLE_BPS 57600
#endif

#if !defined(CTP_BPS)
#define CTP_BPS     38400
#endif


// common functions

static inline void init_rs232_ch0(void)
{
	/* serial line 0: 8-bit async, no parity, internal clock, 1 stop bit */
	REG_EFSIF0_CTL = TXENx | RXENx | SERIAL_8N1;

	/* DIVMD = 1/8, General I/F mode */
	REG_EFSIF0_IRDA = DIVMD_8x | IRMD_GEN_IFx;

	/* set up baud rate timer reload data */
	SET_BRTRD(0, CALC_BAUD(MCLK, 1, DIV, CONSOLE_BPS));

	/* clear interrupt flags */
	REG_INT_FSIF01 = FSRX0 | FSTX0 | FSERR0;
}

static inline void init_rs232_ch1(void)
{
	REG_EFSIF1_CTL = RXENx | SERIAL_8N1;

	REG_EFSIF1_IRDA = DIVMD_8x | IRMD_GEN_IFx;

	SET_BRTRD(1, CALC_BAUD(MCLK, 1, DIV, CTP_BPS));

	REG_INT_FSIF01 = FSRX1 | FSTX1 | FSERR1;
}

#endif
