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
last_glyph_index = 0
last_font_index = 0
package_one_byte = 0
package_two_byte = 0
package_three_byte = 0
glyph_map = {}
font_map = {}
kern_info = {}
huffman_fonts = {}
huffman_glyphs = {}
bit_occurences = {}

font_occurences = {}
glyph_occurences = {}

class BitWriter:
    def __init__(self):
        self.bits = []

    def write_3bits(self, bit):
        assert bit == (bit&0x7)
        self.write_bit( (bit&0x4)>>2 )
        self.write_bit( (bit&0x2)>>1 )
        self.write_bit( (bit&0x1)>>0 )

    def write_4bits(self, bit):
        assert bit == (bit&0xf)
        self.write_bit( (bit&0x8)>>3 )
        self.write_bit( (bit&0x4)>>2 )
        self.write_bit( (bit&0x2)>>1 )
        self.write_bit( (bit&0x1)>>0 )

    def write_6bits(self, bit):
        assert bit == (bit&0x3F)
        self.write_bit( (bit&0x20)>>5)
        self.write_bit( (bit&0x10)>>4)
        self.write_bit( (bit&0x8)>>3 )
        self.write_bit( (bit&0x4)>>2 )
        self.write_bit( (bit&0x2)>>1 )
        self.write_bit( (bit&0x1)>>0 )

    def write_8bits(self, bit):
        assert bit == (bit&0xff)
        self.write_4bits( (bit&0xf0)>>4)
        self.write_4bits( (bit&0x0f)>>0)

    def write_12bits(self, bit):
        assert bit == (bit&0xfff)
        self.write_4bits( (bit&0xf00)>>8)
        self.write_8bits( (bit&0x0ff))

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

        while len(self.bits) >= 8:
            operate = self.bits[0:8]
            self.bits = self.bits[8:]
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

def map_glyph_to_glyph_index(glyph):
    key = "%s" % glyph
    if not key in glyph_map:
        global last_glyph_index
        glyph_map[key] = last_glyph_index
        last_glyph_index = last_glyph_index + 1

    return glyph_map[key]

def map_font_to_index(font):
    key = "%s" % (font)
    if not key in font_map:
        global last_font_index
        font_map[key] = last_font_index
        last_font_index = last_font_index + 1

    return font_map[key]


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

    # Two passes....
    for glyph in glyphs:
        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
            text_runs.append(current)
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
        else:
            print "Omitting glyph due being out of space", glyph

        extract_spacing(last_glyph, glyph)
        last_glyph = glyph

    if current:
        text_runs.append(current)
        current = None

    return (text_runs, glyph_occurences, font_occurences)

