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

#
# Experiment with the encoding of the data
last_glyph_index = 0
package_one_byte = 0
package_two_byte = 0
package_three_byte = 0
glyph_map = {}


# Load glyphs... share this with render_text.py
def load():
    glyphs = []
    for line in open("render_text.blib"):
        split = line.strip().split(',')  

        # Throw away glyphs we will not render (normally zero spaced)
        if split[3] == '3':
            continue

        # Throw out invisible text
        if int(split[0]) > 240:
            continue

        glyph = { 'x'     : int(split[0]),
            'y'     : int(split[1]),
            'font'  : split[2],
            'glyph' : split[3] }
        glyphs.append(glyph)

    return glyphs

def sort(glyphs):
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
        new_glyph = { 'x' : glyph['x'] - last_x,
                      'y' : glyph['y'] - last_y,
                      'font' : glyph['font'],
                      'glyph': glyph['glyph'] }

        # Handle the line wrap...
        # The maximum delta can be 239.
        if new_glyph['x'] < 0:
            x = 240 - last_x
            new_glyph['x'] = x+glyph['x']

        last_x = glyph['x']
        last_y = glyph['y']

        assert last_x > 0
        assert last_y > 0
        new_glyphs.append(new_glyph)
    return new_glyphs

def map_font_description_to_glyph_index(glyph):
    key = "Glyph:%s-Font:%s" % (glyph['glyph'], glyph['font'])
    if not key in glyph_map:
        global last_glyph_index
        glyph_map[key] = last_glyph_index
        last_glyph_index = last_glyph_index + 1

    return glyph_map[key]

def rle_encode(glyphs):
    import math

    def rle(x):
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

        return i

    def zero_ones(x):
        """
        Count zero's and ones in the text...
        """
        if x == 0:
            return {'0' : 1, '1': 0}

        zeros = 0
        ones = 0
        for i in range(0, int(math.ceil(math.log(x,2)))+1) :
            mask = 1 << i
            if (x & mask)>>i == 1:
                ones = ones + 1
            else:
                zeros = zeros + 1

        return {'0' : zeros, '1': ones}
                


    def pack_glyph(x, y, glyph_index):
        """
        Pack the glyph together

        0.) Byte sized packet...
             0      7
             0XXXYGGG
        1.) Two Byte packet
             0      78      15
             10XXXXXYYYGGGGGG
             X == X delta
             Y == Y delta
             G == glyph index
        2.) Three Byte packet... to catch everything else...
             0       ,       ,       ,
             11XXXXXXXYYYYYYYGGGGGGGG

        Alternatively....
        a)
            Bit 0 defines if the package is at an end...
            Bit 12 00 => x, 01 => y, 10 => index
            Bit ...
        """
        x_bits = rle(x)
        y_bits = rle(y)
        index_bits = rle(glyph_index)
        number_bits = x_bits + y_bits + index_bits
        if x_bits <= 3 and y_bits <= 1 and index_bits <= 3:
            global package_one_byte
            package_one_byte = package_one_byte + 1
            pass
        elif x_bits <= 5 and y_bits <= 3 and index_bits <= 6:
            global package_two_byte
            package_two_byte = package_two_byte + 1
            pass
        elif x_bits <= 7 and y_bits <= 7 and index_bits <= 8:
            global package_three_byte
            package_three_byte = package_three_byte + 1
            pass
        else:
            print "Fail:", number_bits, " components", "x: (", x, x_bits, zero_ones(x), ") y: (", y, y_bits, zero_ones(y), ") index: (", glyph_index, index_bits, zero_ones(glyph_index), ")"
            assert False
        return ""
        


    import copy
    glyphs = copy.deepcopy(glyphs)

    delta_compressed = open("delta_compressed", "w")
    delta_compressed_glyph_index = open("delta_compressed_glyph_index", "w")

    prev = None
    smallest_x = 0
    largest_x = 0
    for glyph in glyphs:
        glyph_index = map_font_description_to_glyph_index(glyph)

        # Gather some information
        if glyph['x'] < smallest_x:
            smallest_x = glyph['x']
        elif glyph['x'] > largest_x:
            largest_x = glyph['x']
        prev = glyph

        delta_compressed.write("%(x)d,%(y)d,%(font)s,%(glyph)s" % glyph)
        delta_compressed_glyph_index.write(pack_glyph(glyph['x'], glyph['y'], glyph_index))
        #delta_compressed_glyph_index.write(bit_compress(glyph['x'], glyph['y'], glyph_index))
        #delta_compressed_glyph_index.write(struct.pack("hH", glyph['x']*(glyph['y']+1), glyph_index))
        #delta_compressed_glyph_index.write("%d%d" % (glyph['x']*(glyph['y']*1), glyph_index))

    print largest_x, smallest_x


glyphs = sort(load())
delta = delta_compress(glyphs)
rle_encode(delta)
print last_glyph_index
