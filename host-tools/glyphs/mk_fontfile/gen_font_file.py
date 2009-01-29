#!/usr/bin/python

import gd
import os
import struct
import array

fontpath = "/home/daniel/src/WebKit/WebKit/font-foo/"
outfilename = "fontfile.gen"

def gen_spacing_hints(fontid, glyphid):
	spacingpath = fontpath + "/" + fontid + "/spacing/"
	all = os.listdir(spacingpath)
	filtered = array.array('I')
	spacings = ""

	for s in all:
		if s[:len(glyphid) + 1] == glyphid + "-":
			c = int(s[len(glyphid)+1:])
			filtered.append(c)

	for other in filtered:
		fname = spacingpath + "/" + glyphid + "-" + str(other) + "/spacing"
		try:
			f = open(fname, 'r')
			s = f.read()
			f.close()
			a = s.split(",");
			x = int(a[0])
			y = int(a[1])
			spacings += struct.pack("<hbb", other, x, y)
		except:
			print "unable to parse %s" % (fname)
			raise
			continue

	return spacings

def get_max(list):
	max = 0

	for e in list:
		if int(e) > max:
			max = int(e)

	return max

def gen_font(fontid):
	path = fontpath + "/" + fontid + "/"
	glyphpath = path + "glyphs/"
	glyphlist = os.listdir(glyphpath)

	# the index table will take an unsigned int for each entry
	n_glyphs = get_max(glyphlist) + 1
	offsettable = [ 0 ] * n_glyphs
	offset = n_glyphs * 4
	out = ""
	print "font %s has %d glyphs" % (fontid, n_glyphs)

	for glyphid in glyphlist:
		imagefile = glyphpath + glyphid + "/bitmap.png"
		spacing_hints = gen_spacing_hints(fontid, glyphid)

		try:
			im = gd.image(imagefile)
			(w, h) = im.size()

			offsettable[int(glyphid)] = offset;

			# a spacing hint is always 4 bytes ...
			n_spacing_hints = len(spacing_hints) / 4;
			header = struct.pack("<BBI", w, h, n_spacing_hints)

			#print "%s/ %d,%d" % (fontid, w, h)

			bit = 0;
			outbyte = 0;
			offset += len(header);
			out += header

			for n in range (0, w * h):
				pixel = im.getPixel((n % w, n / w))
				bit = n % 8;
	
				(r, g, b) = im.colorComponents(pixel)
				color = (r + g + b) / 3

				if (color > 127):
					outbyte |= 1 << bit;
				
				if bit == 7:
					out += struct.pack("B", outbyte)
					outbyte = 0
					offset += 1
					bit = 0

			if bit > 0:
				out += struct.pack("B", outbyte)
				offset += 1

#			im.close()

		except:
			print "unable to open bitmap file >%s<" % (imagefile)
			continue

		out += spacing_hints
		offset += len(spacing_hints)

	table = ""
	for i in offsettable:
		table += struct.pack("<I", i)

	print "index to glyph 0 (font %s) is %d" % (fontid, offsettable[0])

	return struct.pack("<I", n_glyphs) + table + out


fontlist = os.listdir(fontpath)
fonttable = [ 0 ] * len(fontlist)
offset = len(fontlist) * 4
fontnum = 0
out = ""

for _font in fontlist:
	f = gen_font(str(fontnum))
	out += f
	fonttable[fontnum] = offset;
	print "offset for font %d is %d" % (fontnum, offset)
	fontnum += 1
	offset += len(f)

outfile = open(outfilename, 'w')

# write the number of fonts in this file
outfile.write(struct.pack("<I", len(fontlist)))

for i in fonttable:
	outfile.write(struct.pack("<I", i))

outfile.write(out)
outfile.close()

print "generated file >%s<, size %d + %d" % (outfilename, len(out), 4)

