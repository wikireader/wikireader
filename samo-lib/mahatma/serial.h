/*
 * mahatma - a simple kernel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
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

#ifndef SERIAL_H
#define SERIAL_H

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

int serial_get_event(struct wl_input_event *ev);

void serial_filled_0(void);
void serial_drained_0(void);

#endif /* SERIAL_H */

