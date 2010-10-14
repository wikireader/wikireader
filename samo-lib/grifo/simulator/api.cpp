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

#include "standard.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <inttypes.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#include "EventQueue.h"
#include "grifo.h"
#include "FrameBuffer.h"
#include "graphics.h"


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

	char buffer[256];
	char *ArgumentStrings[12]; // program name + N-1 arguments + null

	size_t ArgumentCount = 0;
	const char *source = command;
	char *destination = buffer;

	memset(ArgumentStrings, 0, sizeof(ArgumentStrings));

	// parse something like: --option="isn't this easy"', '"it's ok"' and "quotes" can be used'
	while ('\0' != *source && ArgumentCount < SizeOfArray(ArgumentStrings) - 1) {
		while (isspace(*source)) {
			++source;
		}
		if ('\0' == *source) {
			break;
		}

		char quote = '\0';
		*destination = '\0';

		ArgumentStrings[ArgumentCount++] = destination;

		while ('\0' != *source && destination < &buffer[sizeof(buffer) - 1]) {
			if ('\0' != quote) {
				while ('\0' != *source && quote != *source) {
					*destination++ = *source++;
					if (destination >= &buffer[sizeof(buffer) - 1]) {
						break;
					}
				}
				if ('\0' != *source) {
					quote = '\0';
					++source;
				}
			} else if (isspace(*source)) {
				break;
			} else if ('"' == *source || '\'' == *source) {
				quote = *source++;
			} else {
				*destination++ = *source++;
			}
		}
		*destination++ = '\0';
		if (destination >= &buffer[sizeof(buffer)]) {
			break;
		}
	}
	// ensure final terminator
	buffer[sizeof(buffer) - 1] = '\0';

	int rc = execv(ArgumentStrings[0],  ArgumentStrings);

	TerminateApplication("chain command failed(%d) on: %s", rc, command);
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


// Note: TIMER_CountsPerMicroSecond is the
extern unsigned long TimeStamp();

unsigned long timer_get(void) {
	return TimeStamp();
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
	if (EVENT_NONE == queue->head(event)) {
		return EVENT_NONE;
	}
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
	return queue->head(event);
}


// LCD Access
// ----------

extern FrameBuffer *fb;

static int GraphicX = 0;
static int GraphicY = 0;
static int TextRow = 0;
static int TextColumn = 0;
static lcd_colour_t ForegroundColour = LCD_BLACK;


static inline int pos(int x, int y) {
	return y * fb->RowSize() + (x >> 3);
}

uint8_t *lcd_get_framebuffer(void) {
	return fb->address();
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
	for (int i = 0; i < fb->size(); ++i) {
		fb->address()[i] = c;
	}
	GraphicX = 0;
	GraphicY = 0;
	TextRow = 0;
	TextColumn = 0;
	ForegroundColour = colour == LCD_WHITE ? LCD_BLACK : LCD_WHITE;
}


lcd_colour_t lcd_get_pixel(int x, int y) {
	if (x < 0 || x >= LCD_WIDTH ||
	    y < 0 || y >= LCD_HEIGHT) {
		return LCD_BLACK;
	}
	return 0 != (fb->address()[pos(x, y)] & (0x80 >> (x & 0x07))) ? LCD_BLACK : LCD_WHITE;
}

static void set_pixel(int x, int y, uint32_t colour) {
	if (x < 0 || x >= LCD_WIDTH ||
	    y < 0 || y >= LCD_HEIGHT) {
		return;
	}
	if (LCD_BLACK == colour) {
		fb->address()[pos(x, y)] |= (0x80 >> (x & 0x07));
	} else {
		fb->address()[pos(x, y)] &= ~(0x80 >> (x & 0x07));
	}
}


void lcd_set_pixel(int x, int y, lcd_colour_t colour) {
	set_pixel(x, y, (uint32_t)colour);
}


void lcd_point(int x, int y) {
	lcd_move_to(x, y);
	lcd_set_pixel(x, y, ForegroundColour);
}


void lcd_move_to(int x, int y) {
	GraphicX = Standard_ClipValue(x, 0, fb->w() - 1);
	GraphicY = Standard_ClipValue(y, 0, fb->h() - 1);
}

