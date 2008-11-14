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

CreateIndex::CreateIndex(const QString& fileName, const QRegExp& filter)
    : FileOutputArticleHandler(fileName)
    , m_filter(filter)
{
    m_imageEtcFile.setFileName("imageEtc.title");
    m_imageEtcFile.open(QFile::WriteOnly | QFile::Truncate);
    m_imageEtcStream = new QTextStream(&m_imageEtcFile);
    m_imageEtcCount = 0;
}

void CreateIndex::handleArticle(const Article& article)
{
    QString title = article.title().title();
    if (title.startsWith("Image:") || title.startsWith("Category:") ||
        title.startsWith("Talk:") || title.startsWith("Template:")) {
        (*m_imageEtcStream)<<title<<"--"<<article.hash()<<endl;
        m_imageEtcCount++;
        return ;
    }
    if (article.isRedirect()) {
        m_redirectMap.insert(title, article.redirectsTo());
    } else {
        m_map.insert(title, article.hash());
    }
    qDebug()<< m_map.count()<<"---"<<m_redirectMap.count()<<"---"<<m_imageEtcCount<< endl;
}

void CreateIndex::resolveRedirect()
{
    int i=0;
    QString title, redirectTo, hash;
    foreach (title, m_redirectMap.keys()) {
        redirectTo = m_redirectMap[title];
        while (m_redirectMap.contains(redirectTo)) {
            redirectTo = m_redirectMap.value(redirectTo);
        }
        if (m_map.contains(redirectTo))
            m_map.insert(title, m_map.value(redirectTo));
        qDebug() << m_map.count() << "---" << i++ <<endl;
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
    qDebug()<<"begin resolve"<<endl;
    resolveRedirect();
    qDebug()<<"begin match and write"<<endl;
    doMatchAndWrite();

    m_imageEtcFile.close();
    FileOutputArticleHandler::parsingFinished();
}
