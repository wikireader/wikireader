#ifndef WIKIREADER_H
#define WIKIREADER_H

#include "config.h"

static inline void init_pins(void)
{
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
#elif BOARD_PROTO1
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

#if BOARD_PROTO1
	#define enable_card_power()  do { REG_P3_P3D |=	 (1 << 3); } while(0)
	#define disable_card_power() do { REG_P3_P3D &= ~(1 << 3); } while(0)
#else
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
#elif BOARD_PROTO1
	#define SDCARD_CS_LO()	do { REG_P5_P5D &= ~(1 << 0); } while (0)
	#define SDCARD_CS_HI()	do { REG_P5_P5D |=  (1 << 0); } while (0)
	#define EEPROM_WP_HI()	do {} while (0)
#elif BOARD_PRT33L17LCD
	#define SDCARD_CS_LO()	do { REG_P8_P8D &= ~(1 << 4); } while (0)
	#define SDCARD_CS_HI()	do { REG_P8_P8D |=  (1 << 4); } while (0)
	#define EEPROM_WP_HI()	do { REG_P2_P2D	 =  (1 << 6); } while (0)
#endif

#endif /* WIKIREADER_H */

