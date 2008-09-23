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

#include "Article.h"

Article::Article()
    : m_isEmpty(true)
{}

Article::Article(const Title& title, const QString& textContent)
    : m_title(title)
    , m_textContent(textContent)
    , m_isEmpty(false)
{}

bool Article::isEmpty() const
{
    return m_isEmpty;
}

Title Article::title() const
{
    return m_title;
}

QString Article::textContent() const
{
    return m_textContent;
}

QString Article::hash() const
{
    // build sha1 hash of the text content
    return QString();
}
