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
#include "application.h"
#include "lcd.h"
#include "eeprom.h"
#include "samo.h"


struct guilib_image
{
	uint32_t width;
	uint32_t height;
	uint8_t data[];
};
#include "splash.h"

#define MAXIMUM_BLOCKS 8
#define HEADER_MAGIC  0x4f4d4153

struct {
	uint32_t magic;
	char name[32];
} header;

static const char spinner[4] = "-\\|/";


int process(int block, int status);
void print_cpu_type(void);
void battery_status(void);


// this must be the first executable code as the loader executes from the first program address
ReturnType menu(int block, int status)
{
	register int i = 0;
	APPLICATION_INITIALISE();
	init_lcd();
	i = process(block, status);

	// next program
	APPLICATION_FINALISE(i, 0);
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
static void display_image(uint8_t background, uint8_t toggle)
{
	int xOffset = (LCD_WIDTH_PIXELS - splash_image.width) / (2 * 8);
	uint8_t *fb = (uint8_t*)LCD_VRAM;
	unsigned int y = 0;
	unsigned int x = 0;
	unsigned int width = (splash_image.width + 7) / 8;
	const uint8_t *src = splash_image.data;

	fill(background);
	fb += (LCD_HEIGHT_LINES - splash_image.height) / 2 * LCD_VRAM_WIDTH_BYTES;
	for (y = 0; y < splash_image.height; ++y) {
		for (x = 0; x < width; ++x) {
			fb[x + xOffset] = toggle ^ *src++;
		}
		fb += LCD_VRAM_WIDTH_BYTES;
	}
}

int process(int block, int status)
{
	int i = 0;
	int k = 0;
	uint8_t valid[MAXIMUM_BLOCKS] = {0};

	display_image(0x00, 0xff);

	if (0 != status) {
		print("\nCPU: ");
		print_cpu_type();
		print("\nBAT: ");
		print_dec32(get_battery_voltage());
		print(" mV\nREV: A");
		print_dec32(board_revision());

		print("\n\nmenu? ");
		for (i = 0; i <	 20; ++i) {
			for (k = 0; k < sizeof(spinner); ++k) {
				delay_us(10000);
				print_char(spinner[k]);
				print_char('\x08');
				battery_status();
			}
			if (serial_input_available()) {
				status = 0;
				break;
			}
			if (0 != (REG_P6_P6D & 0x07)) {
				status = 1;
				++block;
				break;
			}
		}
	}
	print_char('\n');

	if (0 == status) {
		print("\nBoot Menu\n\n");

		// not zero since this program should be in block zero
		for (i = 1; i < MAXIMUM_BLOCKS; ++i) {
			eeprom_load((i << 13), (void *)&header, sizeof(header));

			if (HEADER_MAGIC == header.magic) {
				print_char(('A' - 1) + i);
				print(". ");
				for (k = 0; k < sizeof(header.name); ++k) {
					if ('\0' == header.name[k] || '\xff' == header.name[k]) {
						break;
					}
					print_char(header.name[k]);
				}
				print_char('\n');
				valid[i] = 1;
			}
		}
		print("\nEnter selection: ");
		for (;;) {
			while (!serial_input_available()) {
				battery_status();
			}
			k = serial_input_char();
			if ('A' <= k && 'Z' >= k) {
				k += 'a' - 'A';
			}
			i = k - 'a' + 1;
			if (0 < i && MAXIMUM_BLOCKS > i) {
				if (valid[i]) {
					print_char(k);
					print_char('\n');
					break;
				}
			}
		}
	} else {
		i = block + 1;
	}
	return i;
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


void battery_status(void)
{
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

	unsigned int v = get_battery_voltage();
	if (v < BATTERY_EMPTY) {
		v = BATTERY_EMPTY;
	} else if (v > BATTERY_FULL) {
		v = BATTERY_FULL;
	}

	int full = 20 * (v - BATTERY_EMPTY) / (BATTERY_FULL - BATTERY_EMPTY);

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
}
