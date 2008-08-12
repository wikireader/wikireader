#include "regs.h"
#include "wikireader.h"
#include "eeprom.h"

void eeprom_load(unsigned int addr, char *dest, int size)
{
	/* read the EEPROM payload, starting from page 2 (512 bytes) */
	EEPROM_CS_LO();

	spi_transmit(0x03);
	spi_transmit(addr >> 16);
	spi_transmit(addr >> 8);
	spi_transmit(addr);

	while (size--) {
		spi_transmit(0x00);
		*dest++ = REG_SPI_RXD;
	}

	EEPROM_CS_HI();
}

