/* wikilib and guilib includes */
#include <guilib.h>
#include <wikilib.h>
#include <input.h>
#include <malloc.h>

/* local includes */
#include "serial.h"
#include "traps.h"
#include "suspend.h"
#include "msg.h"
#include "touchscreen.h"

#define VERSION "0.1"

int wl_input_wait(struct wl_input_event *ev)
{
	/* wl_input_wait() is called from the wikilib mainloop and we will
	 * get here regularily when the system has no other duty. Hence,
	 * the only thing we want to do here is go to sleep - the interrupt
	 * sources are set up and will bring us back to life at some point
	 */
	
	while (1) {
		system_suspend();
		asm("halt");

		/* check whether there was any event in the system. If not,
		 * just go back to halt mode */
		if (serial_get_event(ev))
			break;

		if (touchscreen_get_event(ev))
			break;
	}

	return 0;
}

int main(void)
{
	/* machine-specific init */
	traps_init();
	serial_init();

	/* generic init */
	malloc_init();
	wikilib_init();
	guilib_init();

	msg(MSG_INFO, "Mahatma super slim kernel v%s booting.", VERSION);

	/* the next function will loop forever and call wl_input_wait() */
	wikilib_run();

	/* never reached */
	return 0;
}

