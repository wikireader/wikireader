#ifndef GUILIB_H
#define GUILIB_H

/* this is hard-coded to 1pp for now */
#define FRAMEBUFFER_WIDTH  (240)
#define FRAMEBUFFER_HEIGHT (208)
#define FRAMEBUFFER_SIZE ((FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT) / 8)

/* to be provided by the hardware implementation */
extern char *framebuffer;

/* implemented by guilib */
void guilib_init(void);

/* generic pixel value accessors
 * the get method is only needed by the simulators */
void guilib_set_pixel(int x, int y, int v);
int  guilib_get_pixel(int x, int y);

/* functions for graphics context management */
void guilib_fb_lock(void);
void guilib_fb_unlock(void);

#endif /* GUILIB_H */

