/*
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

#include <QPainter>
//#include <QMouseEvent>
//#include <QKeyEvent>
#include <QTimer>
#include <QWidget>
#include <QtDebug>

#include "FrameBuffer.h"
#include "EventQueue.h"


FrameBuffer::FrameBuffer(EventQueue *queue,
			 const uint8_t *pixels,
			 int width, int height, int bytes_per_row,
			 QWidget *parent)
	: QWidget(parent) {
	this->pixels = pixels;
	this->width = width;
	this->height = height;
	this->bytes_per_row = bytes_per_row;
	this->queue = queue;

	setMaximumSize(this->width, this->height);
	setMinimumSize(this->width, this->height);
	qDebug("Framebuffer %d x %d @ %p\n", this->width, this->height, this->pixels);
	this->startTimer(50);    // milliseconds
}


FrameBuffer::~FrameBuffer() {
}

#define PIXEL(painter, byte, offset, x, y)                              \
	do {                                                            \
		painter.setPen(((byte) & (1 << offset)) != 0 ? Qt::black : Qt::white); \
		painter.drawPoint((x + offset), (y));                   \
	} while (0)


void FrameBuffer::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.setBrush(Qt::SolidPattern);
	painter.fillRect(0, 0, this->width, this->height, Qt::white);

	int row_start = 0;
	for (int y = 0; y < this->height; y++) {
		int i = row_start;
		for (int x = 0; x < this->width; x += 8, ++i) {
			uint8_t byte = this->pixels[i];
			PIXEL(painter, byte, 0, x, y);
			PIXEL(painter, byte, 1, x, y);
			PIXEL(painter, byte, 2, x, y);
			PIXEL(painter, byte, 3, x, y);
			PIXEL(painter, byte, 4, x, y);
			PIXEL(painter, byte, 5, x, y);
			PIXEL(painter, byte, 6, x, y);
			PIXEL(painter, byte, 7, x, y);
		}
		row_start += bytes_per_row;
	}
}


void FrameBuffer::timerEvent(QTimerEvent *) {
	this->update();
}


void FrameBuffer::mousePressEvent(QMouseEvent *event) {
	//qDebug("Mouse Press: %d, %d",  event->pos().x(), event->pos().y());
	event_t e;
	e.item_type = EVENT_TOUCH_DOWN;
	e.touch.x = event->pos().x();
	e.touch.y = event->pos().y();
	this->queue->enqueue(&e);
	//QWidget::mousePressEvent(event);
}

void FrameBuffer::mouseMoveEvent(QMouseEvent *event) {
	//qDebug("Mouse Move: %d, %d",  event->pos().x(), event->pos().y());
	event_t e;
	e.item_type = EVENT_TOUCH_MOTION;
	e.touch.x = event->pos().x();
	e.touch.y = event->pos().y();
	this->queue->enqueue(&e);
	//QWidget::mouseMoveEvent(event);
}

void FrameBuffer::mouseReleaseEvent(QMouseEvent *event) {
	//qDebug("Mouse Release: %d, %d",  event->pos().x(), event->pos().y());
	event_t e;
	e.item_type = EVENT_TOUCH_UP;
	e.touch.x = event->pos().x();
	e.touch.y = event->pos().y();
	this->queue->enqueue(&e);
	//QWidget::mouseReleaseEvent(event);
}

void FrameBuffer::mouseDoubleClickEvent(QMouseEvent *event) {
	//qDebug("Mouse Double Click: %d, %d",  event->pos().x(), event->pos().y());
	(void)event;
	//QWidget::mouseDoubleClickEvent(event);
}
