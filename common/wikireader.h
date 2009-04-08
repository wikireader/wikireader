#ifndef WIKIREADER_H
#define WIKIREADER_H

#include "config.h"

static inline void init_pins(void)
{
#if 0 // not enough space for adding this
	// A low on pin P63 shuts down the power supply - so try
	// to keep it high, without any glitched or we will power down
	// immediately.
	// This _must_ be the setting performed.
	// p60-63: wdt - ensure that P63(#WDTNMI) pin is set high
	REG_P6_P6D |= (1 << 3);		 // P63 = 1 (for safety)
	REG_P6_03_CFP &= 0x3f;		 // P63 = input
	REG_MISC_PUP6 |= (1 << 3);	 // P63 pullup = on
#endif
	/* P85: LCD_CS, P83: TFT_CTL1 */
	REG_P8_IOC8 = 0x28;
	REG_P8_03_CFP = 0x3f;
	REG_P8_45_CFP = 0x03;

	/* P65-67: SPI */
	REG_P6_47_CFP = 0x54;

	/* Serial interface */
	REG_P0_03_CFP = 0x05;

	/* LCD controller */
	REG_P8_03_CFP = 0x15;
	REG_P9_47_CFP = 0x55;

	/* board specific things */
#if BOARD_S1C33E07
	/* SDCARD CS# */
	REG_PA_IOC = (1 << 3);
	/* EEPROM WP# */
	REG_P1_IOC1 = (1 << 4);
	REG_P2_IOC2 = (1 << 6);
#elif BOARD_PROTO1 || BOARD_SAMO_A1
	/* P13 & P14: debug LEDs */
	REG_P1_IOC1 = 0x18;
	/* SDCARD power */
	REG_P3_IOC3 = 0x0f;
	/* SDCARD CS# */
	REG_P5_03_CFP = 0x01;

	REG_MISC_PUP6 = (1 << 5);

#elif BOARD_PRT33L17LCD
	/* SDCARD CS# */
	REG_P8_IOC8 = (1 << 3);
	/* EEPROM WP# */
	REG_P1_IOC1 = (1 << 4);
#else
#error "unsupported board type"
#endif

	/* P50 & P52: CS lines */
	REG_P5_IOC5 = 0x07;
}


static inline void init_rs232(void)
{
	/* serial line 0: 8-bit async, no parity, internal clock, 1 stop bit */
	REG_EFSIF0_CTL = 0xc3;

	/* DIVMD = 1/8, General I/F mode */
	REG_EFSIF0_IRDA = 0x10;

	/* by default MCLKDIV = 0 which means that the internal MCLK is OSC/1,
	 * where OSC = OSC3 as OSCSEL[1:0] = 00b
	 * Hence, MCLK is 48MHz */

	/* set up baud rate timer reload data */
	/*
	 * BRTRD = ((F[brclk] * DIVMD) / (2 * bps)) - 1;
	 * where
	 *	F[brclk] = 48MHz
	 *	DIVMD = 1/8
	 *	bps = 57600
	 *
	 *   = 51
	 */

	REG_EFSIF0_BRTRDL = 51 & 0xff;
	REG_EFSIF0_BRTRDM = 51 >> 8;

	/* baud rate timer: run! */
	REG_EFSIF0_BRTRUN = 0x01;
}

#if SDRAM_EM48AM1684VBD

// number of refresh cycles for initialisation
#define REFRESH_COUNT 8

// mode register settings

#define BURST_LENGTH_1 0x00
#define BURST_LENGTH_2 0x01
#define BURST_LENGTH_4 0x02
#define BURST_LENGTH_8 0x03

#define BURST_TYPE_NORMAL      (0x0 << 3)
#define BURST_TYPE_INTERLEAVE  (0x1 << 3)

#define CAS_LATENCY_2	       (0x2 << 4)
#define CAS_LATENCY_3	       (0x3 << 4)

#define OPERATION_NORMAL       (0x00 << 7)
#define OPERATION_SINGLE       (0x04 << 7)

#define MODE_WORD (0				\
		   | BURST_LENGTH_2		\
		   | BURST_TYPE_NORMAL		\
		   | CAS_LATENCY_2		\
		   | OPERATION_SINGLE		\
		)

// as SDRAM_A0 connects to CPU_A1 => shift of 1 bit
#define ADDRESS_SHIFT	1

// define the start if SDRAM ought to move to .lds file
// since that should be the definitive reference for the memory map
#define SDRAM_START ((volatile unsigned char *) 0x10000000)

#define SDRAM_FIRST_BYTE ((SDRAM_START)[0])

