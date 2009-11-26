/*
 * SPI - unified FLASH and SD Card SPI driver
 *
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

#include <stdlib.h>
#include <stdbool.h>

#include <regs.h>
#include <samo.h>

#include "SPI.h"


enum  {
	SPI_ENABLE_CardOff,
	SPI_ENABLE_CardOn,
	SPI_ENABLE_CardActivated,
};


void SPI_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;

		SDCARD_CS_HI();
		EEPROM_CS_HI();

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
		REG_SPI_CTL2 = 0;                               // must be zero for master mode
		REG_SPI_WAIT = 0;
	}
}


SPI_StateType SPI_select(SPI_SelectType select)
{
	SPI_StateType state = SPI_ENABLE_CardOff;

	SDCARD_CS_HI();
	EEPROM_CS_HI();

	switch (select) {
	case SPI_SELECT_FLASH:
		if (check_card_power()) {
			state = SPI_ENABLE_CardOn;
			disable_card_buffer();
		}
		break;

	case SPI_SELECT_SDCARD:
		if (!check_card_power()) {
			enable_card_power();
			state = SPI_ENABLE_CardActivated;
		}
	case SPI_SELECT_NONE:
		disable_card_power();
		break;
	}

	return state;
}


void SPI_deselect(SPI_StateType state)
{
	SDCARD_CS_HI();
	EEPROM_CS_HI();

	switch (state)
	{
	case SPI_ENABLE_CardOn:
		enable_card_buffer();
		break;
	case SPI_ENABLE_CardActivated:
		enable_card_buffer();
		break;
	default:
		break;
	}
}

uint8_t SPI_exchange(uint8_t out)
{
	REG_SPI_TXD = out;
	do {} while (~REG_SPI_STAT & RDFF);
	return REG_SPI_RXD;
}
