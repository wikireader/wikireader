/*
 * event queue
 *
 * Copyright (c) 2009 Openmoko Inc.
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

#include "standard.h"

#include <string.h>

#include "standard.h"
#include "interrupt.h"
#include "timer.h"
#include "event.h"


// the queue
event_t EventQueue[256];

event_t *head;
event_t *tail;

void Event_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		initialised = true;
		Timer_initialise();
		Event_flush();
	}
}


void Event_flush(void)
{
	Interrupt_type state = Interrupt_disable();

	head = tail = EventQueue;

	Interrupt_enable(state);
}

#include "serial.h"
bool Event_put(const event_t *event)
{
	Interrupt_type state = Interrupt_disable();
	event_t *next = &tail[1];

	if (next >= &EventQueue[SizeOfArray(EventQueue)]) {
		next = EventQueue;
	}
	bool full = head == next;
	if (!full) {
		memcpy(tail, event, sizeof(*tail));
		tail->time_stamp = Timer_get();  // timestamp the event
		tail = next;
	}
	Interrupt_enable(state);
	return !full;  // false if buffer full
}


event_item_t Event_get(event_t *event)
{
	Interrupt_type state = Interrupt_disable();
	if (head == tail) {
		memset(event, 0, sizeof(*event));
		event->item_type = EVENT_NONE;
	} else {
		memcpy(event, head, sizeof(*event));
		++head;
		if (head >= &EventQueue[SizeOfArray(EventQueue)]) {
			head = EventQueue;
		}
	}
	Interrupt_enable(state);
	return event->item_type;
}
