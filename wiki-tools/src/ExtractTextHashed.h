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

#ifndef ExtractTitlesHashed_h
#define ExtractTitlesHashed_h

#include "ArticleHandler.h"

/**
 * Extract the articles into a hashed structure. We build a sha256sum
 * on the text and we take the first two values of the hash to spread the
 * file.
 */
class ExtractTextHashed : public ArticleHandler {
public:
    ExtractTextHashed(const QString& outputDir);

    void parsingStarts();
    void handleArticle(const Article&);

    void parsingFinished() {}

protected:
    QString m_baseDir;
};

#endif
