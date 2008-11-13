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
   if(article.isRedirect()){
      redirectMap.insert(article.title().title(), article.redirectsTo());
   }else{
      map.insert(article.title().title(), article.hash());
   }
}

void CreateIndex::resolveRedirect()
{
   QString title = "",redirectTo = "", hash = "";
   QMap<QString, QString>::const_iterator i = redirectMap.constBegin();
   for( ; i !=redirectMap.constEnd(); i++){
      title = i.key();
      redirectTo = i.value();
      while( redirectMap.contains( redirectTo ) ){
	 redirectTo = redirectMap.value( redirectTo );
      }
      hash = map.value( redirectTo );
      map.insert(title, hash);
   }
}

void CreateIndex::doMatch()
{
   QString title = "",hash = "";
   QTextStream stream(&m_file); 
   //i use a const name here
   QFile notMatchfile;
   notMatchfile.setFileName("notmatch.index");
   notMatchfile.open(QFile::WriteOnly | QFile::Truncate);
   QTextStream notMatchStream(&notMatchfile);
   QMap<QString, QString>::const_iterator i = map.constBegin();

   while (i != map.constEnd()) {
      bool match = m_filter.exactMatch(title);
      if(match){
	 if(title == i.key().toLower() && hash == i.value())
	    continue;
	 stream << i.key().toLower() << "--" << i.value() << endl;

	 title = i.key().toLower();
	 hash = i.value();
      }else{
	 notMatchStream << title << "--" << hash << endl;
      }
      ++i;
   }
}

void CreateIndex::parsingFinished()
{
    resolveRedirect();
    doMatch();
    FileOutputArticleHandler::parsingFinished();
}
