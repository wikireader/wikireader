/*
 * mouse and keyboard capture to serial port
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

#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include <QApplication>
#include <QThread>
#include <QObject>

#include "WikiReader.h"
#include "FrameBuffer.h"
#include "EventQueue.h"

// must be last include to ensure that 64bit OS definitions
// are set up before including the 32bit grifo.h
#include "grifo.h"


EventQueue *queue;
QApplication *application;

FrameBuffer *fb = 0;


class MyThread : public QThread {
private:
	int argc;
	char **argv;

public:
	MyThread(int argc, char *argv[]) {
		this->argc = argc;
		this->argv = argv;
	}
	void run() {
		int rc = grifo_main(this->argc, this->argv);
		application->quit();
		if (0 == rc) {
			chain("init.app restart grifo-kernel");
		}
	}
};


void TerminateApplication(const char *format = 0, ...) {
	if (format) {
		char buffer[1024];
		va_list arguments;

		va_start(arguments, format);

		(void)vsnprintf(buffer, sizeof(buffer), format, arguments);

		va_end(arguments);

		qDebug("Terminated, because: %s\n", buffer);
	}
	for (;;) {
		QThread::yieldCurrentThread();
		sleep(1);
		application->quit();
	}
}


unsigned long TimeStamp() {
	struct timespec tp;

	clock_gettime(4 /*CLOCK_MONOTONIC_RAW*/, &tp);

	return tp.tv_nsec / 1000 * TIMER_CountsPerMicroSecond + tp.tv_sec * 1000000 * TIMER_CountsPerMicroSecond;
}


int main(int argc, char **argv) {
	application = new QApplication(argc, argv);
	queue = new EventQueue(TimeStamp);
	Wikireader w(application, queue);
	MyThread t(argc, argv);

	fb = w.display();

	w.show();
	t.start();
	int rc = application->exec();
	t.terminate();
	t.wait();
	return rc;
}
