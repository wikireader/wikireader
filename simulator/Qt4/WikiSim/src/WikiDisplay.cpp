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
#include "WikiDisplay.h"
#include "guilib.h"

WikiDisplay::WikiDisplay(QWidget *parent)
 : QWidget(parent)
{
	setMinimumSize(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	setMaximumSize(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	framebuffer = new QByteArray(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT, 0);
}

WikiDisplay::~WikiDisplay()
{
	delete framebuffer;
}

void
WikiDisplay::setPixel(int x, int y, int v)
{
	char *data = framebuffer->data();

	v &= 0xf;

	if (x >= FRAMEBUFFER_WIDTH || y >= FRAMEBUFFER_HEIGHT) {
		printf("pixel position out of range: %d,%d\n", x, y);
		return;
	}

	data[y * FRAMEBUFFER_WIDTH + x] |= v;
}

void
WikiDisplay::clear(void)
{
	framebuffer->fill(0);
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
			const char *data = framebuffer->data();
			r = g = b = 255 - (data[y * FRAMEBUFFER_WIDTH + x] * 0xf);
			painter.setPen(QColor::fromRgb(r, g, b, 255));
			painter.drawPoint(x, y);
		}
}
