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
			debug_printchar(spinner[i++]);
			debug_printchar('\010');
		}
		break;

		case EVENT_KEY:
			debug_printf("%10ld: KEY[%d] = %d\n", event.time_stamp, event.item_type,
				     event.key.code);
			break;

		case EVENT_TOUCH_DOWN:
			debug_printf("%10ld: TOUCH DOWN[%d] = (%d,%d)\n", event.time_stamp, event.item_type,
				     event.touch.x, event.touch.y);
			break;

		case EVENT_TOUCH_MOTION:
			debug_printf("%10ld: TOUCH MOTION[%d] = (%d,%d)\n", event.time_stamp, event.item_type,
				     event.touch.x, event.touch.y);
			break;

		case EVENT_TOUCH_UP:
			debug_printf("%10ld: TOUCH UP[%d] = (%d,%d)\n", event.time_stamp, event.item_type,
				     event.touch.x, event.touch.y);
			break;

		case EVENT_BUTTON_DOWN:
			debug_printf("%10ld: BUTTON DOWN[%d] = %d\n", event.time_stamp, event.item_type,
				     event.button.code);
			break;

		case EVENT_BUTTON_UP:
			debug_printf("%10ld: BUTTON UP[%d] = %d\n", event.time_stamp, event.item_type,
				     event.button.code);
			break;

		case EVENT_BATTERY_LOW:
			debug_printf("%10ld: BATTERY LOW[%d] = %lu\n", event.time_stamp, event.item_type,
				     event.battery.millivolts);
			break;

		default:
			debug_printf("Unknown event = %d at %ld\n", event.item_type, event.time_stamp);
			break;
		}
	}
	return 0;
}
