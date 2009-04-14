#!/usr/bin/env python


class TextRun:
    """
    A run of text...
    """
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


def generate_text_runs(glyphs, WIDTH):
    text_runs = []
    current = None
    last_glyph = None

    last_x = 0
    last_y = 0
    skip_paragraph = False

    # Two passes....
    for glyph in glyphs:
        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
            if skip_paragraph:
                print "Skipping paragraph due text being drawn over itself."
            if current.x > WIDTH:
                print "Skipping due too large x position"
            else:
                current.first_x = current.x - last_x
                if current.first_x < 0:
                    current.first_x = (WIDTH - last_x) + current.x
                assert current.first_x >= 0
                current.first_y = current.y - last_y
                text_runs.append(current)
                last_y = current.y
                if len(current.glyphs) != 0:
                    last_x = current.glyphs[-1]['x']
                    assert last_x <= WIDTH

            current = None
            last_glyph = None
            skip_paragraph = False
            continue
        elif skip_paragraph:
            continue
        elif last_glyph and last_glyph['x'] == glyph['x']:
            skip_paragraph = True
            continue
            

        if not current:
            current = TextRun(glyph)

        if glyph['x'] < WIDTH:
            current.add_glyph(glyph)
        else:
            print "Omitting glyph due being out of space", glyph

        last_glyph = glyph

    if current:
        text_runs.append(current)
        current = None

    return text_runs

