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
#include "config.h"

enum {
	SPI_WRITE = 0,
	SPI_READ = 1,
	SPI_CS_HI = 2,
	SPI_CS_LO = 3
};

int main(void)
{
	u8 cmd, dat, len;

	init_pins();
	init_rs232();
	SDCARD_CS_HI();
	EEPROM_CS_HI();

	/* EEPROM WP: off */
	EEPROM_WP_HI();

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x73 | (7 << 10) | (7 << 4);

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

