#include <guilib.h>
#include <wikilib.h>
#include <input.h>

#include "regs.h"
#include "msg-output.h"

static char last_char = 0;

void serial_init(void)
{
}

void serial_in(int port)
{
	last_char = REG_EFSIF0_RXD;
}

void serial_out(int port)
{
	char c;
	if (get_msg_char(&c))
		REG_EFSIF0_TXD = c;
}

int serial_get_event(struct wl_input_event *ev)
{
	if (!last_char)
		return 0;

	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	ev->key_event.keycode = last_char;
	ev->key_event.value = 1;
	last_char = 0;

	return 1;
}

