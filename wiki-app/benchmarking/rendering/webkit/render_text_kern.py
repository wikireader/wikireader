#!/usr/bin/env python

#
# Copyright (C) 2008 Openmoko Inc.
#
# Execute the drawing commands.... put that onto a canvas
import cairo, os

#
# Load everything into ram...
max_height = 10000
glyph_data = []

input = open("auto_kern_encoding")


# Assume the highest font has 60 pixels
max_height = max_height + 60

class Font:
    """Simple font class"""
    def __init__(self, name):
        assert len(name) != 0
        self.name = "".join(name)

    def spacing(self, lgyph, rglyph):
        if not lgyph:
            return (0, 0)

        data = file(os.path.join("font-foo", self.name, "spacing", "%s-%s" % (lgyph, rglyph), "spacing"), "r").read().split(",")
        return (int(data[0]), int(data[1]))

    def bitmap(self, glyph):
        try:
            path = os.path.join("font-foo", self.name, "glyphs", glyph, 'bitmap.png')
            glyph_image = cairo.ImageSurface.create_from_png(path)
            return glyph_image
        except:
            print "Error with", path
            return None

destination_surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, 240, max_height)
context = cairo.Context(destination_surface)
context.rectangle(0, 0, 240, max_height)
context.fill()

COMMAND_NONE = 0
COMMAND_FONT = 1
COMMAND_POS = 2
COMMAND_GLYPH = 3
command = COMMAND_NONE

pending_buf =  []
current_font = None
last_glyph = None
x = 0
y = 0
paragraph_y = 0

def submit_glyph(glyph):
    global last_glyph, x, y
    if len(glyph) == 0:
        return
    glyph = "".join(glyph)
    (x_space, y_space) = current_font.spacing(last_glyph, glyph)
    x = (x + x_space) % 240
    y = y + y_space
    bitmap = current_font.bitmap(glyph)
    if bitmap:
        context.translate(x, y)
        context.set_source_surface(bitmap, 10, 10)
        context.paint()
        context.translate(-x, -y)

    print "Glyph", glyph
    last_glyph = glyph

for char in input.read():
    if char == 'f':
        submit_glyph(pending_buf)
        pending_buf = []

        command = COMMAND_FONT
    elif char == ',' and command == COMMAND_FONT:
        print "Font Change", pending_buf
        current_font = Font(pending_buf)
        last_glyph = None
        pending_buf = []
    elif char == 'p':
        submit_glyph(pending_buf)
        pending_buf = []

        command = COMMAND_POS
    elif char == ';' and command == COMMAND_POS:
        data = "".join(pending_buf)
        if ":" in data:
            split = data.split(':')
            last_x = int(split[0]) % 240
            paragraph_y = paragraph_y + int(split[1])
        else:
            last_x = int(data)

        x = (x + last_x) % 240
        y = paragraph_y
        print "Paragraph", x, paragraph_y
        last_glyph = None
        command = COMMAND_GLYPH
        pending_buf = []
    elif char == ' ' and command == COMMAND_GLYPH:
        submit_glyph(pending_buf)
        pending_buf = []
    elif char in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':']:
        pending_buf.append(char)
    else:
        print "Unknwon char", char, type(char), command
        assert False

if command == COMMAND_GLYPH:
    submit_glyph(pending_buf)


print x, y

destination_surface.write_to_png("rendered_result.png")
