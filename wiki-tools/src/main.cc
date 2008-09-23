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
#include "CreateText.h"
#include "ExtractWords.h"
#include "ExtractTitles.h"

#include "StreamReader.h"

#include <QCoreApplication>

static void setupHandlers(QList<ArticleHandler*>* handlers, int argc, char** argv)
{
    *handlers << new CreateIndex(QString());
    *handlers << new CreateText(QString());
    *handlers << new ExtractTitles(QString());
    *handlers << new ExtractWords(QString());
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QList<ArticleHandler*> handlers;
    StreamReader reader;

    setupHandlers(&handlers, argc, argv);

    foreach(ArticleHandler* handler, handlers)
        handler->parsingStarts();

    do {
        QList<Article> articles = reader.popArticles();
        foreach(ArticleHandler* handler, handlers)
            foreach(Article article, articles)
                handler->handleArticle(article);
    } while (!reader.hasError());

    foreach(ArticleHandler* handler, handlers)
        handler->parsingFinished();
}

