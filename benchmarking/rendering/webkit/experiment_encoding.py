#!/usr/bin/env python

#
# Experiment with the encoding of the data


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
        last_x = glyph['x']
        last_y = glyph['y']
        new_glyphs.append(new_glyph)
    return new_glyphs



glyphs = sort(load())
delta = delta_compress(glyphs)
