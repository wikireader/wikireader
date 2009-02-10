#include <file-io.h>
#include <msg.h>
#include "guilib.h"
#include "glyph.h"
#include "fontfile.h"

void guilib_set_pixel(int x, int y, int v)
{
	unsigned int byte = (x + FRAMEBUFFER_WIDTH * y) / 8;
	unsigned int bit  = (x + FRAMEBUFFER_WIDTH * y) % 8;

	if (v)
		framebuffer[byte] |= (1 << (7 - bit));
	else
		framebuffer[byte] &= ~(1 << (7 - bit));
}

int guilib_get_pixel(int x, int y)
{
	unsigned int byte = (x + FRAMEBUFFER_WIDTH * y) / 8;
	unsigned int bit  = (x + FRAMEBUFFER_WIDTH * y) % 8;

	return (framebuffer[byte] >> (7 - bit)) & 1;
}

/* The idea is that every function which calls painting routines calls
 * guilib_fb_lock() before any operation and guilib_fb_unlock() after
 * it. This way, only the last of these functions in the calling stack
 * will actually execute fb_refresh(). */
static int fb_ref = 0;

void guilib_fb_lock(void)
{
	fb_ref++;
}

void guilib_fb_unlock(void)
{
	if (fb_ref == 0)
		return;
	
	if (--fb_ref == 0)
		fb_refresh();
}

#define FONTFILE "/tmp/fontfile.gen"

void guilib_init(void)
{
	const struct glyph *g;

	memset(framebuffer, 0, FRAMEBUFFER_SIZE);

	/* just some tests ... */
	if (read_font_file(FONTFILE) != 0) {
		msg(MSG_INFO, "unable to load font file %s\n", FONTFILE);
		return;
	}

	msg(MSG_INFO, "loaded font file %s\n", FONTFILE);
	g = get_glyph(0, 0);
	render_glyph(10, 10, g);
}