// put the above together to get the place to write the mode to the SDRAM
#define SDRAM_MODE_STORE (SDRAM_START[(MODE_WORD << ADDRESS_SHIFT)])

// SDRAM controller commands
#define FINAL_CMD 0x10
#define REF_CMD	  0x11
#define PALL_CMD  0x12
#define MRS_CMD	  0x14

static inline void init_ram(void)
{
	// P20..P27 all to SDRAM control functions
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;

	// enable SDA10 on P53
	REG_P5_03_CFP = (REG_P5_03_CFP & 0x3f) | 0x80;

	// What does this do?
	//REG_P8_45_CFP &= 0x03;

	// enable SDRAM clocks
	REG_CMU_PROTECT = 0x96;	 // protect off
	REG_CMU_GATEDCLK0 |= 0x78;
	REG_CMU_PROTECT = 0x00;	 // protect off

	// SDRAMC = ARBON | CAS=2 | APPON | IQB
	// (DBF, INCON = disabled
	REG_SDRAMC_APP = 0x8000000b;

	// 48MHz => 20ns clock cycle

	// SDRAM = 4M * $ Banks * 16 bits
	// refresh = 8k
	// Trp = Trcd = 15ns	     1 clock   (1..4)
	// Tras = 42ns		     3 clocks  (1..8)
	// Trc = 67ns		     4 clocks  (1..16)
#define CLKS_TRP  4
#define CLKS_TRAS 6
#define CLKS_TRC  12

// memory size
#define	ADDR_32M_x_16_bits_x_1	0x7
#define	ADDR_16M_x__8_bits_x_2	0x6
#define	ADDR__8M_x__8_bits_x_2	0x5
#define	ADDR__2M_x__8_bits_x_2	0x4
#define	ADDR_16M_x_16_bits_x_1	0x3
#define	ADDR__8M_x_16_bits_x_1	0x2
#define	ADDR__4M_x_16_bits_x_1	0x1
#define	ADDR__1M_x_16_bits_x_1	0x0

#define ADDRESS_CONFIG ADDR_16M_x_16_bits_x_1

	REG_SDRAMC_CTL = 0\
		| ((CLKS_TRP - 1) << 12)
		| ((CLKS_TRAS - 1) << 8)
		| ((CLKS_TRC - 1) << 4)
		| ADDRESS_CONFIG;

	// enable RAM self-refresh
	REG_SDRAMC_REF = 0x8c | (1 << 23) | (0x7f << 16);

	// start SDRAM initialisation sequence
	//REG_SDRAMC_INI = 0x14;

	REG_SDRAMC_INI = PALL_CMD;
	SDRAM_FIRST_BYTE = 0x0;
	asm volatile ("nop");

	REG_SDRAMC_INI = PALL_CMD;
	SDRAM_FIRST_BYTE = 0x0;
	asm volatile ("nop");

	{
		unsigned int i = 0;
		for (i = 0; i < REFRESH_COUNT; ++i) {
			REG_SDRAMC_INI = REF_CMD;
			SDRAM_FIRST_BYTE = 0x0;
			asm volatile ("nop");
		}
	}

	REG_SDRAMC_INI = MRS_CMD;
	SDRAM_MODE_STORE = 0x0; // the value is part of the address
	asm volatile ("nop");

	// finished the setup sequence
	REG_SDRAMC_INI = FINAL_CMD;

	// wait for SDRAM to come on-line
	while (0 == (REG_SDRAMC_INI & 0x08)) {
		asm volatile ("nop");
	}
}

#else
/* MRS command address for burst length=1, CAS latency = 2 */
#define MRSREG		(*(volatile unsigned char *) 0x10000442)
#define RAMDUMMY	(*(volatile unsigned char *) 0x10000000)

static inline void init_ram(void)
{
	int i;

	/* P20-P27 functions */
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;
	REG_P5_03_CFP |= 0x80;

	/* P85 */
	REG_P8_45_CFP &= 0x03;


	/* disable write protection of clock registers */
	REG_CMU_PROTECT = 0x96;

	/* switch on SDRAM clk */
	REG_CMU_GATEDCLK0 |= 0x78;

	/* re-enable write protection of clock registers */
	REG_CMU_PROTECT = 0x00;

	/* enable SDRAMC application core */
	REG_SDRAMC_APP = 0x8000000b;

	/* set up SDRAM controller */
	/* 4M x 16 bits x 1, 15 Trs/Trfc/Txsr cycles, 3 Tras, 2 Trp */
	REG_SDRAMC_CTL = 0x37e1;

	/* disable RAM self-refresh, ... */
	REG_SDRAMC_REF = 0x8c | (1 << 23) | (0x7f << 16);

	/* enter RAM setup mode */
	REG_SDRAMC_INI = 0x14;

	/* SDRAM command sequence: PALL - REF - REF - MRS (for MT48LC16M16A2) */
	REG_SDRAMC_INI = 0x12;	/* INIPRE */
	RAMDUMMY = 0x0;		/* dummy write */

	REG_SDRAMC_INI = 0x12;	/* INIPRE */
	RAMDUMMY = 0x0;		/* dummy write */

	for (i = 0; i < 2; i++) {
		REG_SDRAMC_INI = 0x11;	/* INIREF */
		RAMDUMMY = 0x0;		/* dummy write */
	}

	REG_SDRAMC_INI = 0x14;	/* INIMRS */
	MRSREG = 0x0;		/* dummy write */

	/* exit RAM setup mode */
	REG_SDRAMC_INI = 0x10;
}
#endif

