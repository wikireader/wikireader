/*
 * flash - stand-alone FLASH ROM upgrade program
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
#include <tff.h>
#include <diskio.h>
#include <delay.h>
#include <misc.h>
#include <regs.h>
#include <samo.h>
#include <lcd.h>
#include <FLASH.h>


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
	ProgramRetries = 5,
	SerialNumberAddress = 0x1fe0,
	SerialNumberLength = 32,
};


static bool process(const char *filename);
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
	bool flag = false;
	for (i = 0; i < ProgramRetries; ++i) {
		print("Begin Pass: ");
		print_dec32(i);
		print_char('\n');
		display_image(&program_image, 0x00, 0xff);
		if (process(filename)) {
			print("Finished sucessfully\n");
			display_image(&ok_image, 0x00, 0xff);
			flag = true;
			break;
		} else {
			print("Error occurred\n");
			display_image(&fail_image, 0x00, 0xff);
		}
	}

	print(flag ? "PASS" : "FAIL");
	print(": FLASH MBR\n");
	for (;;) {
	}
}

static bool process(const char *filename)
{
	bool result = true;
	uint8_t b = 0;

	// power cycle the SD Card
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

	FLASH_initialise();

	print("\nPreserve Serial number");

	FLASH_read(&ROMBuffer[SerialNumberAddress], SerialNumberLength, SerialNumberAddress);

	print("\nErase");

	FLASH_ChipErase();

	print("\nProgram: ");

	size_t i = 0;
	for (i = 0; i < sizeof(ROMBuffer); i += FLASH_PageSize) {
		if (!FLASH_write(&ROMBuffer[i], FLASH_PageSize, i)) {
			print_char('E');
		}
		if (0 == (i & (FLASH_SectorSize - 1))) {
			print_char('.');
		}
	}
	print_char('\n');

	print("Verify: ");
	for (i = 0; i < sizeof(ROMBuffer); i += FLASH_SectorSize) {

		if(FLASH_verify(&ROMBuffer[i], FLASH_SectorSize, i)) {
			print_char('.');
		} else {
			print_char('E');
			result = false;
		}
	}
	print_char('\n');
	return result;
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
