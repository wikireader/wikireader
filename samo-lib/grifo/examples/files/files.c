/*
 * files - an simple example program
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

#include <string.h>

#include "grifo.h"


int grifo_main(int argc, char *argv[])
{
	if (1 >= argc) {
		debug_printf("usage: %s command parameters\n", argv[0]);
		debug_printf("commands: mkdir ls rm rename display\n");
		return 1;
	}

	if (strcmp(argv[1], "mkdir") == 0) {
		int i;
		for (i = 2; i < argc; ++i) {
			debug_printf("mkdir '%s' => %d\n", argv[i], directory_create(argv[i]));
		}
	} else if (strcmp(argv[1], "ls") == 0) {
		int i;
		for (i = 2; i < argc; ++i) {
			debug_printf("ls '%s'\n", argv[i]);
			int handle = directory_open(argv[i]);
			if (handle < 0) {
				debug_printf("error = %d\n", handle);
			} else {
				for (;;) {
					char buffer[256];
					ssize_t r = directory_read(handle, buffer, sizeof(buffer));
					if (r == 0) {
						break;
					} else if (r < 0) {
						debug_printf("error = %ld\n", (long)r);
						break;
					}
					int j;
					for (j = 0; j < r; ++j) {
						debug_print_char(buffer[j]);
					}
					debug_print_char('\n');
				}
				(void)directory_close(handle);
			}
		}
	} else if (strcmp(argv[1], "rename") == 0 && argc == 4) {
		debug_printf("rename '%s' -> '%s' => %d\n", argv[2], argv[3], file_rename(argv[2], argv[3]));
	} else if (strcmp(argv[1], "create") == 0) {
		int i;
		for (i = 2; i < argc; ++i) {
			debug_printf("create '%s'\n", argv[i]);
			int handle = file_create(argv[i], FILE_OPEN_WRITE);
			if (handle < 0) {
				debug_printf("error = %d\n", handle);
			} else {
				int j;
				for (j = 0; j < 5; ++j) {
#define line_1 "This is just some test data\n"
					ssize_t r = file_write(handle, line_1, sizeof(line_1) - 1);
					if (r == 0) {
						break;
					} else if (r < 0) {
						debug_printf("error = %ld\n", (long)r);
						break;
					}
					debug_printf("wrote %ld bytes\n", (long)r);
				}
				(void)file_close(handle);
			}
		}
	} else if (strcmp(argv[1], "display") == 0) {
		int i;
		for (i = 2; i < argc; ++i) {
			debug_printf("display '%s'\n", argv[i]);
			int handle = file_open(argv[i], FILE_OPEN_READ);
			if (handle < 0) {
				debug_printf("error = %d\n", handle);
			} else {
				for (;;) {
					char buffer[256];
					ssize_t r = file_read(handle, buffer, sizeof(buffer));
					if (r == 0) {
						break;
					} else if (r < 0) {
						debug_printf("error = %ld\n", (long)r);
						break;
					}
					int j;
					for (j = 0; j < r; ++j) {
						debug_print_char(buffer[j]);
					}
				}
				(void)file_close(handle);
			}
		}
	} else {
		debug_printf("unknown command: '%s'\n", argv[1]);
		return 1;
	}

	return 0;
}
