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
#include "sha1.h"

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

QByteArray Article::hash() const
{
    // build sha1 hash of the text content
    if (!m_result.isEmpty())
        return m_result;

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    QByteArray utf8 = m_textContent.toUtf8();
    QByteArray result;
    result.resize(20);
    SHA1_Update(&ctx, utf8.constData(), utf8.length());
    SHA1_Final((unsigned char*)result.data(), &ctx);

    m_result = result.toHex();
    return m_result;
}
