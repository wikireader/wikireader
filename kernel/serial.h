#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);
void serial_in(int port);
void serial_out(int port);
int serial_get_event(struct wl_input_event *ev);

#endif /* SERIAL_H */

