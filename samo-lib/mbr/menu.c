/*
 * menu - mbr menuing system
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

#define APPLICATION_TITLE "boot menu"

#include <stdbool.h>
#include <string.h>
#include <samo.h>
#include <lcd.h>
#include <contrast.h>
#include <analog.h>
#include <eeprom.h>

// redirect functions to mbr versions - see application.lds / mbr.c
#define FLASH_read   mbr_FLASH_read
#define SPI_exchange mbr_SPI_exchange

// this is to get constants, do not access any of its functions
#include <FLASH.h>
#include <SPI.h>

#include "application.h"

// enable this to have a small battery voltage indicator during boot
#if !defined(BATTERY_METER)
#define BATTERY_METER 1
#endif

#include "splash.h"
#include "empty.h"
#include "adjust.h"

#define MAXIMUM_BLOCKS 7
#define HEADER_MAGIC  0x4f4d4153
#define MAXIMUM_APPS 8

#define PARAMETER_START    (MAXIMUM_BLOCKS * 8192)
#define PARAMETER_SIZE     (2 * FLASH_SectorSize)
#define PARAMETER_MAGIC_1  0x5041524c
#define PARAMETER_MAGIC_2  0x424c434b

// the byte approximately just above the history key
// to flag contrast has been changed
#define	ContrastChanged         (((uint8_t *)LCD_VRAM)[LCD_VRAM_SIZE - 14])
#define	SetContrastChanged()    do { ContrastChanged = 0xff; } while (0)
#define	ClearContrastChanged()  do { ContrastChanged = 0x00; } while (0)


// NameType length is defined in the awk script: GenerateApplicationHeader.awk
typedef char NameType[32];

struct {
	uint32_t magic;
	uint32_t count;
	NameType name[8];
} header;

typedef struct {
	int block;
	int offset;
} ProcessReturnType;


// the size of this  must be an exact integer multiple of FLASH_PageSize
// or programming FLASH will not work correctly
// (There is some code below to cause a linker error if this is not true)
typedef struct {
	uint32_t magic1;
	uint32_t magic2;
	uint32_t contrast;
	uint32_t spare_1;
} ParameterType;


// copied from drivers/src/FLASH.c
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
} FLASH_CommandType;


static void PrintName(const char *name, size_t size);
static void DisplayInfo(void);
static void SendCommand(uint8_t command);
static void WaitReady(void);
static void SendCommandWithAddress(uint8_t command, uint32_t ROMAddress);
static void ProgramBlock(const void *buffer, size_t length, uint32_t ROMAddress);
static void SectorErase(uint32_t ROMAddress);

ProcessReturnType process(int block, int status);
bool parameters_load(ParameterType *param);
void parameters_save(ParameterType *param);
void print_cpu_type(void);
bool battery_empty(void);
void battery_status(void);


// this must be the first executable code as the loader executes from the first program address
ReturnType menu(int block, int status)
{
	ProcessReturnType result;

	APPLICATION_INITIALISE();
	LCD_initialise();
	Analog_initialise();
	{
		ParameterType param;
		Contrast_initialise();
		memset(&param, 0, sizeof(param));
		if (parameters_load(&param)) {
			Contrast_set(param.contrast);
		}
	}
	result = process(block, status);

	// If the structure ParameterType is not an exact multiple of FLASH_PageSize
	// then cause an error at link time.
	// this is the best that C can do since CPP cannot evaluate such an expression.
	// If typedef is correct compiler will not generate any code for this.
	if (((FLASH_PageSize / sizeof(ParameterType)) * sizeof(ParameterType)) != FLASH_PageSize) {
		void ParameterType_has_invalid_size__Fix_the_typedef(void);
		ParameterType_has_invalid_size__Fix_the_typedef();  // deliberate undefined reference
	}

	// next program
	APPLICATION_FINALISE(result.block, result.offset);
}


static void PrintName(const char *name, size_t size)
{
	register int k;
	for (k = 0; k < size; ++k) {
		register char c =  *name++;
		if ('\0' == c || '\xff' == c) {
			break;
		}
		print_char(c);
	}
}


static void DisplayInfo(void)
{
	Analog_scan();
	print("\nCPU: ");
	print_cpu_type();
	print("\nBAT: ");
	print_uint(Analog_BatteryMilliVolts());
	print(" mV\nTMP: ");
	print_int(Analog_TemperatureCelcius());
	print(" DegC\nLCD: ");
	print_uint(Analog_ContrastMilliVolts());
	print(" mV\nREV: ");
	uint32_t rev = board_revision();
	if (rev >= 6) {
		rev -= 5;
		print_char('V');
	} else {
		print_char('A');
	}
	print_uint(rev);

	print("\nMBR: ");
	FLASH_read(&rev, sizeof(rev), FLASH_RevisionNumberAddress);
	print_uint(0xffffffff == rev ? 0 : rev);

	char SerialNumber[FLASH_SerialNumberSize];
	FLASH_read(SerialNumber, sizeof(SerialNumber), FLASH_SerialNumberAddress);
	print("\nS/N: ");
	PrintName(SerialNumber, sizeof(SerialNumber));
	print("\n");
}


// process:
// status == 0 => return from a program, therefore must display menu
//        != 0 => automatic boot, therefore check keys
//                   run app[0] with status set to:
//                      0 if no keys pressed
//                      1,2 or 3 if any keys left, centre or right are pressed

ProcessReturnType process(int block, int status)
{
	ProcessReturnType rc = {0, 0};

	Analog_scan(); // update analog values
	if (battery_empty()) {
		LCD_DisplayImage(LCD_PositionCentre, true, &empty_image);
	} else {
		LCD_DisplayImage(LCD_PositionCentre, true, &splash_image);
	}

	if (0 != status) {
		bool MenuFlag = false;
		DisplayInfo();

		print("\n\nmenu? ");
		status = 0;

		static const char spinner[4] = "-\\|/";
		unsigned int i;
		for (i = 0; i <	4 * sizeof(spinner); ++i) {
			unsigned int k;
			for (k = 0; k < sizeof(spinner); ++k) {
				delay_us(5000);
				print_char(spinner[k]);
				print_char('\x08');
				battery_status();
			}
			if (console_input_available()) {
				MenuFlag = true;
				break;
			}
			k = REG_P6_P6D & 0x07;
			if (1 == k) {         // right button
				status = 1;
				break;
			} else if (4 == k) {  // centre button
				status = 2;
				break;
			} else if (2 == k) {  // right button
				status = 3;
				break;
			} else if (0 != k) {  // multiple buttons
				MenuFlag = true;
				break;
			}
		}

		print_char('\n');

		if (!MenuFlag) {
			rc.block = block + 1;
			rc.offset = status;
			return rc;
		}
	}

	LCD_DisplayImage(LCD_PositionBottom, false, &adjust_image);

	for (;;) {
		ProcessReturnType app[MAXIMUM_APPS * MAXIMUM_BLOCKS] = {{0, 0}};

		print("\nBoot Menu\n\n");
		print("0. Power Off\n");
		print("1. Board Information\n");
		int MenuItem = 0;
		// not zero since this program should be in block zero
		unsigned int i;
		for (i = 1; i < MAXIMUM_BLOCKS; ++i) {
			FLASH_read(&header, sizeof(header), (i << 13));

			if (HEADER_MAGIC == header.magic && 0 < header.count && MAXIMUM_APPS >= header.count) {
				unsigned int k;
				for (k = 0; k < header.count; ++k) {
					print_char(MenuItem + 'A');
					print(". ");
					PrintName(header.name[k], sizeof(header.name));
					print_char('\n');
					app[MenuItem].block = i;
					app[MenuItem].offset = k;
					++MenuItem;
				}
			}
		}
		print("\nEnter selection: ");
		char k = ' ';
		while (k <= ' ') {
			while (!console_input_available()) {
				switch (REG_P6_P6D & 0x07) {
				case 1:
					Contrast_set(Contrast_get() + 1);
					SetContrastChanged();
					delay_us(3000);
					break;
				case 2:
					Contrast_set(Contrast_get() - 1);
					SetContrastChanged();
					delay_us(3000);
					break;
				case 4:
					// Contrast_set(Contrast_default);
					{
						ParameterType param;
						parameters_load(&param);
						param.contrast = Contrast_get();
						parameters_save(&param);
						ClearContrastChanged();
					}
					break;
				}
				battery_status();
				delay_us(1000);
			}
			k = console_input_char();
		}
		if ('0' == k) {
			power_off();
		} else if ('1' == k) {
			DisplayInfo();
		} else {
			if ('A' <= k && 'Z' >= k) {
				k += 'a' - 'A';
			}
			i = k - 'a';
			if (0 <= i && MAXIMUM_APPS * MAXIMUM_BLOCKS > i) {
				if (0 != app[i].block) {
					print_char(k);
					print_char('\n');
					rc = app[i];
					break;
				}
			}
		}
	}
	return rc;
}


bool parameters_load(ParameterType *param)
{
	SDCARD_CS_HI();
	disable_card_power();
	EEPROM_CS_HI();
	EEPROM_WP_HI();

	unsigned int i;
	for (i = 0; i <= PARAMETER_SIZE - sizeof(*param); i += sizeof(*param)) {
		FLASH_read(param, sizeof(*param), PARAMETER_START + i);
		if (PARAMETER_MAGIC_1 == param->magic1 && PARAMETER_MAGIC_2 == param->magic2) {
			return true;
		}
	}
	return false;
}


void parameters_save(ParameterType *param)
{
	SDCARD_CS_HI();
	disable_card_power();
	EEPROM_CS_HI();
	EEPROM_WP_HI();

	unsigned int i;
	ParameterType p;
	for (i = 0; i < PARAMETER_SIZE - sizeof(p); i += sizeof(p)) {
		FLASH_read(&p, sizeof(p), PARAMETER_START + i);
		if (PARAMETER_MAGIC_1 == p.magic1 && PARAMETER_MAGIC_2 == p.magic2) {
			break;
		}
	}
	if (0 == i || !(0xffffffff == p.magic1 && 0xffffffff == p.magic2)) {
		SectorErase(PARAMETER_START);
		SectorErase(PARAMETER_START + FLASH_SectorSize);
	} else {
		i -= sizeof(p);
	}

	param->magic1 = PARAMETER_MAGIC_1;
	param->magic2 = PARAMETER_MAGIC_2;
	if (0 != memcmp(param, &p, sizeof(p))) {
		ProgramBlock(param, sizeof(*param), PARAMETER_START + i);
	}
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

static void SendCommandWithAddress(uint8_t command, uint32_t ROMAddress)
{
	WaitReady();
	SendCommand(FLASH_COMMAND_WriteEnable);
	EEPROM_CS_LO();
	SPI_exchange(command);
	SPI_exchange(ROMAddress >> 16); // A23..A16
	SPI_exchange(ROMAddress >> 8);  // A15..A08
	SPI_exchange(ROMAddress);       // A07..A00
}

static void ProgramBlock(const void *buffer, size_t length, uint32_t ROMAddress)
{
	SendCommandWithAddress(FLASH_COMMAND_PageProgram, ROMAddress);

	size_t i;
	register uint8_t *bytes = (uint8_t *)buffer;
	for (i = 0; i < length; ++i) {
		SPI_exchange(*bytes++);
	}
	EEPROM_CS_HI();
	WaitReady();
	SendCommand(FLASH_COMMAND_WriteDisable);
}

static void SectorErase(uint32_t ROMAddress)
{
	SendCommandWithAddress(FLASH_COMMAND_SectorErase, ROMAddress);
	EEPROM_CS_HI();
	WaitReady();
}


void print_cpu_type(void)
{
	switch (CORE_ID) {
	case  CORE_ID_STANDARD:
		print(CORE_ID_STANDARD_DESC);
		break;
	case  CORE_ID_MINI:
		print(CORE_ID_MINI_DESC);
		break;
	case  CORE_ID_ADVANCED:
		print(CORE_ID_ADVANCED_DESC);
		break;
	case  CORE_ID_PE:
		print(CORE_ID_PE_DESC);
		break;
	case  CORE_ID_PE_LE:
		print(CORE_ID_PE_LE_DESC);
		break;
	default:
		print_char('?');
		break;
	}
	print("  ");
	switch (PRODUCT_ID) {
	case  PRODUCT_ID_3:
		print(PRODUCT_ID_3_DESC);
		break;
	case  PRODUCT_ID_4:
		print(PRODUCT_ID_4_DESC);
		break;
	case  PRODUCT_ID_3E:
		print(PRODUCT_ID_3E_DESC);
		break;
	case  PRODUCT_ID_3L:
		print(PRODUCT_ID_3L_DESC);
		break;
	default:
		print_char('?');
		break;
	}
	print_byte(MODEL_ID);
	print(" V 0x");
	print_byte(VERSION_ID);
}


bool battery_empty(void)
{
	Analog_scan();
	int v = Analog_BatteryMilliVolts();
	return v <= BATTERY_LOW;
}


void battery_status(void)
{
#if BATTERY_METER
	static bool initialised;
	uint8_t *fb = (uint8_t*)LCD_VRAM;
	static const char pos[] = {
		0x03, 0x02, 0x02, 0x0e,
		0x08, 0x08, 0x08, 0x08,
		0x0e, 0x02, 0x02, 0x03,
	};
	static const char neg[] = {
		0xff, 0xff, 0xfc
	};
	static const char body[] = {
		0x00, 0x00, 0x04
	};
	register int i;
	register int j;
	register uint32_t indicator = 0;

	Analog_scan();
	int v = Analog_BatteryMilliVolts();
	if (v < BATTERY_EMPTY) {
		v = BATTERY_EMPTY;
	} else if (v > BATTERY_FULL) {
		v = BATTERY_FULL;
	}

	unsigned int full = 20 * (v - BATTERY_EMPTY) / (BATTERY_FULL - BATTERY_EMPTY);

	if (!initialised) {
		uint8_t *p = fb + 2 * LCD_VRAM_WIDTH_BYTES;
		initialised = true;
		for (i = 0; i < sizeof(pos); ++i) {
			p[0] = pos[i];
			if (0 == i || sizeof(pos) - 1 == i) {
				for (j = 0; j < sizeof(neg); ++j) {
					p[j + 1] = neg[j];
				}
			} else {
				for (j = 0; j < sizeof(body); ++j) {
					p[j + 1] = body[j];
				}
			}
			p += LCD_VRAM_WIDTH_BYTES;
		}

	}
	for (i = 0; i < full; ++i) {
		indicator |= 0x10 << i;
	}

	fb += 4 * LCD_VRAM_WIDTH_BYTES;
	for (i = 0; i < 8; ++i) {
		fb[1] = indicator >> 16;
		fb[2] = indicator >> 8;
		fb[3] = (indicator >> 0) | 0x04;
		fb += LCD_VRAM_WIDTH_BYTES;
	}
#endif
}
