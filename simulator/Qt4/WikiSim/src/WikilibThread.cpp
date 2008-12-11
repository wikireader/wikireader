/***************************************************************************
 *   Copyright (C) 2008 by Daniel Mack   *
 *   daniel@caiaq.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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

/* this is the gui-lib glue layer */
void fb_set_pixel(int x, int y, int v)
{
	window->display->setPixel(x, y, v);
}

void fb_refresh(void)
{
	window->display->repaint();
}

void fb_clear(void)
{
	window->display->clear();
}

int wl_input_wait(struct wl_input_event *ev)
{
	WikiDisplay *display = window->display;
	QWaitCondition *w = display->waitCondition;
	QKeyEvent *keyEvent;
	QMutex mutex;

	ev->type = -1;
	ev->val_a = 0;
	ev->val_b = 0;

	do {
		mutex.lock();
		w->wait(&mutex);
		QKeyEvent event = display->keyEventQueue->dequeue();
		mutex.unlock();

		switch (event.type()) {
		case QEvent::KeyPress:
			keyEvent = (QKeyEvent *) &event;
			ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
			ev->val_a = keyEvent->text().at(0).unicode();
			ev->val_b = 1;
			break;
		case QEvent::KeyRelease:
			keyEvent = (QKeyEvent *) &event;
			ev->type = WL_INPUT_EV_TYPE_KEYBOARD;
			ev->val_a = keyEvent->text().at(0).unicode();
			ev->val_b = 0;
			break;
		default:
			break;
		}
	} while (ev->type == -1);

	return 0;
}

} /* extern "C" */

WikilibThread::WikilibThread()
 : QThread()
{
}

WikilibThread::~WikilibThread()
{
}

void
WikilibThread::run()
{
	wikilib_init();
	guilib_init();
	wikilib_run();
}
