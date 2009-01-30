/* wikilib and guilib includes */
#include <guilib.h>
#include <wikilib.h>
#include <input.h>

/* local includes */
#include "serial.h"
#include "traps.h"

int wl_input_wait(struct wl_input_event *ev)
{
	/* wl_input_wait() is called from the wikilib mainloop and we will
	 * get here regularily when the system has no other duty. Hence,
	 * the only thing we want to do here is go to sleep - the interrupt
	 * sources are set up and will bring us back to life at some point
	 */
	
	while (1) {
		asm("halt");
		if (serial_input_parse(ev))
			break;
	}
}

int main(void)
{
	/* machine-specific init */
	traps_init();
	serial_init();

	/* generic init */
	wikilib_init();
	guilib_init();

	/* the next function will loop forever and call wl_input_wait() */
	wikilib_run();

	/* never reached */
	return 0;
}

