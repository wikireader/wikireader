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

#include <inttypes.h>

#include <QtDebug>
#include <QApplication>
#include <QObject>
#include <QPainter>
#include <QDesktopWidget>
//#include <QMouseEvent>
//#include <QKeyEvent>
#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "WikiReader.h"
#include "FrameBuffer.h"
#include "EventQueue.h"


Wikireader::Wikireader(QApplication *application, EventQueue *queue, QWidget *parent) : QWidget(parent) {
	this->application = application;
	this->queue = queue;

	this->setFocusPolicy(Qt::StrongFocus);
	this->setFocus();

	QVBoxLayout *layout = new QVBoxLayout;

	QHBoxLayout *screen = new QHBoxLayout;

	screen->addStretch();

	this->fb = new FrameBuffer(this->queue, this->width, this->height, this->bytes_per_row);
	fb->setFocusPolicy(Qt::NoFocus);
	screen->addWidget(fb);

	screen->addStretch();

	layout->addItem(screen);

	QHBoxLayout *controls = new QHBoxLayout;

	QLabel *label = new QLabel("WikiReader");
	label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	label->setFocusPolicy(Qt::NoFocus);
	controls->addWidget(label);

	QPushButton *SearchButton = new QPushButton("Search");
	SearchButton->setFocusPolicy(Qt::NoFocus);
	connect(SearchButton, SIGNAL(clicked()), this, SLOT(SearchPressed()));
	controls->addWidget(SearchButton);

	QPushButton *HistoryButton = new QPushButton("History");
	HistoryButton->setFocusPolicy(Qt::NoFocus);
	connect(HistoryButton, SIGNAL(clicked()), this, SLOT(HistoryPressed()));
	controls->addWidget(HistoryButton);

	QPushButton *RandomButton = new QPushButton("Random");
	RandomButton->setFocusPolicy(Qt::NoFocus);
	connect(RandomButton, SIGNAL(clicked()), this, SLOT(RandomPressed()));
	controls->addWidget(RandomButton);

	layout->addItem(controls);

	this->setLayout(layout);
	this->show();

	this->setMinimumSize(this->size());
	this->setMaximumSize(this->size());
}


Wikireader::~Wikireader() {
	qDebug() << "Exit";
}


void Wikireader::SearchPressed() {
	//qDebug() << "Search";
	event_t event;
	event.button.code = BUTTON_SEARCH;
	event.item_type = EVENT_BUTTON_DOWN;
	this->queue->enqueue(&event);
	event.item_type = EVENT_BUTTON_UP;
	this->queue->enqueue(&event);
}


void Wikireader::HistoryPressed() {
	//qDebug() << "History";
	event_t event;
	event.button.code = BUTTON_HISTORY;
	event.item_type = EVENT_BUTTON_DOWN;
	this->queue->enqueue(&event);
	event.item_type = EVENT_BUTTON_UP;
	this->queue->enqueue(&event);
}


void Wikireader::RandomPressed() {
	//qDebug() << "Random";
	event_t event;
	event.button.code = BUTTON_RANDOM;
	event.item_type = EVENT_BUTTON_DOWN;
	this->queue->enqueue(&event);
	event.item_type = EVENT_BUTTON_UP;
	this->queue->enqueue(&event);
}
