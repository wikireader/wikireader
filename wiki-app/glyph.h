#ifndef GLYPH_H
#define GLYPH_H

struct glyph {
	unsigned char width;
	unsigned char height;
	signed char top_bearing;
	unsigned int n_spacing_hints;
	char data[0];
} __attribute__((packed));

int render_string(const int font, int off_x, int off_y, char*, int len);
int render_string_right(const int font, int off_x, int off_y, char*, int len);
void render_glyph(int start_x, int start_y, const struct glyph *glyph, char *buf);
int draw_glyphs_to_buf(const int font, int start_x, int start_y, const char *string, char *buf);
#endif /* GLYPH_H */

