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
#include "spi.h"

void eeprom_load(u32 addr, u8 *dest, u32 size)
{
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

