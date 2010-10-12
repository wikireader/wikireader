/*
 * thread safe event queue
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

#include <inttypes.h>

#include <QQueue>
#include <QMutex>
#include <QSemaphore>

#include "EventQueue.h"

EventQueue::EventQueue(unsigned long (*TimeStamp)()) {
	this->mutex = new QMutex;
	this->queue = new QQueue<event_t>;
	this->semaphore = new QSemaphore;
	this->TimeStamp = TimeStamp;
}


EventQueue::~EventQueue() {
	delete this->mutex;
	delete this->queue;
	delete this->semaphore;
}


event_item_t EventQueue::head(event_t *event) {

	QMutexLocker lock(this->mutex);
	if (this->queue->isEmpty()) {
		event->item_type = EVENT_NONE;
	} else {
		*event = this->queue->head();
	}
	return event->item_type;

}


event_item_t EventQueue::dequeue(event_t *event, int milliseconds) {

	if (this->semaphore->tryAcquire(1, milliseconds)) {
		QMutexLocker lock(this->mutex);
		*event = this->queue->dequeue();
	} else {
		event->item_type = EVENT_NONE;
	}
	return event->item_type;
}

bool EventQueue::enqueue(event_t *event) {

	QMutexLocker lock(this->mutex);
	event->time_stamp = this->TimeStamp();
	this->queue->enqueue(*event);  // queue a copy of the event

	this->semaphore->release(1);

	return true;
}
