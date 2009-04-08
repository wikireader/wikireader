/*
    e07 bootloader suite
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

#include "regs.h"
#include "types.h"
#include "wikireader.h"
#include "eeprom.h"

static u8 spi_transmit(u8 out)
{
	REG_SPI_TXD = out;
	do {} while (~REG_SPI_STAT & (1 << 2));
	return REG_SPI_RXD;
}

void eeprom_load(u32 addr, u8 *dest, u32 size)
{
	EEPROM_CS_LO();

#if EEPROM_SST25VF040 || EEPROM_PM25LV512
	/* read high-speed */
	spi_transmit(0x0b);
#define REQUIRES_DUMMY_CYCLE 1
#elif EEPROM_MP45PE80
	/* read normal-speed */
	spi_transmit(0x03);
#define REQUIRES_DUMMY_CYCLE 0
#else
#error "Unsupported EEPROM"
#endif

	spi_transmit(addr >> 16);
	spi_transmit(addr >> 8);
	spi_transmit(addr);

#if REQUIRES_DUMMY_CYCLE
	/* dummy cycle */
	spi_transmit(0x00);
#endif

	while (size--) {
		spi_transmit(0x00);
		*dest++ = REG_SPI_RXD;
	}

	EEPROM_CS_HI();
}

