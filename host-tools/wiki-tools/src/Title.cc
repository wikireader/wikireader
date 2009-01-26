/*
 * Wiki Handling Tool
 *
 * Copyright (C) 2008 Openmoko Inc.
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

#include "Title.h"
#include <QStringList>
#include <QTextBoundaryFinder>

Title::Title(const QString& title)
    : m_title(title)
{}

QString Title::title() const
{
    return m_title;
}

QStringList Title::words() const
{
    QStringList usedWords;
    QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Word, m_title);

    int lastPosition = 0;
    boundaryFinder.setPosition(lastPosition);

    for (int pos = boundaryFinder.toNextBoundary(); pos > 0; pos = boundaryFinder.toNextBoundary()) {
        QString word = m_title.mid(lastPosition, pos - lastPosition);

        // Do not add words that only contain ' '
        if (!word.trimmed().isEmpty())
            usedWords << m_title.mid(lastPosition, pos-lastPosition);
        lastPosition = pos;
    }

    return usedWords;
}
