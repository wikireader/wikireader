#include <wikilib.h>
#include <msg.h>
#include <file-io.h>
#include <malloc.h>
#include <wl-endian.h>

/* gui-lib includes */
#include "glyph.h"
#include "fontfile.h"

/* reads the file described in glyphs/fonts/README */

struct glyph_spacing {
	unsigned short next_glyph;
	unsigned char x, y;
} __attribute__((packed));

#define BLOCK_SIZE		512

unsigned char	*file_buf;
unsigned int	*font_index;
unsigned int	 file_size;
unsigned int	 n_fonts;

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

const struct glyph *get_glyph(unsigned int font, unsigned int glyph)
{
	unsigned int font_start, n_glyphs, glyph_index, *glyph_table;

	/* the entry in the font_index table points to the position in our
	 * file where the font definition starts */
	font_start = WL_LTONL(font_index[font]);

	if (font_start > file_size)
		return NULL;

	/* the first integer at this position is the numbers of glyphs */
	n_glyphs = WL_LTONL(*(unsigned int *) (file_buf + font_start));
	if (glyph >= n_glyphs)
		return NULL;

	/* subsequent indices are relative to the beginning of the
	 * glyph table */
	font_start += sizeof(n_glyphs);

	/* look up our glyph and cast the struct */
	glyph_table = (unsigned int *) (file_buf + font_start);
	glyph_index = WL_LTONL(glyph_table[glyph]);

	return (struct glyph *) (file_buf + font_start + glyph_index);
}

int read_font_file(const char *filename)
{
	unsigned int len, block_size, r;
	unsigned char *buf;
	int fd = wl_open(filename, WL_O_RDONLY);

	if (fd < 0)
		return fd;

	/* read the whole file */
	wl_fsize(fd, &len);
	file_size = len;
	file_buf = malloc(file_size);

	block_size = BLOCK_SIZE;
	buf = file_buf;

	do {
		r = wl_read(fd, buf, block_size);
		len -= r;
		buf += r;
		block_size = (len > BLOCK_SIZE) ? BLOCK_SIZE : len;
	} while (r && len);

	n_fonts = *(unsigned int *) file_buf;

	msg(MSG_INFO, "font file has %d fonts in %d bytes, buf %p\n", n_fonts, file_size, file_buf);
	file_buf += sizeof(n_fonts);

	/* FIXME - this is not endian-safe */
	font_index = (unsigned int *) file_buf;
	return 0;
}

