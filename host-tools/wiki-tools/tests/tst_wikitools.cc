/*
 * Wiki Handling Tool - Tests
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

#include <QtTest/QtTest>
#include "Article.h"

class ArticleRedirectTest : public QObject {
    Q_OBJECT
public:
    ArticleRedirectTest();

private Q_SLOTS:
    void testRedirect();
    void testRedirect_data();
};


ArticleRedirectTest::ArticleRedirectTest()
{}


void ArticleRedirectTest::testRedirect_data()
{
    QTest::addColumn<QString>("article");
    QTest::addColumn<bool>("isRedirect");
    QTest::addColumn<QString>("target");

    QTest::newRow("redirect 1")
            << QString::fromLatin1("#REDIRECT [[Geography of Afghanistan]] {{R from CamelCase}}")
            << true
            << QString::fromLatin1("Geography of Afghanistan");
    QTest::newRow("redirect 2")
            << QString::fromLatin1("#REDIRECT [[African American]] {{R from CamelCase}}")
            << true
            << QString::fromLatin1("African American");
    QTest::newRow("redirect 3")
            << QString::fromLatin1("#redirect [[applied_mathematics]]{{R from other capitalisation}}")
            << true
            << QString::fromLatin1("applied_mathematics");
    QTest::newRow("normal 1")
            << QString::fromLatin1("{{Otherusesof|A}}")
            << false
            << QString();
}

void ArticleRedirectTest::testRedirect()
{
    QFETCH(QString, article);
    QFETCH(bool, isRedirect);
    QFETCH(QString, target);

    Article wikiArticle(Title(), article);
    QCOMPARE(wikiArticle.isRedirect(), isRedirect);
    QCOMPARE(wikiArticle.redirectsTo(), target);
}

QTEST_MAIN(ArticleRedirectTest)
#include "tst_wikitools.moc"
