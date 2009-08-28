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

#ifndef _PCF_PRIVATE_H
#define _PCF_PRIVATE_H

/* #include <X11/Xproto.h> */
#include "general_header.h"
#include "pcf.h"
#define GLYPHPADOPTIONS 4 

#define BUFFILESIZE     8192
#define BUFFILEEOF      -1

#ifndef LSBFirst
#define LSBFirst        0
#define MSBFirst        1
#endif

#ifndef TRUE
#define TRUE 	1
#endif
#ifndef FALSE
#define FALSE 	0
#endif

#ifndef DEBUG
#define debug(x)
#define Ndebug(x)
#else
#define debug(x)        x
#define Ndebug(x)
#endif

#define None 0l
#define BITS    16
#define Successful      85
#define AllocError      80
#define OPTARG_SET      1
#define OPTARG_ADD      2
#define OPTARG_SUB      3
#define NBPB    8

typedef unsigned char *pointer;
typedef struct _Font *FontPtr;
typedef struct _FontInfo *FontInfoPtr;
typedef unsigned long  fsBitmapFormat;	
typedef struct _FontProp *FontPropPtr;

typedef unsigned char BufChar;
typedef unsigned long Atom; 

#if BITS > 15
typedef long int        code_int;
#else
typedef int             code_int;
#endif

#ifdef NO_UCHAR
 typedef char   char_type;
#else
 typedef        unsigned char   char_type;
#endif /* UCHAR */
char_type magic_header[] = { "\037\235" };      /* 1F 9D */
#define BIT_MASK        0x1f
#define BLOCK_MASK      0x80
#define INIT_BITS 9                     /* initial number of bits/code */
#define FIRST   257     /* first free entry */
#define CLEAR   256     /* table clear output code */

#define CUF_ILCH -1
#define CUF_NICH -2

#ifndef UCS4_MAXVAL
#define UCS4_MAXVAL 0x0000ffff
#endif

#ifdef COMPATIBLE		/* But wrong! */
# define MAXCODE(n_bits)	(1 << (n_bits) - 1)
#else
# define MAXCODE(n_bits)	((1 << (n_bits)) - 1)
#endif /* COMPATIBLE */

#define dprintf2(f,a1,a2)               fprintf(stderr,f,a1,a2)
#define dprintf3(f,a1,a2,a3)            fprintf(stderr,f,a1,a2,a3)
#define dprintf6(f,a1,a2,a3,a4,a5,a6)   fprintf(stderr,f,a1,a2,a3,a4,a5,a6)

#define xalloc(n)   Xalloc ((unsigned) n)
#define xfree(p)    if ((p)) free(p)
#define xrealloc(p,n)   Xrealloc ((pointer)p,n)
#define lowbit(x) ((x) & (~(x) + 1))
#define FontFileRead(f,b,n) BufFileRead(f,b,n)
#define NO_SUCH_CHAR    -1
#define PCF_BIT_MASK            (1<<3)
#define PCF_SIZE_TO_INDEX(s)    ((s) == 4 ? 2 : (s) == 2 ? 1 : 0)
#define PCF_ACCEL_W_INKBOUNDS   0x00000100
#define PCF_BDF_ENCODINGS           (1<<5)
#define PCF_BYTE_MASK           (1<<2)
#define PCF_COMPRESSED_METRICS  0x00000100
#define PCF_DEFAULT_FORMAT      0x00000000
#define PCF_FORMAT_MASK         0xffffff00
#define PCF_FORMAT_MATCH(a,b) (((a)&PCF_FORMAT_MASK) == ((b)&PCF_FORMAT_MASK))
#define PCF_FILE_VERSION        (('p'<<24)|('c'<<16)|('f'<<8)|1)
#define PCF_BYTE_ORDER(f)       (((f) & PCF_BYTE_MASK)?MSBFirst:LSBFirst)
#define PCF_BIT_ORDER(f)        (((f) & PCF_BIT_MASK)?MSBFirst:LSBFirst)
#define PCF_PROPERTIES              (1<<0)
#define PCF_ACCELERATORS            (1<<1)
#define PCF_METRICS                 (1<<2)
#define PCF_BITMAPS                 (1<<3)
#define PCF_INK_METRICS             (1<<4)
#define PCF_BDF_ENCODINGS           (1<<5)
#define PCF_SWIDTHS                 (1<<6)
#define PCF_GLYPH_NAMES             (1<<7)
#define PCF_BDF_ACCELERATORS        (1<<8)
#define PCF_GLYPH_PAD_MASK      (3<<0)
#define PCF_GLYPH_PAD_INDEX(f)  ((f) & PCF_GLYPH_PAD_MASK)
#define PCF_GLYPH_PAD(f)        (1<<PCF_GLYPH_PAD_INDEX(f))
#define PCF_SCAN_UNIT_MASK      (3<<4)
#define PCF_SCAN_UNIT_INDEX(f)  (((f) & PCF_SCAN_UNIT_MASK) >> 4)
#define PCF_SCAN_UNIT(f)        (1<<PCF_SCAN_UNIT_INDEX(f))
#define BufFileSkip(f,c)    ((*(f)->skip) (f, c))
#define BufFileGet(f)   ((f)->left-- ? *(f)->bufp++ : (*(f)->io) (f))

