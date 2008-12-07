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
#include <QtDebug>

CreateIndex::CreateIndex(const QString& splitChars, const QString& indexFileName, 
                         const QString& notMatchName, 
                         const QRegExp& notArticle,
                         const QRegExp& match)
    : FileOutputArticleHandler(indexFileName)
    , m_splitChars(splitChars)
    , m_notArticle(notArticle)
    , m_match(match)
    , m_notMatchStream(&m_notMatchFile)
{
    m_notMatchFile.setFileName(notMatchName);
    m_notMatchFile.open(QFile::WriteOnly | QFile::Truncate);
    m_notMatchStream << "----------after here is not arctile. like Image: etc.\n";
}

void CreateIndex::handleArticle(const Article& article)
{
    QString title = article.title().title();

    /*
     * black listed articles
     */
    if (m_notArticle.exactMatch(title)) {
        m_notMatchStream << title << m_splitChars << article.hash() << "\n";
        return;
    }

    if (article.isRedirect())
        m_redirectMap.insert(title, article.redirectsTo());
    else
        m_titleMap.insert(title, article.hash());
}

void CreateIndex::resolveRedirects()
{
    m_notMatchStream << "----------after here is fail redirect title\n";
    foreach (QString title, m_redirectMap.keys()) {
        QString redirectsTo = m_redirectMap[title];

        /*
         * if we run in a circle redirectTo will be title...
         */
        while (m_redirectMap.contains(redirectsTo) && title != redirectsTo)
            redirectsTo = m_redirectMap.value(redirectsTo);

        if (m_titleMap.contains(redirectsTo))
            m_titleMap.insert(title, m_titleMap.value(redirectsTo));
        else
            m_notMatchStream << "Ignoring redirect for: " << title << " to: " << redirectsTo;
    }

    m_notMatchStream << "----------fail redirect titles count is: " << "\n";
}

void CreateIndex::doMatchAndWrite()
{
    QString longestTitle;
    QTextStream stream(&m_file); 

    m_notMatchStream << "----------after here is not match titles.\n";
    QMap<QString, QString>::const_iterator it, end = m_titleMap.end();
    for (it = m_titleMap.begin(); it != end; ++it) {
        QString indexLine = QString("%1%2%3\n").arg(it.key().toLower())
                                               .arg(m_splitChars)
                                               .arg(it.value());

        if (m_match.exactMatch(it.key())) {
            if (longestTitle.length() < it.key().length())
                longestTitle = it.key();

            stream << indexLine;
        } else {
            m_notMatchStream << indexLine;
        }
    }

    m_notMatchStream << "----------longest Title is: " << longestTitle << "length is: " << longestTitle.length() << "\n";
    m_notMatchStream << "----------all over :-) \n";
}

void CreateIndex::parsingFinished()
{
    resolveRedirects();
    doMatchAndWrite();

    m_notMatchFile.close();
    FileOutputArticleHandler::parsingFinished();
}
