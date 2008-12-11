#include <t_services.h>
#include "search.h"
#include <stdio.h>
#include <string.h>

#include <wikilib.h>
#include <input.h>
#include "sample1.h"

int wl_input_wait(struct wl_input_event *ev)
{
	ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
	syscall(serial_rea_dat(TASK_PORTID, &ev->val_a, 1));

	/* force the result to 8bit */
	ev->val_a &= 0xff;

	/* key press only from console, no key release */
	ev->val_b = 1;
	
	return 0;
}

