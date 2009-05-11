/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#define _MACRO_ONLY

#include "jsp_kernel.h"

	.set noreorder
	.align 2

/*
 *  ターゲットハードウェア依存する初期化処理 (reset.S から呼び出される)
 */
#ifndef GDB_STUB

	.section .reset
	.global	hardware_init_hook

hardware_init_hook:

	/*
	 *  バスコントロールユニット(BCU)関係の初期設定
	 */
	li	t0, ASM_SIL( ROMSIZEREG )
	lh	t1, (t0)
	ori	t1, t1, (SIZE3_4 | SIZE2_4 | SIZE1_4 | SIZE0_4)
	sh	t1, (t0)

	li	t0, ASM_SIL( ROMSPEEDREG )
	lh	t1, (t0)
	ori	t1, t1, (ROM4_WAIT_5VTClock | ROM2_WAIT_8VTClock)
	sh	t1, (t0)

	li	t0, ASM_SIL( BCUCNTREG3 )
	lh	t1, (t0)
	ori	t1, t1, (EXT_ROMCS_3ROM_2ROM | IO32 | LCDSEL1_BUFFER | LCDSEL0_BUFFER)
	sh	t1, (t0)

	/*
	 * SDRAMコントロールユニット(SDRAMU)関係の初期設定
	 */

	li	t0, ASM_SIL( SDRAMMODEREG )
	lh	t1, (t0)
	ori	t1, t1, (SCLK | LTMODE_2)
	sh	t1, (t0)

	li	t0, ASM_SIL( SDRAMCNTREG )
	lh	t1, (t0)
	ori	t1, t1, (TRC_3VTClock | TDAL_2VTClock | TRCD_2VTClock)
	sh	t1, (t0)

	li	t0, ASM_SIL( BCURFCNTREG )
	lh	t1, (t0)
	ori	t1, t1, 0x3ec
	sh	t1, (t0)

	li	t0, ASM_SIL( BCURFCNTREG )
	lh	t1, (t0)
	ori	t1, t1, 0x3ec
	sh	t1, (t0)

	li	t0, ASM_SIL( RAMSIZEREG )
	lh	t1, (t0)
	ori	t1, t1, (SIZE3_64 | SIZE2_64 | SIZE1_64 | SIZE0_64)
	sh	t1, (t0)

	/*
	 *  バスコントロールユニット(BCU)関係の初期設定
	 */
	li	t0, ASM_SIL( IO0SPEEDREG )
	lh	t1, (t0)
	ori	t1, t1, IO0_1_WAIT_4
	sh	t1, (t0)

	li	t0, ASM_SIL( IO1SPEEDREG )
	lh	t1, (t0)
	ori	t1, t1, IO1_3_WAIT_11
	sh	t1, (t0)

	/*
	 *  HALTimerシャットダウンのクリア
	 *  CPU起動後、4秒以内に行わなければならない
	 *  PMUコントロールレジスタのHALTimerリセットビットに１を書き込む
	 */
	li	t0, ASM_SIL( PMUCNTREG )
	lh	t1, (t0)
	ori	t1, t1, HALTIMERRST
	sh	t1, (t0)

	/*
	 *  HALTimerリセットのクリア
	 *  PMU割込み／ステータスレジスタのTIMOUTRSTビットに１を書き込む
	 */
	li	t0, ASM_SIL( PMUINTREG )
	lh	t1, (t0)
	ori	t1, t1, TIMOUTRST
	sh	t1, (t0)

	/*
	 * TLBの初期化
	 */
tlb_initialize:
	mtc0	zero, PageMask
	mtc0	zero, EntryLo0
	mtc0	zero, EntryLo1

	li	t0, TMAX_TLB
	li	t1, 0xA0000000		/* kseg1 の中のDRAMの所を使用 */
loop_TLB_clear:
	mtc0	t0, Index
	mtc0	t1, EntryHi
	addiu	t0, t0, -1
	addiu	t1, t1, TLB_VPN2	/* 1ページが4kバイトで、2ページ分をマッピ
					   ングするため */
	tlbwi
	bgez	t0, loop_TLB_clear
	nop

	/*
	 * キャッシュの初期化
	 */
cache_initialize:
	mtc0	zero, TagLo
	mtc0	zero, TagHi

	li	t0, 0x80001ff0	/* 512 line 分 (1 line = 16 byte) */
	li	t1, 0x80000000
loop_I_cache_clear:
	cache	Index_Invalidate_I, 0x0000(t0)
	cache	Index_Invalidate_I, 0x2000(t0)
	bne	t0, t1, loop_I_cache_clear
	addiu	t0, t0, -I_CACHE_LINE_SIZE

	li	t0, 0x80001ff0	/* 512 line 分 (1 line = 16 byte) */
	li	t1, 0x80000000
loop_D_cache_clear:
	cache	Index_Store_Tag_D, 0x0000(t0)
	cache	Index_Store_Tag_D, 0x2000(t0)
	bne	t0, t1, loop_D_cache_clear
	addiu	t0, t0, -D_CACHE_LINE_SIZE

	/*
	 * 呼び出し元へリターン
	 */
return_to_common:
	j	ra
	nop

#endif /* GDB_STUB */
