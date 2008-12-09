/* FIXME: remove this includes */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "guilib.h"
#include "glyph.h"

/* FIXME: solve this with memory management */
static char _buf[0xffff];

#define MAX_GLYPHS 0xffff

static struct Glyph glyph_array[MAX_GLYPHS];

int load_glyphs(void)
{
	char *buf = _buf;
	struct Glyph *glyph;

	/* FIXME: make this file I/O portable */
	memset(_buf, 0, sizeof(_buf));
	int fd = open("/tmp/glyphfile", O_RDONLY);
	if (!fd)
		return -1;

	for (;;) {
		char tmp[4];
		int size, index;

		if (read(fd, tmp, sizeof(tmp)) != sizeof(tmp))
			break;

		index = tmp[1] << 8 | tmp[0];
		if (index > MAX_GLYPHS)
			break;

		glyph = glyph_array + index;
		glyph->w = tmp[2];
		glyph->h = tmp[3];
		glyph->data = buf;
		size = (glyph->h * glyph->w) / 2;

		if (read(fd, buf, size) != size)
			break;

		buf += size;
	}

	close(fd);
	return 0;
}

#define FONTSIZE 5

static inline char glyph_pixel(struct Glyph *g, int x, int y)
{
	int byte = (y * g->w + x);
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

	delta = MIN(a->w, b->w);

	/* we do some very simple kerning here. The idea is to scan
	 * the left edge of the right glyph and the right edge of the
	 * left glyph and find out how far the two could move towards
	 * each other without colliding. */
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
	struct Glyph *last_glyph = 0;

	guilib_fb_lock();

	while (*s) {
		int index = *s++;
		struct Glyph *g = glyph_array + index;

		if (index == ' ') {
			off_x += FONTSIZE;
			continue;
		}

		if (g->w == 0 || g->h == 0)
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

