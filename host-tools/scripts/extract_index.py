#!/usr/bin/env python
"""
 Query the index database and combine the offset table and the
 index table to produce the final index.

 Copyright (C) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import optparse
import sys
import sqlite3


def parse():
    parser = optparse.OptionParser(version = "Generate index source",
                                   usage = """%prog [options] database""")
    parser.add_option("-o", "--output", help = "The index source output file",
                      action = "store", dest = "output", default = "indexfile.index")

    return parser.parse_args(sys.argv)

(opts, args) = parse()

print args[1]
connection = sqlite3.connect(args[1])
output = open(opts.output, "w")
cursor = connection.execute("SELECT IndexTable.title, Offsets.file, Offsets.offset FROM Offsets, IndexTable WHERE Offsets.hash = IndexTable.hash")

for row in cursor:
    title = row[0].encode("utf-8")
    print >> output, "%s%d%.10d" % (title, (row[1] % 10), row[2])
