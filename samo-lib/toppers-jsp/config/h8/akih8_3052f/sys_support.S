/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: sys_support.S,v 1.11 2007/03/23 07:22:15 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール アセンブリ言語部
 */

#define _MACRO_ONLY
#include "jsp_kernel.h"

/*
 *  システム依存の初期化ルーチン
 */
	.h8300h
	.text
	.align 2

	.global	_hardware_init_hook
_hardware_init_hook:
	/*
	 *  リリースでは、外部DRAMを有効にする。
	 */
#ifdef ROM
	mov.l	#H8P1DDR, er0
	mov.b	#0xff, r1l
	mov.b	r1l, @er0	/* enable A0  - A7  */

	add.b	#(H8P2DDR - H8P1DDR), r0l
	mov.b	r1l, @er0	/* enable A8  - A15 */

	add.b	#(H8P3DDR - H8P2DDR), r0l
	mov.b	r1l, @er0	/* enable D8  - D15 */

	add.b	#(H8P5DDR - H8P3DDR), r0l
	mov.b	r1l, @er0	/* enable A16 - A19 */

#ifdef ENABLE_LOWER_DATA

	mov.l	#H8P4DDR, er0
	mov.b	r1l, @er0	/* enable D0  - D7  */

#endif	/* #if ENABLE_LOWER_DATA */

#ifdef ENABLE_P8_CS

	mov.l	#H8P8DDR, er0
	mov.b	#ENABLE_P8_CS, r1l
	mov.b	r1l, @er0	/* enable CS0 - CS3 */

#endif	/* #ifdef ENABLE_P8_CS */

#ifdef ENABLE_PA_CS

	mov.l	#H8PADDR, er0
	mov.b	#ENABLE_PA_CS, r1l
	mov.b	r1l, @er0	/* enable CS4 - CS6 */

#elif defined(ENABLE_PA_A21_A23)

	mov.l	#H8PADDR, er0
	mov.b	#ENABLE_PA_A21_A23, r1l
	mov.b	r1l, @er0	/* enable A21 - A23 */

#endif	/* #ifdef ENABLE_PA_CS */

#ifdef ENABLE_PB_CS

	mov.l	#H8PBDDR, er0
	mov.b	#ENABLE_PB_CS, r1l
	mov.b	r1l, @er0	/* enable CS7 */

#endif	/* #ifdef ENABLE_PB_CS */

#endif	/*  #ifdef ROM  */

	rts
