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

#include <QPainter>
#include <QByteArray>
#include <QKeyEvent>
#include <QString>

#include "WikiDisplay.h"

#include <stdio.h>

extern "C" {
#include "guilib.h"
}

WikiDisplay::WikiDisplay(QWidget *parent)
 : QWidget(parent)
{
	setMinimumSize(guilib_framebuffer_width(), guilib_framebuffer_height());
	setMaximumSize(guilib_framebuffer_width(), guilib_framebuffer_height());
	printf("initializing display for %dx%d pixels\n", guilib_framebuffer_width(), guilib_framebuffer_height());
	framebuffer = new QByteArray(guilib_framebuffer_width() * guilib_framebuffer_height(), 0);
	keyEventQueue = new QQueue<QKeyEvent>;
	mouseEventQueue = new QQueue<QMouseEvent>;
	waitCondition = new QWaitCondition();
	keyQueueMutex = new QMutex();
	mouseQueueMutex = new QMutex();
}

WikiDisplay::~WikiDisplay()
{
	delete framebuffer;
	delete keyEventQueue;
	delete waitCondition;
	delete keyQueueMutex;
	delete mouseQueueMutex;
}

void
WikiDisplay::keyPressEvent(QKeyEvent *event)
{
	/* send a vt100 sequence... */
	if (event->text().isEmpty()
	    && event->key() != Qt::Key_Down && event->key() != Qt::Key_Up)
	    return;

	keyQueueMutex->lock();
	keyEventQueue->enqueue(*event);
	keyQueueMutex->unlock();
	waitCondition->wakeAll();
}

void
WikiDisplay::mousePressEvent(QMouseEvent *event)
{
	mouseQueueMutex->lock();
	mouseEventQueue->enqueue(*event);
	mouseQueueMutex->unlock();
	waitCondition->wakeAll();
}

void
WikiDisplay::mouseReleaseEvent(QMouseEvent *event)
{
	mouseQueueMutex->lock();
	mouseEventQueue->enqueue(*event);
	mouseQueueMutex->unlock();
	waitCondition->wakeAll();
}

void
WikiDisplay::paintEvent(QPaintEvent *)
{
	unsigned int x, y;

	QPainter painter(this);
	painter.setBrush(Qt::SolidPattern);
	painter.fillRect(0, 0, guilib_framebuffer_width(), guilib_framebuffer_height(), Qt::white);

	for (x = 0; x < guilib_framebuffer_width(); x++)
		for (y = 0; y < guilib_framebuffer_height(); y++) {
			int r, g, b;
			//r = g = b = guilib_get_pixel(x, y) ? 0xFF: 0;
			r = g = b = guilib_get_pixel(x, y) ? 0: 0xFF;
			painter.setPen(QColor::fromRgb(r, g, b, 0xff));
			painter.drawPoint(x, y);
		}
}
