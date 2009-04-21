/*
    e07 bootloader suite
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>
    Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>

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
#include "config.h"

#define RAM_SIZE	   8192
#define RAM_LOAD_ADDRESS   0x200
#define EEPROM_CODE_OFFSET 0x300

#define EEPROM_PAYLOAD_SIZE (RAM_SIZE - EEPROM_CODE_OFFSET)

typedef int application(void);


#define DEST ((u8 *)RAM_LOAD_ADDRESS)
#define APPLICATION ((application *)RAM_LOAD_ADDRESS)

#define PRINT_CHAR(c)						\
	do {							\
		do {						\
		} while (0 == (REG_EFSIF0_STATUS & TDBEx));	\
		REG_EFSIF0_TXD = c;				\
	} while (0)


// this defines the entry points to the MBR code
//__attribute__ ((noreturn))
void start(void)
{
	asm volatile (
		"\tjp\tmaster_boot\n"	// location 0 - boot
		"\t.extern\teeprom_load\n"
		"\txjp\teeprom_load\n"	// location 2 - so eeprom_load can be re-used
		);
}


__attribute__ ((noreturn))
void master_boot(void)
{
	register int block = 0;

	for (;;) {
		asm volatile ("xld.w   %r15, __dp");
		init_pins();
		init_rs232_ch0();
		//init_ram(); // but will be too big

		PRINT_CHAR('>');

		/* enable SPI: master mode, no DMA, 8 bit transfers */
		REG_SPI_CTL1 = 0x03 | (7 << 10);

		eeprom_load((block << 13) | EEPROM_CODE_OFFSET, DEST, EEPROM_PAYLOAD_SIZE);

		block = (APPLICATION)();
		PRINT_CHAR('<');
	}
}
