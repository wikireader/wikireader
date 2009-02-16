#ifndef GLYPH_H
#define GLYPH_H

struct glyph {
	unsigned char width;
	unsigned char height;
	signed char top_bearing;
	unsigned int n_spacing_hints;
	const char data[0];
} __attribute__((packed));

int render_string(const int font, int off_x, int off_y, const char*);
void render_glyph(int start_x, int start_y, const struct glyph *glyph);

#endif /* GLYPH_H */

