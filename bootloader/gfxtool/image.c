/*
    e07 bootloader suite
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

static int open_output_file(const char *filename)
{
	unlink(filename);
	output_fd = open(filename, O_RDWR | O_CREAT |
				  S_IRUSR | S_IWUSR |
			          S_IRGRP | S_IROTH , 0666);
	if (output_fd < 0) {
		printf("Unable to open output file: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

static void close_output_file(void)
{
	close(output_fd);
}

static void output_pixel(unsigned int val)
{
	val >>= (8 - bpp);
	output_char |= val;
	output_bitpos += bpp;
	//val <<= (8 - bpp);

	if (output_bitpos >= 8) {
		write(output_fd, &output_char, 1);
		output_bitpos = 0;
		output_char = 0;
	} else
		output_char <<= bpp;
}

static int image_read_file(const char *filename)
{
	FILE *in;
	gdImagePtr im;
	int x, y;
	in = fopen(filename, "r");
	if (!in) {
		printf("Unable to open input file: %s\n", strerror(errno));
		return -1;
	}

	im = gdImageCreateFromPng(in);
	fclose(in);

	if (!im) {
		printf("Unable to create gdImage from file.\n");
		return -1;
	}
	
	for (y = 0; y < gdImageSY(im); y++)
		for (x = 0; x < gdImageSX(im); x++) {
			int c = gdImageGetPixel(im, x, y);
			int p = gdImageRed(im, c);
			output_pixel(p);
		}

	return 0;
}

int main (int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage: %s <infile> <outfile>\n", argv[0]);
		printf("\t<infile> is a image file, preferrably .png\n");
		printf("\t<outfile> is the file where the binary content will be written to\n");
		return -1;
	}

	if (open_output_file(argv[2]) < 0)
		return -1;

	if (image_read_file(argv[1]) < 0)
		return -1;

	close_output_file();
	return 0;
}
