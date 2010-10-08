/* -*- mode: c++ -*-
 * framebuffer simulation
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

#if !defined(_FRAMEBUFFER_H_)
#define _FRAMEBUFFER_H_ 1

#include <inttypes.h>

#include <QObject>
#include <QPainter>
#include <QMouseEvent>
//#include <QKeyEvent>
#include <QWidget>
#include <QTimer>

#include "EventQueue.h"

class FrameBuffer : public QWidget {
	Q_OBJECT

private:
	int width;
	int height;
	int bytes_per_row;
	const uint8_t *pixels;
	EventQueue *queue;

	// no copying
	FrameBuffer(const FrameBuffer &);
	FrameBuffer &operator=(const FrameBuffer &);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
//	void keyPressEvent(QKeyEvent *event);
	void timerEvent(QTimerEvent *event);
	void paintEvent(QPaintEvent *event);

public:
	FrameBuffer(EventQueue *queue, const uint8_t *pixels,
		    int width, int height, int bytes_per_row, QWidget *parent = 0);
	virtual ~FrameBuffer();
};

#endif