#if BOARD_PROTO1
	#define enable_card_power()  do { REG_P3_P3D |=	 (1 << 3); } while(0)
	#define disable_card_power() do { REG_P3_P3D &= ~(1 << 3); } while(0)
#elif BOARD_SAMO_A1
// The ports are:
//   P32 = SD_CARD_VCCEN  active low
//   P33 = SD_CARD_PWR	  active high
// Note:
//   P33 is N/C on Caiac version and VCCEN derived from P32
//     with a special driver chip

#define P32_BIT (1 << 2)
#define P33_BIT (1 << 3)

// P32 = 0, P33 = 1
#define P3_23_MASK (~(P32_BIT | P33_BIT))
#define enable_card_power()  do {					\
		REG_P3_P3D = (REG_P3_P3D & P3_23_MASK) | P33_BIT;	\
	} while(0)

#define disable_card_power() do {					\
		REG_P3_P3D = (REG_P3_P3D & P3_23_MASK) | P32_BIT;	\
	} while(0)

#elif BOARD_S1C33E07 || BOARD_PRT33L17LCD

static inline void enable_card_power(void)
{
	REG_SRAMC_A0_BSL |= 1 << 1;
	*(volatile unsigned int *) 0x200000 = 0xffffffff;
}

static inline void disable_card_power(void)
{
	REG_SRAMC_A0_BSL |= 1 << 1;
	*(volatile unsigned int *) 0x200000 = 0;
}
#else
#error "unsupported board type"
#endif

#define DEBUGLED1_ON()	do { REG_P1_P1D &= ~(1 << 4); } while (0)
#define DEBUGLED1_OFF()	do { REG_P1_P1D |=  (1 << 4); } while (0)

#define DEBUGLED2_ON()	do { REG_P1_P1D &= ~(1 << 3); } while (0)
#define DEBUGLED2_OFF()	do { REG_P1_P1D |=  (1 << 3); } while (0)

#define EEPROM_CS_LO()	do { REG_P5_P5D &= ~(1 << 2); } while (0)
#define EEPROM_CS_HI()	do { REG_P5_P5D |=  (1 << 2); } while (0)

#define LCD_CS_LO()	do { REG_P8_P8D &= ~(1 << 5); } while (0)
#define LCD_CS_HI()	do { REG_P8_P8D |=  (1 << 5); } while (0)

#define TFT_CTL1_LO()	do { REG_P8_P8D &= ~(1 << 3); } while (0)
#define TFT_CTL1_HI()	do { REG_P8_P8D |=  (1 << 3); } while (0)

/* board specific GPIO functions */
#if BOARD_S1C33E07
	#define SDCARD_CS_LO()	do { REG_PA_DATA &= ~(1 << 3); } while (0)
	#define SDCARD_CS_HI()	do { REG_PA_DATA |=  (1 << 3); } while (0)
	#define EEPROM_WP_HI()	do { REG_P2_P2D	 =   (1 << 6); } while (0)
#elif BOARD_PROTO1 || BOARD_SAMO_A1
	#define SDCARD_CS_LO()	do { REG_P5_P5D &= ~(1 << 0); } while (0)
	#define SDCARD_CS_HI()	do { REG_P5_P5D |=  (1 << 0); } while (0)
	#define EEPROM_WP_HI()	do {} while (0)
#elif BOARD_PRT33L17LCD
	#define SDCARD_CS_LO()	do { REG_P8_P8D &= ~(1 << 4); } while (0)
	#define SDCARD_CS_HI()	do { REG_P8_P8D |=  (1 << 4); } while (0)
	#define EEPROM_WP_HI()	do { REG_P2_P2D	 =  (1 << 6); } while (0)
#else
#error "unsupported board type"
#endif

#endif /* WIKIREADER_H */

