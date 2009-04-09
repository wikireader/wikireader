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

import bitwriter
import sys
import huffmanCode
import fontmap
import textrun
import codetree
import optparse
import os
import gzip

# Globals
glyph_map = {}
font_map = {}
huffman_fonts = {}
huffman_glyphs = {}
huffman_x = {}
huffman_y = {}
huffman_length = {}


def determine_by_occurence(occurences):
    """Count the occurcences of something"""
    items = []
    for item in occurences.keys():
        items.append((occurences[item], item))
    items.sort(reverse=True)

    i = 0
    dict = {}
    for (count, item) in items:
        dict[item] = i
        i = i + 1

    return dict

def prepare_run(text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences):
    """Count the occurences of fonts, glyphs and position"""

    # Sort by y position
    text_runs.sort(textrun.TextRun.cmp)

    global glyph_map, font_map
    glyph_map = determine_by_occurence(glyph_occurences)
    font_map = determine_by_occurence(font_occurences)

    # huffman foo...
    global huffman_fonts, huffman_glyphs, huffman_x, huffman_y, huffman_length
    font_input = []
    for font in font_occurences:
        font_input.append((font_occurences[font]/len(font_occurences), font))
    huffman_fonts = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(font_input))

    glyph_input = []
    for glyph in glyph_occurences:
        glyph_input.append((glyph_occurences[glyph]/len(glyph_occurences), glyph))
    huffman_glyphs = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(glyph_input))

    x_input = []
    for x in x_occurences:
        x_input.append((x_occurences[x]/len(x_occurences), x))
    huffman_x = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(x_input))

    y_input = []
    for y in y_occurences:
        y_input.append((y_occurences[y]/len(y_occurences), y))
    huffman_y = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(y_input))

    length_input = []
    for length in length_occurences:
        length_input.append((length_occurences[length]/len(length_occurences), length))
    huffman_length = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(length_input))

    return text_runs

def write_to_file(text_runs, fonts, auto_kern_bit):
    """
    A function saving the text runs and hoping autokern will do its job


    # The bitcode.....
    0    - Paragraph
    1    - Font Change

    Parapgraph:
        [0,1] - 0 no y change, 1 x and y change
        number[number] 

    Font:
        Huffman code of the font
    """

    def write_header(file):
        """
        Write out the header, the header is the code used
        for this one article
        """
        def write_foo(file, dict):
            tree = codetree.CodeTree()
            for word in dict.keys():
                tree.addCodeWord(dict[word], word)
            tree.writeTo(file)
        write_foo(file, huffman_x)
        write_foo(file, huffman_y)
        write_foo(file, huffman_length)
        write_foo(file, huffman_glyphs)

        tree = codetree.CodeTree()
        for word in huffman_fonts.keys():
            tree.addCodeWord(huffman_fonts[word], fonts[word])
        tree.writeTo(file)
    
    def write_pending_bit(writer, run):
        """
        The text run is sorted by paragrah and all glyphs of
        one paragraph are on the same line and have roughly the
        same height.
        """

        writer.write_bit(0)
        writer.write_bits(huffman_x[run.first_x])
        writer.write_bits(huffman_y[run.first_y])
 
        writer.write_bits(huffman_length[len(run.glyphs)])

        list = []
        last_glyph = None
        for glyph in run.glyphs:
            huffman_glyph = huffman_glyphs[glyph['glyph']]
            writer.write_bits(huffman_glyph)
            last_glyph = glyph

    # Code
    write_header(auto_kern_bit)

    last_font = None
    writer = bitwriter.BitWriter()
    for text_run in text_runs:
        # we migt have a new font now
        font = text_run.font
        if last_font != font:
            writer.write_bit(1)
            writer.write_bits(huffman_fonts[text_run.font])
            last_font = font

        write_pending_bit(writer, text_run)



    bytes = writer.finish()
    auto_kern_bit.write("".join(bytes))


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
    (text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences) = textrun.generate_text_runs(glyphs, 240)
    prepare_run(text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences)
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
        file_name = "%s.blib.gz" % file_name
        glyphs = textrun.load(gzip.open(file_name, 'rb'))
        (text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences) = textrun.generate_text_runs(glyphs, 240)
        prepare_run(text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences)

        # write the offset to another file...
        print >> offset_marker, "%s %d" % (file_name, batch_output.tell())
        write_to_file(text_runs, fonts, batch_output)

    for arg in range(1, len(args)):
        for work in os.path.join(arg, "*.work"):
            file = open(work)
            for line in file:
                data = line[:-1].split(" ", 1)
                try:
                    convert(arg, data[0])
                except:
                    print >> failed, "Error: %s from %s" % (data[0], work)

print "Done. Have fun!"
