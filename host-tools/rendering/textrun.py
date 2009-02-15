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
    for line in open(file):
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

    font_occurences = {}
    glyph_occurences = {}
    length_occurences = {}
    x_occurences = {}
    y_occurences = {}

    # Two passes....
    for glyph in glyphs:
        if glyph['x'] == 0 and glyph['y'] == 0 and glyph['font'] == '0' and glyph['glyph'] == '0':
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
                last_x = current.glyphs[-1]['x']
                assert last_x <= WIDTH

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

        if glyph['x'] < WIDTH:
            current.add_glyph(glyph)
        else:
            print "Omitting glyph due being out of space", glyph

        last_glyph = glyph

    if current:
        text_runs.append(current)
        current = None

    return (text_runs, glyph_occurences, font_occurences, x_occurences, y_occurences, length_occurences)

