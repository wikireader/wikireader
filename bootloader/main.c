#include "regs.h"
#include "wikireader.h"

#define WAIT_FOR_SPI_RDY() \
	do {} while (REG_SPI_STAT & (1 << 6))


int main(void) {
	unsigned char b;

	init_pins();
	
	/* serial line 0: 8-bit async, no parity, internal clock, 1 stop bit */
	REG_EFSIF0_CTL = 0xc3;

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x0001c73;

	/* EEPROM: write enable */
	REG_SPI_TXD = 0x06;
	WAIT_FOR_SPI_RDY();

	/* EEPROM: chip erase */
	REG_SPI_TXD = 0x60;
	WAIT_FOR_SPI_RDY();

	/* EEPROM: auto address increment (AAI), start at address 0 */
	REG_SPI_TXD = 0xaf;
	WAIT_FOR_SPI_RDY();
	
	REG_SPI_TXD = 0x0;
	WAIT_FOR_SPI_RDY();
	
	REG_SPI_TXD = 0x0;
	WAIT_FOR_SPI_RDY();
	
	REG_SPI_TXD = 0x0;
	WAIT_FOR_SPI_RDY();

	/* get more bytes and write them to the EEPROM */
	for (;;) {
		while (!(REG_EFSIF0_STATUS & 0x1));
		b = REG_EFSIF0_RXD;

		REG_SPI_TXD = b;
		WAIT_FOR_SPI_RDY();

		REG_SPI_TXD = 0xaf;
		WAIT_FOR_SPI_RDY();
	}

	return 0;
}

