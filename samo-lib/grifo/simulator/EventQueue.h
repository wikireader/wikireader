/* -*- mode: c++ -*-
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

#if !defined(_QUEUE_H_)
#define _QUEUE_H_ 1

#include <inttypes.h>

#include <QQueue>
#include <QMutex>
#include <QSemaphore>

#include "grifo.h"

class EventQueue {

private:
	QMutex *mutex;
	QSemaphore *semaphore;
	QQueue<event_t> *queue;

	// no copying
	EventQueue(const EventQueue &);
	EventQueue &operator=(const EventQueue &);

public:
	EventQueue();
	virtual ~EventQueue();

	bool enqueue(event_t *event);
	event_item_t dequeue(event_t *event, int milliseconds = -1);  // default = wait forever
	bool isEmpty() const;

};

#endif
