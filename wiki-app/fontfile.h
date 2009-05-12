#ifndef FONTFILE_H
#define FONTFILE_H

int		glyph_spacing (struct glyph *first, unsigned short second,
				unsigned char *x, unsigned char *y);
const struct	glyph *get_glyph (unsigned int font, unsigned int glyph);
int		read_font_file	(const char *filename);
unsigned int	guilib_nr_fonts ();

#endif /* FONTFILE_H */

