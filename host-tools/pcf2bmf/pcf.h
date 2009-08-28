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

#ifndef _PCF_H
#define _PCF_H
/*#include "Xproto.h" */
#include "general_header.h"
#define XU_IGNORE               -2
#define XU_UCS4UNDEF            -4
#define XU_MOTION_CHAR          1
#define XU_PSBM_CACHED          2
#define XU_PSBM_NOTCACHED       3
#define NOBITMAPREPL		0xfffd	/* specific to cu12.pcf.gz "-mutt-clearlyu-medium-r-normal--17-120-100-100-p-123-iso10646-1" */
#define FALLBACK_FONT	"cu12.pcf"
#define PPI             (72)            /* Postscript Points Per Inch */
#define DEF_XRES        PPI     /* default X resolution (72 dpi) */
#define DEF_YRES        PPI     /* default Y resolution (72 dpi) */
#define START_NEW_PAGE  -200.0

typedef unsigned char pcf_bm_t;

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

typedef struct {
    INT16 leftSideBearing B16,
          rightSideBearing B16,
          characterWidth B16,
          ascent B16,
          descent B16;
    CARD16 attributes B16;
} xCharInfo;

typedef struct _CharInfo {
	xCharInfo   metrics;        /* info preformatted for Queries */
	char       *bits;           /* pointer to glyph image */
}CharInfoRec;

struct scaled_fontmetric {
	double ascent;
	double descent;
	double linespace;
};

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
struct font_header {
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

typedef struct fontmetric pcf_fontmet_t;
typedef struct charmetric pcf_charmet_t;
typedef struct scaled_fontmetric pcf_SCfontmet_t;
typedef struct scaled_charmetric pcf_SCcharmet_t;
typedef struct _CharInfo *CharInfoPtr;
typedef struct font_header pcf_font_header;
struct pcffont{
    	char *name;
    	char *file;
	char *cufsym;
	char *cufobj;
	int (*cuf)(ucs4_t);
	int loaded;
	double Xscale;
	double Yscale;
	pcf_fontmet_t Fmetrics;
	pcf_SCfontmet_t scFmetrics;
	CharInfoPtr *bitmaps;       /* ...array of CharInfoPtr */
};
typedef struct pcffont pcffont_t;

       
/*extern void scaling_factors(pcffont_t *, double , int , int );*/

typedef enum {
    Linear8Bit, TwoD8Bit, Linear16Bit, TwoD16Bit
}           FontEncoding;

int load_pcf(pcffont_t *font);
pcf_SCcharmet_t *get_SCmetrics(pcffont_t *font, ucs4_t val);
int pres_pcfbm(ucs4_t *, pcffont_t *, pcf_bm_t **, pcf_charmet_t *, pcf_SCcharmet_t *, int);
void put_PSbitmap(ucs4_t code, pcf_bm_t *bitmap, pcf_charmet_t *Cmetrics, pcf_SCcharmet_t *Smetrics);
void draw_bitmap(int line_ascent, int line_descent, int line_space, pcf_bm_t *bitmap, pcf_charmet_t *Cmetrics);
#endif /* _PCF_H */
