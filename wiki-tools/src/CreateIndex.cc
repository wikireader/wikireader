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

CreateIndex::CreateIndex(const QString& fileName, QRegExp *filter)
    : FileOutputArticleHandler(fileName)
    , m_filter(filter)
{}

// TODO recognize redirections and resolve them
void CreateIndex::handleArticle(const Article& article)
{
    QTextStream stream(&m_file); 
    QString title = article.title().title().toLower();
    QString hash = article.isRedirect() ? article.redirectsTo() : article.hash();
    bool match = (*m_filter).exactMatch(title);                                   

    if(match){
        if (map.contains(title) && map.value(title) == hash){

        }else{
            map.insert(title, hash);

            stream << title << "--";
            stream << hash;
            stream << "\n"; 
        }
    }else{
	    //TODO: figure out the title we remove.
	    //1. not match 2.double (lowcase) 3. redirect
    }
}
