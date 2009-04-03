#!/usr/bin/env python


#
# What is not working:
#    -delta compress glyphs... the diff is too random
#     the glyph_index gets pretty high with the more
#     font faces we have....reducing the number of faces
#     will reduce this number...
#
# What might work:
#    -omit the sign of the x-delta and let it wrap
#     around at the page width. So the maximum x
#     number would be the page width...
#
#

import copy
import os
import huffmanCode
import math

#
# Experiment with the encoding of the data
package_one_byte = 0
package_two_byte = 0
package_three_byte = 0
glyph_map = {}
font_map = {}
kern_info = {}
huffman_fonts = {}
huffman_glyphs = {}
huffman_x = {}
huffman_y = {}
huffman_length = {}
bit_occurences = {}
bigrams = {}

class BitWriter:
    def __init__(self):
        self.bits = []

    def write_bit(self, bit):
        assert bit == (bit&0x1)
        if bit:
            self.bits.append(1)
        else:
            self.bits.append(0)

    def write_bits(self, bit_string):
        for bit in bit_string:
            if bit == '1':
                self.write_bit(1)
            elif bit == '0':
                self.write_bit(0)
            else:
                print "Wrong", bit_string
                assert False

    def _consume(self):
        "Consume bits to a packed 8byte"
        import struct

        consumed = []

        for index in range(0, len(self.bits), 8):
            operate = self.bits[index:index+8]
            data = (operate[0]<<7) | (operate[1]<<6) | (operate[2]<<5) | (operate[3]<<4) | (operate[4]<<3) | (operate[5]<<2) | (operate[6]<<1) | (operate[7]<<0)
            byte = struct.pack("<B", data)
            consumed.append(byte)

        return consumed

    def finish(self):
        "Consume everything that is still left"
        remainder = len(self.bits)%8
        for i in range(0,8-remainder):
            self.write_bit(0)
        assert len(self.bits)%8 == 0
        return self._consume()

class TextRun:
    def __init__(self, glyph):
        self.x = glyph['x']
        self.y = glyph['y']
        self.font = glyph['font']
        self.glyphs = []

    def add_glyph(self, glyph):
        self.glyphs.append(glyph)

    def cmp(left, right):
        if left.y < right.y:
            return -1
        elif left.y > right.y:
            return 1

        if left.x < right.x:
            return -1
        elif left.x > right.x:
            return 1
        return 0


def mkdir(path):
    try:
        os.mkdir(path)
    except:
        pass

def determine_by_occurence(occurences):
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


# Load glyphs... share this with render_text.py
def load():
    glyphs = []
    for line in open("render_text.blib"):
        split = line.strip().split(',')


        glyph = { 'x'     : int(split[0]),
            'y'     : int(split[1]),
            'font'  : split[2],
            'glyph' : split[3] }
        glyphs.append(glyph)

    return glyphs

def highest_bit(x):
    """
    Runtime length encode the number
    """
    if x == 0:
        return 1

    # find the highest bit
    highest_bit = 0
    for i in range(0, int(math.ceil(math.log(x,2)))+1) :
        mask = 1 << i
        if (x & mask)>>i == 1:
            highest_bit = i

    return highest_bit+1

def extract_spacing(last_glyph, glyph):
    """Extract the spacing between two glyphs and save that"""
    # Look into the kerning...
    global kern_info
    if not last_glyph:
        return

    if not last_glyph['x'] < glyph['x']:
        print "Something went wrong", last_glyph, glyph
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

def determine_space(last_glyph, glyph):
    """Compare the spacing between the known spacing and this pair.
    If it is different the caller code needs to have an override for that..."""
    global kern_info
    if not last_glyph:
        return None


    kern_x = glyph['x'] - last_glyph['x']
    kern_y = glyph['y'] - last_glyph['y']
    kern = (kern_x, kern_y)
    glyph_pair = (last_glyph['glyph'], glyph['glyph'])
    glyph_font = glyph['font']

    try:
        space = kern_info[glyph_font][glyph_pair]
    except KeyError:
        return None
    if space == kern:
        return None
    return space

