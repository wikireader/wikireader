#include <file-io.h>
#include <msg.h>
#include "guilib.h"
#include "glyph.h"
#include "fontfile.h"

/* The idea is that every function which calls painting routine calls guilib_fb_lock()
 * before any operation and guilib_fb_unlock() after it. This way, only the last of
 * these functions in the calling stack will actually execute fb_refresh(). */
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

void guilib_draw_vline(unsigned int x1, unsigned int x2, unsigned int y, unsigned int val)
{
	guilib_fb_lock();

	while (x2 >= x1)
		fb_set_pixel(x1++, y, val);

	guilib_fb_unlock();
}

void guilib_draw_hline(unsigned int x, unsigned int y1, unsigned int y2, unsigned int val)
{
	guilib_fb_lock();

	while (y2 >= y1)
		fb_set_pixel(x, y1++, val);
	
	guilib_fb_unlock();
}

#define FONTFILE "/tmp/fontfile.gen"

void guilib_init(void)
{
	/* just some tests ... */
	if (read_font_file(FONTFILE) != 0) {
		msg(MSG_INFO, "unable to load font file %s", FONTFILE);
		return;
	}

	msg(MSG_INFO, "loaded font file %s", FONTFILE);
//	render_string("Oh my god, it's techno music", 10, 10);
//	render_string("WAR WAS?", 10, 30);
	
	const struct glyph *g = get_glyph(0, 0);
	msg(MSG_INFO, "glyph: %d x %d ...", g->width, g->height);

	render_glyph(10, 10, g);
}

