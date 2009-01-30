#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

void touchscreen_read_char(char c);
int touchscreen_get_event(struct wl_input_event *ev);

#endif /* TOUCHSCREEN_H */

