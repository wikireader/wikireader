/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
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

#include <inttypes.h>
#include "regs.h"

// override the default console bps for faster programming
#define CONSOLE_BPS 115200

// default is to program the on-board flash
#if !defined(PROGRAM_TEST_JIG)
#define PROGRAM_TEST_JIG 0
#endif

#define SAMO_RESTRICTIONS 1
#include "samo.h"


// do not add any static variables
// this program is restricted to 512 bytes .text
// and stack/register variables only


enum {
	COMMAND_BOARD_REVISION = 'A',
	COMMAND_SPI_WRITE = 'W',
	COMMAND_SPI_READ = 'R',
	COMMAND_SPI_DESELECT = 'H',
	COMMAND_SPI_SELECT = 'L',
};


static void serial_put(uint8_t c);
static uint8_t serial_get(void);
static uint32_t serial_get16(void);
static uint8_t spi_transfer(uint8_t c);


int main(void)
{
	init_pins();
	init_rs232_ch0();
	SDCARD_CS_HI();
	EEPROM_CS_HI();
	EEPROM_WP_HI();

	REG_SPI_CTL1 =
		//BPT_32_BITS |
		//BPT_16_BITS |
		BPT_8_BITS |
		//BPT_1_BITS |

		//CPHA |
		//CPOL |

		//MCBR_MCLK_DIV_512 |
		//MCBR_MCLK_DIV_256 |
		//MCBR_MCLK_DIV_128 |
		//MCBR_MCLK_DIV_64 |
		//MCBR_MCLK_DIV_32 |
		//MCBR_MCLK_DIV_16 |
		//MCBR_MCLK_DIV_8 |
		MCBR_MCLK_DIV_4 |

		//TXDE |
		//RXDE |

		MODE_MASTER |
		//MODE_SLAVE |

		ENA |
		0;

#if PROGRAM_TEST_JIG
	// set P05 high to enable external boot FLASH ROM
	REG_P5_P5D |= 0x20;
	REG_P5_IOC5 |= 0x20;
#else
	// set P05 low to disable external boot FLASH ROM
	REG_P5_P5D &= ~0x20;
	REG_P5_IOC5 |= 0x20;
#endif
	// flush spi buffer
	(void)REG_SPI_RXD;

	for (;;) {
		uint8_t command = serial_get();

		// switch will create jump table and overflow our 512 bytes
		if (COMMAND_BOARD_REVISION == command) {

			serial_put('A');
			serial_put(board_revision() + '0');

		} else if (COMMAND_SPI_DESELECT == command) {

			EEPROM_CS_HI();

		} else if (COMMAND_SPI_SELECT == command) {

			EEPROM_CS_LO();

		} else if (COMMAND_SPI_WRITE == command) {

			uint32_t len = serial_get16();
			while (len--) {
				(void)spi_transfer(serial_get());
			}

		} else if (COMMAND_SPI_READ == command) {

			uint32_t len = serial_get16();
			while (len--) {
				serial_put(spi_transfer(0x00));
			}

		} else {
			serial_put('?');
		}
	}
}

static void serial_put(uint8_t c)
{
	while (0 == (REG_EFSIF0_STATUS & TDBEx)) {
	}
	REG_EFSIF0_TXD = c;
}


static uint8_t serial_get(void)
{
	while (0 == (REG_EFSIF0_STATUS & RDBFx)) {
	}
	return REG_EFSIF0_RXD;
}


static uint32_t serial_get16(void)
{
	uint32_t value = serial_get();
	return value | serial_get() << 8;
}


static uint8_t spi_transfer(uint8_t out)
{
	REG_SPI_TXD = out;
	do {} while (~REG_SPI_STAT & RDFF);
	return REG_SPI_RXD;
}
