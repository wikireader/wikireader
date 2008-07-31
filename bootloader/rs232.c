#include "regs.h"
#include "wikireader.h"

enum {
	SPI_WRITE = 0,
	SPI_READ = 1,
	SPI_CS_HI = 2,
	SPI_CS_LO = 3
};

/* MRS command address for burst length=1, CAS latency = 2 */
#define MRSREG		(*(volatile unsigned char *) 0x10000442)
#define RAMDUMMY	(*(volatile unsigned char *) 0x10000000)


#define DELAY() \
	{ int delay=0xfff; while(delay--) asm("nop"); }

int main(void)
{
	unsigned char cmd, dat, len;
	int i, bla;

	INIT_PINS();
	SDCARD_CS_HI();
	EEPROM_CS_HI();


#if 0
	asm("xld.w %r15, __dxp");
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
	 * 	F[brclk] = 48MHz
	 * 	DIVMD = 1/8
	 *	bps = 38400
	 *
	 *   = 77
	 */

	REG_EFSIF0_BRTRDL = 77 & 0xff;
	REG_EFSIF0_BRTRDM = 77 >> 8;

	/* baud rate timer: run! */
	REG_EFSIF0_BRTRUN = 0x01;

	unsigned int yyy = (unsigned int) xxx;

	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

for (;;) {
	REG_EFSIF0_TXD = (yyy >> 24) & 0xff;
	do {} while (REG_EFSIF0_STATUS & (1 << 5));
	DELAY();

	REG_EFSIF0_TXD = (yyy >> 16) & 0xff;
	do {} while (REG_EFSIF0_STATUS & (1 << 5));
	DELAY();

	REG_EFSIF0_TXD = (yyy >> 8) & 0xff;
	do {} while (REG_EFSIF0_STATUS & (1 << 5));
	DELAY();

	REG_EFSIF0_TXD = yyy & 0xff;
	do {} while (REG_EFSIF0_STATUS & (1 << 5));
	DELAY();
	
	REG_EFSIF0_TXD = 0x23;
	do {} while (REG_EFSIF0_STATUS & (1 << 5));
	DELAY();
}


#endif
#if 0
	/*****************************************************/

	/* disable write protection of clock registers */
	REG_CMU_PROTECT = 0x96;

	/* switch on SDRAM clk */
	REG_CMU_GATEDCLK0 = 0x78;

	/* P20-P27 functions */
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;
	REG_P5_03_CFP = 0x80;
	
	/* re-enable write protection of clock registers */
	REG_CMU_PROTECT = 0x00;
	
	/* enable SDRAMC application core */
	REG_SDRAMC_APP = 0x8000000b;
			REG_EFSIF0_TXD = dest[i];
			do {} while (REG_EFSIF0_STATUS & (1 << 5));

	/* set up SDRAM controller */
	/* 4M x 16 bits x 1, 15 Trs/Trfc/Txsr cycles, 3 Tras, 2 Trp */
	REG_SDRAMC_CTL = 0x37f1;

	/* enable RAM self-refresh */
	//REG_SDRAMC_REF |= (1 << 25);

	REG_SDRAMC_INI = 0x10;	/* exit setup mode */

	/* SDRAM command sequence: PALL - REF - REF - MRS (for MT48LC16M16A2) */
	REG_SDRAMC_INI = 0x12;	/* INIPRE */
	RAMDUMMY = 0x0;		/* dummy write */
	
	for (i = 0; i < 2; i++) {
		REG_SDRAMC_INI = 0x11;	/* INIREF */
		RAMDUMMY = 0x0;		/* dummy write */
	}

	REG_SDRAMC_INI = 0x14;	/* INIMRS */
	MRSREG = 0x0;		/* dummy write */

	REG_SDRAMC_INI = 0x10;	/* exit setup mode */

	/***************************************************/

	volatile unsigned char *dest = (volatile unsigned char *) 0x10000000;
//	volatile unsigned char *dest = (volatile unsigned char *) 0x400000;

	for (i = 0; i < 0x100; i++) {
		dest[i] = i;
	}
	
	for(;;) {
		for(i = 0; i < 0x100; i++) {
			REG_EFSIF0_TXD = dest[i];
			do {} while (REG_EFSIF0_STATUS & (1 << 5));
			
			for (bla = 0; bla < 0xfff; bla++)
				asm("nop");
		}
	}

#endif
#if 1

	/* EEPROM WP: off */
	REG_P2_IOC2 = (1 << 6);
	REG_P2_P2D  = (1 << 6);

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
	 * 	F[brclk] = 48MHz
	 * 	DIVMD = 1/8
	 *	bps = 38400
	 *
	 *   = 77
	 */

	REG_EFSIF0_BRTRDL = 77 & 0xff;
	REG_EFSIF0_BRTRDM = 77 >> 8;

	/* baud rate timer: run! */
	REG_EFSIF0_BRTRUN = 0x01;

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x73 | (7 << 10);

	for (;;) {
		do {} while (!(REG_EFSIF0_STATUS & 0x1));
		cmd = REG_EFSIF0_RXD;

		switch (cmd) {
			case SPI_CS_HI:
				EEPROM_CS_HI();
				break;
			case SPI_CS_LO:
				EEPROM_CS_LO();
				break;
			case SPI_WRITE:
				do {} while (!(REG_EFSIF0_STATUS & 0x1));
				len = REG_EFSIF0_RXD;

				while (len--) {
					do {} while (!(REG_EFSIF0_STATUS & 0x1));
					dat = REG_EFSIF0_RXD;
					REG_SPI_TXD = dat;
					do {} while (REG_SPI_STAT & (1 << 6));
				}
				break;
			case SPI_READ:
				do {} while (!(REG_EFSIF0_STATUS & 0x1));
				len = REG_EFSIF0_RXD;
				
				while (len--) {
					REG_SPI_TXD = 0x00;
					do {} while (REG_SPI_STAT & (1 << 6));
					dat = REG_SPI_RXD;
					REG_EFSIF0_TXD = dat;
					do {} while (REG_EFSIF0_STATUS & (1 << 5));
				}
				break;
		}
	}
#endif
}

