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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ExtractTextHashed::ExtractTextHashed(const QString& outputDir)
    : m_baseDir(outputDir)
{}

void ExtractTextHashed::parsingStarts()
{}


// now put the text into the right place
void ExtractTextHashed::handleArticle(const Article& article)
{
    const QString name = article.hash(); 

    // Create the directory structure on the fly
    QDir dir;

    dir.mkdir(m_baseDir);
    dir.cd(m_baseDir);

    for (int i = 0; i < 4; ++i) {
        dir.mkdir(name.mid(i*2, 2));
        dir.cd(name.mid(i*2, 2));
    }


    QFile file;
    QString fileName = m_baseDir + QDir::separator();

    for (int i = 0; i < 4; ++i)
        fileName += name.mid(i*2, 2) + QDir::separator();

    fileName += name;

    file.setFileName(fileName);
    file.open(QFile::WriteOnly);

    QTextStream stream(&file);
    stream << article.textContent();

    file.close();
}