void lcd_line_to(int x, int y) {
	register int x0 = GraphicX;
	register int y0 = GraphicY;
	lcd_move_to(x, y);
	Graphics_DrawLine(x0, y0, GraphicX, GraphicY, ForegroundColour, set_pixel);
}


void lcd_at_xy(int x, int y) {
	TextRow = Standard_ClipValue(y, 0, lcd_max_rows() - 1);
	TextColumn = Standard_ClipValue(x, 0, lcd_max_columns() - 1);
}

void lcd_print(const char *text) {
	while ('\0' != *text) {
		lcd_print_char(*text++);
	}
}

static void scroll(void) {
	TextColumn = 0;
	++TextRow;
	if (TextRow >= lcd_max_rows()) {
		TextRow = lcd_max_rows() - 1;

		uint8_t *p = lcd_get_framebuffer();
		const uint32_t one_line =  Graphics_FontHeight() * fb->RowSize();

		memcpy(p, p + one_line, fb->size() - one_line);
		memset(p + fb->size() - one_line, ForegroundColour == LCD_BLACK ? 0 : 0xff, one_line);
	}
}

int lcd_print_char(char c) {
	if ('\r' == c) {
		TextColumn = 0;
		return c;
	}
	if ('\n' == c) {
		scroll();
		return c;
	}

	if (TextColumn >= lcd_max_columns()) {
		scroll();
	}
	const uint8_t *font = Graphics_GetFont(c);
	uint8_t *p = lcd_get_framebuffer();

	int h = Graphics_FontHeight();
	int w = Graphics_FontWidth();
	p += w / 8 * TextColumn + h * fb->RowSize() * TextRow;

	int i;
	if (LCD_BLACK == ForegroundColour) {
		for (i = 0; i < h; ++i, p += fb->RowSize()) {
			*p = *font++;
		}
	} else {
		for (i = 0; i < h; ++i, p += fb->RowSize()) {
			*p = ~*font++;
		}
	}
	++TextColumn;
	return c;
}

int lcd_printf(const char *format, ...) {
	va_list arguments;

	va_start(arguments, format);

	char *p = NULL;
	int rc = vasprintf(&p, format, arguments);

	lcd_print(p);
	free(p);

	va_end(arguments);

	return rc;
}

lcd_colour_t lcd_set_colour(lcd_colour_t colour) {
	register lcd_colour_t previous = ForegroundColour;
	ForegroundColour = colour;
	return previous;
}

lcd_colour_t lcd_get_colour(void) {
	return ForegroundColour;
}

void lcd_framebuffer_set_byte(int byte_idx, uint8_t value) {
	if (byte_idx < 0 || byte_idx >= fb->size()) {
		TerminateApplication("lcd_framebuffer_set_byte[%d]=%d outside 0..%d-1", byte_idx, value, fb->size());
	}
	fb->address()[byte_idx] = value;
}

uint8_t lcd_framebuffer_get_byte(int byte_idx) {
	if (byte_idx < 0 || byte_idx > fb->size()) {
		TerminateApplication("lcd_framebuffer_get_byte[%d] outside 0..%d", byte_idx, fb->size());
	}
	return fb->address()[byte_idx];
}


int lcd_max_columns(void) {
	return fb->w() / Graphics_FontWidth();
}


int lcd_max_rows(void) {
	return fb->h() / Graphics_FontHeight();
}


// LCD Window (picture-in-picture)
// -------------------------------


static int WindowGraphicX = 0;
static int WindowGraphicY = 0;
static int WindowTextRow = 0;
static int WindowTextColumn = 0;
static lcd_colour_t WindowForegroundColour = LCD_BLACK;


size_t lcd_window(int x, int y, int w, int h) {
	WindowGraphicX = 0;
	WindowGraphicY = 0;
	WindowTextRow = 0;
	WindowTextColumn = 0;
	WindowForegroundColour = LCD_BLACK;
	return fb->SetWindow(x, y, w, h);
}


size_t lcd_window_get_buffer_size(void) {
	return fb->WindowSize();
}


