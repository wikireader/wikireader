#ifndef GLYPH_H
#define GLYPH_H

struct Glyph {
	char h, w;
	char *data;
};

int render_string(const char *s, int off_x, int off_y);
int load_glyphs(void);

#endif /* GLYPH_H */

