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
        self.name = name

    def spacing(self, lgyph, rglyph):
        if not lgyph:
            return (0, 0)
        return (1,1)

    def bitmap(self, glyph):
        path = os.path.join(base_path, 'bitmap.png')
        glyph_image = cairo.ImageSurface.create_from_png(path)
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

for char in input.read():
    if char == 'f':
        command = COMMAND_FONT
    elif char == ',' and command == COMMAND_FONT:
        pass
    elif char == 'p':
        command = COMMAND_POS
    elif char == ';' and command == COMMAND_POS:
        pass
    elif char == ' ' and command == COMMAND_POS:
        command = COMMAND_GLYPH
    elif char == ' ' and command == COMMAND_GLYPH:
        pass 
    elif char in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':']:
        pending_buf.append(char)
    else:
        print "Unknwon char", char, type(char), command
        assert False


#    context.translate(x, y)
#    context.set_source_surface(glyph_image, 10, 10)
#    context.paint()

destination_surface.write_to_png("rendered_result.png")
