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
#include "watchdog.h"
#include "suspend.h"
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
		Suspend_initialise();
		Event_flush();
	}
}


void Event_flush(void)
{
	Interrupt_type state = Interrupt_disable();

	head = tail = EventQueue;

	Interrupt_enable(state);
}


bool Event_PutButton(const event_t *event)
{
	event_item_t opposite = EVENT_NONE;
	if (EVENT_NONE == event->item_type) {
		return true;
	} else if (EVENT_BUTTON_UP == event->item_type) {
		opposite = EVENT_BUTTON_DOWN;
	} else if (EVENT_BUTTON_DOWN == event->item_type) {
		opposite = EVENT_BUTTON_UP;
	} else {  // in case called by non-button event, just redirect
		return Event_put(event);
	}

	Interrupt_type state = Interrupt_disable();
	if (head != tail) {
		event_t *prev = &tail[-1];
		if (prev < EventQueue) {
			prev = &EventQueue[SizeOfArray(EventQueue) - 1];
		}
		if (head != prev) {
			event_t *prev2 = prev - 1;
			if (prev2 < EventQueue) {
				prev2 = &EventQueue[SizeOfArray(EventQueue) - 1];
			}
			if (head != prev) {  // have two previous entries in the queue
				if (opposite == prev->item_type &&
				    prev->button.code == event->button.code &&
				    prev2->item_type == event->item_type &&
				    prev2->button.code == event->button.code) {
					tail = prev2;
				}
			}
		}
	}
	Interrupt_enable(state);
	return Event_put(event);
}


bool Event_put(const event_t *event)
{
	if (EVENT_NONE == event->item_type) {
		return true;
	}
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


event_item_t Event_peek(event_t *event)
{
	Watchdog_KeepAlive(WATCHDOG_KEY);
	Interrupt_type state = Interrupt_disable();
	if (head == tail) {
		memset(event, 0, sizeof(*event));
		event->item_type = EVENT_NONE;
	} else {
		memcpy(event, head, sizeof(*event));
	}
	Interrupt_enable(state);
	return event->item_type;
}


event_item_t Event_get(event_t *event)
{
	Watchdog_KeepAlive(WATCHDOG_KEY);
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


event_item_t Event_wait(event_t *event, Standard_BoolCallBackType *callback, void *arg)
{
	for (;;) {
		event_item_t e = Event_get(event);
		if (EVENT_NONE != e) {
			return e;
		}
		Suspend(callback, arg);
	}
}

