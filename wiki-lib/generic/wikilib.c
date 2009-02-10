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
	int fd, i, j;
	char *result;

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
	search_reset();

	char *search_string = "india";
	for (i = 0; i < (int)strlen(search_string); ++i) {
		msg(MSG_INFO, "Adding to search : '%c'\n", search_string[i]);
		search_add(search_string[i]);
		j = 0;
		while (j++ < 5 && (result = search_fetch_result()))
			msg(MSG_INFO, "Result: %s\n", result);
		search_print_stats();
	}

	for (;;) {
		struct wl_input_event ev;
		wl_input_wait(&ev);

		switch (ev.type) {
		case WL_INPUT_EV_TYPE_KEYBOARD:
			msg(MSG_INFO, "%s() got key: %d\n", __func__, ev.key_event.keycode);
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