size_t lcd_window_get_byte_width(void) {
	return fb->WindowRowSize();
}


uint8_t *lcd_window_get_buffer(void) {
	return fb->WindowAddress();
}


uint32_t *lcd_window_set_buffer(uint32_t *address) {
	TerminateApplication("lcd_window_set_buffer: %p", address);
	return 0;
}


void lcd_window_disable(void) {
	return fb->SetWindowEnable(false);
}


void lcd_window_enable(void) {
	return fb->SetWindowEnable(true);
}


void lcd_window_clear(lcd_colour_t colour) {
	if (!fb->WindowAvailable()) {
		return;
	}
	int i;
	uint8_t fill = colour == LCD_WHITE ? 0 : ~0;
	for (i = 0; i < fb->WindowSize(); ++i) {
		fb->WindowAddress()[i] = fill;
	}
	WindowGraphicX = 0;
	WindowGraphicY = 0;
	WindowTextRow = 0;
	WindowTextColumn = 0;
	WindowForegroundColour = colour == LCD_WHITE ? LCD_BLACK : LCD_WHITE;
}


static inline int WindowPos(int x, int y) {
	return y * fb->WindowRowSize() + (x >> 3);
}


lcd_colour_t lcd_window_get_pixel(int x, int y) {
	if (!fb->WindowAvailable() ||
	    x < 0 || x >= fb->WindowW() ||
	    y < 0 || y >= fb->WindowH()) {
		return LCD_BLACK;
	}
	return 0 != (fb->WindowAddress()[WindowPos(x, y)] & (0x80 >> (x & 0x07))) ? LCD_BLACK : LCD_WHITE;
}


static void window_set_pixel(int x, int y, uint32_t colour) {
	if (!fb->WindowAvailable() ||
	    x < 0 || x >= fb->WindowW() ||
	    y < 0 || y >= fb->WindowH()) {
		return;
	}
	if (LCD_BLACK == colour) {
		fb->WindowAddress()[WindowPos(x, y)] |= (0x80 >> (x & 0x07));
	} else {
		fb->WindowAddress()[WindowPos(x, y)] &= ~(0x80 >> (x & 0x07));
	}
}


void lcd_window_set_pixel(int x, int y, lcd_colour_t colour) {
	window_set_pixel(x, y, (uint32_t)colour);
}


lcd_colour_t lcd_window_set_colour(lcd_colour_t colour) {
	register lcd_colour_t previous = WindowForegroundColour;
	WindowForegroundColour = colour;
	return previous;
}


lcd_colour_t lcd_window_get_colour(void) {
	return WindowForegroundColour;
}


void lcd_window_point(int x, int y) {
	lcd_window_move_to(x, y);
	lcd_window_set_pixel(x, y, WindowForegroundColour);
}


void lcd_window_move_to(int x, int y) {
	WindowGraphicX = Standard_ClipValue(x, 0, fb->WindowW() - 1);
	WindowGraphicY = Standard_ClipValue(y, 0, fb->WindowH() - 1);
}


void lcd_window_line_to(int x, int y) {
	register int x0 = WindowGraphicX;
	register int y0 = WindowGraphicY;
	lcd_window_move_to(x, y);
	Graphics_DrawLine(x0, y0, WindowGraphicX, WindowGraphicY, WindowForegroundColour, window_set_pixel);
}


void lcd_bitmap(void *framebuffer, size_t BufferWidth, int x, int y, size_t width, size_t height,
		bool reverse, const uint8_t *bits) {
	Graphics_PutBitMap(framebuffer, BufferWidth, x, y, width, height, reverse, bits);
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

file_error_t file_size(const char *filename, unsigned long *length) {
	struct stat sb;
	*length = 0;
	if (0 != stat(filename, &sb)) {
		return FILE_ERROR_DENIED;
	}
	*length = sb.st_size;
	return FILE_ERROR_OK;
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
	struct stat sb;
	return 0 == stat(directoryname, &sb) && S_ISDIR(sb.st_mode);
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


void *memory_allocate(size_t size, const char *tag) {
	if (0 == size) {
		TerminateApplication("memory_allocate zero bytes: %s", tag);
	}
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
