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

font_occurences = {}
glyph_occurences = {}

class BitWriter:
    def __init__(self):
        self.bits = []

    def write_4bits(self, bit):
        assert bit == (bit&0xf)
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

    def consume(self):
        "Consume bits to a packed 8byte"
        import struct

        result = ""
        while len(self.bits) >= 8:
            operate = self.bits[0:8]
            self.bits = self.bits[8:]

            data = (operate[0]<<7) | (operate[1]<<6) | (operate[2]<<5) | (operate[3]<<4) | (operate[4]<<3) | (operate[5]<<2) | (operate[6]<<1) | (operate[7]<<0)
            result = "%s%s" % (result,struct.pack("<B", data))
        return result

    def finish(self):
        "Consume everything that is still left"
        remainder = len(self.bits)%8
        for i in range(0,8-remainder):
            self.write_bit(0)
        assert len(self.bits)%8 == 0
        return self.consume()


bit_writer = BitWriter()


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

def sort_glyphs(glyphs):
    # Sort glyphs so that glyphs are on the same line are next to each other
    def glyph_cmp(left, right):
        if left['y'] < right['y']:
            return -1
        elif left['y'] > right['y']:
            return 1

        if left['x'] < right['x']:
            return -1
        elif left['x'] > right['x']:
            return 1
        return 0

    glyphs.sort(glyph_cmp)
    return glyphs

def delta_compress(glyphs):
    """
    Save the delta between the last and the current glyph. On a line
    wrap the delat will be insanely huge and negative... we can detect
    that later and enter a return -XYZ => - Positive option...
    """
    new_glyphs = []
    last_x = 0
    last_y = 0

    for glyph in glyphs:
        # Throw away glyphs we will not render (normally zero spaced)
        if glyph['glyph'] == '3':
            continue

        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
            continue

        # Throw out invisible text
        if int(glyph['x']) > 240:
            continue

        new_glyph = { 'x' : glyph['x'] - last_x,
                      'y' : glyph['y'] - last_y,
                      'font' : glyph['font'],
                      'glyph': glyph['glyph'],
                      'original_x' : glyph['x'],
                      'original_y' : glyph['y'] }

        # Handle the line wrap...
        # The maximum delta can be 239.
        if new_glyph['x'] < 0:
            x = 240 - last_x
            new_glyph['x'] = x+glyph['x']

        # Sanity
        assert glyph['x'] >= 0
        assert new_glyph['x'] >= 0

        last_x = glyph['x']
        last_y = glyph['y']
        assert last_x >= 0
        assert last_y > 0

        new_glyphs.append(new_glyph)
    return new_glyphs

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

def rle_encode(glyphs):
    import math

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

    def pack_glyph(x, y, glyph_index, old_font, new_font):
        """
        Pack the glyph together

        Simple bitcode...
        00 = X,Glyph Position 4 byte
            Legal combination
            X-Glyph-X
            X-Y-Glyph-X
            X-X would not be legal.. so combining these
            is legal

        01 = X,Glyph Position Position 8 byte
            Legal combinations as above
        10 = Y Position 4 Byte
        11 = Extended package...
        111 - 12-Byte Y,Glyph
        110 - Extended...
        1101

        """
        x_bits = highest_bit(x)
        if x_bits <= 4:
            bit_writer.write_bit(0)
            bit_writer.write_bit(0)
            bit_writer.write_4bits(x)
        elif x_bits <= 8:
            bit_writer.write_bit(0)
            bit_writer.write_bit(1)
            bit_writer.write_8bits(x)
        else:
            print x, x_bits, glyph_index
            assert False

        y_bits = highest_bit(y)
        # If we have no delta... omit the data
        if y == 0:
            pass
        elif y_bits <= 4:
            bit_writer.write_bit(1)
            bit_writer.write_bit(0)
            bit_writer.write_4bits(y)
        elif y_bits <= 12:
            bit_writer.write_bit(1)
            bit_writer.write_bit(1)
            bit_writer.write_bit(1)
            bit_writer.write_12bits(y)
        else:
            print y, y_bits, glyph_index
            assert False


        index_bits = highest_bit(glyph_index)
        if index_bits <= 4:
            bit_writer.write_bit(0)
            bit_writer.write_bit(0)
            bit_writer.write_4bits(glyph_index)
        elif index_bits <= 8:
            bit_writer.write_bit(0)
            bit_writer.write_bit(1)
            bit_writer.write_8bits(glyph_index)
        elif index_bits <= 12:
            bit_writer.write_bit(1)
            bit_writer.write_bit(1)
            bit_writer.write_bit(1)
            bit_writer.write_12bits(glyph_index)
        else:
            print x, x_bits, index_bits, glyph_index
            assert False

        if last_font != new_font:
            bit_writer.write_bit(1)
            bit_writer.write_bit(1)
            bit_writer.write_bit(0)
            bit_writer.write_bit(1)

            bit_writer.write_4bits(map_font_to_index(new_font))


        return bit_writer.consume()



    import copy
    glyphs = copy.deepcopy(glyphs)

    delta_compressed = open("delta_compressed", "w")
    delta_compressed_glyph_index = open("delta_compressed_glyph_index", "w")

    prev = None
    smallest_x = 0
    largest_x = 0
    last_font = None
    for glyph in glyphs:
        glyph['glyph_index'] = map_glyph_to_glyph_index(glyph['glyph'])

        # Gather some information
        if glyph['x'] < smallest_x:
            smallest_x = glyph['x']
        elif glyph['x'] > largest_x:
            largest_x = glyph['x']
        prev = glyph

        if last_font != glyph['font']:
            delta_compressed.write("+%d" % map_font_to_index(glyph['font']))

        if glyph['y'] == 0:
            delta_compressed.write("%(original_x)d;%(glyph_index)d:" % glyph)
        else:
            delta_compressed.write("%(original_x)d,%(original_y)d,%(glyph_index)d:" % glyph)
        delta_compressed_glyph_index.write(pack_glyph(glyph['x'], glyph['y'], glyph['glyph_index'], last_font, glyph['font']))
        last_font = glyph['font']

    # Write the pending bits
    delta_compressed_glyph_index.write(bit_writer.finish())
    print "Larges and smallest x delta", largest_x, smallest_x

