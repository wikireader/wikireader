/*
    e07 bootloader suite - boot menu
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

#define APPLICATION_TITLE "boot menu"

#include <stdbool.h>
#include <samo.h>
#include <lcd.h>
#include <contrast.h>
#include <analog.h>
#include <eeprom.h>

#include "application.h"

#if !defined(BATTERY_METER)
#define BATTERY_METER 1
#endif

struct guilib_image
{
	uint32_t width;
	uint32_t height;
	uint8_t data[];
};
typedef struct guilib_image ImageType;

#include "splash.h"
#include "empty.h"

#define MAXIMUM_BLOCKS 8
#define HEADER_MAGIC  0x4f4d4153
#define MAXIMUM_APPS 8
#define SERIAL_NUMBER_OFFSET 0x1fe0

// NameType length is defined in the awk script: GenerateApplicationHeader.awk
typedef char NameType[32];

struct {
	uint32_t magic;
	uint32_t count;
	NameType name[8];
} header;

char SerialNumber[32];

typedef struct {
	int block;
	int offset;
} ProcessReturnType;

static const char spinner[4] = "-\\|/";



ProcessReturnType process(int block, int status);
void print_cpu_type(void);
bool battery_empty(void);
void battery_status(void);


// this must be the first executable code as the loader executes from the first program address
ReturnType menu(int block, int status)
{
	ProcessReturnType result;

	APPLICATION_INITIALISE();
	init_lcd();
	Analog_initialise();
	Contrast_initialise();
	result = process(block, status);

	// next program
	APPLICATION_FINALISE(result.block, result.offset);
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


static void PrintName(const NameType name)
{
	int k;
	for (k = 0; k < sizeof(NameType); ++k) {
		if ('\0' == name[k] || '\xff' == name[k]) {
			break;
		}
		print_char(name[k]);
	}
}


static void DisplayInfo(void)
{
	Analog_scan();
	print("\nCPU: ");
	print_cpu_type();
	print("\nBAT: ");
	print_dec32(Analog_BatteryMilliVolts());
	print(" mV\nTMP: ");
	print_int32(Analog_TemperatureCelcius());
	print(" DegC\nLCD: ");
	print_dec32(Analog_ContrastMilliVolts());
	print(" mV\nREV: ");
	{
		int rev = board_revision();
		if (rev >= 6) {
			rev -= 5;
			print_char('V');
		} else {
			print_char('A');
		}
		print_dec32(rev);
	}
	eeprom_load(SERIAL_NUMBER_OFFSET, SerialNumber, sizeof(SerialNumber));
	print("\nS/N: ");

	int i;
	for (i = 0; i <	32; ++i) {
		const char c = SerialNumber[i];
		if ('\0' == c) {
			break;
		}
		print_char(c);
	}
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
	int i = 0;
	int k = 0;

	Analog_scan(); // update analog values
	if (battery_empty()) {
		display_image(&empty_image, 0x00, 0xff);
	} else {
		display_image(&splash_image, 0x00, 0xff);
	}

	if (0 != status) {
		bool MenuFlag = false;
		DisplayInfo();

		print("\n\nmenu? ");
		status = 0;
		for (i = 0; i <	4 * sizeof(spinner); ++i) {
			for (k = 0; k < sizeof(spinner); ++k) {
				delay_us(5000);
				print_char(spinner[k]);
				print_char('\x08');
				battery_status();
			}
			if (serial_input_available()) {
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
			}
		}

		print_char('\n');

		if (!MenuFlag) {
			rc.block = block + 1;
			rc.offset = status;
			return rc;
		}
	}

	for (;;) {
		ProcessReturnType app[MAXIMUM_APPS * MAXIMUM_BLOCKS] = {{0, 0}};

		print("\nBoot Menu\n\n");
		print("0. Power Off\n");
		print("1. Display Board Information\n");
		int MenuItem = 0;
		// not zero since this program should be in block zero
		for (i = 1; i < MAXIMUM_BLOCKS; ++i) {
			eeprom_load((i << 13), (void *)&header, sizeof(header));

			if (HEADER_MAGIC == header.magic && 0 < header.count && MAXIMUM_APPS >= header.count) {
				for (k = 0; k < header.count; ++k) {
					print_char(MenuItem + 'A');
					print(". ");
					PrintName(header.name[k]);
					print_char('\n');
					app[MenuItem].block = i;
					app[MenuItem].offset = k;
					++MenuItem;
				}
			}
		}
		print("\nEnter selection: ");
		k = ' ';
		while (k <= ' ') {
			while (!serial_input_available()) {
				switch (REG_P6_P6D & 0x07) {
				case 1:
					Contrast_set(Contrast_get() + 1);
					break;
				case 2:
					Contrast_set(Contrast_get() - 1);
					break;
				case 4:
					Contrast_set(Contrast_default);
					break;
				}
				battery_status();
				delay_us(1000);
			}
			k = serial_input_char();
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
		print("CORE unknown");
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
