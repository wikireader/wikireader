/*
 * events - an simple example program
 *
 * Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
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

int main(int argc, char **argv)
{
	debug_printf("events start\n");
	const char *button[] = {
		[BUTTON_RANDOM] "random",
		[BUTTON_SEARCH] "search",
		[BUTTON_HISTORY] "history",
		[BUTTON_POWER] "power",
	};

	for (;;) {
		event_t event;

		switch(event_get(&event)) {

		case EVENT_NONE:
		{
			const char spinner[4] = "-\\|/";
			static size_t i;
			if (i >= sizeof(spinner)) {
				i = 0;
			}
			debug_print_char(spinner[i++]);
			debug_print_char('\010');
		}
		break;

		case EVENT_KEY:
			debug_printf("%10lu: KEY[%d] = %d\n", event.time_stamp, event.item_type,
				     event.key.code);
			break;

		case EVENT_TOUCH_DOWN:
			debug_printf("%10lu: TOUCH DOWN[%d] = (%d,%d)\n", event.time_stamp, event.item_type,
				     event.touch.x, event.touch.y);
			break;

		case EVENT_TOUCH_MOTION:
			debug_printf("%10lu: TOUCH MOTION[%d] = (%d,%d)\n", event.time_stamp, event.item_type,
				     event.touch.x, event.touch.y);
			break;

		case EVENT_TOUCH_UP:
			debug_printf("%10lu: TOUCH UP[%d] = (%d,%d)\n", event.time_stamp, event.item_type,
				     event.touch.x, event.touch.y);
			break;

		case EVENT_BUTTON_DOWN:
			debug_printf("%10lu: BUTTON DOWN[%d] = %s[%d]\n", event.time_stamp, event.item_type,
				     button[event.button.code], event.button.code);
			break;

		case EVENT_BUTTON_UP:
			debug_printf("%10lu: BUTTON UP[%d] = %s[%d]\n", event.time_stamp, event.item_type,
				      button[event.button.code], event.button.code);
			break;

		case EVENT_BATTERY_LOW:
			debug_printf("%10lu: BATTERY LOW[%d] = %lu\n", event.time_stamp, event.item_type,
				     event.battery.millivolts);
			break;

		default:
			debug_printf("%10lu: Unknown event[%d]\n", event.time_stamp, event.item_type);
			break;
		}
	}
	return 0;
}
