/*
 * flash - driver for FLASH ROM
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
#include "FLASH.h"

static bool WriteEnable;


typedef enum {
	FLASH_COMMAND_NoOperation = 0x00,
	FLASH_COMMAND_WriteStatus = 0x01,
	FLASH_COMMAND_PageProgram = 0x02,
	FLASH_COMMAND_ReadData = 0x03,
	FLASH_COMMAND_WriteDisable = 0x04,
	FLASH_COMMAND_ReadStatus = 0x05,
	FLASH_COMMAND_WriteEnable = 0x06,
	FLASH_COMMAND_FastRead = 0x0b,
	FLASH_COMMAND_SectorErase = 0x20,
	FLASH_COMMAND_ChipErase = 0xc7,
} FLASH_COMMAND_type;


void FLASH_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		SPI_initialise();
		FLASH_SelectInternal();
	}
	WriteEnable = false;
}


void FLASH_SelectInternal(void)
{
	// set P05 low to disable external boot FLASH ROM
	REG_P5_P5D &= ~0x20;
	REG_P5_IOC5 |= 0x20;
}


void FLASH_SelectExternal(void)
{
	// set P05 high to enable external boot FLASH ROM
	REG_P5_P5D |= 0x20;
	REG_P5_IOC5 |= 0x20;
}


bool FLASH_WriteEnable(void)
{
	WriteEnable = true;
	return WriteEnable;
}


static void SendCommand(uint8_t command)
{
	delay_us(10);
	EEPROM_CS_LO();
	SPI_exchange(command);
	EEPROM_CS_HI();
}


static void WaitReady(void)
{
	delay_us(10);
	EEPROM_CS_LO();
	SPI_exchange(FLASH_COMMAND_ReadStatus);
	while (0 != (SPI_exchange(FLASH_COMMAND_NoOperation) & 0x01)) {
	}
	EEPROM_CS_HI();
}


bool FLASH_read(void *buffer, size_t length, uint32_t ROMAddress)
{
	WriteEnable = false;

	SPI_StateType state = SPI_select(SPI_SELECT_FLASH);

	WaitReady();
	SendCommand(FLASH_COMMAND_WriteDisable);
	EEPROM_CS_LO();
	SPI_exchange(FLASH_COMMAND_FastRead);
	SPI_exchange(ROMAddress >> 16); // A23..A16
	SPI_exchange(ROMAddress >> 8);  // A15..A08
	SPI_exchange(ROMAddress);       // A07..A00
	(void)SPI_exchange(FLASH_COMMAND_NoOperation);

	size_t i = 0;
	uint8_t *bytes = (uint8_t *)buffer;

	for (i = 0; i < length; ++i) {
			*bytes++ = SPI_exchange(FLASH_COMMAND_NoOperation);
	}
	EEPROM_CS_HI();

	SPI_deselect(state);

	return true;
}


bool FLASH_write(const void *buffer, size_t length, uint32_t ROMAddress)
{
	if (!WriteEnable || length > FLASH_PageSize) {
		WriteEnable = false;
		return false;
	}
	WriteEnable = false;

	// do not program empty buffer (i.e. all bytes == 0xff)
	bool rc = false;
	const uint8_t *bytes = (uint8_t *)buffer;

	size_t i = 0;
	for (i = 0; i < length; ++i) {
		if (0xff != bytes[i]) {
			rc = true;
			break;
		}
	}
	if (!rc) {
		return true;
	}

	SPI_StateType state = SPI_select(SPI_SELECT_FLASH);

	WaitReady();
	SendCommand(FLASH_COMMAND_WriteEnable);
	EEPROM_CS_LO();
	SPI_exchange(FLASH_COMMAND_PageProgram);
	SPI_exchange(ROMAddress >> 16); // A23..A16
	SPI_exchange(ROMAddress >> 8);  // A15..A08
	SPI_exchange(ROMAddress);       // A07..A00

	for (i = 0; i < length; ++i) {
		SPI_exchange(*bytes++);
	}
	EEPROM_CS_HI();
	WaitReady();

	SPI_deselect(state);

	return true;
}


bool FLASH_verify(const uint8_t *buffer, size_t length, uint32_t ROMAddress)
{
	bool rc = true;
	WriteEnable = false;

	SPI_StateType state = SPI_select(SPI_SELECT_FLASH);

	WaitReady();
	SendCommand(FLASH_COMMAND_WriteDisable);
	EEPROM_CS_LO();
	SPI_exchange(FLASH_COMMAND_FastRead);
	SPI_exchange(ROMAddress >> 16); // A23..A16
	SPI_exchange(ROMAddress >> 8);  // A15..A08
	SPI_exchange(ROMAddress);       // A07..A00
	SPI_exchange(FLASH_COMMAND_NoOperation);

	size_t i = 0;
	const uint8_t *bytes = (uint8_t *)buffer;
	for (i = 0; i < length; ++i) {
		if (SPI_exchange(FLASH_COMMAND_NoOperation) != *bytes++) {
			rc = false;
			break;
		}
	}
	EEPROM_CS_HI();

	SPI_deselect(state);

	return rc;
}


bool FLASH_SectorErase(uint32_t ROMAddress)
{
	if (!WriteEnable) {
		return false;
	}
	WriteEnable = false;

	SPI_StateType state = SPI_select(SPI_SELECT_FLASH);

	WaitReady();
	SendCommand(FLASH_COMMAND_WriteEnable);
	EEPROM_CS_LO();
	SPI_exchange(FLASH_COMMAND_SectorErase);
	SPI_exchange(ROMAddress >> 16); // A23..A16
	SPI_exchange(ROMAddress >> 8);  // A15..A08
	SPI_exchange(ROMAddress);       // A07..A00
	EEPROM_CS_HI();
	WaitReady();

	SPI_deselect(state);

	return true;
}


bool FLASH_ChipErase(void)
{
	if (!WriteEnable) {
		return false;
	}
	WriteEnable = false;

	SPI_StateType state = SPI_select(SPI_SELECT_FLASH);

	WaitReady();
	SendCommand(FLASH_COMMAND_WriteEnable);
	SendCommand(FLASH_COMMAND_ChipErase);
	WaitReady();

	SPI_deselect(state);

	return true;
}
