#!/usr/bin/env python

#
# Copyright (C) 2008 Openmoko Inc.
#
# Execute the drawing commands.... put that onto a canvas
import cairo, os

#
# Load everything into ram...
max_height = 0
glyph_data = []

input = open("render_text.blib")
for line in input:
    split = line.strip().split(',')  
    glyph = { 'x'     : int(split[0]),
              'y'     : int(split[1]),
              'font'  : split[2],
              'glyph' : split[3] }
    glyph_data.append(glyph)

    if max_height < glyph['y']:
        max_height = glyph['y']


# Assume the highest font has 60 pixels
max_height = max_height + 60


destination_surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, 640, max_height)
context = cairo.Context(destination_surface)
context.rectangle(0, 0, 640, max_height)
context.fill()

for glyph in glyph_data:
    base_path = os.path.join("fonts", glyph['font'], glyph['glyph'])

    try:
        path = os.path.join(base_path, 'bitmap.png')
        glyph_image = cairo.ImageSurface.create_from_png(path)
    except IOError, e:
        print "Issue with", path, glyph, e
        continue

    x = glyph['x'] #+ int(open(os.path.join(base_path, 'bitmap_left_bearing')).readline().strip())
    y = glyph['y'] #- int(open(os.path.join(base_path, 'bitmap_top_bearing')).readline().strip())

    context.translate(x, y)
    context.set_source_surface(glyph_image, 10, 10)
    context.paint()
    context.translate(-x, -y)

destination_surface.write_to_png("rendered_result.png")
