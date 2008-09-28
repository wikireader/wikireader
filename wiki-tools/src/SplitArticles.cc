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

#include "SplitArticles.h"

#include <QTextStream>

// from QXmlStreamWriterPrivate::writeEscaped
static QString escape(const QString& s)
{
    QString escaped;
    escaped.reserve(s.size());
    for ( int i = 0; i < s.size(); ++i ) {
        QChar c = s.at(i);
        if (c.unicode() == '<' )
            escaped.append(QLatin1String("&lt;"));
        else if (c.unicode() == '>' )
            escaped.append(QLatin1String("&gt;"));
        else if (c.unicode() == '&' )
            escaped.append(QLatin1String("&amp;"));
        else if (c.unicode() == '\"' )
            escaped.append(QLatin1String("&quot;"));
        else
            escaped += QChar(c);
    }

    return escaped;
}

SplitArticles::SplitArticles(int numberOfArticles, const QString& baseName)
    : FileOutputArticleHandler(baseName)
    , m_splitAtNumberOfArticles(numberOfArticles)
    , m_readArticles(0)
    , m_fileNumber(0)
{}

void SplitArticles::parsingStarts()
{
    m_readArticles = 0;
    m_fileNumber = 0;
    startNextSet();
}

void SplitArticles::startNextSet()
{
    if (m_file.isOpen()) {
        QTextStream stream(&m_file);
        stream << "</mediawiki>";
        closeCurrentFile();
    }


    openFile(m_fileName + "_" + QString::number(m_fileNumber++));

    QTextStream stream(&m_file);
    stream << "<mediawiki xmlns=\"http://www.mediawiki.org/xml/export-0.3/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.mediawiki.org/xml/export-0.3/ http://www.mediawiki.org/xml/export-0.3.xsd\" version=\"0.3\" xml:lang=\"en\">\n"
"  <siteinfo>\n"
"    <sitename>Wikipedia</sitename>\n"
"    <base>http://en.wikipedia.org/wiki/Main_Page</base>\n"
"    <generator>MediaWiki 1.14alpha</generator>\n"
"    <case>first-letter</case>\n"
"      <namespaces>\n"
"      <namespace key=\"-2\">Media</namespace>\n"
"      <namespace key=\"-1\">Special</namespace>\n"
"      <namespace key=\"0\" />\n"
"      <namespace key=\"1\">Talk</namespace>\n"
"      <namespace key=\"2\">User</namespace>\n"
"      <namespace key=\"3\">User talk</namespace>\n"
"      <namespace key=\"4\">Wikipedia</namespace>\n"
"      <namespace key=\"5\">Wikipedia talk</namespace>\n"
"      <namespace key=\"6\">Image</namespace>\n"
"      <namespace key=\"7\">Image talk</namespace>\n"
"      <namespace key=\"8\">MediaWiki</namespace>\n"
"      <namespace key=\"9\">MediaWiki talk</namespace>\n"
"      <namespace key=\"10\">Template</namespace>\n"
"      <namespace key=\"11\">Template talk</namespace>\n"
"      <namespace key=\"12\">Help</namespace>\n"
"      <namespace key=\"13\">Help talk</namespace>\n"
"      <namespace key=\"14\">Category</namespace>\n"
"      <namespace key=\"15\">Category talk</namespace>\n"
"      <namespace key=\"100\">Portal</namespace>\n"
"      <namespace key=\"101\">Portal talk</namespace>\n"
"    </namespaces>\n"
"  </siteinfo>\n";

}

void SplitArticles::handleArticle(const Article& article)
{
    // Start a new page whenever it is needed
    if ((++m_readArticles % m_splitAtNumberOfArticles) == 0)
        startNextSet();

    // Write the article out with some fake info
    QTextStream stream(&m_file);
    stream << QString::fromLatin1("   <page>\n"
"    <title>%1</title>\n"
"    <id>%2</id>\n"
"    <revision>\n"
"      <id>133452270</id>\n"
"      <timestamp>2007-05-25T17:12:06Z</timestamp>\n"
"      <contributor>\n"
"        <username>Fakse</username>\n"
"        <id>241822</id>\n"
"      </contributor>\n"
"      <minor />\n"
"      <comment>Fake comment</comment>\n"
"      <text xml:space=\"preserve\">%3</text>\n"
"    </revision>\n"
"  </page>\n").arg(escape(article.title().title())).arg(m_readArticles).arg(escape(article.textContent()));

}

