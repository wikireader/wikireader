/*
 * serial port handler
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Christopher Hall <hsw@openmoko.com>
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

#if !defined(SERIAL_H)
#define SERIAL_H 1

void serial_init(void);

typedef struct serial_buffer_struct serial_buffer_type;

typedef void serial_callback_type(serial_buffer_type *buffer);

struct serial_buffer_struct
{
	char *text;
	size_t size;
	serial_buffer_type *link;
	serial_callback_type *callback;
};

void serial_put(serial_buffer_type *buffer);

bool serial_output_pending(void);

void serial_out(int port, char c);

bool serial_event_pending(void);
int serial_get_event(struct wl_input_event *ev);

void serial_input_interrupt(void) __attribute__((interrupt_handler));
void serial_output_interrupt(void) __attribute__((interrupt_handler));

#endif
