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

#ifndef StreamReader_h
#define StreamReader_h

#include "Article.h"
#include <QByteArray>
#include <QXmlStreamReader>

/**
 * This class will parse Article's from a WikiMedia stream. You can pipe
 * chunks of data into it and pop the parsed articles.
 */
class StreamReader {
public:
    StreamReader();

    bool finished() const;
    bool write(const QByteArray& data);
    QList<Article> popArticles();

private:
    enum State {
        State_None,
        State_Page,
        State_Title,
        State_Revision,
        State_Text,
    };

    QXmlStreamReader m_xmlReader;
    QList<Article> m_queuedArticles;
    Title m_currentTitle;
    QString m_currentText;
    QString m_pendingString;
    State m_parseState;
};

#endif
