/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Daniel Mack <daniel@caiaq.de>
 *           Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <QThread>
#include <QQueue>
#include <QEvent>
#include <QKeyEvent>
#include <QMutex>

#include "WikilibThread.h"
#include "main.h"

extern "C" {
#include "guilib.h"
#include "wikilib.h"
#include "input.h"

unsigned char *framebuffer;

/* this is the gui-lib glue layer */
void fb_refresh(void)
{
	window->display->update();
}

void wl_input_wait(struct wl_input_event *ev, int sleep)
{
	WikiDisplay *display = window->display;
	QWaitCondition *w = display->waitCondition;
	QMutex mutex;

	ev->type = -1;

	do {
		mutex.lock();
		if (sleep && display->keyEventQueue->isEmpty() && display->mouseEventQueue->isEmpty()) {
			w->wait(&mutex);
		}

		if (!display->keyEventQueue->isEmpty()) {
			display->keyQueueMutex->lock();
			QKeyEvent keyEvent = display->keyEventQueue->dequeue();
			display->keyQueueMutex->unlock();
			ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
			/* determine key type */
			if (keyEvent.key() == Qt::Key_Down) {
				ev->key_event.keycode = WL_INPUT_KEY_CURSOR_DOWN;
				ev->type = WL_INPUT_EV_TYPE_CURSOR;
			} else if (keyEvent.key() == Qt::Key_Up) {
				ev->key_event.keycode = WL_INPUT_KEY_CURSOR_UP;
				ev->type = WL_INPUT_EV_TYPE_CURSOR;
			} else if (keyEvent.text().length() > 0) {
				ev->key_event.keycode = keyEvent.text().at(0).unicode();
			} else {
				ev->key_event.keycode = keyEvent.key();
			}

			switch (keyEvent.type()) {
			case QEvent::KeyPress:
				ev->key_event.value = 1;
				break;
			case QEvent::KeyRelease:
				ev->key_event.value = 0;
				break;
			default:
				break;
			}
		}

		if (!display->mouseEventQueue->isEmpty()) {
			display->mouseQueueMutex->lock();
			QMouseEvent mouseEvent = display->mouseEventQueue->dequeue();
			display->mouseQueueMutex->unlock();
			ev->type = WL_INPUT_EV_TYPE_TOUCH;
			ev->touch_event.x = mouseEvent.x();
			ev->touch_event.y = mouseEvent.y();
			ev->touch_event.value = (mouseEvent.type() == QEvent::MouseButtonPress) ? 1 : 0;
		}

		mutex.unlock();
	} while (ev->type == -1 && sleep);
}

} /* extern "C" */

WikilibThread::WikilibThread()
 : QThread()
{
	framebuffer = (unsigned char *) malloc(guilib_framebuffer_size());
}

WikilibThread::~WikilibThread()
{
	free(framebuffer);
}

void
WikilibThread::run()
{
	wikilib_init();
	guilib_init();
	wikilib_run();
}
