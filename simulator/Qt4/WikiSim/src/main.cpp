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

#include <QtGui>
#include <QApplication>
#include "MainWindow.h"
#include "WikiDisplay.h"

/* we keep this one static for the plain-C callbacks */
MainWindow *window;

extern "C" {
#include "guilib.h"

/* this is the gui-lib glue layer */
void fb_set_pixel(int x, int y, int v)
{
	WikiDisplay *display = window->getDisplay();
	display->setPixel(x, y, v);
}

void fb_refresh(void)
{
	WikiDisplay *display = window->getDisplay();
	display->repaint();
}

void fb_clear(void)
{
	WikiDisplay *display = window->getDisplay();
	display->clear();
}

} /* extern "C" */

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	window = new MainWindow();
	window->show();
	guilib_init();
	app.exec();
	return 0;
}