def generate_text_runs(glyphs):
    text_runs = []
    current = None
    last_glyph = None

    last_x = 0
    last_y = 0

    font_occurences = {}
    glyph_occurences = {}
    length_occurences = {}
    x_occurences = {}
    y_occurences = {}

    # Two passes....
    for glyph in glyphs:
        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
            if current.x > 240:
                print "Skipping due too large x position"
            else:
                current.first_x = current.x - last_x
                if current.first_x < 0:
                    current.first_x = (240 - last_x) + current.x
                assert current.first_x >= 0
                current.first_y = current.y - last_y
                text_runs.append(current)
                last_y = current.y
                last_x = current.glyphs[-1]['x']
                assert last_x <= 240

                # Occurence accounting
                if not len(current.glyphs) in length_occurences:
                    length_occurences[len(current.glyphs)] = 0
                length_occurences[len(current.glyphs)] = length_occurences[len(current.glyphs)] + 1
                if not current.first_x in x_occurences:
                    x_occurences[current.first_x] = 0
                x_occurences[current.first_x] = x_occurences[current.first_x] + 1
                if not current.first_y in y_occurences:
                    y_occurences[current.first_y] = 0
                y_occurences[current.first_y] = y_occurences[current.first_y] + 1

            current = None
            last_glyph = None
            continue

        if not current:
            # Count
            if not glyph['font'] in font_occurences:
                font_occurences[glyph['font']] = 0
            font_occurences[glyph['font']] = font_occurences[glyph['font']] + 1

            current = TextRun(glyph)

        # Count more
        if not glyph['glyph'] in glyph_occurences:
            glyph_occurences[glyph['glyph']] = 0
        glyph_occurences[glyph['glyph']] = glyph_occurences[glyph['glyph']] + 1

        if glyph['x'] < 240:
            current.add_glyph(glyph)
            # bigram...
            if last_glyph and last_glyph['glyph'] != '0':
                try:
                    global bigrams
                    pair = (last_glyph['glyph'], glyph['glyph'])
                    bigrams[pair] = bigrams[pair] + 1
                except:
                    bigrams[pair] = 1
        else:
            print "Omitting glyph due being out of space", glyph

        extract_spacing(last_glyph, glyph)
        last_glyph = glyph

    if current:
        text_runs.append(current)
        current = None

    return (text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences)

def prepare_run(text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences):
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
        

def write_to_file(text_runs):
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

    
    def write_pending_bit(writer, run):
        """All glyphs are on the same height..."""

        writer.write_bit(0)
        if run.first_y == 0:
            writer.write_bits(huffman_x[run.first_x])
        else:
            writer.write_bits(huffman_x[run.first_x])
            writer.write_bits(huffman_y[run.first_y])
 
        writer.write_bits(huffman_length[len(run.glyphs)])

        list = []
        last_glyph = None
        for glyph in run.glyphs:
            spacing = determine_space(last_glyph, glyph)
            if spacing:
                print "Something broken with spacing", last_glyph, glyph
                assert False
            huffman_glyph = huffman_glyphs[glyph['glyph']]
            writer.write_bits(huffman_glyph)
            last_glyph = glyph

    # Code
    last_font = None
    writer = BitWriter()
    for text_run in text_runs:
        # we migt have a new font now
        font = text_run.font
        if last_font != font:
            writer.write_bit(1)
            writer.write_bits(huffman_fonts[text_run.font])
            last_font = font

        write_pending_bit(writer, text_run)



    auto_kern_bit = open("auto_kern_encoding_bit", "w")
    bytes = writer.finish()
    auto_kern_bit.write("".join(bytes))

# Import Psyco if available
try:
    import psyco
    psyco.full()
except ImportError:
    pass

raw_glyphs = load()
(text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences) = generate_text_runs(raw_glyphs)
text_runs = prepare_run(text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences)
write_to_file(text_runs)
