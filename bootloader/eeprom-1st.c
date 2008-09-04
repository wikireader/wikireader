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
#include "spi.h"
#include "eeprom.h"

#define DEST 0x200

#define LOAD_FROM_EEPROM 1
//#define LOAD_FROM_RS232 1

int main(void) {
#ifdef LOAD_FROM_RS232
	unsigned int len = 8192;
#endif
	u8 *dest = (u8 *) DEST;

	init_pins();
	SDCARD_CS_HI();
	EEPROM_CS_HI();

#ifdef LOAD_FROM_EEPROM
	/* read bytes from EEPROM and copy them to RAM */

	/* enable SPI: master mode, no DMA, 8 bit transfers */
	REG_SPI_CTL1 = 0x03 | (7 << 10);

	/* read the EEPROM payload, starting from page 3 */
	eeprom_load(0x300, dest, EEPROM_PAYLOAD_SIZE);
#endif

#ifdef LOAD_FROM_RS232
	init_rs232();

	/* read bytes from serial port and copy them to RAM */
	while (len--) {
		do {} while (!(REG_EFSIF0_STATUS & 0x1));
		*dest = REG_EFSIF0_RXD;
		dest++;
	}
#endif

	REG_EFSIF0_TXD = '!';
	do {} while (REG_EFSIF0_STATUS & (1 << 5));


	/* Gimme Van Halen */
	((void (*) (void)) DEST) ();

	return 0;
}

