/*
 * mbr - the initial 512 byte bootstrap program
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define APPLICATION_TITLE "mbr"
#include "application.h"


#define RAM_SIZE	   8192
#define RAM_LOAD_ADDRESS   0x200
#define EEPROM_CODE_OFFSET 0x300

#define EEPROM_PAYLOAD_SIZE (RAM_SIZE - EEPROM_CODE_OFFSET)

#define APPLICATION_RAM_START ((uint8_t *)RAM_LOAD_ADDRESS)
#define APPLICATION ((application *)RAM_LOAD_ADDRESS)


// this is actually local
uint8_t SPI_exchange(uint8_t out);

// this defines the entry points to the MBR code
// and the SPI_exchange code
// uint32_t is used to keep the compiler from generating extraneous code
uint32_t start(uint32_t out)
{
	asm volatile (
		"jp\tmaster_boot   \n\t" // location 0 - boot
		"jp\tFLASH_read    \n\t" // location 2 - so eeprom_load can be reused
		"SPI_exchange:         " // location 4 - so SPI_exchange can be reused
		);

	// start of code for: uint8_t SPI_exchange(uint8_t out)
	REG_SPI_TXD = out;
	do {} while (~REG_SPI_STAT & RDFF);
	return REG_SPI_RXD;

}


bool FLASH_read(void *buffer, size_t size, uint32_t ROMAddress)
{
	EEPROM_CS_HI();
	SPI_exchange(0x00);
	EEPROM_CS_LO();

#if EEPROM_SST25VF040 || EEPROM_PM25LV512
	/* read high-speed */
	SPI_exchange(0x0b);
#define REQUIRES_DUMMY_CYCLE 1
#elif EEPROM_MP45PE80
	/* read normal-speed */
	SPI_exchange(0x03);
#define REQUIRES_DUMMY_CYCLE 0
#else
#error "Unsupported EEPROM"
#endif

	SPI_exchange(ROMAddress >> 16);
	SPI_exchange(ROMAddress >> 8);
	SPI_exchange(ROMAddress);

#if REQUIRES_DUMMY_CYCLE
	/* dummy cycle */
	SPI_exchange(0x00);
#endif

	uint8_t *bytes = (uint8_t *)buffer;
	while (size--) {
		*bytes++ = SPI_exchange(0x00);
	}

	EEPROM_CS_HI();
	return true;
}


__attribute__ ((noreturn))
void master_boot(void)
{
	asm volatile ("xld.w   %r15, __dp");
	{
		register ReturnType rc = { 0, 0xffffffff };

		for (;;) {
			asm volatile ("xld.w   %r15, __dp");
			init_pins();
			init_rs232_ch0();
			//init_ram(); // but will be too big

			// enable SPI
			REG_SPI_CTL1 =
				BPT_8_BITS |
				//CPHA |
				//CPOL |
				MCBR_MCLK_DIV_4 |
				//TXDE |
				//RXDE |
				MODE_MASTER |
				ENA |
				0;

			FLASH_read(APPLICATION_RAM_START, EEPROM_PAYLOAD_SIZE, (rc.block << 13) | EEPROM_CODE_OFFSET);

			rc = (APPLICATION)(rc.block, rc.status);
		}
	}
}
