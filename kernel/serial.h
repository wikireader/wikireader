#ifndef SERIAL_H
#define SERIAL_H

void serial_init(void);
void serial_irq_handler(void);
int serial_input_parse(struct wl_input_event *ev);

#endif /* SERIAL_H */

