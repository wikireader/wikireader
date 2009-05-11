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

#ifndef Compression_h
#define Compression_h

#include <QByteArray>
#include <stdlib.h>

class Compression {
public:
    enum CompressionMethod {
        Method_Zlib,
        Method_Bzip2,
        Method_Lzma
    };

    static QByteArray compress(const QByteArray&, enum CompressionMethod&, int level);
};

#endif
