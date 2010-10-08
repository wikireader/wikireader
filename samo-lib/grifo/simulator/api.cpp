/*
 * Simulation of the GRIFO API
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <inttypes.h>
#include <fcntl.h>
#include <ctype.h>

#include "EventQueue.h"
#include "grifo.h"

void TerminateApplication(const char *format = 0, ...) __attribute__ ((noreturn));


void panic(const char *format, ...) {
	printf("PANIC: ");

	va_list arguments;

	va_start(arguments, format);

	(void)vprintf(format, arguments);

	va_end(arguments);
	TerminateApplication();  // exit with no additional message
}


// Console Debugging
// -----------------


void debug_print(const char *message) {
	puts(message);
}


int debug_print_char(int c) {
	return putchar(c);
}


int debug_printf(const char *format, ...) {
	va_list arguments;

	va_start(arguments, format);

	int rc = vprintf(format, arguments);

	va_end(arguments);

	return rc;
}


void debug_hex_dump(const void *buffer, size_t length) {
	unsigned long start = (unsigned long)buffer & 0x0f;
	unsigned long address = (unsigned long)buffer & ~0x0f;
	const uint8_t *bytes = (const uint8_t *)address;

	size_t offset;
	for (offset = 0; offset < start + length; offset += 16) {

		printf("%08lx: ", address + offset);

		int i;
		for (i = 0; i < 16; ++i) {
			if (8 == i) {
				putchar(' ');
			}
			if (offset + i < start || offset + i >= start + length) {
				puts(" --");
			} else {
				printf(" %02x", bytes[offset + i]);
			}
		}

		puts("  |");

		for (i = 0; i < 16; i++) {
			if (8 == i) {
				putchar(' ');
			}
			if (offset + i < start || offset + i >= start + length) {
				putchar(' ');
			} else if (isprint(bytes[offset + i])) {
				putchar(bytes[offset + i]);
			} else {
				putchar('.');
			}
		}

		puts("|\n");
	}
}


// Power Off / Reboot
// ------------------

void grifo_exit(int result) {
	TerminateApplication("exit with result: %d", result);
}

void chain(const char *command) {
	TerminateApplication("chain command: %s", command);
}

void power_off(void) {
	TerminateApplication("Power off");
}

void reboot(void) {
	TerminateApplication("Reboot");
}


// Interrupt Handlers
// ------------------

interrupt_handler * vector_get(vector_t vector_number) {
	TerminateApplication("get interrupt vector: %d", vector_number);
	return 0;
}

interrupt_handler * vector_set(vector_t vector_number, interrupt_handler *vector) {
	TerminateApplication("set interrupt vector: %d = %p", vector_number, vector);
	return 0;
}


// Critcal Sections
// ----------------

//critical_t critcal_enter(void) {return 0;}
//void critical_exit(critical_t) {}


// Timer and Delay
// ---------------

void delay_us(unsigned long microseconds) {
	usleep(microseconds);
}

//	TIMER_CountsPerMicroSecond = 60,

unsigned long timer_get(void) {
	return TIMER_CountsPerMicroSecond; // * current microseconds from system clock
}

void watchdog(watchdog_t) {}  // just ignore this


// Event Queue
// -----------

extern EventQueue *queue;

void event_flush(void) {
	event_t event;
	while (EVENT_NONE != event_peek(&event)) {
		event_get(&event);
	}
}

event_item_t event_get(event_t *event) {
	return queue->dequeue(event);
}

event_item_t event_wait(event_t *event, event_callback_t *callback, void *arg) {
	for (;;) {
		event_item_t item = queue->dequeue(event, 3 * 60 * 100); // milliseconds
		if (EVENT_NONE != item) {
			return event->item_type;
		}
		if (!callback(arg)) {
			TerminateApplication("event_wait timeout => power off");
		}
	}
}

event_item_t event_peek(event_t *event) {
	return queue->dequeue(event, 0); // milliseconds
}


// LCD Access
// ----------

extern uint8_t *fb;
extern int fb_max;

uint8_t *lcd_get_framebuffer(void) {
	return fb;
}

uint32_t *lcd_set_framebuffer(uint32_t *address) {
	TerminateApplication("lcd_set_framebuffer: %p", address);
	return 0;
}


uint32_t *lcd_set_default_framebuffer(void) {
	TerminateApplication("lcd_set_default_framebuffer");
	return 0;
}


void lcd_clear(lcd_colour_t colour) {
	uint8_t c = LCD_WHITE == colour ? 0x00 : 0xff;
	for (int i = 0; i < fb_max; ++i) {
		fb[i] = c;
	}
}


lcd_colour_t lcd_get_pixel(int x, int y) {
	TerminateApplication("lcd_get_pixel @ (%d, %d)", x, y);
	return LCD_BLACK;
}

void lcd_set_pixel(int x, int y, lcd_colour_t colour) {
	TerminateApplication("lcd_set_pixel @ (%d, %d) = %d", x, y, (int)colour);
}


void lcd_point(int x, int y) {
	TerminateApplication("lcd_point @ (%d, %d)", x, y);
}


void lcd_move_to(int x, int y) {
	TerminateApplication("lcd_move_to: (%d, %d)", x, y);
}

void lcd_line_to(int x, int y) {
	TerminateApplication("lcd_line_to: (%d, %d)", x, y);
}


void lcd_at_xy(int x, int y) {
	TerminateApplication("lcd_at_xy: (%d, %d)", x, y);
}

int lcd_print(const char *text) {
	TerminateApplication("lcd_print: %s", text);
	return 0;
}

int lcd_print_char(char c) {
	TerminateApplication("lcd_print_char: %c", c);
	return 0;
}

int lcd_printf(const char *format, ...) {
	TerminateApplication("lcd_printf: %s", format);
	return 0;
}

lcd_colour_t lcd_set_colour(lcd_colour_t value) {
	TerminateApplication("lcd_set_colour: %d", (int)value);
	return LCD_BLACK;
}

lcd_colour_t lcd_get_colour(void) {
	TerminateApplication("lcd_get_colour");
	return LCD_BLACK;
}

void lcd_framebuffer_set_byte(int byte_idx, uint8_t value) {
	if (byte_idx < 0 || byte_idx > fb_max) {
		TerminateApplication("lcd_framebuffer_set_byte[%d]=%d outside 0..%d", byte_idx, value, fb_max);
	}
	fb[byte_idx] = value;
}

uint8_t lcd_framebuffer_get_byte(int byte_idx) {
	if (byte_idx < 0 || byte_idx > fb_max) {
		TerminateApplication("lcd_framebuffer_get_byte[%d] outside 0..%d", byte_idx, fb_max);
	}
	return fb[byte_idx];
}



// LCD Window (picture-in-picture)
// -------------------------------

size_t lcd_window(int x, int y, int w, int h) {
	TerminateApplication("lcd_window: (%d, %d, %d, %d)", x, y, w, h);
	return 0;
}


size_t lcd_window_get_buffer_size(void) {
	TerminateApplication("lcd_window_get_buffer_size");
	return 0;
}


size_t lcd_window_get_byte_width(void) {
	TerminateApplication("lcd_window_get_byte_width");
	return 0;
}


uint8_t *lcd_window_get_buffer(void) {
	TerminateApplication("lcd_window_get_buffer");
	return 0;
}


uint32_t *lcd_window_set_buffer(uint32_t *address) {
	TerminateApplication("lcd_window_set_buffer: %p", address);
	return 0;
}


void lcd_window_disable(void) {
	TerminateApplication("lcd_window_disable");
}


void lcd_window_enable(void) {
	TerminateApplication("lcd_window_enable");
}


void lcd_window_clear(lcd_colour_t colour) {
	TerminateApplication("lcd_window_clear: %d", (int)colour);
}

lcd_colour_t lcd_window_get_pixel(int x, int y) {
	TerminateApplication("lcd_window_get_pixel @ (%d, %d)", x, y);
	return LCD_BLACK;
}

void lcd_window_set_pixel(int x, int y, lcd_colour_t colour) {
	TerminateApplication("lcd_window_set_pixel @ (%d, %d) = %d", x, y, (int)colour);
}

lcd_colour_t lcd_window_set_colour(lcd_colour_t colour) {
	TerminateApplication("lcd_window_set_colour: %d", (int)colour);
	return LCD_BLACK;
}

lcd_colour_t lcd_window_get_colour(void) {
	TerminateApplication("lcd_window_get_colour");
	return LCD_BLACK;
}

void lcd_window_point(int x, int y) {
	TerminateApplication("lcd_window_point @ (%d, %d)", x, y);
}

void lcd_window_move_to(int x, int y) {
	TerminateApplication("lcd_window_move_to: (%d, %d)", x, y);
}

void lcd_window_line_to(int x, int y) {
	TerminateApplication("lcd_window_line_to: (%d, %d)", x, y);
}

void lcd_bitmap(void *framebuffer, size_t BufferWidth, int x, int y, size_t width, size_t height, bool reverse, const uint8_t *bits) {
	TerminateApplication("lcd_bitmap: %p[%lu], (%d, %d, %d, %d) rev: %d from: %p",
			     framebuffer, BufferWidth,
			     x, y, width, height,
			     reverse, bits);
}



// Files and Directory Access
// --------------------------

#if 0
typedef enum {
	FILE_OPEN_READ = 1,
	FILE_OPEN_WRITE = 2,
	FILE_OPEN_CREATE = 4,
	FILE_OPEN_TRUNCATE = 8,
} file_access_t;
typedef enum {
	FILE_ERROR_OK			= 0,
	FILE_ERROR_NOT_READY		= -1,
	FILE_ERROR_NO_FILE		= -2,
	FILE_ERROR_NO_PATH		= -3,
	FILE_ERROR_INVALID_NAME		= -4,
	FILE_ERROR_INVALID_DRIVE	= -5,
	FILE_ERROR_DENIED		= -6,
	FILE_ERROR_EXIST		= -7,
	FILE_ERROR_RW_ERROR		= -8,
	FILE_ERROR_WRITE_PROTECTED	= -9,
	FILE_ERROR_NOT_ENABLED		= -10,
	FILE_ERROR_NO_FILESYSTEM	= -11,
	FILE_ERROR_INVALID_OBJECT	= -12,
} file_error_t;
#endif

void file_close_all(void) {
}

file_error_t file_rename(const char *OldFilename, const char *NewFilename) {
	return rename(OldFilename, NewFilename) == -1 ? FILE_ERROR_DENIED : FILE_ERROR_OK;
}

file_error_t file_delete(const char *filename) {
	return unlink(filename) == -1 ? FILE_ERROR_DENIED : FILE_ERROR_OK;
}

file_error_t file_size( const char *filename, unsigned long *length) {
	TerminateApplication("file_size: %s -> %p", filename, length);
	return FILE_ERROR_DENIED;
}

file_error_t file_create(const char *filename, file_access_t fam) {
	return file_open(filename, (file_access_t)(fam | FILE_OPEN_CREATE));
}

file_error_t file_open(const char *filename, file_access_t fam) {
	int mode = 0;

	if (0 != (fam & FILE_OPEN_READ)) {
		mode |= O_RDONLY;
	}
	if (0 != (fam & FILE_OPEN_WRITE)) {
		mode |= O_WRONLY;
	}
	if (0 != (fam & FILE_OPEN_CREATE)) {
		mode |= O_CREAT;
	}
	if (0 != (fam & FILE_OPEN_TRUNCATE)) {
		mode |= O_TRUNC;
	}
	int fd = open(filename, mode);
	return -1 == fd ? FILE_ERROR_DENIED : (file_error_t)fd;
}

file_error_t file_close(int handle) {
	close(handle);
	return FILE_ERROR_OK;
}

ssize_t file_read(int handle, void *buffer, size_t length) {
	return read(handle, buffer, length);
}

ssize_t file_write(int handle, void *buffer, size_t length) {
	return write(handle, buffer, length);
}

file_error_t file_sync(int handle) {
	fsync(handle);
	fdatasync(handle);
	return FILE_ERROR_OK;
}

file_error_t file_lseek(int handle, unsigned long pos) {
	off_t rc = lseek(handle, pos, SEEK_SET);
	return (off_t)-1  == rc ? FILE_ERROR_DENIED : FILE_ERROR_OK;
}

file_error_t directory_create(const char *directoryname) {
       int rc = mkdir(directoryname, 0777);
       return -1 == rc ? FILE_ERROR_DENIED : FILE_ERROR_OK;
}

file_error_t directory_open(const char *directoryname) {
	TerminateApplication("directory_open: %s", directoryname);
	return FILE_ERROR_DENIED;
}

file_error_t directory_close(int handle) {
	TerminateApplication("directory_close: %d", handle);
	return FILE_ERROR_DENIED;
}

ssize_t directory_read(int handle, void *buffer, size_t length) {
	TerminateApplication("directory_read: %d, %p[%lu]", handle, buffer, length);
	return 0;
}

bool directory_exists(const char *directoryname) {
	TerminateApplication("directory_exists: %s", directoryname);
	return 0;
}

file_error_t sector_read(unsigned long sector, void *buffer, int count) {
	TerminateApplication("sector_read %d -> %p[%d]", sector, buffer, count);
	return FILE_ERROR_DENIED;
}

file_error_t sector_write(unsigned long sector, const void *buffer, int count) {
	TerminateApplication("sector_write %d <- %p[%d]", sector, buffer, count);
	return FILE_ERROR_DENIED;
}


// Memory Allocation
// -----------------


void *memory_allocate(size_t size, const char *) {
	return malloc(size);
}

void memory_free(void *address, const char *) {
	free(address);
}

void memory_debug(const char *message) {
	printf("memory_debug: ignored for: %s\n", message);
}


// Analog Inputs
// -------------

long analog_input(analog_channel_t channel) {
	TerminateApplication("analog_input: %d ", channel);
	return 0;
}



// Serial Port
// -----------

int serial_putchar(int ch) {
	TerminateApplication("serial_putchar called: %c", ch);
	return 0;
}

char serial_getchar(void) {
	TerminateApplication("serial_getchar called");
	return 0;
}

bool serial_inputavailable(void) {
	TerminateApplication("serial_inputavailable called");
	return false;
}
