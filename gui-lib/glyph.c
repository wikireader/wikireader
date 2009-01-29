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
			fb_set_pixel(start_x + x, start_y + y, (*d >> bit) & 1);
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
