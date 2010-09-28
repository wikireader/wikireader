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
#ifndef _GENERAL_HEADER_H
#define _GENERAL_HEADER_H

#include <sys/types.h>
/* #include <nl_types.h> */
#define WARN_SET        1
#define ERR_SET         2
#define USAGE_SET       3

#ifndef MAXPATHLEN
#define MAXPATHLEN	1024
#endif

#define MAXLINE		4096
#define MAX_ENV_STRING		1024

#define MAXLEN 4096	/* Maximum input line length */

#define PTSZ_SCALE      10
#define DEF_PTSZ        10
#define TP1_STYLE	1
#define TTF_STYLE	2
#define PCF_STYLE	3
#define DEFAULT_STYLE   TP1_STYLE

#define ROMNFONT                0
#define BOLDFONT                1
#define ITALFONT                2
#define BDITFONT                3
#define HDNGFONT                4

#define DICTSIZE        256
#define DICTTYPES       4

#define FEND                0
#define NRML                1

#define IS_PCF(x)  (print_info_st[x].font_style==PCF_STYLE)
#define IS_TTF(x)   (print_info_st[x].font_style==TTF_STYLE)
#define IS_TP1(x) (print_info_st[x].font_style==TP1_STYLE)

#define SPACE_WIDTH_PER_PTSZ	0.5866667

#define TAB_STOPS       8
#define ends_with_NL(s)         (s[strlen(s)-1] == '\n')
#define zapNL(s)                (s[strlen(s)-1] = '\0')
#define CFFS_STR                " \t"
#define CMNT_CHAR               '#'
#define is_comment_char(c)      (c == CMNT_CHAR)
#define next_tok() strtok(NULL, CFFS_STR)
#define next_string_tok() strtok(NULL, "\"")
#define is_tab(c)       ((c)=='\t')
#define is_newline(c)   ((c)=='\n')
#define is_backspace(c) ((c)=='\010')
#define is_formfeed(c)  ((c)=='\014')
#define non_graphic_char(u)     ((u<=0x1f)||(u>=0x7f&&u<=0x9f))
#define is_motion_char(u)       (is_tab(u)||is_newline(u)||		\
				 is_formfeed(u)||is_backspace(u))
#define eq(s1,s2)       (strcmp((s1),(s2)) == 0)
#define eqn(s1,s2,n)    (strncmp((s1),(s2),n) == 0)

typedef unsigned int ucs4_t;
#ifndef _XLIB_H_
typedef int Bool;
#endif
/*extern nl_catd  cat_fd ;
//extern char *progname;
//extern int current_pt_sz;
//extern int set_pt_sz;
//extern int mp_newdict_flg; */
/*
  extern int     printstyle;
*/
/*extern int pcf_ret, ttf_ret, tp1_ret;
//extern double SPACINGwidth;
//extern ucs4_t SPACINGchar;
//extern ucs4_t REFERENCEchar;
//extern char *target_printer ;
//extern int gen_incremental_pos(ucs4_t , double *, double *);
//extern int presform_fontndx(ucs4_t);
//extern int pres_pcfbm();
//extern void err_exit(char *, ...);
//extern void malloc_err_disp_exit(int , char *);

//extern void set_xpr_headings(char *, unsigned char *, int);
//extern void xpr_print(unsigned char *, int, int);
//extern void xpr_end(); */
#endif /* _GENERAL_HEADER_H */
