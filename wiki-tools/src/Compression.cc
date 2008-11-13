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

#include "Compression.h"

static QByteArray compress_zlib(const QByteArray& data, int level)
{
    return QByteArray();
}

static QByteArray compress_bzip2(const QByteArray& data, int level)
{
    return QByteArray();
}

static QByteArray compress_lzma(const QByteArray& data, int level)
{
    return QByteArray();
}

QByteArray Compression::compress(const QByteArray& data, enum CompressionMethod& method, int level)
{
    switch(method) {
    case Method_Zlib:
        return compress_zlib(data, level);
    case Method_Bzip2:
        return compress_bzip2(data, level);
    case Method_Lzma:
        return compress_lzma(data, level);
    }

    // never reached
    Q_ASSERT(false);
    abort();
}
