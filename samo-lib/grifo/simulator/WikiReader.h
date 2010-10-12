/*
 * Simulation of the WikiReader hardware
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

#if !defined(_WIKIREADER_H_)
#define _WIKIREADER_H_ 1

#include <inttypes.h>

#include <QApplication>
#include <QObject>
//#include <QMouseEvent>
//#include <QKeyEvent>
#include <QWidget>

#include "EventQueue.h"


class Wikireader : public QWidget {
	Q_OBJECT

private:
	QApplication *application;
	EventQueue *queue;

	static const int width = 240;
	static const int height = 208;
	static const int bytes_per_row = 32;

	uint8_t pixels[height * bytes_per_row];

	// no copying
	Wikireader(const Wikireader &);
	Wikireader &operator=(const Wikireader &);

private slots:
	void SearchPressed();
	void HistoryPressed();
	void RandomPressed();

public:
	Wikireader(QApplication *application, EventQueue *queue, QWidget *parent = 0);
	virtual ~Wikireader();

	uint8_t *FrameBufferAddress() {return pixels;}
	int FrameBufferSize() {return sizeof(pixels);}
	int FrameBufferRowSize() {return bytes_per_row;}

};

#endif
