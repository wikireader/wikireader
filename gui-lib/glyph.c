/*
 * guilib - a minimal pixel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* wiki-lib includes */
#include <wikilib.h>
#include <file-io.h>

/* gui-lib includes */
#include "guilib.h"
#include "glyph.h"
#include "fontfile.h"

void render_glyph(int start_x, int start_y, const struct glyph *glyph)
{
	int x, y, bit = 0;
	const char *d = glyph->data;
				
	for (y = 0; y < glyph->height; y++)
		for (x = 0; x < glyph->width; x++) {
			guilib_set_pixel(start_x + x, start_y + y, (*d >> bit) & 1);
			bit++;
			if (bit == 8) {
				bit = 0;
				d++;
			}
		}
}

#if 0
/* we might need that later ... */
static int simple_kerning(struct Glyph *a, struct Glyph *b)
{
	int y, delta;

	if (!a || !b)
		return 0;

	/* we do some very simple kerning here. The idea is to scan
	 * the left edge of the right glyph and the right edge of the
	 * left glyph and find out how far the two could move towards
	 * each other without colliding. */
	delta = MIN(a->w, b->w);
	for (y = 0; y < MIN(a->h, b->h); y++) {
		int x, d = 0;

		/* right edge of left glyph */
		for (x = a->w - 1; x > 0; x--, d++)
			if (glyph_pixel(a, x, y) != 0)
				break;

		/* left edge of right pixel */
		for (x = 0; x < b->w; x++, d++)
			if (glyph_pixel(b, x, y) != 0)
				break;

		/* find smallest value for all rows */
		if (d < delta)
			delta = d;
	}

	return delta;
}
#endif

/*
 * static copy a char map... true for some fonts e.g.
 * the DejaVu family
 */
static const int char_to_glyph[127] = {
    0, /* index:   0 */
    0, /* index:   1 */
    0, /* index:   2 */
    0, /* index:   3 */
    0, /* index:   4 */
    0, /* index:   5 */
    0, /* index:   6 */
    0, /* index:   7 */
    0, /* index:   8 */
    0, /* index:   9 */
    0, /* index:  10 */
    0, /* index:  11 */
    0, /* index:  12 */
    0, /* index:  13 */
    0, /* index:  14 */
    0, /* index:  15 */
    0, /* index:  16 */
    0, /* index:  17 */
    0, /* index:  18 */
    0, /* index:  19 */
    0, /* index:  20 */
    0, /* index:  21 */
    0, /* index:  22 */
    0, /* index:  23 */
    0, /* index:  24 */
    0, /* index:  25 */
    0, /* index:  26 */
    0, /* index:  27 */
    0, /* index:  28 */
    0, /* index:  29 */
    0, /* index:  30 */
    0, /* index:  31 */
    0, /* index:  32 */
    0, /* index:  33 */
    0, /* index:  34 */
    0, /* index:  35 */
    0, /* index:  36 */
    0, /* index:  37 */
    0, /* index:  38 */
    0, /* index:  39 */
    0, /* index:  40 */
    0, /* index:  41 */
    0, /* index:  42 */
    0, /* index:  43 */
    0, /* index:  44 */
    0, /* index:  45 */
    0, /* index:  46 */
    18, /* index:  47 */
    19, /* index:  48 */
    20, /* index:  49 */
    21, /* index:  50 */
    22, /* index:  51 */
    23, /* index:  52 */
    24, /* index:  53 */
    25, /* index:  54 */
    26, /* index:  55 */
    27, /* index:  56 */
    28, /* index:  57 */
    0, /* index:  58 */
    0, /* index:  59 */
    0, /* index:  60 */
    0, /* index:  61 */
    0, /* index:  62 */
    0, /* index:  63 */
    0, /* index:  64 */
    0, /* index:  65 */
    0, /* index:  66 */
    0, /* index:  67 */
    0, /* index:  68 */
    0, /* index:  69 */
    0, /* index:  70 */
    0, /* index:  71 */
    0, /* index:  72 */
    0, /* index:  73 */
    0, /* index:  74 */
    0, /* index:  75 */
    0, /* index:  76 */
    0, /* index:  77 */
    0, /* index:  78 */
    0, /* index:  79 */
    0, /* index:  80 */
    0, /* index:  81 */
    0, /* index:  82 */
    0, /* index:  83 */
    0, /* index:  84 */
    0, /* index:  85 */
    0, /* index:  86 */
    0, /* index:  87 */
    0, /* index:  88 */
    0, /* index:  89 */
    0, /* index:  90 */
    0, /* index:  91 */
    0, /* index:  92 */
    0, /* index:  93 */
    0, /* index:  94 */
    0, /* index:  95 */
    0, /* index:  96 */
    0, /* index:  97 */
    0, /* index:  98 */
    0, /* index:  99 */
    0, /* index: 100 */
    0, /* index: 101 */
    0, /* index: 102 */
    0, /* index: 103 */
    0, /* index: 104 */
    0, /* index: 105 */
    0, /* index: 106 */
    0, /* index: 107 */
    0, /* index: 108 */
    0, /* index: 109 */
    0, /* index: 110 */
    0, /* index: 111 */
    0, /* index: 112 */
    0, /* index: 113 */
    0, /* index: 114 */
    0, /* index: 115 */
    0, /* index: 116 */
    0, /* index: 117 */
    0, /* index: 118 */
    0, /* index: 119 */
    0, /* index: 120 */
    0, /* index: 121 */
    0, /* index: 122 */
    0, /* index: 123 */
    0, /* index: 124 */
    0, /* index: 125 */
    0, /* index: 126 */
    0, /* index: 127 */
};

/**
 * Simplistic string drawing
 *
 * @param font The font index to use
 * @param string The string to draw. No text wrapping will be done
 * @param start_x From where to start drawing (upper left)
 * @param start_y From where to start drawing (upper left)
 */
int render_string(const int font, const char *string, int start_x, int start_y)
{
	int i;
	const int text_length = strlen(string);

	int x = start_x;

	for (i = 0; i < text_length; ++i) {
		struct glyph *glyph = get_glyph(font,
				    char_to_glyph[string[i] & 0x7f]);

		/* painting and advance */
		/* TODO: use the above auto kerning for the advance */
		render_glyph(x, start_y, glyph);
		x += glyph->width + 1;
	}

	return x;
}
