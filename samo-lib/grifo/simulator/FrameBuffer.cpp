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
			 int width, int height, int BytesPerRow,
			 QWidget *parent)
	: QWidget(parent) {
	this->MainWidth = width;
	this->MainHeight = height;
	this->MainBytesPerRow = BytesPerRow;
	this->MainBufferSize = height * BytesPerRow;
	this->queue = queue;
	this->MainPixels = new uint8_t[this->MainBufferSize];
	this->WindowPixels = new uint8_t[this->MainBufferSize];
	this->WindowX = 0;
	this->WindowY = 0;
	this->WindowWidth = 0;
	this->WindowHeight = 0;
	this->WindowEnable = false;
	this->WindowOperational = false;
	setMaximumSize(width, height);
	setMinimumSize(width, height);
	qDebug("Framebuffer %d x %d\n", width, height);
	this->startTimer(20);    // milliseconds
}


FrameBuffer::~FrameBuffer() {
	delete[] this->MainPixels;
	delete[] this->WindowPixels;
}


int FrameBuffer::SetWindow(int x, int y, int w, int h) {
	this->WindowOperational = false;
	if (x < 0 || y < 0 || w <= 0 || h <= 0) {
		return 0;
	}
	// same limitations as real hardware
	x = (x + 31) & ~31;
	w = (w + 31) & ~31;
	this->WindowX = x;
	this->WindowY = y;
	this->WindowWidth = w;
	this->WindowHeight = h;
	this->WindowBytesPerRow = ((w + 31) >> 5) * sizeof(uint32_t);
	this->WindowBufferSize = this->WindowBytesPerRow * h;
	this->WindowOperational = this->WindowBufferSize != 0;
	return this->WindowBufferSize;
}


#define PIXEL(painter, byte, offset, x, y)                              \
	do {                                                            \
		painter.setPen(((byte) & (0x80 >> offset)) != 0 ? Qt::black : Qt::white); \
		painter.drawPoint((x + offset), (y));                   \
	} while (0)


void FrameBuffer::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.setBrush(Qt::SolidPattern);
	painter.fillRect(0, 0, this->MainWidth, this->MainHeight, Qt::white);


	int row_start = 0;
	for (int y = 0; y < this->MainHeight; y++) {
		int i = row_start;
		for (int x = 0; x < this->MainWidth; x += 8, ++i) {
			uint8_t byte = this->MainPixels[i];
			PIXEL(painter, byte, 0, x, y);
			PIXEL(painter, byte, 1, x, y);
			PIXEL(painter, byte, 2, x, y);
			PIXEL(painter, byte, 3, x, y);
			PIXEL(painter, byte, 4, x, y);
			PIXEL(painter, byte, 5, x, y);
			PIXEL(painter, byte, 6, x, y);
			PIXEL(painter, byte, 7, x, y);
		}
		row_start += this->MainBytesPerRow;
	}
	if (this->WindowEnable) {
		int row_start = 0;
		for (int y = 0; y < this->WindowHeight; y++) {
			int i = row_start;
			int y1 = y + this->WindowY;
			for (int x = 0; x < this->WindowWidth; x += 8, ++i) {
				int x1 = x + this->WindowX;
				uint8_t byte = this->WindowPixels[i];
				PIXEL(painter, byte, 0, x1, y1);
				PIXEL(painter, byte, 1, x1, y1);
				PIXEL(painter, byte, 2, x1, y1);
				PIXEL(painter, byte, 3, x1, y1);
				PIXEL(painter, byte, 4, x1, y1);
				PIXEL(painter, byte, 5, x1, y1);
				PIXEL(painter, byte, 6, x1, y1);
				PIXEL(painter, byte, 7, x1, y1);
			}
			row_start += this->WindowBytesPerRow;
		}
	}
}


void FrameBuffer::timerEvent(QTimerEvent *) {
	this->update();
}


static inline int RestrictValue(int value, int min, int max) {
	if (value < min) {
		return min;
	} else if (value > max - 1) {
		return max - 1;
	}
	return value;
}


void FrameBuffer::mousePressEvent(QMouseEvent *event) {
	//qDebug("Mouse Press: %d, %d",  event->pos().x(), event->pos().y());
	event_t e;
	e.item_type = EVENT_TOUCH_DOWN;
	e.touch.x = RestrictValue(event->pos().x(), 0, this->MainWidth);
	e.touch.y = RestrictValue(event->pos().y(), 0, this->MainHeight);
	this->queue->enqueue(&e);
	//QWidget::mousePressEvent(event);
}

void FrameBuffer::mouseMoveEvent(QMouseEvent *event) {
	//qDebug("Mouse Move: %d, %d",  event->pos().x(), event->pos().y());
	event_t e;
	e.item_type = EVENT_TOUCH_MOTION;
	e.touch.x = RestrictValue(event->pos().x(), 0, this->MainWidth);
	e.touch.y = RestrictValue(event->pos().y(), 0, this->MainHeight);
	this->queue->enqueue(&e);
	//QWidget::mouseMoveEvent(event);
}

void FrameBuffer::mouseReleaseEvent(QMouseEvent *event) {
	//qDebug("Mouse Release: %d, %d",  event->pos().x(), event->pos().y());
	event_t e;
	e.item_type = EVENT_TOUCH_UP;
	e.touch.x = RestrictValue(event->pos().x(), 0, this->MainWidth);
	e.touch.y = RestrictValue(event->pos().y(), 0, this->MainHeight);
	this->queue->enqueue(&e);
	//QWidget::mouseReleaseEvent(event);
}

void FrameBuffer::mouseDoubleClickEvent(QMouseEvent *event) {
	//qDebug("Mouse Double Click: %d, %d",  event->pos().x(), event->pos().y());
	(void)event;
	//QWidget::mouseDoubleClickEvent(event);
}
