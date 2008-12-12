#include <wikilib.h>
#include <input.h>
#include <msg.h>
#include <search-test.h>

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	search_test();

	/* the above function does never return, so the code here
	 * is unreached. I still leave it in as reference */

	for (;;) {
		struct wl_input_event ev;
		wl_input_wait(&ev);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_KEYBOARD:
			msg(MSG_INFO, "%s() got key: %d", __func__, ev.key_event.keycode);
			break;
		case WL_INPUT_EV_TYPE_TOUCH:
			msg(MSG_INFO, "%s() touch event @%d,%d val %d\n", __func__,
				ev.touch_event.x,
				ev.touch_event.y,
				ev.touch_event.value);
			break;
		}
	}

	/* never reached */
	return 0;
}

