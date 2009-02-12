#include <wikilib.h>
#include <guilib.h>
#include <glyph.h>
#include <input.h>
#include <msg.h>
#include <malloc.h>
#include <file-io.h>
#include <search.h>
#include <string.h>

static void handle_search_key(char keycode)
{
    char *result;
    int j = 0, y_pos = 10;

    if (keycode == 8) {
	search_remove_char();
    } else if (isalnum(keycode) || isspace(keycode)) {
	msg(MSG_INFO, "Adding to search : '%c'\n", keycode);
	search_add(tolower(keycode));
    } else {
	msg(MSG_INFO, "%s() unhandled key: %d\n", __func__, keycode);
	return;
    }


    /* paint the results */
    guilib_fb_lock();
    guilib_clear();

    y_pos += 2 + render_string(0, "Search results:", 1, y_pos);
    while (y_pos < 200 && (result = search_fetch_result())) {
	y_pos += 2 + render_string(0, result, 1, y_pos);
	msg(MSG_INFO, "Result: %s\n", result);
    }

    search_print_stats();
    guilib_fb_unlock();
}

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
		case WL_INPUT_EV_TYPE_KEYBOARD:
			handle_search_key(ev.key_event.keycode);
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

