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

#if  !defined(_SPI_H_)
#define _SPI_H_ 1

void SPI_initialise(void);


typedef enum {
	SPI_SELECT_NONE,    // turns off SD card
	SPI_SELECT_FLASH,   // affect SD card buffer enable
	SPI_SELECT_SDCARD,  // turns on SD card if off
} SPI_SelectType;

typedef int SPI_StateType;

SPI_StateType SPI_select(SPI_SelectType select);
void SPI_deselect(SPI_StateType enable);

// exchanger a byte via SPI
uint8_t SPI_exchange(uint8_t out);

#endif
