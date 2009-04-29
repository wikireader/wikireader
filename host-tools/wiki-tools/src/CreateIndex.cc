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

#include "CreateIndex.h"
#include <QTextStream>
#include <QRegExp>
#include <QSqlQuery>
#include <QVariant>
#include <QtDebug>

class LowercaseString {
public:
    explicit LowercaseString(const QString&);
    bool operator< (const LowercaseString&) const;
    bool operator==(const LowercaseString&) const;

    QString string() const;

private:
    QString m_string;
};

LowercaseString::LowercaseString(const QString& string)
    : m_string(string)
{}

bool LowercaseString::operator< (const LowercaseString& rhs) const
{
    return m_string.toLower().operator<(rhs.m_string.toLower());
}

bool LowercaseString::operator==(const LowercaseString& rhs) const
{
    return m_string.toLower().operator==(rhs.m_string.toLower());
}

QString LowercaseString::string() const
{
    return m_string;
}


CreateIndex::CreateIndex(const QString& splitChars, const QSqlDatabase& db,
                         const QString& notMatchName, 
                         const QRegExp& notArticle,
                         const QRegExp& match)
    : ArticleHandler()
    , m_splitChars(splitChars)
    , m_notArticle(notArticle)
    , m_match(match)
    , m_notMatchStream(&m_notMatchFile)
    , m_database(db)
{
    m_notMatchFile.setFileName(notMatchName);
    m_notMatchFile.open(QFile::WriteOnly | QFile::Truncate);
    m_notMatchStream << "----------after here is not arctile. like Image: etc.\n";

    /* create some tables now */
    QSqlQuery query(m_database);
    query.exec("CREATE TABLE IF NOT EXISTS IndexTable ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "title TEXT NOT NULL,"
                    "hash TEXT NOT NULL)");
    query.exec("CREATE TABLE IF NOT EXISTS Offsets ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "file NUMERIC NOT NULL,"
                    "offset NUMERIC UNIQUE NOT NULL,"
                    "hash TEXT UNIQUE NOT NULL)");
    query.exec("CREATE INDEX IF NOT EXISTS IndexHash ON IndexTable (hash)");
    query.exec("CREATE INDEX IF NOT EXISTS OffsetsHash On Offsets (hash)");
}

void CreateIndex::handleArticle(const Article& article)
{
    QString title = article.title().title();

    /*
     * black listed articles
     */
    if (m_notArticle.exactMatch(title)) {
        m_notMatchStream << title << m_splitChars << article.hash() << "\n";
        return;
    }

    if (article.isRedirect())
        m_redirectMap.insert(title, article.redirectsTo());
    else
        m_titleMap.insert(title, article.hash());
}

void CreateIndex::resolveRedirects()
{
    m_notMatchStream << "----------after here is fail redirect title\n";
    foreach (QString title, m_redirectMap.keys()) {
        QString redirectsTo = m_redirectMap[title];

        /*
         * if we run in a circle redirectTo will be title...
         */
        QHash<QString, bool> circleDetector;
        circleDetector.insert(title, 1);

        while (m_redirectMap.contains(redirectsTo)) {
            redirectsTo = m_redirectMap.value(redirectsTo);

            if (circleDetector.contains(redirectsTo)) {
                qWarning() << "Circle detected: " << title << circleDetector;
                break;
            }

            circleDetector.insert(redirectsTo, 1);
        }

        if (m_titleMap.contains(redirectsTo))
            m_titleMap.insert(title, m_titleMap.value(redirectsTo));
        else
            m_notMatchStream << "Ignoring redirect for: " << title << " to: " << redirectsTo << endl;
    }

    m_notMatchStream << "----------fail redirect titles count is: " << "\n";
}

void CreateIndex::doMatchAndWrite()
{
    m_notMatchStream << "----------after here is not match titles.\n";

    QString longestTitle;

    /*
     * reshuffle the maps. Now we actually have a multimap... but sometimes
     * it does not make sense to add two identical versions... so add it to
     * the multimap when we don't have the key yet or the values are different.
     *
     * E.g. currently we have
     *   Doener c90b14
     *   Doener kebab c90b14
     *   Doener Kebab c90b14
     *   Doener kebab c90b14
     *
     * Doener and Doener Kebab would be enough but
     * Doener and Doener kebab would do as welll...
     */
    QMap<QString, QString>::const_iterator mapIt, mapEnd = m_titleMap.end();
    QMultiMap<LowercaseString, QString> titleMap;
    for (mapIt = m_titleMap.begin(); mapIt != mapEnd; ++mapIt) {
        LowercaseString lowercaseTitle(mapIt.key());

        /* see if we have the value already... */
        bool found = false;
        QMap<LowercaseString, QString>::iterator existingValue = titleMap.find(lowercaseTitle);
        for ( ; existingValue != titleMap.end() && !found; ++existingValue)
            found = existingValue.value() == mapIt.value();

        /* add it now */
        if (!found)
            titleMap.insert(lowercaseTitle, mapIt.value());
    }

    /*
     * safe memory after we had doubled it... we could even avoid
     * the peak when removing it from m_titleMap in the above loop
     */
    m_titleMap = QMap<QString,QString>();


    m_database.transaction();
    QMultiMap<LowercaseString, QString>::const_iterator multiIt, multiEnd = titleMap.end();
    for (multiIt = titleMap.begin(); multiIt != multiEnd; ++multiIt) {
        QString title = multiIt.key().string();

        if (m_match.exactMatch(title)) {
            if (longestTitle.length() < title.length())
                longestTitle = title;

            QSqlQuery query(m_database);
            query.prepare("INSERT INTO IndexTable (title, hash) VALUES (:title, :hash)");
            query.bindValue(QLatin1String(":title"), title);
            query.bindValue(QLatin1String(":hash"), multiIt.value());
            query.exec();
        } else {
            QString indexLine = QString("%1%2%3\n")
                                                   .arg(title)
                                                   .arg(m_splitChars)
                                                   .arg(multiIt.value().left(6));
            m_notMatchStream << indexLine;
        }
    }

    m_database.commit();
    m_notMatchStream << "----------longest Title is: " << longestTitle << "length is: " << longestTitle.length() << "\n";
    m_notMatchStream << "----------all over :-) \n";
}

void CreateIndex::parsingFinished()
{
    resolveRedirects();
    doMatchAndWrite();

    m_notMatchFile.close();
}