def extract_spacing(last_glyph, glyph):
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
        #print "spacing between:", glyph_pair,  "  => %d" % (kern), last_glyph, glyph
    elif not glyph_pair in kern_info[glyph_font]:
        kern_info[glyph_font][glyph_pair] = kern
        #print "spacing between:", glyph_pair,  "  => %d" % (kern), last_glyph, glyph
    elif kern != kern_info[glyph_font][glyph_pair]:
        print "Not matching spacing will need to fixup: new: %d old: %d" % (kern, kern_info[glyph_font][glyph_pair]), last_glyph, glyph

def determine_space(last_glyph, glyph):
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

def use_auto_kern(glyphs):
    """A function saving the text runs and hoping autokern will do its job"""
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
            
            

    def write_pending(file, run, old_x, old_y):
        """All glyphs are on the same height..."""
        first_x = run.x - old_x
        first_y = run.y - old_y

        if first_x < 0:
            first_x = (240 - old_x) + run.x

        if first_y == 0:
            file.write("p%d;" % first_x)
        else:
            file.write("p%d:%d;" % (first_x, first_y))
 
        list = []
        last_glyph = None
        for glyph in run.glyphs:
            spacing = determine_space(last_glyph, glyph)
            if spacing:
                (x,y) = spacing
                list.append("%d-%d-%d" % (x,y, map_glyph_to_glyph_index(glyph['glyph'])))
            else:
                list.append("%d" % map_glyph_to_glyph_index(glyph['glyph']))
            last_glyph = glyph
        file.write(" ".join(list))
 

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

        current.add_glyph(glyph)

        extract_spacing(last_glyph, glyph)
        last_glyph = glyph

    if current:
        text_runs.append(current)
        current = None

    # Sort by y position
    text_runs.sort(TextRun.cmp)

    global glyph_map, font_map
    glyph_map = determine_by_occurence(glyph_occurences)
    font_map = determine_by_occurence(font_occurences)
        
    auto_kern = open("auto_kern_encoding", "w")
    last_font = None
    last_x = 0
    last_y = 0
    for text_run in text_runs:
        # we mig have a new font now
        font = map_font_to_index(text_run.font)
        if last_font != font:
            auto_kern.write("f%d," % font)
            last_font = font

        write_pending(auto_kern, text_run, last_x, last_y)
        last_y = text_run.y
        last_x = text_run.glyphs[-1]['x']

    # Write options
    mkdir("font-foo")

    for font in font_map.keys():
        font_index = "%s" % font_map[font]
        font_path = os.path.join("font-foo", font_index)
        mkdir(font_path)
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

    for font in kern_info.keys():
        font_index = "%s" % map_font_to_index(font)
        font_path = os.path.join("font-foo", font_index)
        mkdir(font_path)
        for (l_glyph, r_glyph) in kern_info[font].keys():
            glyph_path = os.path.join(font_path, "spacing")
            mkdir(glyph_path)

            glyph_path = os.path.join(glyph_path, "%s-%s" % (l_glyph, r_glyph))
            mkdir(glyph_path)

            # Copy some things
            sp = file(os.path.join(glyph_path, "spacing"), "w")
            (x,y) = kern_info[font][(l_glyph, r_glyph)]
            sp.write("%d,%d" % (x,y))
            sp = file(os.path.join(glyph_path, "glyph"), "w")
    print kern_info
            
        


raw_glyphs = load()
use_auto_kern(raw_glyphs)
glyphs = sort_glyphs(copy.deepcopy(raw_glyphs))
delta = delta_compress(glyphs)
rle_encode(delta)
print "Last glyph", last_glyph_index
print "Last font", last_font_index
