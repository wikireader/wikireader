#!/usr/bin/env python
"""
 A code utilizing huffman to convert a .blib file to something that
 compresses nicely and can be uniquely decoded. This version is without
 some optimisations like bigrams.

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

import sys
import fontmap
import textrun
import optparse
import os
import glob

try:
    import lzo
    imported_lzo = True
except:
    print "WARNING: LZO not imported performance will be degraded"
    imported_lzo = False

def prepare_run(text_runs):
    """Count the occurences of fonts, glyphs and position"""

    # Sort by y position
    text_runs.sort(textrun.TextRun.cmp)
    return text_runs

def write_to_file(text_runs, fonts, auto_kern_bit):
    """
    A function saving the text runs and hoping autokern will do its job

    """

    def write_pending_bit(output, run):
        """
        The text run is sorted by paragrah and all glyphs of
        one paragraph are on the same line and have roughly the
        same height.
        """

        if run.first_y == 0:
            output.append(",%d," % (run.first_x))
        else:
            output.append(",%d_%d," % (run.first_x, run.first_y))
 
        list = []
        for glyph in run.glyphs:
            list.append(glyph['glyph'])
        output.append("-".join(list))

    last_font = None
    output = []
    for text_run in text_runs:
        # we migt have a new font now
        font = text_run.font
        if last_font != font:
            output.append("f%s" % fonts[font])
            last_font = font

        write_pending_bit(output, text_run)

    text = "".join(output)
    if imported_lzo:
        auto_kern_bit.write(lzo.compress(text, 9))
    else:
        auto_kern_bit.write(text)


def parse():
    parser = optparse.OptionParser(version = "Generate Huffman code utility 0.1",
                                   usage = """%prog [options] input_file
Two modes are supported. Single conversion or batch conversion""")

    parser.add_option("-f", "--fontmap", help = "specify the fontmap.map to use",
                      action = "store", dest = "fontmap", default = "fontmap.map")
    parser.add_option("-o", "--output", help = "Output file",
                      action = "store", dest = "output_file", default = "huffmaned.cde")
    parser.add_option("-b", "--batch", help = "start a batch job",
                      action = "store_true", dest = "batch", default = False)
    parser.add_option("-a", "--batch-output", help = "Output file for the batch",
                      action = "store", dest = "output_batch_file", default = "wikipedia.set")
    parser.add_option("-c", "--batch-offset", help = "File with offsets of articles",
                      action = "store", dest = "output_marker", default = "wikipedia.offset")
    parser.add_option("-e", "--error-file", help = "File where to put errors",
                      action = "store", dest = "error_file", default = "failed_blib.files")

    return parser.parse_args(sys.argv)


options, args = parse()
 
# Import Psyco if available
try:
    import psyco
    psyco.full()
except ImportError:
    pass


if not options.batch:
    glyphs = textrun.load(open(args[1]))
    text_runs = textrun.generate_text_runs(glyphs, 240)
    prepare_run(text_runs)
    fonts  = fontmap.load(options.fontmap)
    auto_kern_bit = open(options.output_file, "w")
    write_to_file(text_runs, fonts, auto_kern_bit)
else:
    # We got pointed to a list of directories and will collect the
    # the 'work' files from there and will pick up the objects and then
    # do some work on it.
    offset_marker = open(options.output_marker, "w")
    batch_output = open(options.output_batch_file, "w") 
    fonts  = fontmap.load(options.fontmap)
    failed = open(options.error_file, "a")

    def convert(base_name, file_name):
        """
        Convert a single file
        """
        file_name = os.path.join(base_name, "articles", file_name[0], file_name[1:3], file_name)
        file_name = "%s.blib" % file_name
        glyphs = textrun.load(open(file_name, 'rb'))
        text_runs = textrun.generate_text_runs(glyphs, 240)
        prepare_run(text_runs)

        # write the offset to another file...
        print >> offset_marker, "%s %d" % (file_name, batch_output.tell())
        write_to_file(text_runs, fonts, batch_output)

    for arg in range(1, len(args)):
        for work in glob.glob(os.path.join(args[arg], "*.work")):
            print "Working on %s" % work
            file = open(work)
            for line in file:
                data = line[:-1].split(" ", 1)
                try:
                    convert(args[arg], data[0])
                except:
                    print >> failed, "Error: %s from %s" % (data[0], work)

print "Done. Have fun!"
