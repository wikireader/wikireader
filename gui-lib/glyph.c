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

#define GLYPH_PIXEL(g, x, y)	\

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

/* HACK - just a test */
int render_string(const char *s, int off_x, int off_y)
{
	int x, y;

	guilib_fb_lock();

	while (*s) {
		int index = *s++;
		struct Glyph *g = glyph_array + index;

		if (index == ' ')
			off_x += FONTSIZE;

		if (g->w == 0 || g->h == 0)
			continue;

		for (y = 0; y < g->h; y++)
			for (x = 0; x < g->w; x++) {
				char v = g->data[(y * g->w + x) / 2];
				if (x & 1)
					v &= 0xf;
				else
					v >>= 4;

				fb_set_pixel(x + off_x, y + off_y, v);
			}

		off_x += g->w + (FONTSIZE / 2);
	}

	guilib_fb_unlock();
	return 0;
}

