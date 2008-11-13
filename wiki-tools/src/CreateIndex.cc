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

// TODO recognize redirections and resolve them
void CreateIndex::handleArticle(const Article& article)
{
    QString title = article.title().title();
    QString hash = article.isRedirect() ? article.redirectsTo() : article.hash();
    bool match = m_filter.exactMatch(title);                                   

    if(match){
        if (map.contains(title) && map.value(title) == hash){

        }else{
            map.insert(title, hash);
            //TODO:redirect code here
        }
    }else{
	    //TODO: figure out the title we remove.
	    //1. not match 2.double (lowcase) 3. redirect
    }
}

void CreateIndex::parsingFinished()
{
   QTextStream stream(&m_file); 
   QMap<QString, QString>::const_iterator i = map.constBegin();
   while (i != map.constEnd()) {
      stream << i.key().toLower() << "--" << i.value() << endl;
      ++i;
   }
   FileOutputArticleHandler::parsingFinished();
}
