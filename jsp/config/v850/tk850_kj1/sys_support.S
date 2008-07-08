/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005 by Freelines CO.,Ltd 
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
 *  @(#) $Id: sys_support.S,v 1.4 2007/01/05 02:10:17 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール アセンブリ言語部（TK-850ES/KJ1+用）
 */

#define	_MACRO_ONLY
#include "jsp_kernel.h"
#include "v850asm.inc"

#define Set(x,y,z) Lea x, r6; Lea y, r7; st.##z r7, 0[r6]

/*
 *  低レベルのターゲットシステム依存の初期化
 *
 *  スタートアップモジュールの中で，メモリの初期化の前に呼び出される．
 */

	.text
	.globl hardware_init_hook
hardware_init_hook:

	/* システムウェイトコントロールレジスタ：5MHz×4=20MHz時 */
	Set(VSWC,  0x01,   b)
	/* 保護された特定レジスタへのアクセス
	   プロセッサクロックコントロールレジスタ：メインクロックのfxx出力(PCC=0) */
	lea	PRCMD, r6
	lea	PCC, r7
	st.b	r0, 0[r6]
	st.b	r0, 0[r7]
	nop
	nop
	nop
	nop
	nop
	Set(PLLCTL,0x03,   h)   /* PLLコントロールレジスタ：PLL動作 */
	Set(WDTM2, 0x07,   b)   /* WDTM2 stop */
#if 0
	/* 外部バスの設定：
	     CS0に外部SRAMを増設した場合の参考。ここではバスはセパレートバスで、
	     12ns品のSRAMで0waitで動作することを仮定した設定。
	 */
	Set(EXIMC, 0x01,   b)	/* 外部バスインターフェイスモードレジスタ：セパレートバス */
	Set(PMC9,  0xFFFF, h)	/* P9モードコントロールレジスタ：A0-A15出力 */
	Set(PMCCS, 0x01,   b)	/* PCSモードコントロールレジスタ：C0出力 */
	Set(PMCCT, 0x53,   b)	/* PCTモードコントロールレジスタ：RD/WR/ASTB出力 */
	Set(PMCDH, 0xFF,   b)	/* PDHモードコントロールレジスタ：A16-A23出力 */
	Set(PMCDL, 0xFFFF, h)	/* PDLモードコントロールレジスタ：AD0-AD15出力 */
	Set(DWC0,  0x7770, h)	/* データウェイト制御レジスタ：CS0wait=0 */
	Set(AWC,   0xFFFC, h)	/* アドレスウェイト制御レジスタ：CS0wait=0 */
	Set(BCC,   0xAAA8, h)	/* バスサイクル制御レジスタ：CS0wait=0 */
#endif

	jmp		r31



	/* 標準割込み処理とスぺーサー */
	.macro StandardInterruptEntry name
	.globl \name
\name:
	jr	vector_handler
	nop;nop;nop;nop;nop;nop
	.endm

	.macro ReservedInterruptEntry
	nop;nop;nop;nop;nop;nop;nop;nop
	.endm

/*
 * 割込みハンドラ部分の定義
 * 		とりあえず一箇所に集める。
 */

	.section	".vectors","ax"
	.align      4
	.globl      __reset
__reset:
	jr	start
	nop;nop;nop;nop;nop;nop

	StandardInterruptEntry NMI
	StandardInterruptEntry INTWDT1
	StandardInterruptEntry INTWDT2
	StandardInterruptEntry TRAP0
	StandardInterruptEntry TRAP1
	StandardInterruptEntry DBG
/*#-------------------------------------------------------------------
  #	SECURITY_ID
  #-------------------------------------------------------------------*/
/*	.section	"SECURITY_ID" */
	.word	0xffffffff	/* --0-3 byte code,Address is 0x70-0x73 */
	.word	0xffffffff	/* --4-7 byte code,Address is 0x74-0x77 */
	.hword	0xffff		/* --8-9 byte code,Address is 0x78-0x79 */
/*#-------------------------------------------------------------------
  #	OPTION_BYTES
  #-------------------------------------------------------------------*/
/*	.section	"OPTION_BYTES" */
	.hword	0x0000		/* --0-1 byte code,Address is 0x7a-0x7b */
	.hword	0x0000		/* --2-3 byte code,Address is 0x7c-0x7d */
	.hword	0x0000		/* --4-5 byte code,Address is 0x7e-0x7f */

	StandardInterruptEntry INTWDTM1
	StandardInterruptEntry INTP0
	StandardInterruptEntry INTP1
	StandardInterruptEntry INTP2
	StandardInterruptEntry INTP3
	StandardInterruptEntry INTP4
	StandardInterruptEntry INTP5
	StandardInterruptEntry INTP6
	StandardInterruptEntry INTTM000
	StandardInterruptEntry INTTM001
	StandardInterruptEntry INTTM010
	StandardInterruptEntry INTTM011
	StandardInterruptEntry INTTM50
	StandardInterruptEntry INTTM51
	StandardInterruptEntry INTCSI00
	StandardInterruptEntry INTCSI01
	StandardInterruptEntry INTSRE0
	StandardInterruptEntry INTSR0
	StandardInterruptEntry INTST0
	StandardInterruptEntry INTSRE1
	StandardInterruptEntry INTSR1
	StandardInterruptEntry INTST1
	StandardInterruptEntry INTTMH0
	StandardInterruptEntry INTTMH1
	StandardInterruptEntry INTCSIA0
	StandardInterruptEntry INTIIC0
	StandardInterruptEntry INTAD
	StandardInterruptEntry INTKR
	StandardInterruptEntry INTWTI
	StandardInterruptEntry INTWT
	StandardInterruptEntry INTBRG
	StandardInterruptEntry INTTM020
	StandardInterruptEntry INTTM021
	StandardInterruptEntry INTTM030
	StandardInterruptEntry INTTM031
	StandardInterruptEntry INTCSIA1
	StandardInterruptEntry INTTM040
	StandardInterruptEntry INTTM041
	StandardInterruptEntry INTTM050
	StandardInterruptEntry INTTM051
	StandardInterruptEntry INTCSI02
	StandardInterruptEntry INTSRE2
	StandardInterruptEntry INTSR2
	StandardInterruptEntry INTST2
	StandardInterruptEntry INTIIC1
	ReservedInterruptEntry 
	ReservedInterruptEntry 
	ReservedInterruptEntry 
	ReservedInterruptEntry 
	ReservedInterruptEntry 
	StandardInterruptEntry INTTP0OV
	StandardInterruptEntry INTTP0CC0
	StandardInterruptEntry INTTP0CC1

/*
 * 評価用高性能?タイマ
 *  (今回は評価用フリーランニングタイマを作らずに直接タイマカウンタをのぞく)
 */
Function _vxget_tim
	Lea	TM00, r7
	ld.h	0[r7], r8
	st.w	r8, 0[r6]
	mov	r0, r10
	jmp	r31
