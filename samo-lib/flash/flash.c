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
#include <print.h>
#include <lcd.h>
#include <FLASH.h>

#include "program.h"
#include "ok.h"
#include "fail.h"


// size of the flash memory
uint8_t ROMBuffer[FLASH_TotalBytes];

enum {
	ProgramRetries = 5,
};


static bool process(const char *filename);

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

	FLASH_initialise();

	if (0 == arg) {
		print("Internal");
		FLASH_SelectInternal();
	} else {
		print("Test Jig");
		filename = "test-jig.rom";
		FLASH_SelectExternal();
	}

	print(" FLASH\n");

	int i = 0;
	bool flag = false;
	for (i = 0; i < ProgramRetries; ++i) {
		print("Begin Pass: ");
		print_int(i);
		print_char('\n');
		LCD_DisplayImage(LCD_PositionTop, true, &program_image);
		if (process(filename)) {
			print("Finished sucessfully\n");
			LCD_DisplayImage(LCD_PositionTop, true, &ok_image);
			flag = true;
			break;
		} else {
			print("Error occurred\n");
			LCD_DisplayImage(LCD_PositionTop, true, &fail_image);
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
		print_int(rc);
		print_char('\n');
		return false;
	}

	unsigned int length;
	rc = f_read(&file, ROMBuffer, sizeof(ROMBuffer), &length);
	if (FR_OK != rc) {
		print("read error = ");
		print_int(rc);
		print_char('\n');
		f_close(&file);
		return false;
	}
	if (sizeof(ROMBuffer) != length) {
		print("only read: ");
		print_int(length);
		print(" required ");
		print_int(sizeof(ROMBuffer));
		print_char('\n');
		f_close(&file);
		return false;
	}
	f_close(&file);

	print("OK\n");

	print("Preserve Serial number: ");

	FLASH_read(&ROMBuffer[FLASH_SerialNumberAddress], FLASH_SerialNumberSize, FLASH_SerialNumberAddress);

	size_t i;
	for (i = 0; i < FLASH_SerialNumberSize; ++i) {
		char c = ROMBuffer[FLASH_SerialNumberAddress + i];
		if (' ' > c || '\xff' == c) {
			break;
		}
		print_char(c);
	}

	print("\nErase: ");

	if (FLASH_WriteEnable() && FLASH_ChipErase()) {
		print("OK");
	} else {
		print("FAIL\n");
		return false;
	}

	print("\nProgram: ");

	for (i = 0; i < sizeof(ROMBuffer); i += FLASH_PageSize) {
		if (!FLASH_WriteEnable() || !FLASH_write(&ROMBuffer[i], FLASH_PageSize, i)) {
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
