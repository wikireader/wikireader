#ifndef WL_EVENT_H
#define WL_EVENT_H

#define WL_KEY_BACKSPACE	8
#define WL_KEY_RETURN		13
#define WL_KEY_ESC		27
#define WL_KEY_SPACE		32
#define WL_KEY_HASH		35
#define WL_KEY_PLUS		43
#define WL_KEY_MINUS		45

enum {
	WL_INPUT_KEY_SEARCH	= 0x1000,
	WL_INPUT_KEY_HISTORY	= 0x1001,
	WL_INPUT_KEY_RANDOM	= 0x1002
};

enum {
	WL_INPUT_EV_TYPE_KEYBOARD = 0,
	WL_INPUT_EV_TYPE_TOUCH,
	WL_INPUT_EV_TYPE_SYSTEM,
	WL_INPUT_EV_TYPE_CURSOR
};

enum {
	WL_INPUT_KEY_CURSOR_UP = 0,
	WL_INPUT_KEY_CURSOR_DOWN
};

struct wl_input_event {
	int type;

	union {
		struct {
			int keycode;
			int value;
		} key_event;

		struct {
			int x, y;
			int value;
		} touch_event;
	}; /* union */
};

int wl_input_wait(struct wl_input_event *ev, int sleep);

#endif /* WL_INPUT_H */

