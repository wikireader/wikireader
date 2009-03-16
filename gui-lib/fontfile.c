/*
 * guilib - a minimal pixel framework
 * Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <wikilib.h>
#include <msg.h>
#include <file-io.h>
#include <malloc.h>
#include <wl-endian.h>
#include <compress.h>

/* gui-lib includes */
#include "glyph.h"
#include "fontfile.h"

/* reads the file described in glyphs/fonts/README */

struct glyph_spacing {
	unsigned short next_glyph;
	unsigned char x, y;
} __attribute__((packed));

unsigned char	*file_buf;
unsigned int	*font_index;
unsigned int	 file_size;
unsigned int	 n_fonts;

unsigned int guilib_nr_fonts (void)
{
	return n_fonts;
}

int glyph_spacing (struct glyph *first, unsigned short second,
		   unsigned char *x, unsigned char *y)
{
	unsigned int i;
	char *tmp = (char *) first;
	/* jump over fixed members and bitmap data */
	struct glyph_spacing *space = (struct glyph_spacing *)
					(tmp + sizeof(*first) +
					((first->width * first->height) + 7) / 8);
	
	for (i = 0; i < first->n_spacing_hints; i++, space++)
		if (space->next_glyph == second) {
			*x = space->x;
			*y = space->y;
			return 1;
		}

	return 0;
}

unsigned int read_u32(const unsigned char* start, unsigned int offset)
{
	if (offset % 4 != 0) {
#if BYTE_ORDER == LITTLE_ENDIAN
		return  (start+offset)[3] << 24 |
			(start+offset)[2] << 16 |
			(start+offset)[1] <<  8 |
			(start+offset)[0] <<  0;
#else
		return  (start+offset)[0] << 24 |
			(start+offset)[1] << 16 |
			(start+offset)[2] <<  8 |
			(start+offset)[3] <<  0;
#endif
	} else {
		return *(unsigned int *)(start + offset);
	}
}

const struct glyph *get_glyph(unsigned int font, unsigned int glyph)
{
	unsigned int font_start, n_glyphs, glyph_index, *glyph_table;

	/* the entry in the font_index table points to the position in our
	 * file where the font definition starts */
	font_start = WL_LTONL(font_index[font]);

	if (font_start > file_size)
		return NULL;

	/* the first integer at this position is the numbers of glyphs */
	n_glyphs = WL_LTONL(read_u32(file_buf, font_start));
	if (glyph >= n_glyphs)
		return NULL;

	/* subsequent indices are relative to the beginning of the
	 * glyph table */
	font_start += sizeof(n_glyphs);

	/* look up our glyph and cast the struct */
	glyph_table = (unsigned int *) (file_buf + font_start);
	glyph_index = WL_LTONL(read_u32(file_buf, font_start + 4 * glyph));

	return (struct glyph *) (file_buf + font_start + glyph_index);
}

int read_font_file(const char *filename)
{
	void *buf;

	buf = decompress(filename, &file_size);
	if (!buf)
		return -1;

	file_buf = buf;

	n_fonts = *(unsigned int *) file_buf;

	msg(MSG_INFO, "font file has %d fonts in %d bytes, buf %p\n", n_fonts, file_size, file_buf);
	file_buf += sizeof(n_fonts);

	font_index = WL_LTONL((unsigned int *) file_buf);
	return 0;
}

