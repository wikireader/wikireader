/*
 * Wiki Handling Tool
 *
 * Copyright (C) 2008, 2009 Openmoko Inc.
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

#include "ExtractArticleUrl.h"

#include <QTextStream>

ExtractArticleUrl::ExtractArticleUrl(const QString& outputFile)
    : FileOutputArticleHandler(outputFile)
{}

void ExtractArticleUrl::handleArticle(const Article& article)
{
    if (article.isRedirect())
        return;

    /* the secret of our class... what to hide */
    if (article.title().title().startsWith("Image:")
        || article.title().title().startsWith("Template:")
        || article.title().title().startsWith("Category:")
        || article.title().title().startsWith("Wikipedia:"))
        return;

    QTextStream stream(&m_file);
    stream << "http://127.0.0.1/mediawiki/index.php/" + article.title().title() << endl;
}
