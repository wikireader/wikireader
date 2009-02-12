/*
 * guilib - a minimal pixel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
 * Copyright (c) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>
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
static const int char_to_glyph(char c)
{
	if (c < 30)
		return 0;
	else
		return c - 29;
}
 
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
				    char_to_glyph(string[i] & 0x7f));

		/* painting and advance */
		/* TODO: use the above auto kerning for the advance */
		render_glyph(x, start_y - glyph->top_bearing, glyph);
		x += glyph->width + 1;
	}

	return x;
}
