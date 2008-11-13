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

#ifndef ExtractTextCompressed_h
#define ExtractTextCompressed_h

#include "ArticleHandler.h"
#include "Compression.h"

/**
 * Extract the "Text" of wikipedia into one big file
 */
class ExtractTextCompressed : public FileOutputArticleHandler {
public:
    ExtractTextCompressed(const QString& outputFile, Compression::CompressionMethod, int level);

    void handleArticle(const Article& text);

private:
    Compression::CompressionMethod m_method;
    int m_level;
};

#endif
