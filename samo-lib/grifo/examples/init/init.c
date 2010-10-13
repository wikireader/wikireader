/*
 * init - the first user process to run
 *
 * Copyright (c) 2010 Openmoko Inc.
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

#include "grifo.h"

#include <string.h>
#include <ctype.h>


// icon layout
enum {
	X_COUNT = 3,
	Y_COUNT = 3,
	X_START = 8,
	Y_START = 4,
	X_GAP = 8,
	Y_GAP = 4,
	X_SIZE = 64,
	Y_SIZE = 64,
	ICON_BYTE_SIZE = X_SIZE * Y_SIZE / 8,
	TOTAL_ICONS = X_COUNT * Y_COUNT,
};


struct {
	bool ok;
	const char *IconName;
	const char *command;
	int x;
	int y;
	uint8_t icon[ICON_BYTE_SIZE];
} MenuRecord[TOTAL_ICONS];


char buffer[65536];


bool ReadCommands(const char *filename);
int MenuHandler(void);
int CursorPosition(int x, int y);
bool ReadIcon(void *icon, ssize_t size, const char *filename);


int grifo_main(int argc, char *argv[])
{
	debug_print("init starting\n");

	if (!ReadCommands("init.ini")) {
		return EXIT_POWER_OFF;
	}

	// run the auto-boot item 0
	if (argc > 1 && 0 == strcmp("auto-boot", argv[1]) && MenuRecord[0].ok) {
		chain(MenuRecord[0].command);
	}

	// menu for non-auto-boot
	int m = MenuHandler();
	if (MenuRecord[m].ok) {
		chain(MenuRecord[m].command);
	}

	return EXIT_POWER_OFF;
}


bool ReadCommands(const char *filename)
{
	int handle = file_open(filename, FILE_OPEN_READ);
	if (handle < 0) {
		debug_printf("init: open error = %d\n", handle);
		return false;
	}

	ssize_t r = file_read(handle, buffer, sizeof(buffer));
	(void)file_close(handle);
	if (r == 0) {
		debug_print("init: empty file\n");
		return false;
	} else if (r < 0) {
		debug_printf("init: read error = %ld\n", r);
		return false;
	}

	memset(MenuRecord, 0, sizeof(MenuRecord));

	enum {
		STATE_SkipSpaces,
		STATE_ignore,
		STATE_icon,
		STATE_StartCommand,
		STATE_EndCommand,
	} state = STATE_SkipSpaces;
	size_t item = 0;
	ssize_t i;
	for (i = 0; i < r; ++i) {
		char c = buffer[i];

		// parse:
		// <spaces> <icon> <spaces> : <spaces> <command>
		switch (state) {

		case STATE_SkipSpaces:
			if (';' == c || '#' == c) {
				state = STATE_ignore;
			} else if (!isspace(c)) {
				MenuRecord[item].IconName = &buffer[i];
				state =  STATE_icon;
				if ('\n' == c || ':' == c) {
					buffer[i] = '\0';
					state =  STATE_SkipSpaces;  // ignore blank icon
				}
			}
			break;

		case STATE_ignore:
			if ('\n' == c) {
				state =  STATE_SkipSpaces;
			}
			break;

		case STATE_icon:
			if ('\n' == c) {
				buffer[i] = '\0';
				state =  STATE_SkipSpaces;         // ignore if command is missing
			} else if (':' == c) {
				buffer[i] = '\0';
				size_t j;
				for (j = i - 1; j > 0 && isspace(buffer[j]) ; --j) {
					buffer[j] = '\0';
				}
				if (ReadIcon(MenuRecord[item].icon, sizeof(MenuRecord[item].icon),
					     MenuRecord[item].IconName)) {
					state = STATE_StartCommand;
				} else {
					state =  STATE_SkipSpaces;  // ignore invalid icon
				}
			}
			break;

		case STATE_StartCommand:
			if (!isspace(c)) {
				MenuRecord[item].command = &buffer[i];
				state =  STATE_EndCommand;
				if ('\n' == c) {
					buffer[i] = '\0';
					state =  STATE_SkipSpaces;  // ignore blank commands
				}
			}
			break;

		case STATE_EndCommand:
			if ('\n' == c) {
				MenuRecord[item].ok = true;
				buffer[i] = '\0';
				state =  STATE_SkipSpaces;
				size_t j;
				for (j = i - 1; j > 0 && isspace(buffer[j]) ; --j) {
					buffer[j] = '\0';
				}
				++item;
				if (item >= SizeOfArray(MenuRecord)) {
					return true;
				}
			}
			break;
		}
	}

	return item > 0;
}


int MenuHandler(void)
{
	lcd_clear(LCD_WHITE);

	int x;
	int y;
	size_t i = 0;
	for (y = 0; y < Y_COUNT; ++y) {
		for (x = 0; x < X_COUNT; ++x) {
			if (MenuRecord[i].ok) {
				MenuRecord[i].x = X_START + (X_SIZE + X_GAP) * x;
				MenuRecord[i].y = Y_START + (Y_SIZE + Y_GAP) * y;
				lcd_bitmap(lcd_get_framebuffer(), LCD_BUFFER_WIDTH_BYTES,
					   MenuRecord[i].x, MenuRecord[i].y,
					   X_SIZE, Y_SIZE, false, MenuRecord[i].icon);
			}
			++i;
		}
	}


	event_flush();

	int cursor = -1;  // valid cursor positions are: 0 .. SizeOfArray(MenuRecord) - 1

	for (;;) {
		event_t event;

		switch(event_wait(&event, NULL, NULL)) {  // no callback, just power off

		case EVENT_NONE:
			break;

		case EVENT_KEY:
			break;

		case EVENT_TOUCH_DOWN:
		case EVENT_TOUCH_MOTION:
		{
			int position = CursorPosition(event.touch.x, event.touch.y);

			if (cursor != position) {
				if (cursor >= 0) {
					lcd_bitmap(lcd_get_framebuffer(), LCD_BUFFER_WIDTH_BYTES,
						   MenuRecord[cursor].x, MenuRecord[cursor].y,
						   X_SIZE, Y_SIZE, false, MenuRecord[cursor].icon);
				}
				if (position >= 0 && position < (int)SizeOfArray(MenuRecord)
				    && MenuRecord[position].ok) {
					cursor = position;
					lcd_bitmap(lcd_get_framebuffer(), LCD_BUFFER_WIDTH_BYTES,
						   MenuRecord[cursor].x, MenuRecord[cursor].y,
						   X_SIZE, Y_SIZE, true, MenuRecord[cursor].icon);
				} else {
					cursor = -1;
				}
			}
		}
		break;

		case EVENT_TOUCH_UP:
			if (cursor >= 0 && cursor < (int)SizeOfArray(MenuRecord)) {
				chain(MenuRecord[cursor].command);
			}
			break;

		case EVENT_BUTTON_DOWN:
			break;

		case EVENT_BUTTON_UP:
			break;

		case EVENT_BATTERY_LOW:
			break;
		}
	}
	return EXIT_POWER_OFF;
}


int CursorPosition(int x, int y)
{
	y -= Y_START;
	x -= X_START;

	if (y < 0 || y < 0) {
		return -1;
	}

	int cursor = -1;
	int n;

	for (n = 0; n < Y_COUNT; ++n) {
		if (y < Y_SIZE) {
			cursor = X_COUNT * n;
			break;
		}
		y -= Y_SIZE + Y_GAP;
		if (y < 0) {
			return -1;
		}
	}

	for (n = 0; n < X_COUNT; ++n) {
		if (x < X_SIZE) {
			return cursor + n;
			break;
		}
		x -= X_SIZE + X_GAP;
		if (x < 0) {
			return -1;
		}
	}
	return -1;
}


bool ReadIcon(void *icon, ssize_t size, const char *filename)
{
	int handle = file_open(filename, FILE_OPEN_READ);
	if (handle < 0) {
		debug_printf("init: icon open error = %d\n", handle);
		return false;
	}

	ssize_t r = file_read(handle, icon, size);
	(void)file_close(handle);
	if (r != size) {
		debug_print("init: invalid icon file\n");
		return false;
	} else if (r < 0) {
		debug_printf("init: icon read error = %ld\n", r);
		return false;
	}

	return true;
}
