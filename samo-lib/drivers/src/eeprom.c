/*
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#include <regs.h>
#include <samo.h>

#include "eeprom.h"

// this is global but must not be in the header file
// mbr creates a jump to this using assembler code
uint8_t eeprom_spi_exchange(uint8_t out)
{
	REG_SPI_TXD = out;
	do {} while (~REG_SPI_STAT & RDFF);
	return REG_SPI_RXD;
}

void eeprom_load(uint32_t eeprom_address, void *buffer, size_t size)
{
	EEPROM_CS_HI();
	eeprom_spi_exchange(0x00);
	EEPROM_CS_LO();

#if EEPROM_SST25VF040 || EEPROM_PM25LV512
	/* read high-speed */
	eeprom_spi_exchange(0x0b);
#define REQUIRES_DUMMY_CYCLE 1
#elif EEPROM_MP45PE80
	/* read normal-speed */
	eeprom_spi_exchange(0x03);
#define REQUIRES_DUMMY_CYCLE 0
#else
#error "Unsupported EEPROM"
#endif

	eeprom_spi_exchange(eeprom_address >> 16);
	eeprom_spi_exchange(eeprom_address >> 8);
	eeprom_spi_exchange(eeprom_address);

#if REQUIRES_DUMMY_CYCLE
	/* dummy cycle */
	eeprom_spi_exchange(0x00);
#endif

	uint8_t *bytes = (uint8_t *)buffer;
	while (size--) {
		*bytes++ = eeprom_spi_exchange(0x00);
	}

	EEPROM_CS_HI();
}

