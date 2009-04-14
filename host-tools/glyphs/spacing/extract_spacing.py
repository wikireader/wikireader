#!/usr/bin/env python
"""
 Generate a font file to be used on the target device

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

def parse():
    parser = optparse.OptionParser(version = "Extract spacing from a rendertext file",
                                   usage = """%prog [options] input_file
Two modes are supported. Single conversion and batch mode""")
    parser.add_option("-b", "--batch", help = "start a batch job",
                      action = "store_true", dest = "batch", default = False)
    parser.add_option("-e", "--error-file", help = "File where to put errors",
                      action = "store", dest = "error_file", default = "failed_spacing.files")


    return parser.parse_args(sys.argv)

# Load glyphs... share this with render_text.py
def load(file):
    glyphs = []
    for line in file:
        split = line.strip().split(',')


        glyph = { 'x'     : int(split[0]),
            'y'     : int(split[1]),
            'font'  : split[2],
            'glyph' : split[3] }
        glyphs.append(glyph)

    return glyphs

def extract_spacing(kern_info, last_glyph, glyph):
    """Extract the spacing between two glyphs and save that"""
    if not last_glyph:
        return

    if not last_glyph['x'] < glyph['x']:
        print "Something went wrong, overlapping glyphs...", last_glyph, glyph
        return

    kern_x = glyph['x'] - last_glyph['x']
    kern_y = glyph['y'] - last_glyph['y']
    kern = (kern_x, kern_y)
    glyph_pair = (last_glyph['glyph'], glyph['glyph'])
    glyph_font = glyph['font']
    if not glyph['font'] in kern_info:
        kern_info[glyph_font] = {}
        kern_info[glyph_font][glyph_pair] = kern
    elif not glyph_pair in kern_info[glyph_font]:
        kern_info[glyph_font][glyph_pair] = kern
    elif kern != kern_info[glyph_font][glyph_pair]:
        print "Not matching spacing will need to fixup: new: %s old: %s" % (kern, kern_info[glyph_font][glyph_pair]), last_glyph, glyph
        assert False

def generate_text_runs(kern_info, glyphs):
    current = None
    last_glyph = None

    for glyph in glyphs:
        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
            last_glyph = None
            continue

        extract_spacing(kern_info, last_glyph, glyph)
        last_glyph = glyph



def write_mappings(kern_info):
    """Write out the mappings used for this article"""

    # Write options
    mkdir("fonts")

    for font in kern_info.keys():
        font_index = "%s" % font
        font_path = os.path.join("fonts", font_index)
        mkdir(font_path)
        glyph_path_base = os.path.join(font_path, "spacing")
        mkdir(glyph_path_base)
        spacing_file = gzip.open(os.path.join(glyph_path_base, "spacing-file.gz"), "a")

        for (l_glyph, r_glyph) in kern_info[font].keys():
            (x,y) = kern_info[font][(l_glyph, r_glyph)]
            glyph_text =  "%s-%s: %d,%d" % (l_glyph, r_glyph, x, y)
            print >> spacing_file, glyph_text


# Import Psyco if available
try:
    import psyco
    psyco.full()
except ImportError:
    pass


opts, args = parse()

if not opts.batch:
    raw_glyphs = load(file(args[1]))

    kern = {}
    generate_text_runs(kern, raw_glyphs)
    write_mappings(kern)
else:
    failed = open(opts.error_file, "a")

    def convert(base_name, file_name):
        """
        Convert a single blib.gz
        """
        file_name = os.path.join(base_name, "articles", file_name[0], file_name[1:3], file_name)
        file_name = "%s.blib.gz" % file_name
        raw_glyphs = load(gzip.open(file_name, 'rb'))

        kern = {}
        generate_text_runs(kern, raw_glyphs)
        write_mappings(kern)

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

