#ifndef GUILIB_H
#define GUILIB_H

/* this is hard-coded tu 4pp for now */
#define FRAMEBUFFER_WIDTH  (320)
#define FRAMEBUFFER_HEIGHT (240)
#define FRAMEBUFFER_SIZE ((FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT) / 2)

/* to be implemented by hardware abtraction layer */
extern void fb_set_pixel(int x, int y, int v);
extern void fb_refresh(void);
extern void fb_clear(void);

/* functions for graphics context management */
void guilib_fb_lock(void);
void guilib_fb_unlock(void);

/* implemented by guilib */
void guilib_init(void);

#endif /* GUILIB_H */

