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

#include "ExtractTextCompressed.h"

#include <QTextStream>
#include <QDebug>

ExtractTextCompressed::ExtractTextCompressed(const QString& outputFile, Compression::CompressionMethod method, int level)
    : FileOutputArticleHandler(outputFile)
    , m_method(method)
    , m_level(level)
{}

void ExtractTextCompressed::handleArticle(const Article& text)
{
    if (text.isRedirect())
        return;

    const QByteArray input = text.textContent().toUtf8();
    const QByteArray output = Compression::compress(input, m_method, m_level);

    if (output.size() == 0 && input.size() != 0)
        qWarning() << "Failed to compress article" << text.title().title();

    m_file.write(output);
}
