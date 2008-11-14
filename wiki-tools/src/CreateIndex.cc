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

#include "CreateIndex.h"
#include <QTextStream>
#include <QRegExp>

CreateIndex::CreateIndex(const QString& fileName, const QRegExp& filter)
    : FileOutputArticleHandler(fileName)
    , m_filter(filter)
{}

void CreateIndex::handleArticle(const Article& article)
{
    if ( article.isRedirect() ) {
        m_redirectMap.insert(article.title().title(), article.redirectsTo());
    } else {
        m_map.insert(article.title().title(), article.hash());
    }
}

void CreateIndex::resolveRedirect()
{
    QString title, redirectTo, hash;
    QMap<QString, QString>::const_iterator i = m_redirectMap.constBegin();
    for ( ; i != m_redirectMap.constEnd(); i++) {
        title = i.key();
        redirectTo = i.value();
        while (m_redirectMap.contains(redirectTo)) {
            redirectTo = m_redirectMap.value(redirectTo);
        }
        hash = m_map.value(redirectTo);
        m_map.insert(title, hash);
    }
}

void CreateIndex::doMatchAndWrite()
{
    QTextStream stream(&m_file); 

    //i use a const name here
    QFile notMatchfile;
    notMatchfile.setFileName("notmatch.index");
    notMatchfile.open(QFile::WriteOnly | QFile::Truncate);
    QTextStream notMatchStream(&notMatchfile);

    foreach (QString key, m_map.keys()) {
        QString indexLine = QString::fromLatin1("%1--%2").arg(key.toLower()).arg(m_map[key]);
        if (m_filter.exactMatch(key))
            stream << indexLine;
        else
            notMatchStream << indexLine;
    }
}

void CreateIndex::parsingFinished()
{
    resolveRedirect();
    doMatchAndWrite();
    FileOutputArticleHandler::parsingFinished();
}
