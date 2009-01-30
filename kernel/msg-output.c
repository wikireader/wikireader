#include <stdio.h>
#include <stdarg.h>
#include <regs.h>

#include "types.h"
#include "msg.h"
#include "irq.h"

#define MAX_MSGS	100
#define MSG_LEN		80

struct message {
	int level;
	char text[MSG_LEN];
};

static struct message messages[MAX_MSGS];
static int current_msg_read = 0;
static int current_msg_write = 0;
static int lost_messages = 0;

int get_msg_char(char *c)
{
	static int current_char = 0;
	
	while (1) {
		struct message *m;

		if (current_msg_read == current_msg_write)
			return 0;

		m = messages + current_msg_read;
		*c = m->text[current_char];

		if (*c == '\0') {
			current_msg_read++;
			current_char = 0;
			continue;
		}

		current_char++;
		break;
	}

	if (lost_messages) {
		msg(MSG_WARNING, "%d messages lost.", lost_messages);
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
}

