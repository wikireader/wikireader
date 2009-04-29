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

#include <QApplication>

Article::Article()
    : m_isEmpty(true)
{}

Article::Article(const Title& title, const QString& textContent)
    : m_title(title)
    , m_textContent(textContent)
    , m_isEmpty(false)
    , m_isRedirect(false)
    , m_determinedRedirect(false)
{}

bool Article::isEmpty() const
{
    return m_isEmpty;
}

QString Article::redirectsTo() const
{
    return m_redirectsTo;
}

bool Article::isRedirect() const
{
    if (m_determinedRedirect)
        return m_isRedirect;

    // in Python notation re.compile("^\#REDIRECT\s\[\[(?P<title>[a-zA-Z0-9- \t]*)\]\]$")
    // and done manually here. This means the string is walked multiple times but only
    // in the case of a #REDIRECT.
    m_determinedRedirect = true;

    bool redirectSpace  = m_textContent.startsWith("#REDIRECT [[", Qt::CaseInsensitive);
    bool redirectDirect = m_textContent.startsWith("#REDIRECT[[", Qt::CaseInsensitive);

    if (!redirectSpace && !redirectDirect)
        return m_isRedirect;

    if (m_textContent.contains("\n"))
        return m_isRedirect;

    if (!m_textContent.contains("]]"))
        return m_isRedirect;

    m_isRedirect = true;
    if (redirectSpace)
        m_redirectsTo = m_textContent.mid(sizeof "#REDIRECT [[" - 1);
    else
        m_redirectsTo = m_textContent.mid(sizeof "#REDIRECT[[" - 1);

    if (m_redirectsTo.contains("|"))
        m_redirectsTo = m_redirectsTo.left(m_redirectsTo.indexOf("|"));

    m_redirectsTo = m_redirectsTo.left(m_redirectsTo.indexOf("]]"));
    return m_isRedirect;
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

