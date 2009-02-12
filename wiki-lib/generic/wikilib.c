#include <wikilib.h>
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>
#include <search.h>
#include <string.h>

int wikilib_init (void)
{
	return 0;
}

int wikilib_run(void)
{
	void *a;
	int fd, i;

	a = malloc(512);
	msg(MSG_INFO, " a = %p\n", a);
	
	msg(MSG_INFO, " 1. run\n");
	fd = wl_open("/kernel", WL_O_RDONLY);
	msg(MSG_INFO, " fd = %d\n", fd);

	wl_read(fd, a, 512);
	wl_close(fd);

	for (i = 0; i < 10; i++) {
		msg(MSG_INFO, " 2. run\n");
		fd = wl_open("/kernel", WL_O_RDONLY);
		wl_read(fd, a, 512);
		wl_read(fd, a, 512);
		wl_read(fd, a, 512);
		wl_close(fd);
	}

//	dump_cache_stats();

	/*
	 * test searching code...
	 */
	search_init();

	for (;;) {
		struct wl_input_event ev;
		wl_input_wait(&ev);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_KEYBOARD: {
			char *result;
			int j = 0;

			if (ev.key_event.keycode == 8) {
				search_remove_char();
			} else if (isalnum(ev.key_event.keycode) ||
				    isspace(ev.key_event.keycode)) {
				msg(MSG_INFO, "Adding to search : '%c'\n",
					ev.key_event.keycode);
				search_add(ev.key_event.keycode);
			} else {
				msg(MSG_INFO, "%s() unhandled key: %d\n", __func__,
					ev.key_event.keycode);
				continue;
			}

			while (j++ < 5 && (result = search_fetch_result()))
				msg(MSG_INFO, "Result: %s\n", result);
			search_print_stats();

			break;
		}
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

