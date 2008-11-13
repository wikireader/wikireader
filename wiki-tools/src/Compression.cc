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

#include <zlib.h>
#include <bzlib.h>

static QByteArray compress_zlib(const QByteArray& data, int level)
{
    QByteArray compressedResult;
    compressedResult.resize(compressBound(data.size()));

    unsigned long long size = compressedResult.size();

    int result = compress2((Bytef*)compressedResult.data(), (uLongf*)&size,
                           (Bytef*)data.data(), data.size(), level);

    if (result != Z_OK)
        return QByteArray();
    
    compressedResult.resize(size);
    return compressedResult;
}

static QByteArray compress_bzip2(const QByteArray& data, int level)
{
    unsigned int size = 0;

    // We need to use %1 + 600 bytes more
    if (data.size() < 100)
        size = data.size() * 2 + 600;
    else
        size = data.size() + 601 + (data.size()/100);

    QByteArray compressedResult;
    compressedResult.resize(size);

    int result = BZ2_bzBuffToBuffCompress(compressedResult.data(), &size,
                                          (char*)data.data(), data.size(),
                                          level, 0, 0); 
    if (result != BZ_OK)
        return QByteArray();
    
    compressedResult.resize(size);
    return compressedResult;
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
