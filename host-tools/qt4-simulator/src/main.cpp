/***************************************************************************
 *   Copyright (C) 2009 Openmoko Inc.                                      *
 *                                                                         *
 *   Authors   Daniel Mack <daniel@caiaq.de>                               *
 *             Christopher Hall <hsw@openmoko.com>                         *
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

#include <QtGui>
#include <QApplication>
#include "MainWindow.h"
#include "WikiDisplay.h"
#include "WikilibThread.h"

MainWindow *window;

long idx_init_article = 0;

int main(int argc, char *argv[])
{
	if (argc > 1) {
		idx_init_article = atol(argv[1]);
		if (0 > idx_init_article) {
			idx_init_article = 0;
		}
	}

	if ( 0 != idx_init_article) {
		printf("initial article to load: %ld\n", idx_init_article);
	}

	WikilibThread *thread = new WikilibThread();
	QApplication app(argc, argv);

	window = new MainWindow();
	window->show();
	thread->start(QThread::HighPriority);
	app.exec();
	return 0;
}
