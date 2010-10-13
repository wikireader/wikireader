/*
 * events - an simple example program
 *
 * Copyright (c) 2010 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "grifo.h"


void PrintEvent(const event_t *event);
bool callback(void *arg);

int grifo_main(int argc, char *argv[]) {

	int DelayTime = 1000000; // microseconds
	enum {polling, waiting, delayed} EventMethod = waiting;
	if (argc > 1 && 'p' == argv[1][0]) {
		EventMethod = polling;
		debug_printf("events start polling\n");
	} else if (argc > 2 && 'd' == argv[1][0]) {
		EventMethod = delayed;
		DelayTime = atoi(argv[2]);
		debug_printf("events start delay %d us\n", DelayTime);
	} else {
		debug_printf("events start waiting\n");
	}
	delay_us(2000000);
	debug_printf("RUN:\n");
	for (;;) {
		event_t event;

		switch (EventMethod) {
		case waiting:
			event_wait(&event, callback, "main loop callback");
			PrintEvent(&event);
			break;
		case polling:
			event_get(&event);
			PrintEvent(&event);
			break;
		case delayed:
			debug_printf("Delaying...\n");
			delay_us(DelayTime);
			debug_printf("Reading Events...\n");
			while (EVENT_NONE != event_get(&event)) {
				PrintEvent(&event);
			}
			break;
		}
	}
	return 0;

}

void PrintEvent(const event_t *event) {

	static const char *const button[] = {
		[BUTTON_RANDOM] "random",
		[BUTTON_SEARCH] "search",
		[BUTTON_HISTORY] "history",
		[BUTTON_POWER] "power",
	};

	switch (event->item_type) {

	case EVENT_NONE: {
		const char spinner[4] = "-\\|/";
		static size_t i;
		if (i >= sizeof(spinner)) {
			i = 0;
		}
		debug_print_char(spinner[i++]);
		debug_print_char('\010');
		delay_us(5000);
		break;
	}

	case EVENT_KEY:
		debug_printf("%10lu: KEY[%d] = %d\n", event->time_stamp, event->item_type,
			     event->key.code);
		break;

	case EVENT_TOUCH_DOWN:
		debug_printf("%10lu: TOUCH DOWN[%d] = (%d,%d)\n", event->time_stamp, event->item_type,
			     event->touch.x, event->touch.y);
		break;

	case EVENT_TOUCH_MOTION:
		debug_printf("%10lu: TOUCH MOTION[%d] = (%d,%d)\n", event->time_stamp, event->item_type,
			     event->touch.x, event->touch.y);
		break;

	case EVENT_TOUCH_UP:
		debug_printf("%10lu: TOUCH UP[%d] = (%d,%d)\n", event->time_stamp, event->item_type,
			     event->touch.x, event->touch.y);
		break;

	case EVENT_BUTTON_DOWN:
		debug_printf("%10lu: BUTTON DOWN[%d] = %s[%d]\n", event->time_stamp, event->item_type,
			     button[event->button.code], event->button.code);
		break;

	case EVENT_BUTTON_UP:
		debug_printf("%10lu: BUTTON UP[%d] = %s[%d]\n", event->time_stamp, event->item_type,
			     button[event->button.code], event->button.code);
		break;

	case EVENT_BATTERY_LOW:
		debug_printf("%10lu: BATTERY LOW[%d] = %lu\n", event->time_stamp, event->item_type,
			     (unsigned long)event->battery.millivolts);
		break;

	default:
		debug_printf("%10lu: Unknown event[%d]\n", event->time_stamp, event->item_type);
		break;
	}
	return;
}


bool callback(void *arg) {
	const char *message = (const char *)arg;

	debug_printf("Callback: %s\n", message);
	return false;  // request shutdown
}
