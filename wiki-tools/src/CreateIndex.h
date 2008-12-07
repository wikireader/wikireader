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

#ifndef CreateIndex_h
#define CreateIndex_h

#include "ArticleHandler.h" 
#include <QTextStream>
#include <QRegExp>
#include <QMap>
/**
 * Extract Titles and build a simple index
 *   Title => Hash
 */
class CreateIndex : public FileOutputArticleHandler {
  public:
    CreateIndex(const QString& splitChars, 
                const QString& indexFileName, 
                const QString& notMatchName,
                const QRegExp& notArticle,
                const QRegExp& match);

    void handleArticle(const Article&);
    void parsingFinished();

  private:
    QString m_splitChars;
    QRegExp m_notArticle;
    QRegExp m_match;

    // title and hash(sha1)
    QMap<QString, QString> m_titleMap;
    QMap<QString, QString> m_redirectMap;

    void resolveRedirects();
    void doMatchAndWrite();

    QFile m_notMatchFile;
    QTextStream m_notMatchStream;
};

#endif
