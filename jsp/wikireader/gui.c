// This is just a stub, not compileable yet. but GPL.

#include <guilib/guilib.h>

unsigned char framebuffer = ...;

void fb_clear(void)
{
	memset(framebuffer, (FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT) / 2);
}

void fb_refresh(void)
{
	/* is there a pragma to omit the frame pointer? */
	return;
}

void fb_set_pixel(unsigned int x, unsigned int y, unsigned char val)
{
        if (x & 1) {
                framebuffer[(y * FRAMEBUFFER_WIDTH + x) / 2] &= 0xf0;
                framebuffer[(y * FRAMEBUFFER_WIDTH + x) / 2] |= val & 0xf;
        } else {
                framebuffer[(y * FRAMEBUFFER_WIDTH + x) / 2] &= 0x0f;
                framebuffer[(y * FRAMEBUFFER_WIDTH + x) / 2] |= val << 4;
        }
}

