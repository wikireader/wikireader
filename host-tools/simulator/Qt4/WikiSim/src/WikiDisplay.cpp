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
#include <QPainter>
#include <QByteArray>
#include <QKeyEvent>
#include <QString>

#include "WikiDisplay.h"

extern "C" {
#include "guilib.h"
}

WikiDisplay::WikiDisplay(QWidget *parent)
 : QWidget(parent)
{
	setMinimumSize(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	setMaximumSize(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	printf("initializing display for %dx%d pixels\n", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	framebuffer = new QByteArray(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT, 0);
	keyEventQueue = new QQueue<QKeyEvent>;
	mouseEventQueue = new QQueue<QMouseEvent>;
	waitCondition = new QWaitCondition();
}

WikiDisplay::~WikiDisplay()
{
	delete framebuffer;
	delete keyEventQueue;
	delete waitCondition;
}

void
WikiDisplay::keyPressEvent(QKeyEvent *event)
{
	/* send a vt100 sequence... */
	if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up) {
		keyEventQueue->enqueue(QKeyEvent(QKeyEvent::KeyPress, 27, Qt::NoModifier));
		keyEventQueue->enqueue(QKeyEvent(QKeyEvent::KeyPress, 91, Qt::NoModifier));
		keyEventQueue->enqueue(QKeyEvent(QKeyEvent::KeyPress,
				event->key() == Qt::Key_Down ? 66 : 65, Qt::NoModifier));
		waitCondition->wakeAll();
		return;
	} else if (event->text().isEmpty())
		return;

	keyEventQueue->enqueue(*event);
	waitCondition->wakeAll();
}

void
WikiDisplay::mousePressEvent(QMouseEvent *event)
{
	mouseEventQueue->enqueue(*event);
	waitCondition->wakeAll();
}

void
WikiDisplay::mouseReleaseEvent(QMouseEvent *event)
{
	mouseEventQueue->enqueue(*event);
	waitCondition->wakeAll();
}

void
WikiDisplay::paintEvent(QPaintEvent *)
{
	int x, y;

	QPainter painter(this);
	painter.setBrush(Qt::SolidPattern);
	painter.fillRect(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, Qt::white);

	for (x = 0; x < FRAMEBUFFER_WIDTH; x++)
		for (y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
			int r, g, b;
			r = g = b = guilib_get_pixel(x, y) * 0xff;
			painter.setPen(QColor::fromRgb(r, g, b, 0xff));
			painter.drawPoint(x, y);
		}
}
