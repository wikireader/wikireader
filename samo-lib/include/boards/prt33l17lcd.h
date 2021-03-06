#ifndef WIKIREADER_PRT33L17LCD_H
#define WIKIREADER_PRT33L17LCD_H

#define EEPROM_MP45PE80		1

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
	/* SDCARD CS# */
	REG_P8_IOC8 = (1 << 3);
	/* EEPROM WP# */
	REG_P1_IOC1 = (1 << 4);

	/* P50 & P52: CS lines */
	REG_P5_IOC5 = 0x07;
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
	//REG_SDRAMC_CTL = 0x37e1;
	REG_SDRAMC_CTL = 0x37e3;

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

#define SDCARD_CS_LO()	do { REG_P8_P8D &= ~(1 << 4); } while (0)
#define SDCARD_CS_HI()	do { REG_P8_P8D |=  (1 << 4); } while (0)
#define EEPROM_WP_HI()	do { REG_P2_P2D	 =  (1 << 6); } while (0)

#define power_off()	do {} while(0)
#define prepare_keys()  do {} while(0)
#define get_key_state() 0

#endif /* WIKIREADER_PRT33L17LCD_H */

