#include <stdio.h>
#include "guilib.h"
#include "glyph.h"

static int fb_ref = 0;

/* The idea is that every function which calls painting routine calls guilib_fb_lock()
 * before any operation and guilib_fb_unlock() after it. This way, only the last of
 * these functions in the calling stack will actually execute fb_refresh(). */

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

void guilib_init(void)
{
	/* just some tests ... */
	glyph_init("/tmp/glyphfile");
	render_string("Oh my god, it's techno music", 10, 10);
	render_string("WAR WAS?", 10, 30);
}

