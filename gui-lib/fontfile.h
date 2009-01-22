#ifndef FONTFILE_H
#define FONTFILE_H

struct glyph {
	unsigned char width;
	unsigned char height;
	unsigned short num;
	unsigned int n_spacing_hints;
	const char data[0];
} __attribute__((packed));

int		glyph_spacing (struct glyph *first, unsigned short second,
				unsigned char *x, unsigned char *y);
const struct	glyph *get_glyph (unsigned int font, unsigned int glyph);
int		read_font_file	(const char *filename);

#endif /* FONTFILE_H */

