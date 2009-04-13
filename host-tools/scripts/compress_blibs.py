#!/usr/bin/env python
"""
 Compress the blib files that were created or report missing ones

 Copyright (C) 2008, 2009 Holger Hans Peter Freyther <zecke@openmoko.org>

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

import gzip
import glob
import optparse
import os
import sys

def mkdir(path):
    try:
        os.mkdir(path)
    except:
        pass

def compress(base_name, file_name):
    """
    Compress a simple blib file... not using fsync... so it is not hip...
    """
    file_name = os.path.join(base_name, "articles", file_name[0], file_name[1:3], file_name)
    input_path = "%s.blib" % file_name
    output_path = "%s.blib.gz" % file_name

    input = open(input_path, "rb")
    output = gzip.open(output_path, 'wb')
    output.write(input.read())
    input.close()
    output.close()
    os.remove(input_path)

def parse():
    parser = optparse.OptionParser(version = "Compress the generated .blib files",
                                   usage = """%prog [options] input_file""")
    parser.add_option("-e", "--error-file", help = "File where to put errors",
                      action = "store", dest = "error_file", default = "failed_gzip.files")


    return parser.parse_args(sys.argv)

# Import Psyco if available
try:
    import psyco
    psyco.full()
except ImportError:
    pass


opts, args = parse()

failed = open(opts.error_file, "a")


for arg in range(1, len(args)):
    for work in glob.glob(os.path.join(args[arg], "*.work")):
        print "Working on %s" % work
        file = open(work)
        for line in file:
            data = line[:-1].split(" ", 1)
            try:
                compress(args[arg], data[0])
            except:
                print >> failed, "Error: %s from %s" % (data[0], work)

