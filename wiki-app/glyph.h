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

#ifndef GLYPH_H
#define GLYPH_H

struct glyph {
	unsigned char width;
	unsigned char height;
	signed char top_bearing;
	unsigned int n_spacing_hints;
	char data[0];
} __attribute__((packed));

int render_string(const int font, int off_x, int off_y, char*, int len, int inverted);
int render_string_right(const int font, int off_x, int off_y, char*, int len, int inverted);
void render_glyph(int start_x, int start_y, const struct glyph *glyph, char *buf);
int draw_glyphs_to_buf(const int font, int start_x, int start_y, const char *string, char *buf);
#endif /* GLYPH_H */

