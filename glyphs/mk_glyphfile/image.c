/*
    Copyright (c) 2008 Daniel Mack <daniel@caiaq.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "image.h"

static int bpp = 4;
static int output_bitpos = 0;
static unsigned char output_char = 0;
static int output_fd = -1;
static int output_pos = 0;
static unsigned int index_table[0xffff] = { 0 };

static int open_output_file(const char *filename)
{
	unlink(filename);
	output_fd = open(filename, O_RDWR | O_CREAT,
				  S_IRUSR | S_IWUSR |
			          S_IRGRP | S_IROTH);
	if (output_fd < 0) {
		printf("Unable to open output file: %s\n", strerror(errno));
		return output_fd;
	}

	return 0;
}

static int output_pixel(unsigned int val)
{
	val >>= (8 - bpp);
	output_char |= val;
	output_bitpos += bpp;

	if (output_bitpos >= 8) {
		if (write(output_fd, &output_char, 1) != 1)
			return -3;
		
		output_pos++;
		output_bitpos = 0;
		output_char = 0;
	} else
		output_char <<= bpp;

	return 0;
}

static int image_read_file(const char *filename, int unicode)
{
	FILE *in;
	gdImagePtr im;
	int x, y, w, h;

	unicode &= 0xffff;

	in = fopen(filename, "r");
	if (!in) {
		printf("Unable to open input file: %s\n", strerror(errno));
		return -1;
	}

	im = gdImageCreateFromPng(in);
	fclose(in);

	if (!im) {
		printf("Unable to create gdImage from file.\n");
		return -2;
	}

	index_table[unicode] = output_pos;
	w = gdImageSX(im);
	h = gdImageSY(im);
	printf("adding glyph for unicode #%d from %s (%d x %d pixels), offset %d\n",
		unicode, filename, w, h, output_pos);

	if (write(output_fd, &w, 1) != 1)
		return -3;
	
	if (write(output_fd, &h, 1) != 1)
		return -3;

	output_pos += 2;
	output_bitpos = output_char = 0;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++) {
			int c = gdImageGetPixel(im, x, y);
			int p = gdImageRed(im, c);
			int ret = output_pixel(255 - p);
			if (ret)
				return ret;
		}

	gdImageDestroy(im);
	return 0;
}

static void strchomp(char *s)
{
	if (strlen(s) < 1)
		return;

	while(s[strlen(s) - 1] == '\n')
		s[strlen(s) - 1] = '\0';
}

int main (int argc, char **argv)
{
	char buf[0xff];
	FILE *mapfile;

	if (argc < 3) {
		printf("Usage: %s <mapfile> <outfile>\n", argv[0]);
		printf("\t<infile> is a image file, preferrably .png\n");
		printf("\t<outfile> is the file where the binary content will be written to\n");
		return -1;
	}

	mapfile = fopen(argv[1], "r");
	if (!mapfile) {
		printf("unable to open %s: %s", argv[1], strerror(errno));
		return errno;
	}
	
	if (open_output_file(argv[2]) < 0)
		return errno;

	if (write(output_fd, index_table, sizeof(index_table)) != sizeof(index_table)) {
		printf("WARNING: unable to write: %s\n", strerror(errno));
		return errno;
	}

	output_pos += sizeof(index_table);

	while (fgets(buf, sizeof(buf), mapfile)) {
		char *s, *filename;
		int ret, unicode;

		if (strlen(buf) == 0 || buf[0] == '#')
			continue;

		s = strtok(buf, " \t");
		if (!s)
			continue;

		unicode = strtol(s, NULL, 0);
		if (unicode == 0)
			unicode = s[0];

		filename = strtok(NULL, " \t");
		if (!filename)
			continue;

		strchomp(filename);
		ret = image_read_file(filename, unicode);
		if (ret)
			printf("WARNING: image_read_file() returned %d\n", ret);
	}

	lseek(output_fd, 0, SEEK_SET);
	if (write(output_fd, index_table, sizeof(index_table)) != sizeof(index_table))
		printf("WARNING: unable to write: %s\n", strerror(errno));

	int i;
	for (i = 0; i < sizeof(index_table)/4; i++)
		if (index_table[i] != 0)
			printf(" %d: %d\n", i, index_table[i]);

	close(output_fd);
	return 0;
}

