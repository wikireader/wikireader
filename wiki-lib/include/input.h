#ifndef WL_EVENT_H
#define WL_EVENT_H

#define WL_KEY_BACKSPACE	8
#define WL_KEY_RETURN		13
#define WL_KEY_ESC		27
#define WL_KEY_SPACE		32
#define WL_KEY_HASH		35
#define WL_KEY_PLUS		43
#define WL_KEY_MINUS		45
#define WL_KEY_UP		65
#define WL_KEY_DOWN		66

#define WL_BUTTON_BASE		0x1000

enum {
#if BOARD_SAMO_A1
	WL_INPUT_KEY_RANDOM_UP    = WL_BUTTON_BASE + 0,
	WL_INPUT_KEY_RANDOM_DOWN  = WL_BUTTON_BASE + 1,
	WL_INPUT_KEY_SEARCH_UP    = WL_BUTTON_BASE + 2,
	WL_INPUT_KEY_SEARCH_DOWN  = WL_BUTTON_BASE + 3,
	WL_INPUT_KEY_HISTORY_UP   = WL_BUTTON_BASE + 4,
	WL_INPUT_KEY_HISTORY_DOWN = WL_BUTTON_BASE + 5,
#else
	WL_INPUT_KEY_SEARCH_UP    = WL_BUTTON_BASE + 0,
	WL_INPUT_KEY_SEARCH_DOWN  = WL_BUTTON_BASE + 1,
	WL_INPUT_KEY_HISTORY_UP   = WL_BUTTON_BASE + 2,
	WL_INPUT_KEY_HISTORY_DOWN = WL_BUTTON_BASE + 3,
	WL_INPUT_KEY_RANDOM_UP    = WL_BUTTON_BASE + 4,
	WL_INPUT_KEY_RANDOM_DOWN  = WL_BUTTON_BASE + 5,
#endif
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

