#include <wikilib.h>
#include <input.h>
#include <msg.h>

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	for (;;) {
		struct wl_input_event ev;
		wl_input_wait(&ev);
		msg(MSG_INFO, "%s() got key: %d", __func__, ev.val_a);
	}

	/* never reached */
	return 0;
}

