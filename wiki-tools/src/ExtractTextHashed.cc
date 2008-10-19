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

#include "ExtractTextHashed.h"

#include <QDir>
#include <QTextStream>

ExtractTextHashed::ExtractTextHashed(const QString& outputDir)
    : m_baseDir(outputDir)
{}

void ExtractTextHashed::parsingStarts()
{
    QDir dir;
    dir.mkdir(m_baseDir);
    dir.cd(m_baseDir);

    // Create the hash directories 0-f 0-f
#define CREATE_DIR           \
        name[0] = QChar(i);  \
        name[1] = QChar(j);  \
        dir.mkdir(name);

    QString name;
    name.resize(2);
    for (int i = 48; i <= 57; ++i) {
        for (int j = 48; j <= 57; ++j) {
            CREATE_DIR
        }

        for (int j = 'a'; j <= 'f'; ++j) {
            CREATE_DIR
        }
    }

    for (int i = 'a'; i <= 'f'; ++i) {
        for (int j = 48; j <= 57; ++j) {
            CREATE_DIR
        }

        for (int j = 'a'; j <= 'f'; ++j) {
            CREATE_DIR
        }
    }
}


// now put the text into the right place
void ExtractTextHashed::handleArticle(const Article& article)
{
    QString name = article.hash(); 
    
    QFile file;
    file.setFileName(m_baseDir + QDir::separator() + name.left(2) + QDir::separator() + name);
    file.open(QFile::WriteOnly);

    QTextStream stream(&file);
    stream << article.textContent();
}
