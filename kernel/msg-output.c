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

#include <stdio.h>
#include <stdarg.h>
#include <regs.h>

#include <input.h>
#include "types.h"
#include "msg.h"
#include "irq.h"
#include "serial.h"

#define MAX_MSGS	100
#define MSG_LEN		80

struct message {
	int level;
	char text[MSG_LEN];
};

static struct message messages[MAX_MSGS];
static unsigned int current_msg_read = 0;
static unsigned int current_msg_write = 0;
static int lost_messages = 0;
static unsigned int output_newline = 0;

int get_msg_char(char *c)
{
	static int current_char = 0;

	if (output_newline) {
		*c = '\r';
		output_newline = 0;
		return 1;
	}

	while (1) {
		struct message *m;

		if (current_msg_read == current_msg_write)
			return 0;

		m = messages + current_msg_read;
		*c = m->text[current_char];

		if (*c == '\0') {
			current_msg_read++;
			current_msg_read %= MAX_MSGS;
			current_char = 0;
			continue;
		}

		if (*c == '\n')
			output_newline = 1;
		
		current_char++;
		break;
	}

	if (lost_messages) {
		msg(MSG_WARNING, "%d messages lost.\n", lost_messages);
		lost_messages = 0;
	}

	return 1;
}

void msg(int level, const char *fmt, ...)
{
	struct message *m;
	va_list va;

	if ((current_msg_write + 1) % MAX_MSGS == MAX_MSGS) {
		lost_messages++;
		return;
	}

	va_start(va, fmt);
	DISABLE_IRQ();
	m = messages + current_msg_write;
	m->level = level;
	vsnprintf(m->text, MSG_LEN - 1, fmt, va);
	current_msg_write++;
	current_msg_write %= MAX_MSGS;
	ENABLE_IRQ();
	va_end(va);

	if (!serial_transfer_running(0)) {
		char c;
		if (get_msg_char(&c))
			serial_out(0, c);
	}
}

