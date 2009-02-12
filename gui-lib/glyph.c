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
    1, /* index:  30 */
    2, /* index:  31 */
    3, /* index:  32 */
    4, /* index:  33 */
    5, /* index:  34 */
    6, /* index:  35 */
    7, /* index:  36 */
    8, /* index:  37 */
    9, /* index:  38 */
    10, /* index:  39 */
    11, /* index:  40 */
    12, /* index:  41 */
    13, /* index:  42 */
    14, /* index:  43 */
    15, /* index:  44 */
    16, /* index:  45 */
    17, /* index:  46 */
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
    29, /* index:  58 */
    30, /* index:  59 */
    31, /* index:  60 */
    32, /* index:  61 */
    33, /* index:  62 */
    34, /* index:  63 */
    35, /* index:  64 */
    36, /* index:  65 */
    37, /* index:  66 */
    38, /* index:  67 */
    39, /* index:  68 */
    40, /* index:  69 */
    41, /* index:  70 */
    42, /* index:  71 */
    43, /* index:  72 */
    44, /* index:  73 */
    45, /* index:  74 */
    46, /* index:  75 */
    47, /* index:  76 */
    48, /* index:  77 */
    49, /* index:  78 */
    50, /* index:  79 */
    51, /* index:  80 */
    52, /* index:  81 */
    53, /* index:  82 */
    54, /* index:  83 */
    55, /* index:  84 */
    56, /* index:  85 */
    57, /* index:  86 */
    58, /* index:  87 */
    59, /* index:  88 */
    60, /* index:  89 */
    61, /* index:  90 */
    62, /* index:  91 */
    63, /* index:  92 */
    64, /* index:  93 */
    65, /* index:  94 */
    66, /* index:  95 */
    67, /* index:  96 */
    68, /* index:  97 */
    69, /* index:  98 */
    70, /* index:  99 */
    71, /* index: 100 */
    72, /* index: 101 */
    73, /* index: 102 */
    74, /* index: 103 */
    75, /* index: 104 */
    76, /* index: 105 */
    77, /* index: 106 */
    78, /* index: 107 */
    79, /* index: 108 */
    80, /* index: 109 */
    81, /* index: 110 */
    82, /* index: 111 */
    83, /* index: 112 */
    84, /* index: 113 */
    85, /* index: 114 */
    86, /* index: 115 */
    87, /* index: 116 */
    88, /* index: 117 */
    89, /* index: 118 */
    90, /* index: 119 */
    91, /* index: 120 */
    92, /* index: 121 */
    93, /* index: 122 */
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
