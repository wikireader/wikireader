/*
 * Copyright (c) 2009 Openmoko Inc.
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
void guilib_invert(int start, int lines);
void guilib_invert_area(int start_x, int start_y, int end_x, int end_y);
void guilib_clear(void);
void guilib_clear_area(int start_x, int start_y, int end_x, int end_y);
void guilib_buffer_invert_area(unsigned char *membuffer, int start_x, int start_y, int end_x, int end_y);
void guilib_buffer_set_pixel(unsigned char *membuffer, int x, int y);
void guilib_buffer_clear_pixel(unsigned char *membuffer, int x, int y);
void guilib_buffer_clear_area(unsigned char *membuffer,
			      int width, int height, int buf_width_bytes,
			      int start_x, int start_y, int end_x, int end_y);

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

