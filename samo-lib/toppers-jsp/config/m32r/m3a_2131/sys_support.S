/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: sys_support.S,v 1.2 2007/05/30 08:38:51 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール アセンブリ言語部（M3A-2131用）
 */

#define	_MACRO_ONLY

#include <m32rasm.inc>
#include <s_services.h>

/*
 *  低レベルのターゲットシステム依存の初期化
 *
 *  スタートアップモジュールの中で，メモリの初期化の前に呼び出される．
 */
	.macro	out		addr, val
		ld24	r0, \addr
		seth	r1, #((\val) >> 16)
		or3		r1, r1, #((\val) & 0xffff)
		st		r1, @r0
	.endm

	.macro	outh	addr, val
		ld24	r0, #(\addr)
		ldi		r1, #(\val)
		sth		r1, @r0
	.endm

	.macro	outb	addr, val
		ld24	r0, #(\addr)
		ldi		r1, #(\val)
		stb		r1, @r0

	.endm

Function hardware_init_hook

	outb	PLLCR+3,   0x1			/* PLLクロック動作開始 */
	out		BSELCR(1), 0x0fffb50f	/* 外部バス，ブロック１を使用（SDRAM） */

	/* PIOの設定 */
	outh	PMOD(0),   0x0100	/* 外部バスBSEL1を使用 */
	outh	PMOD(1),   0x1		/* 外部バスRD/WR */
	outh	PMOD(2),   0x1		/* 外部バスWS3 */
	outh	PMOD(3),   0x5555	/* 外部バス */
	outh	PMOD(4),   0x5555	/* 外部バス */
	outh	PMOD(5),   0x0555	/* 内部周辺I/O */
	outh	PMOD(6),   0x0555	/* 内部周辺I/O */

	outh	SDIR0+2,   0x182

	/* SDRAM初期化 */
	ld24	r0, SDIR1+3
	ldi		r1, 1
	stb		r1, @r0
	addi	r0, -2
L1:	ldb		r1, @r0
	bnez	r1, L1					/* DINSTビットがクリアされると初期化終了 */

	outb	SDBR+3, 0x2
	outh	SDRF1+2, 0x0020
	outh	SDMOD(0)+2, 0
	out		SDADR(0), 0x01000042	/* 0x01000000, 32bit, bank 4, 16MB */
	outh	SDTR(0)+2,0x0001

	outb	SDRF1+1, 1				/* リフレッシュ有効 */
	outb	SDER(0)+3, 1			/* SDRAM有効 */
	outb	CPUCLKCR+1, 0x2			/* CPUクロックとしてPLLクロックを使用 */
	rts

Function software_init_hook
	rts

Function vxget_tim
	rts
