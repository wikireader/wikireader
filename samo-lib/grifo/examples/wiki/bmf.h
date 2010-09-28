/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 1999 by Sun Microsystems, Inc.
 * All rights reserved.
 */
/*
 * Copyright (C) 1994 X Consortium
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNEC-
 * TION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the X Consortium shall not
 * be used in advertising or otherwise to promote the sale, use or other deal-
 * ings in this Software without prior written authorization from the X Consor-
 * tium.
 *
 * X Window System is a trademark of X Consortium, Inc.
 */

#ifndef _BMF_H
#define _BMF_H

#define FONT_FD_NOT_INITED 9999
#define FONT_COUNT 7

#define FONT_FILE_DEFAULT "text.bmf"
#define FONT_FILE_ITALIC "texti.bmf"
#define FONT_FILE_TITLE "title.bmf"
#define FONT_FILE_SUBTITLE "subtitle.bmf"
#define FONT_FILE_DEFAULT_ALL "textall.bmf"
#define FONT_FILE_TITLE_ALL "titleall.bmf"
#define FONT_FILE_SUBTITLE_ALL "subtlall.bmf"


#include <inttypes.h>

typedef unsigned char bmf_bm_t;
#ifndef ucs4_t
#define ucs4_t unsigned int
#endif

#define bzero(b,n)	(memset((b),0,(n)))

typedef unsigned char_bm_t;

struct fontmetric {
	int ptsz;
	int Xres;
	int Yres;
	int ascent;
	int descent;
	int linespace;
	int firstchar;
	int firstCol;
	int lastCol;
	int lastchar;
	int default_char;
};
struct charmetric {
	int width;
	int height;
	int widthBits;
	int widthBytes;
	int ascent;
	int descent;
	int LSBearing;
	int RSBearing;
	int origin_xoff;
};

struct scaled_charmetric {
	double width;
	double height;
	double widthBits;
	double ascent;
	double descent;
	double origin_xoff;
};

typedef struct charmetric_bmf{
	int8_t   width;
	int8_t   height;
	int8_t   widthBytes;
	int8_t   widthBits;
	int8_t   ascent;
	int8_t   descent;
	int8_t   LSBearing;
	int8_t   widthDevice;
	char   bitmap[48];
}charmetric_bmf;

typedef struct charmetric_bmf_header{
	int8_t   width;
	int8_t   height;
	int8_t   widthBytes;
	int8_t   widthBits;
	int8_t   ascent;
	int8_t   descent;
	int8_t   LSBearing;
	int8_t   RSBearing;
	int32_t  pos;
}charmetric_bmf_header;

typedef struct font_bmf_header{
	int8_t   linespace;
	int8_t   ascent;
	int8_t   descent;
	int8_t   bmp_buffer_len;
	int32_t  default_char;
}font_bmf_header;


typedef struct fontmetric pcf_fontmet_t;


struct pcffont_bmf {
	char *file;
	int fd;
	pcf_fontmet_t Fmetrics;
	int bPartialFont;
	struct pcffont_bmf *supplement_font;
	char *charmetric;
	unsigned long file_size;
	int bmp_buffer_len;
};

typedef struct pcffont_bmf pcffont_bmf_t;

int load_bmf(pcffont_bmf_t *font);
int pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics);
#endif /* _PCF_H */
