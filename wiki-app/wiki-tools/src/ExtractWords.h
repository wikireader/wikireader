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

#ifndef ExtractWords_h
#define ExtractWords_h

#include "ArticleHandler.h"

/**
 * Generate a dictionary to be used by tools like Qtopia for the predictive
 * keyboard.
 */
class ExtractWords : public ArticleHandler {
public:
    ExtractWords(const QString& wordbook);

    void parsingStarts();
    void parsingFinished();
    void handleArticle(const Article&);

private:
    QString m_wordBook;
};

#endif
