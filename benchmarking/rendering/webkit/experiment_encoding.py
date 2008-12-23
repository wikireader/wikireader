#!/usr/bin/env python


#
# What is not working:
#    -delta compress glyphs... the diff is too random
#     the glyph_index gets pretty high with the more
#     font faces we have....reducing the number of faces
#     will reduce this number...
#

#
# Experiment with the encoding of the data
last_glyph_index = 0
glyph_map = {}


# Load glyphs... share this with render_text.py
def load():
    glyphs = []
    for line in open("render_text.blib"):
        split = line.strip().split(',')  

        # Throw away glyphs we will not render (normally zero spaced)
        if split[3] == '3':
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

        # Handle the line wrap... negative sign
        if new_glyph['x'] < 0:
            new_glyph['x'] = -glyph['x']

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
            return "0"

        number_of_bits = int(math.ceil(math.log(x, 2)))
        return number_of_bits * "y"

    def bit_compress(x, y, glyph_index):
        """
        Compress x,y,glyph_index into the smallest possible
        recoverable representation
        """
        assert glyph_index >= 0
        assert y >= 0

        if x < 0:
            str = "1"
        else:
            str = "0"
        #return "%s%s%s%s" % (str, rle(abs(x)), rle(y), rle(glyph_index))
        
        str = "%s%s%s" % (rle(abs(x)), rle(y), rle(glyph_index))
        print len(str)
        return str
        


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
        delta_compressed_glyph_index.write(bit_compress(glyph['x'], glyph['y'], glyph_index))

    print largest_x, smallest_x


glyphs = sort(load())
delta = delta_compress(glyphs)
rle_encode(delta)
print last_glyph_index
