#ifndef GLYPH_H
#define GLYPH_H

struct Glyph {
	char h, w;
	char *data;
};

int render_string(const char *s, int off_x, int off_y);
int glyph_init(const char *filename);

#endif /* GLYPH_H */

