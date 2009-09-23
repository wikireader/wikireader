/*
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

#include <stdlib.h>
#include <stdbool.h>
#include <tff.h>
#include <diskio.h>
#include <delay.h>
#include <misc.h>
#include <regs.h>
#include <samo.h>
#include <lcd.h>

struct guilib_image
{
	uint32_t width;
	uint32_t height;
	uint8_t data[];
};
typedef struct guilib_image ImageType;

#include "program.h"
#include "ok.h"
#include "fail.h"



// size of the flash memory
uint8_t ROMBuffer[65536];

enum {
	PageSize = 256,
	SectorSize = 4096,
	ProgramRetries = 5,
	SerialNumberAddress = 0x1fe0,
	SerialNumberLength = 32,
};

typedef enum {
	SPI_WriteStatus = 0x01,
	SPI_PageProgram = 0x02,
	SPI_ReadData = 0x03,
	SPI_WriteDisable = 0x04,
	SPI_ReadStatus = 0x05,
	SPI_WriteEnable = 0x06,
	SPI_FastRead = 0x0b,
	SPI_ChipErase = 0xc7,
} SPI_type;


static bool process(const char *filename);
static void SendCommand(uint8_t command);
static void WaitReady(void);
static void WriteEnable(void);
static void WriteDisable(void);
static void ReadBlock(uint8_t *buffer, size_t length, uint32_t ROMAddress);
static void ProgramBlock(const uint8_t *buffer, size_t length, uint32_t ROMAddress);
static bool VerifyBlock(const uint8_t *buffer, size_t length, uint32_t ROMAddress);
static void ChipErase(void);

static void SPI_put(uint8_t c);
static uint8_t SPI_get(void);

static void fill(uint8_t value);
static void display_image(const ImageType *image, uint8_t background, uint8_t toggle);

int flash(int arg)
{
	// set the initial stack and data pointers
	asm volatile (
		"\txld.w\t%r15, __MAIN_STACK\n"
		"\tld.w\t%sp, %r15\n"
		"\txld.w\t%r15, __dp\n"
		"\tld.w\t%r5, 0\n"
		"\tld.w\t%psr, %r5\n"
		);

	print("Flash Program: ");
	const char *filename = "flash.rom";

	if (0 == arg) {
		print("Internal");
		// set P05 low to disable external boot FLASH ROM
		REG_P5_P5D &= ~0x20;
		REG_P5_IOC5 |= 0x20;
	} else {
		print("Test Jig");
		filename = "test-jig.rom";
		// set P05 high to enable external boot FLASH ROM
		REG_P5_P5D |= 0x20;
		REG_P5_IOC5 |= 0x20;
	}

	print(" FLASH\n");

	int i = 0;
	for (i = 0; i < ProgramRetries; ++i) {
		display_image(&program_image, 0x00, 0xff);
		if (process(filename)) {
			print("Finished sucessfully\n");
			display_image(&ok_image, 0x00, 0xff);
			break;
		} else {
			print("Error occurred\n");
			display_image(&fail_image, 0x00, 0xff);
		}
	}

	for (;;) {
	}
}

static bool process(const char *filename)
{
	bool result = true;
	uint8_t b = 0;
	disk_ioctl(0, CTRL_POWER, &b);
	disk_initialize(0);

	FATFS TheFileSystem;
	f_mount(0, &TheFileSystem);  // only possible value is zero

	FIL file;
	FRESULT rc = FR_OK;

	print("Loading: ");
	print(filename);
	print("  ");
	rc = f_open(&file, filename, FA_READ);
	if (FR_OK != rc) {
		print("open error = ");
		print_dec32(rc);
		print_char('\n');
		return false;
	}
	unsigned int length;
	rc = f_read(&file, ROMBuffer, sizeof(ROMBuffer), &length);
	if (FR_OK != rc) {
		print("read error = ");
		print_dec32(rc);
		print_char('\n');
		f_close(&file);
		return false;
	}
	if (sizeof(ROMBuffer) != length) {
		print("only read: ");
		print_dec32(length);
		print(" required ");
		print_dec32(sizeof(ROMBuffer));
		print_char('\n');
		f_close(&file);
		return false;
	}
	f_close(&file);

	print("OK\n");

	// deselect SD Card and enable FLASH
	disable_card_power();
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

	print("\nPreserve Serial number");

	ReadBlock(&ROMBuffer[SerialNumberAddress], SerialNumberLength, SerialNumberAddress);

	print("\nErase");

	ChipErase();

	print("\nProgram: ");

	size_t i = 0;
	for (i = 0; i < sizeof(ROMBuffer); i += PageSize) {
		ProgramBlock(&ROMBuffer[i], PageSize, i);
		if (0 == (i & (SectorSize - 1))) {
			print_char('.');
		}
	}
	print_char('\n');

	WriteDisable();


	print("Verify: ");
	for (i = 0; i < sizeof(ROMBuffer); i += SectorSize) {

		if(VerifyBlock(&ROMBuffer[i], PageSize, i)) {
			print_char('.');
		} else {
			print_char('E');
			result = false;
		}
	}
	print_char('\n');
	return result;
}



static void SendCommand(uint8_t command)
{
	delay_us(10);
	EEPROM_CS_LO();
	SPI_put(command);
	EEPROM_CS_HI();
}


static void WaitReady(void)
{
	delay_us(10);
	EEPROM_CS_LO();
	SPI_put(SPI_ReadStatus);
	while (0 != (SPI_get() & 0x01)) {
	}
	EEPROM_CS_HI();
}

static void WriteEnable(void)
{
	SendCommand(SPI_WriteEnable);
}

static void WriteDisable(void)
{
	SendCommand(SPI_WriteDisable);
}

static void ReadBlock(uint8_t *buffer, size_t length, uint32_t ROMAddress)
{
	WaitReady();
	WriteEnable();
	EEPROM_CS_LO();
	SPI_put(SPI_FastRead);
	SPI_put(ROMAddress >> 16); // A23..A16
	SPI_put(ROMAddress >> 8);  // A15..A08
	SPI_put(ROMAddress);       // A07..A00
	(void)SPI_get();           // dummy

	size_t i = 0;
	for (i = 0; i < length; ++i) {
			*buffer++ = SPI_get();
	}
	EEPROM_CS_HI();
}

static void ProgramBlock(const uint8_t *buffer, size_t length, uint32_t ROMAddress)
{
	WaitReady();
	WriteEnable();
	EEPROM_CS_LO();
	SPI_put(SPI_PageProgram);
	SPI_put(ROMAddress >> 16); // A23..A16
	SPI_put(ROMAddress >> 8);  // A15..A08
	SPI_put(ROMAddress);       // A07..A00

	size_t i = 0;
	for (i = 0; i < length; ++i) {
		SPI_put(*buffer++);
	}
	EEPROM_CS_HI();
	WaitReady();
}

static bool VerifyBlock(const uint8_t *buffer, size_t length, uint32_t ROMAddress)
{
	bool rc = false;
	size_t i = 0;
	for (i = 0; i < length; ++i) {
		if (0xff != buffer[i]) {
			rc = true;
			break;
		}
	}
	if (!rc) {
		return true;
	}

	WaitReady();
	WriteEnable();
	EEPROM_CS_LO();
	SPI_put(SPI_FastRead);
	SPI_put(ROMAddress >> 16); // A23..A16
	SPI_put(ROMAddress >> 8);  // A15..A08
	SPI_put(ROMAddress);       // A07..A00
	(void)SPI_get();           // dummy
	for (i = 0; i < length; ++i) {
		if (SPI_get() != *buffer++) {
			rc = false;
			break;
		}
	}
	EEPROM_CS_HI();
	return rc;
}

static void ChipErase(void)
{
	WaitReady();
	WriteEnable();
	SendCommand(SPI_ChipErase);
	WaitReady();
}

static void SPI_put(uint8_t c)
{
	while (0 == (REG_SPI_STAT & TDEF)) {
	}
	REG_SPI_TXD = c;
}


static uint8_t SPI_get(void)
{
	SPI_put(0x00);
	while (0 == (REG_SPI_STAT & RDFF)) {
	}
	return REG_SPI_RXD;
}


static void fill(uint8_t value)
{
	int x = 0;
	int y = 0;
	uint8_t *fb = (uint8_t*)LCD_VRAM;

	for (y = 0; y < LCD_HEIGHT_LINES; ++y) {
		for (x = 0; x < LCD_VRAM_WIDTH_BYTES; ++x) {
			*fb++ = value;
		}
	}
}


static void display_image(const ImageType *image, uint8_t background, uint8_t toggle)
{
	int xOffset = (LCD_WIDTH_PIXELS - image->width) / (2 * 8);
	uint8_t *fb = (uint8_t*)LCD_VRAM;
	unsigned int y = 0;
	unsigned int x = 0;
	unsigned int width = (image->width + 7) / 8;
	const uint8_t *src = image->data;

	fill(background);
	fb += (LCD_HEIGHT_LINES - image->height) / 2 * LCD_VRAM_WIDTH_BYTES;
	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < width; ++x) {
			fb[x + xOffset] = toggle ^ *src++;
		}
		fb += LCD_VRAM_WIDTH_BYTES;
	}
}