#define GLYPHWIDTHPIXELS(pci) \
	((pci)->metrics.rightSideBearing - (pci)->metrics.leftSideBearing)

#define GLYPHHEIGHTPIXELS(pci) \
	((pci)->metrics.ascent + (pci)->metrics.descent)

#define GLYPHWIDTHBYTES(pci)    (((GLYPHWIDTHPIXELS(pci))+7) >> 3)

#define GLYPHWIDTHPADDED(bc)    (((bc)+7) & ~0x7)

typedef unsigned long ulong_t;

typedef struct _AtomList {
	char                *name;
	int                 len;
	int                 hash;
	Atom                atom;
} AtomListRec, *AtomListPtr;

typedef struct _PCFTable {
    CARD32      type;
    CARD32      format;
    CARD32      size;
    CARD32      offset;
}           PCFTableRec, *PCFTablePtr;


typedef struct _buffile {
	BufChar *bufp;
	int	    left;
	BufChar buffer[BUFFILESIZE];
	int	    (*io)(/* BufFilePtr f */);
	int	    (*skip)(/* BufFilePtr f, int count */);
	int	    (*close)(/* BufFilePtr f */);
	char    *_private;
} BufFileRec, *BufFilePtr;

typedef BufFilePtr  FontFilePtr;

#define FontFileGetc(f)     BufFileGet(f)
#define FontFileSkip(f,n)   (BufFileSkip (f, n) != BUFFILEEOF)

typedef struct _FontProp {
	long        name;
	long        value;          /* assumes ATOM is not larger than INT32 */
}           FontPropRec;

typedef struct _FontInfo {
    unsigned short firstCol;
    unsigned short lastCol;
    unsigned short firstRow;
    unsigned short lastRow;
    unsigned short defaultCh;
    unsigned int noOverlap:1;
    unsigned int terminalFont:1;
    unsigned int constantMetrics:1;
    unsigned int constantWidth:1;
    unsigned int inkInside:1;
    unsigned int inkMetrics:1;
    unsigned int allExist:1;
    unsigned int drawDirection:2;
    unsigned int cachable:1;
    unsigned int anamorphic:1;
    short       maxOverlap;
    short       pad;
    xCharInfo   maxbounds;
    xCharInfo   minbounds;
    xCharInfo   ink_maxbounds;
    xCharInfo   ink_minbounds;
    short       fontAscent;
    short       fontDescent;
    int         nprops;
    FontPropPtr props;
    char       *isStringProp;
}           FontInfoRec;

typedef struct _BitmapExtra {
    Atom       *glyphNames;
    int        *sWidths;
    CARD32      bitmapsSizes[GLYPHPADOPTIONS];
    FontInfoRec info;
}    BitmapExtraRec , *BitmapExtraPtr;

typedef struct _BitmapFont {
    unsigned    version_num;
    int         num_chars;
    int         num_tables;
    CharInfoPtr metrics;	/* font metrics, including glyph pointers */
    xCharInfo  *ink_metrics;	/* ink metrics */
    char       *bitmaps;	/* base of bitmaps, useful only to free */
    CharInfoPtr *encoding;	/* array of char info pointers */
    CharInfoPtr pDefault;	/* default character */
    BitmapExtraPtr bitmapExtra;	/* stuff not used by X server */
}           BitmapFontRec, *BitmapFontPtr;

typedef struct _FontPathElement {
    int         name_length;
    char       *name;
    int         type;
    int         refcount;
    pointer     _private;
}           FontPathElementRec;

typedef struct _FontPathElement *FontPathElementPtr;

typedef struct _Font {
    int         refcnt;
    FontInfoRec info;
    char        bit;
    char        byte;
    char        glyph;
    char        scan;
    fsBitmapFormat format;
    int         (*get_glyphs) ( /* font, count, chars, encoding, count, glyphs */ );
    int         (*get_metrics) ( /* font, count, chars, encoding, count, glyphs */ );
    void        (*unload_font) ( /* font */ );
    FontPathElementPtr fpe;
    pointer     svrPrivate;
    pointer     fontPrivate;
    pointer     fpePrivate;
    int		maxPrivate;
    pointer	*devPrivates;
}           FontRec;

#define STACK_SIZE  8192

typedef struct _compressedFILE {
    BufFilePtr	    file;

    char_type	    *stackp;
    code_int	    oldcode;
    char_type	    finchar;

    int		block_compress;
    int		maxbits;
    code_int	maxcode, maxmaxcode;

    code_int	free_ent;
    int		clear_flg;
    int		n_bits;

    /* bit buffer */
    int		offset, size;
    char_type	buf[BITS];

    char_type	    de_stack[STACK_SIZE];
    char_type	    *tab_suffix;
    unsigned short  *tab_prefix;
} CompressedFile;


#endif /*_PCF_PRIVATE_H */
