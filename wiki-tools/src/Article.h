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

#ifndef Article_h
#define Article_h

#include "Title.h"

/**
 * Class representing a simple wiki (wikimedia) page also called article.
 */
class Article {
public:
    Article();
    Article(const Title& title, const QString& textContent);

    bool isEmpty() const;
    Title   title() const;
    QString textContent() const;
    QString hash() const;

private:
    Title   m_title;
    QString m_textContent;
    bool    m_isEmpty;
};

#endif
