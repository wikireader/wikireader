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
#include "ExtractText.h"
#include "ExtractTextHashed.h"
#include "ExtractTitles.h"
#include "SplitArticles.h"

#include "StreamReader.h"

#include <QCoreApplication>
#include <QRegExp>

static void setupHandlers(QList<ArticleHandler*>* handlers, int, char**)
{
    *handlers << new CreateIndex(QLatin1String("indexfile.index"), 
				 QRegExp("[0-9A-Za-z\\s.]*") );
    *handlers << new CreateText(QString());
    *handlers << new ExtractTitles(QString("extracted.titles"));
    *handlers << new ExtractWords(QString());
    *handlers << new ExtractText(QString("extracted.text"));

    // Enable that if you want to split articles
    //*handlers << new SplitArticles(50, QString("split_enwiki"));

    // Enable that if you want to split the articles into a hash structur
    //*handlers << new ExtractTextHashed(QString("articles"));
}

#define INVOKE_HANDLERS(function)               \
    foreach(ArticleHandler* handler, handlers)  \
        handler->function;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QList<ArticleHandler*> handlers;
    StreamReader reader;

    QFile file;
    if (!file.open(STDIN_FILENO, QFile::ReadOnly)) {
        fprintf(stderr, "Failed to open stdin\n");
        return EXIT_FAILURE;
    }

    setupHandlers(&handlers, argc, argv);

    INVOKE_HANDLERS(parsingStarts())

    do {
        // Write whatever is available
        file.waitForReadyRead(-1);
        reader.write(file.read(file.bytesAvailable()));

        // Did we finish parsing an article?
        QList<Article> articles = reader.popArticles();
        foreach(ArticleHandler* handler, handlers)
            foreach(Article article, articles)
                handler->handleArticle(article);
    } while (!reader.finished());

    INVOKE_HANDLERS(parsingFinished())
}

