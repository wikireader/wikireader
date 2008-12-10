/* wiki-lib includes */
#include <wikilib.h>
#include <file-io.h>

/* gui-lib includes */
#include "guilib.h"
#include "glyph.h"

#define FONTSIZE 5

static int glyph_fd = -1;

/* FIXME: solve this with memory management */
static struct Glyph staticGlyph[2];
static char _buf_a[200], _buf_b[200];

int glyph_init(const char *filename)
{
	glyph_fd = wl_open(filename, WL_O_RDONLY);
	if (!glyph_fd)
		return -1;

	staticGlyph[0].data = _buf_a;
	staticGlyph[1].data = _buf_b;
	return 0;
}

static struct Glyph *get_glyph(int index)
{
	unsigned int offset, size;
	static int cnt = 0;
	struct Glyph *g = &staticGlyph[cnt++ & 1];

	index &= 0xffff;
	wl_seek(glyph_fd, index * sizeof(offset));
	wl_read(glyph_fd, &offset, sizeof(offset));
	if (offset == 0)
		return NULL;

	wl_seek(glyph_fd, offset);
	wl_read(glyph_fd, &g->w, 1);
	wl_read(glyph_fd, &g->h, 1);
	size = (g->h * g->w) / 2;
	wl_read(glyph_fd, g->data, size);

	return g;
}

static inline char glyph_pixel(struct Glyph *g, int x, int y)
{
	int byte = y * g->w + x;
	char v = g->data[byte / 2];
	if (byte & 1)
		return v & 0xf;
	else
		return v >> 4;
}

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

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

/* HACK - just a test */
int render_string(const char *s, int off_x, int off_y)
{
	int x, y;
	struct Glyph *last_glyph = NULL;
	
	guilib_fb_lock();
	
	while (*s) {
		int index = *s++;
		struct Glyph *g = get_glyph(index);

		if (index == ' ') {
			off_x += FONTSIZE;
			continue;
		}

		if (!g || g->w == 0 || g->h == 0)
			continue;
		
		off_x -= simple_kerning(last_glyph, g);

		for (y = 0; y < g->h; y++)
			for (x = 0; x < g->w; x++) 
				fb_set_pixel(x + off_x, y + off_y,
						glyph_pixel(g, x, y));

		off_x += g->w + (FONTSIZE / 2);
		last_glyph = g;
	}

	guilib_fb_unlock();
	return 0;
}

