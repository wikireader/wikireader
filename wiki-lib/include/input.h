#ifndef WL_EVENT_H
#define WL_EVENT_H

enum {
	WL_INPUT_EV_TYPE_KEYBOARD = 0,
	WL_INPUT_EV_TYPE_TOUCH,
	WL_INPUT_EV_TYPE_SYSTEM
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

int wl_input_wait(struct wl_input_event *ev);

#endif /* WL_INPUT_H */

