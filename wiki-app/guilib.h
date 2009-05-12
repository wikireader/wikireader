#ifndef GUILIB_H
#define GUILIB_H

/* to be provided by the hardware implementation */
extern unsigned char *framebuffer;
extern void fb_refresh();

unsigned int guilib_framebuffer_width(void);
unsigned int guilib_framebuffer_height(void);
unsigned int guilib_framebuffer_size(void);

/* implemented by guilib */
void guilib_init(void);

/* generic pixel value accessors
 * the get method is only needed by the simulators */
void guilib_set_pixel(int x, int y, int v);
int  guilib_get_pixel(int x, int y);
void guilib_invert(int start, int lines);
void guilib_clear(void);
void guilib_clear_area(unsigned int start_x, unsigned int start_y, unsigned int end_x, unsigned int end_y);

/* functions for graphics context management */
void guilib_fb_lock(void);
void guilib_fb_unlock(void);

struct guilib_image {
	unsigned int width;
	unsigned int height;
	char data[];
};

void guilib_blit_image(const struct guilib_image *img, int x, int y);

#endif /* GUILIB_H */