def prepare_run(text_runs, glyph_occurences, font_occurences):
    # Sort by y position
    text_runs.sort(TextRun.cmp)

    global glyph_map, font_map
    glyph_map = determine_by_occurence(glyph_occurences)
    font_map = determine_by_occurence(font_occurences)

    # huffman foo...
    global huffman_fonts, huffman_glyphs
    font_input = []
    for font in font_occurences:
        font_input.append((font_occurences[font]/len(font_occurences), font))
    huffman_fonts = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(font_input))

    glyph_input = []
    for glyph in glyph_occurences:
        glyph_input.append((glyph_occurences[glyph]/len(glyph_occurences), glyph))
    huffman_glyphs = huffmanCode.createCodeWordMap(huffmanCode.makeHuffTree(glyph_input))

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

    Font/Paragraph... data encoding
    0   - 4 bit
    10  - 8 Bit
    110 - 12 Bit

    """

    
    def write_number(writer, number):
        """Find the best way to describe number and write it"""
        bits = highest_bit(number)

        if not bits in bit_occurences:
            bit_occurences[bits] = 0
        bit_occurences[bits] = bit_occurences[bits] + 1

        if bits <= 4:
            writer.write_bit(0)
            writer.write_4bits(number) 
        elif bits <= 8:
            writer.write_bit(1)
            writer.write_bit(0)
            writer.write_8bits(number) 
        elif bits <= 12:
            writer.write_bit(1)
            writer.write_bit(1)
            writer.write_12bits(number) 
        else:
            print "Can not encode...", number, bits
            assert False

    def write_pending_bit(writer, run, old_x, old_y):
        """All glyphs are on the same height..."""
        first_x = run.x - old_x
        first_y = run.y - old_y

        if first_x < 0:
            first_x = (240 - old_x) + run.x
        assert first_x >= 0

        writer.write_bit(0)
        if first_y == 0:
            writer.write_bit(0)
            write_number(writer, first_x)
        else:
            writer.write_bit(1)
            write_number(writer, first_x)
            write_number(writer, first_y)
 
        write_number(writer, len(run.glyphs))
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
    last_x = 0
    last_y = 0
    writer = BitWriter()
    for text_run in text_runs:
        # we mig have a new font now
        font = map_font_to_index(text_run.font)
        if last_font != font:
            writer.write_bit(1)
            writer.write_bits(huffman_fonts[text_run.font])
            last_font = font

        if text_run.x > 240:
            print "Skipping due too large x position"
            continue

        write_pending_bit(writer, text_run, last_x, last_y)
        last_y = text_run.y
        last_x = text_run.glyphs[-1]['x']
        assert last_x <= 240



    auto_kern_bit = open("auto_kern_encoding_bit", "w")
    bytes = writer.finish()
    auto_kern_bit.write("".join(bytes))

def write_mappings():
    """Write out the mappings used for this article"""

    # Write options
    mkdir("font-foo")

    for font in font_map.keys():
        font_index = "%s" % font_map[font]
        font_path = os.path.join("font-foo", font_index)
        mkdir(font_path)
        sp = file(os.path.join(font_path, "huffman"), "w")
        sp.write("%s" % huffman_fonts[font])

        for glyph in glyph_map.keys():
            try:
                bitmap = file(os.path.join("fonts", font, glyph, "bitmap.png"), "r")
            except:
                continue

            glyph_path = os.path.join(font_path, "glyphs")
            mkdir(glyph_path)
            glyph_path = os.path.join(glyph_path, "%s" % glyph_map[glyph])
            mkdir(glyph_path)

            sp = file(os.path.join(glyph_path, "original"), "w")
            sp.write("%s" % glyph)
            sp = file(os.path.join(glyph_path, "bitmap.png"), "w")
            sp.write(bitmap.read())
            sp = file(os.path.join(glyph_path, "huffman"), "w")
            sp.write("%s" % huffman_glyphs[glyph])

    for font in kern_info.keys():
        font_index = "%s" % map_font_to_index(font)
        font_path = os.path.join("font-foo", font_index)
        mkdir(font_path)
        for (l_glyph, r_glyph) in kern_info[font].keys():
            glyph_path = os.path.join(font_path, "spacing")
            mkdir(glyph_path)

            mapped_l_glyph = map_glyph_to_glyph_index(l_glyph)
            mapped_r_glyph = map_glyph_to_glyph_index(r_glyph)

            glyph_path = os.path.join(glyph_path, "%s-%s" % (mapped_l_glyph, mapped_r_glyph))
            mkdir(glyph_path)

            # Copy some things
            sp = file(os.path.join(glyph_path, "spacing"), "w")
            (x,y) = kern_info[font][(l_glyph, r_glyph)]
            sp.write("%d,%d" % (x,y))
            sp = file(os.path.join(glyph_path, "glyph"), "w")
    print kern_info
    print bit_occurences
            
        


raw_glyphs = load()
(text_runs, glyph_occurences, font_occurences) = generate_text_runs(raw_glyphs)
text_runs = prepare_run(text_runs, glyph_occurences, font_occurences)
write_to_file(text_runs)
write_mappings()
print "Last glyph", last_glyph_index
print "Last font", last_font_index
