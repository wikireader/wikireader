#!/usr/bin/python
"""
 Generate an image from a png which can be used by guilib.

 Copyright (C) 2009 Daniel Mack <daniel@caiaq.de>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import gd
import os
import sys

def usage():
	print "guilib image file generator"
	print "Usage: %s <image> <outfilename> <imagename>" % (sys.argv[0])
	print "This tool will output a 'static struct guilib_image' to <outfile>"
	print "containing the pixel information from <imagefile>. The struct will"
	print "be named <imagename>."
	print "All paramters are mandatory."
	sys.exit(1)

try:
	imagefile = sys.argv[1]
	outfile = sys.argv[2]
	imagename = sys.argv[3]
except:
	usage()


bit = 0
outbyte = 0
count = 0

try:
	im = gd.image(imagefile)
	(w, h) = im.size()

	out =  "static struct guilib_image %s = {\n" % (imagename)
	out += "\t.width = %d,\n" % (w)
	out += "\t.height = %d,\n" % (h)
	out += "\t.data = {\n"
	out += "\t\t"

	for n in range (0, w * h):
		pixel = im.getPixel((n % w, n / w))
		bit = n % 8;

		(r, g, b) = im.colorComponents(pixel)
		color = (r + g + b) / 3

		if (color > 127):
			outbyte |= 1 << (7 - bit);
				
		if bit == 7:
			out += "0x%02x, " % outbyte
			outbyte = 0
			bit = 0
			count += 1

			if (count % 16) == 0:
				out += "\n\t\t"

	out += "}\n"
	out += "};\n"

	if bit > 0:
		out += struct.pack("B", outbyte)

	#im.close()

except:
	print "unable to open bitmap file >%s<" % (imagefile)
	sys.exit(2)

f = open(outfile, 'w')
print >> f, out
f.close()

