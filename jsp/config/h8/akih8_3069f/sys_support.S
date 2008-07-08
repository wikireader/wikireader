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
 *  Copyright (C) 2001-2004 by Kunihiko Ohnaka
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
 *  @(#) $Id: sys_support.S,v 1.13 2007/03/23 07:22:15 honda Exp $
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
	 *　　（外部DRAMを使用する場合）
	 */
#ifdef ROM
	mov.b	#0xff, r1l
	mov.l	#H8P1DDR, er0
	mov.b	r1l, @er0	/* enable A0  - A7  */

	add.b	#(H8P2DDR - H8P1DDR), r0l
	mov.b	r1l, @er0	/* enable A8  - A15 */

	add.b	#(H8P3DDR - H8P2DDR), r0l
	mov.b	r1l, @er0	/* enable D8  - D15 */

	mov.b	#H8P5DDR_A16, r0h
	mov.b	r0h, @H8P5DDR	/*  Enable A16		*/
#endif	/*  #ifdef ROM  */

	/*
	 *  Ethernetデバイスを使用する場合、ここでCS1を設定しないと
	 *　デバイスが誤動作する。
	 */
#if defined(ROM) || defined(INMEM_ONLY)
	mov.b	#H8P8DDR0, r0h
	mov.b	r0h, @H8P8DDR	/*  Enable CS1/CS2	*/
#endif /* defined(ROM) || defined(INMEM_ONLY) */


#ifdef ROM
	mov.b	#H8PBDDR_UCAS, r0h
	mov.b	r0h, @H8PBDDR	/*  Enable UCAS		*/

		/*  DRAM: 2,048/32ms: 32ms / 2,048 = 15.625us	*/
		/*  20MHz / 8 / 39 = 1 / 15.6us			*/
	mov.b	#((32*(CPU_CLOCK/1000))/(8*2048)), r0h
	mov.b	r0h, @H8RTCOR

	mov.b	#H8RTMCSR_CKS1, r0h
	mov.b	r0h, @H8RTMCSR	/*  clock / 8			*/

	mov.b	#(H8DRCRB_RCYCE | H8DRCRB_MXC1), r0h
	mov.b	r0h, @H8DRCRB	/*  column address: 10 bit, A23 - A10	*/

	/*
	 *　ロットにより使用されている外部RAMが異なる
	 *　　・日立製 HM5117800
	 *　　・OKI製  M5117805D
	 *
	 *　両対応にするため、
	 *　　・BE ビット＝１
	 *　　・RDMビット＝０
	 *
	 *　詳細は(toppers-users 1375)を参照
	 *
	 */
	mov.b	#(H8DRCRA_BE | H8DRCRA_DRAS0), r0h
	mov.b	r0h, @H8DRCRA

	mov.l	#(CPU_CLOCK / 1000), er0    /*  Wait for DRAM stable	*/
_hw_init_hock_1:
	dec.l	#1, er0
	bne	_hw_init_hock_1

#endif	/*  #ifdef ROM  */

	rts

