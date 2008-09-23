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

#include "ArticleHandler.h"

ArticleHandler::~ArticleHandler()
{}

FileOutputArticleHandler::FileOutputArticleHandler(const QString& fileName)
    : m_fileName(fileName)
{}

void FileOutputArticleHandler::parsingStarts()
{
    m_file.close();
    m_file.setFileName(m_fileName);
    m_file.open(QFile::WriteOnly | QFile::Truncate);
}

void FileOutputArticleHandler::parsingFinished()
{
    m_file.close();
}
