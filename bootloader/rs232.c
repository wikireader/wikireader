#include "regs.h"
#include "wikireader.h"

enum {
	SPI_WRITE = 0,
	SPI_READ = 1,
	SPI_CS_HI = 2,
	SPI_CS_LO = 3
};

int main(void) {
	unsigned char cmd, dat, len;

	INIT_PINS();
	SDCARD_CS_HI();
	EEPROM_CS_HI();

	/* EEPROM WP: off */
	REG_P2_IOC2 |= (1 << 6);
	REG_P2_P2D  |= (1 << 6);

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
}

