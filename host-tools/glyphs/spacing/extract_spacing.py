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

import optparse
import os
import sys

#
# Experiment with the encoding of the data
kern_info = {}

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

def extract_spacing(last_glyph, glyph):
    """Extract the spacing between two glyphs and save that"""
    global kern_info
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
        print "Not matching spacing will need to fixup: new: %d old: %d" % (kern, kern_info[glyph_font][glyph_pair]), last_glyph, glyph

def generate_text_runs(glyphs):
    current = None
    last_glyph = None

    for glyph in glyphs:
        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
            last_glyph = None
            continue

        extract_spacing(last_glyph, glyph)
        last_glyph = glyph



def write_mappings():
    """Write out the mappings used for this article"""

    # Write options
    mkdir("fonts")

    for font in kern_info.keys():
        font_index = "%s" % font
        font_path = os.path.join("fonts", font_index)
        mkdir(font_path)
        glyph_path_base = os.path.join(font_path, "spacing")
        mkdir(glyph_path_base)
        spacing_file = open(os.path.join(glyph_path_base, "spacing-file"))

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
    generate_text_runs(raw_glyphs)
    write_mappings()
else:
    pass
