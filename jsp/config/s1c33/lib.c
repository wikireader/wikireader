/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
 *
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 *
 */
#include	"smcvals.h"
#include	"stdio.h"
#include	"cpu_rename.h"

FILE	_iob[FOPEN_MAX + 1];		/* standard i/o stream */

FILE	*stdin; 			/* standard input */
FILE	*stdout;			/* standard output */
FILE	*stderr;			/* standard error */

int 	errno;				/* error number */

unsigned int  seed; 			/* seed of random value */

unsigned char *ucNxtAlcP;	/* next memory allocate address */
unsigned char *ucBefAlcP;	/* one previous allocated address */
unsigned char *end_alloc;	/* end memory allocate address */

#ifndef DBL_LNG_TYPE

#define DBL_LNG_TYPE

/*  the architecture of double floating point
 *
 *   0 1          11                  31 32                               63 bit
 *   -----------------------------------------------------------------------
 *  | |   exponent  |                    fraction                           |
 *   -----------------------------------------------------------------------
 *
 *  |               |                   |                                   |
 *  |     12bits          20 bits       |             32 bits               |
 *  |            lower word             |            higher word            |
 *
 *         bit    0         sign bit         (  1 bit  )
 *              1 - 11      exponent part    ( 11 bits )
 *             12 - 63      fraction part    ( 52 bits )
 *
 */

typedef union {
		struct {
#ifdef	BIG_ENDIAN
			unsigned long _LL;
			unsigned long _LH;
#else
			unsigned long _LH;
			unsigned long _LL;
#endif
		} st;
	} DBL_LNG;

#endif

DBL_LNG _HUGE;				/* HUGE VALUE */

unsigned char *_STACK_TOP;		/* address of stack top */
unsigned char *_STACK_BOTTOM;		/* address of stack bottom */

long gm_sec;

/*
 * initialize routine
 */

void init_lib()
{

  /* initialize allocate address */
  end_alloc = (unsigned char *)_STACK_TOP;
  ucNxtAlcP = (unsigned char *)(_BOUND((unsigned long)_STACK_BOTTOM));
  ucBefAlcP = (unsigned char *)NULL;

  errno = 0;				/* clear error number */

  seed = 1; 				/* initialize random seed */

  gm_sec = -1;				/* initialize time */

  _HUGE.st._LL = 0x7ff00000;		/* HUGE VALUE */
  _HUGE.st._LH = 0;

/*
 *  INITIALIZE STANDARD STREAMS
 */

  _iob[0]._flg = _UGETN;		/* initialize stdin stream */
  _iob[0]._buf = 0;
  _iob[0]._fd = 0;

  _iob[1]._flg = _UGETN;		/* initialize stdout stream */
  _iob[1]._buf = 0;
  _iob[1]._fd = 1;

  _iob[2]._flg = _UGETN;		/* initialize stderr stream */
  _iob[2]._buf = 0;
  _iob[2]._fd = 2;

  stdin = &_iob[0];
  stdout = &_iob[1];
  stderr = &_iob[2];

}

