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

#define FONT_COUNT 8

#define FONT_FILE_DEFAULT "text.bmf"
#define FONT_FILE_BOLD "textb.bmf"
#define FONT_FILE_ITALIC "texti.bmf"
#define FONT_FILE_BOLD_ITALIC "textbi.bmf"
#define FONT_FILE_TITLE "title.bmf"
#define FONT_FILE_SUBTITLE "subtitle.bmf"
//#define FONT_FILE_SEARCH_HEADING "heading.bmf"
//#define FONT_FILE_SEARCH_LIST "list.bmf"
//#define FONT_FILE_MESSAGE "message.bmf"
#define FONT_FILE_DEFAULT_ALL "textall.bmf"
#define FONT_FILE_BOLD_ALL "textallb.bmf"
//#define FONT_FILE_KEY "key.bmf"


#ifdef WIKIPCF
#define openfile open
#define seekfile lseek
#define read_file read
#define closefile close
#else
#define openfile wl_open
#define seekfile wl_seek
#define readfile wl_read
#define closefile wl_close
#endif

typedef unsigned char bmf_bm_t;
typedef unsigned int  ucs4_t;

#ifdef WORD64
typedef long INT64;
typedef unsigned long CARD64;
#define B32 :32
#define B16 :16
#ifdef UNSIGNEDBITFIELDS
typedef unsigned int INT32;
typedef unsigned int INT16;
#else
#ifdef __STDC__
typedef signed int INT32;
typedef signed int INT16;
#else
typedef int INT32;
typedef int INT16;
#endif
#endif
#else
#define B32
#define B16
#ifdef LONG64
typedef long INT64;
typedef int INT32;
#else
typedef long INT32;
#endif
typedef short INT16;
#endif

#if defined(__STDC__) || defined(sgi) || defined(AIXV3)
typedef signed char    INT8;
#else
typedef char           INT8;
#endif

#ifdef LONG64
typedef unsigned long CARD64;
typedef unsigned int CARD32;
#else
typedef unsigned long CARD32;
#endif
typedef unsigned short CARD16;
typedef unsigned char  CARD8;

typedef CARD32          BITS32;
typedef CARD16          BITS16;

#ifndef I_NEED_OS2_H
typedef CARD8           BYTE;
typedef CARD8           BOOL;
#else
#define BYTE    CARD8
#define BOOL    CARD8
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
       INT8   width;
       INT8   height;
       INT8   widthBytes;
       INT8   widthBits;
       INT8   ascent;
       INT8   descent;
       INT8   LSBearing;
       INT8   RSBearing;
       char   bitmap[48];
}charmetric_bmf;

typedef struct charmetric_bmf_header{
       INT8   width;
       INT8   height;
       INT8   widthBytes;
       INT8   widthBits;
       INT8   ascent;
       INT8   descent;
       INT8   LSBearing;
       INT8   RSBearing;
       INT32  pos;
}charmetric_bmf_header;

typedef struct font_bmf_header{
       INT8   linespace;
       INT8   ascent;
       INT8   descent;
       INT8   bmp_buffer_len;
}font_bmf_header;


typedef struct fontmetric pcf_fontmet_t;


struct pcffont_bmf {
    	char *file;
        int fd;
        pcf_fontmet_t Fmetrics;
        int bPartialFont;
        struct pcffont_bmf *supplement_font;
        char *charmetric;
        int file_size;
        int bmp_buffer_len;
};

typedef struct pcffont_bmf pcffont_bmf_t;

int load_bmf(pcffont_bmf_t *font);
int pres_bmfbm(ucs4_t val, pcffont_bmf_t *font, bmf_bm_t **bitmap,charmetric_bmf *Cmetrics);
unsigned long * Xalloc(int);
#endif /* _PCF_H */
