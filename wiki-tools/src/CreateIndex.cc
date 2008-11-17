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
    , m_match(match)
    , m_notArticle(notArticle)
{
    m_notMatchFile.setFileName(notMatchName);
    m_notMatchFile.open(QFile::WriteOnly | QFile::Truncate);
    m_notMatchStream = new QTextStream(&m_notMatchFile);
    (*m_notMatchStream) << "----------after here is not arctile. like Image: etc.\n";
    m_notMatchCount = 0;
}

void CreateIndex::handleArticle(const Article& article)
{
    QString title = article.title().title();
    if (m_notArticle.exactMatch(title)) {
        (*m_notMatchStream) << title << m_splitChars << article.hash() << "\n";
        m_notMatchCount++;
        return ;
    }
    if (article.isRedirect()) {
        m_redirectMap.insert(title, article.redirectsTo());
    } else {
        m_titleMap.insert(title, article.hash());
    }
    qDebug()<< m_titleMap.count()<<m_splitChars<<m_redirectMap.count()<<m_splitChars<<m_notMatchCount;
}

void CreateIndex::resolveRedirect()
{
    int i=0,findTimes = 0;
    QString title, redirectTo, hash;
    
    (*m_notMatchStream) << "----------after here is fail redirect title\n";
    m_notMatchCount = 0;
    foreach (title, m_redirectMap.keys()) {
        redirectTo = m_redirectMap[title];
        while (m_redirectMap.contains(redirectTo)) {
            redirectTo = m_redirectMap.value(redirectTo);
            findTimes++;
            if (findTimes == 1000) {
                findTimes = 0;
                m_notMatchCount ++;
                (*m_notMatchStream) << title << m_splitChars << redirectTo <<"\n";
                qDebug()<<"find 1000 times";
                break;
            }
        }
        if (m_titleMap.contains(redirectTo))
            m_titleMap.insert(title, m_titleMap.value(redirectTo));
        qDebug() << m_titleMap.count() << m_splitChars << i++ ;
    }
    (*m_notMatchStream) << "----------fail redirect titles count is: " << m_notMatchCount << "\n";
}

void CreateIndex::doMatchAndWrite()
{
    QTextStream stream(&m_file); 
    QString title, hash;

    (*m_notMatchStream) << "----------after here is not match titles.\n";
    m_notMatchCount = 0;
    foreach (QString key, m_titleMap.keys()) {
        if ( title == key.toLower() && hash == m_titleMap.keys())
            continue;
        QString indexLine = key.toLower() + m_splitChars +m_titleMap[key];
        if (m_match.exactMatch(key))
            stream << indexLine;
        else {
            (*m_notMatchStream) << indexLine;
            m_notMatchCount ++;
        }
        title = key.toLower();
        hash = m_titleMap.keys();
    }
    (*m_notMatchStream) << "----------not match titles count: " << m_notMatchCount << "\n";
    (*m_notMatchStream) << "----------all over :-) \n";
}

void CreateIndex::parsingFinished()
{
    (*m_notMatchStream) << "----------not article count is: "<<m_notMatchCount<<"\n";
    resolveRedirect();
    doMatchAndWrite();

    m_notMatchFile.close();
    FileOutputArticleHandler::parsingFinished();
}
