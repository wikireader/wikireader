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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <regs.h>
#include <wikilib.h>
#include <input.h>
#include <interrupt.h>

#include "msg.h"
#include "serial.h"

typedef char message_line_type[128];

static serial_buffer_type messages[50];
static message_line_type lines[ARRAY_SIZE(messages)];

static serial_buffer_type *free_list;

static uint32_t lost_messages = 0;

serial_callback_type free_buffer;

// output everything by default
static int loglevel = MSG_LEVEL_MAX;


void msg_init(void)
{
	static bool initialised = false;
	if (!initialised) {
		size_t i = 0;
		free_list = NULL;

		// create linked list of free buffers
		for (i = 0; i < ARRAY_SIZE(messages); ++i) {
			messages[i].text = lines[i];
			messages[i].size = sizeof(lines[i]);
			messages[i].callback = free_buffer;
			messages[i].link = free_list;
			free_list = &messages[i];
		}

		serial_init();

		initialised = true;
	}
}


void msg(int level, const char *fmt, ...)
{
	serial_buffer_type *m;
	va_list va;

	if (level > loglevel) {
		return;
	}

	if (NULL == free_list) {
		lost_messages++;
		return;
	}

	va_start(va, fmt);

	{
		// critcal code, since free is called from interrupt state
		InterruptType s = Interrupt_disable();
		m = free_list;
		free_list = free_list->link;
		Interrupt_enable(s);
	}

	//m->level = level;
	vsnprintf(m->text, m->size, fmt, va);

	va_end(va);

	serial_put(m);
}


// this is called at interrupt state, keep it short
void free_buffer(serial_buffer_type *buffer)
{
	buffer->link = free_list;
	free_list = buffer;
}


void set_loglevel(int level)
{
	if (level < MSG_ERROR || level > MSG_LEVEL_MAX) {
		return;
	}
	loglevel = level;
}
