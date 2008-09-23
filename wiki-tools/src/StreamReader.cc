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

#include "StreamReader.h"

StreamReader::StreamReader()
    : m_parseState(State_None)
{}

bool StreamReader::finished() const
{
    return m_xmlReader.isEndDocument();
}

bool StreamReader::write(const QByteArray& data)
{
    m_xmlReader.addData(data);

    // Extract title and content from the XML Document
    while (!m_xmlReader.atEnd()) {
        m_xmlReader.readNext();

        if (m_xmlReader.isStartElement()) {
            if (m_xmlReader.name() == QLatin1String("page") && m_parseState == State_None) {
                m_currentText = QString();
                m_currentTitle = Title();
                m_parseState = State_Page;
            } else if (m_xmlReader.name() == QLatin1String("title") && m_parseState == State_Page) {
                m_pendingString = QString();
                m_parseState = State_Title;
            } else if (m_xmlReader.name() == QLatin1String("revision") && m_parseState == State_Page) {
                m_parseState = State_Revision;
            } else if (m_xmlReader.name() == QLatin1String("text") && m_parseState == State_Revision) {
                m_pendingString = QString();
                m_parseState = State_Text;
            }
        } else if (m_xmlReader.isEndElement()) {
            if (m_xmlReader.name() == QLatin1String("page") && m_parseState == State_Page) {
                m_queuedArticles << Article(m_currentTitle, m_currentText);
                m_parseState = State_None;
            } else if (m_xmlReader.name() == QLatin1String("title") && m_parseState == State_Title) {
                m_currentTitle = Title(m_pendingString);
                m_pendingString = QString();
                m_parseState = State_Page;
            } else if (m_xmlReader.name() == QLatin1String("revision") && m_parseState == State_Revision) {
                m_parseState = State_Page;
            } else if (m_xmlReader.name() == QLatin1String("text") && m_parseState == State_Text) {
                m_currentText = m_pendingString;
                m_pendingString = QString();
                m_parseState = State_Revision;
            }
        } else if (m_xmlReader.isCharacters() && (m_parseState == State_Title || m_parseState == State_Text)) {
            m_pendingString.append(m_xmlReader.text());
        }
    }

    return true;
}

QList<Article> StreamReader::popArticles()
{
    QList<Article> articles = m_queuedArticles;
    m_queuedArticles = QList<Article>();

    return articles;
}
