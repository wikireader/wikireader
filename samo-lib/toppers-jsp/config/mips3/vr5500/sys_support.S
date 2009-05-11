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
#include "offset.h"		/* INT_TABLE_intmaskのために必要 */

	.set noreorder
	.align 2

/*
 *  ターゲットハードウェア依存する初期化処理
 */
#ifndef GDB_STUB

	.section .reset
	.global	hardware_init_hook

hardware_init_hook:

	/*
	 * キャッシュの初期化
	 */
	mtc0	zero, TagLo
	mtc0	zero, TagHi

	li	t0, 0x80003fe0
	li	t1, 0x80000000
loop_I_cache_clear:
	cache	Index_Store_Tag_I, +0(t0)
	cache	Index_Store_Tag_I, +1(t0)
	bne	t0, t1, loop_I_cache_clear
	addiu	t0, t0, -I_CACHE_LINE_SIZE

	li	t0, 0x80003fe0
	li	t1, 0x80000000
loop_D_cache_clear:
	cache	Index_Store_Tag_D, +0(t0)
	cache	Index_Store_Tag_D, +1(t0)
	bne	t0, t1, loop_D_cache_clear
	addiu	t0, t0, -D_CACHE_LINE_SIZE

	/*
	 * TLBの初期化
	 */
	mtc0	zero, PageMask
	mtc0	zero, EntryLo0
	mtc0	zero, EntryLo1

	li	t0, TMAX_TLB
	li	t1, 0xA8000000		/* kseg1 の中のDRAMの所を使用 */
loop_TLB_clear:
	mtc0	t0, Index
	mtc0	t1, EntryHi
	addiu	t0, t0, -1
	addiu	t1, t1, TLB_VPN2	/* 1ページが4kバイトで、2ページ分を
					   マッピングするため */
	tlbwi
	bgez	t0, loop_TLB_clear
	nop

	/*
	 * ボードの初期化
	 */

	/* SDRAM コントローラの初期化 */
	li	t0, 0x00
	sb	t0, 0xb8006000		/* SRAMC_SWAIT */

	li	t0, 0x02
	sb	t0, 0xb8005000		/* DRAMC_RCD */
	li	t0, 0x02
	sb	t0, 0xb8005010		/* DRAMC_RP */
	li	t0, 0x05
	sb	t0, 0xb8005020		/* DRAMC_RC */
	li	t0, 0x06
	sb	t0, 0xb8005030		/* DRAMC_RRC */
	li	t0, 0x04
	sb	t0, 0xb8005040		/* DRAMC_RAS */
	li	t0, 0x02
	sb	t0, 0xb8005050		/* DRAMC_LAT */
	li	t0, 0x02
	sb	t0, 0xb8005060		/* DRAMC_RSC */
	li	t0, 0x01
	sb	t0, 0xb8005070		/* DRAMC_AP */

	li	t0, 0x00
	sb	t0, 0xb8004000		/* DRAM_INIT */

	/* DRAMリフレッシュ用タイマの初期化 */
	li	t0, 0xb4
	sb	t0, 0xb9005030		/* PCNTL */
	li	t0, 0x1f
	sb	t0, 0xb9005020		/* PCNT2 */
	li	t0, 0x00
	sb	t0, 0xb9005020		/* PCNT2 */

	/* SRAMの初期化 */

	/* SRAMの全領域に対して、一度書き込みを行う処理が必要 */
	la	t0, 0xa0000000
	li	t1, 0x00100000-4
loop_sram_clear:
	add	t2, t1, t0
	sw	zero, (t2)
	bgez	t1, loop_sram_clear
	addiu	t1, t1, -4

	/* 割り込みコントローラの設定 */
	sb	zero, 0xb9000000	/* PIC_INT0M；全割込みのマスク */
	sb	zero, 0xb9000010	/* PIC_INT1M；全割込みのマスク */
	sb	zero, 0xb9000020	/* PIC_INTR ；割込み要求のクリア */
	li	t0, 0x01		/* 割込み線は片方のみ使う */
	sb	t0, 0xb9000030		/* PIC_INTEN */

	/*----------*/

	j	ra		/*  呼び出し元へリターン  */
	nop

#endif /* GDB_STUB */

/*============================================================================*/

/* 割込み処理に関する、割込み要因の判断分岐処理のシステム依存部 */

	.align 2
	.section .text
	.global proc_interrupt_sys
	.global int_table	/* 割込みハンドラの擬似ベクタテーブル
				   cpu_config.h, cpu_config.cで定義 */

	/*  呼び出されたとき、       */
	/*    a1にステータスレジスタ */
	/*    a2に原因レジスタ       */
	/*  の値が入っている         */

proc_interrupt_sys:
	and     t2, a2, a1          /*  割込み要求ビットにマスクをかける  */
	andi    t3, t2, Cause_IP0   /*  IP0ビット取り出し  */
	bne     t3, zero, proc_IP0
	andi    t4, t2, Cause_IP1   /*  IP1ビット取り出し  */
	bne     t4, zero, proc_IP1
	andi    t5, t2, Cause_IP2   /*  IP2ビット取り出し  */
	bne     t5, zero, proc_IP2
	andi    t6, t2, Cause_IP3   /*  IP3ビット取り出し  */
	bne     t6, zero, proc_IP3
	andi    t7, t2, Cause_IP4   /*  IP4ビット取り出し  */
	bne     t7, zero, proc_IP4
	andi    t8, t2, Cause_IP5   /*  IP5ビット取り出し  */
	bne     t8, zero, proc_IP5
	andi    t9, t2, Cause_IP6   /*  IP6ビット取り出し  */
	bne     t9, zero, proc_IP6
	nop

	/* なんらかの原因で分岐できない場合 */
	j       join_interrupt_and_exception
	nop

/*  MIPS3コアレベルで分岐したレベルでの処理	*/
/*    割込み要因番号をt0に入れて		*/
/*    割込み要求クリアのための定数をt1に入れて	*/
/*    set_ICU_IPMへ飛ぶ				*/
proc_IP7:   /*  割込み要因IP7（タイマ）の場合  */
	xori    t1, zero, Cause_IP7
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP7

proc_IP0:   /*  割込み要因IP0（ソフトウェア割込み0）の場合  */
	xori    t1, zero, Cause_IP0
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP0

proc_IP1:   /*  割込み要因IP1（ソフトウェア割込み1）の場合  */
	xori    t1, zero, Cause_IP1
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP1

proc_IP2:   /*  割込み要因IP2の場合  */ 
            /*    割込みコントローラ依存の定義があれば、そちらを実行する  */
#ifdef PROC_INT0
	PROC_INT0		/* システム依存部で定義するマクロ */
	xori    t1, zero, Cause_IP2
	j       set_ICU_IPM
	nop
#else /* PROC_INT0 */
	xori    t1, zero, Cause_IP2
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP2
#endif /* PROC_INT0 */

proc_IP3:   /*  割込み要因IP3の場合  */ 
            /*    割込みコントローラ依存の定義があれば、そちらを実行する  */
#ifdef PROC_INT1
	PROC_INT1		/* システム依存部で定義するマクロ */
	xori    t1, zero, Cause_IP3
	j       set_ICU_IPM
	nop
#else /* PROC_INT1 */
	xori    t1, zero, Cause_IP3
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP3
#endif /* PROC_INT1 */

proc_IP4:   /*  割込み要因IP4の場合  */ 
            /*    割込みコントローラ依存の定義があれば、そちらを実行する  */
#ifdef PROC_INT2
	PROC_INT2		/* システム依存部で定義するマクロ */
	xori    t1, zero, Cause_IP4
	j       set_ICU_IPM
	nop
#else /* PROC_INT2 */
	xori    t1, zero, Cause_IP4
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP4
#endif /* PROC_INT2 */

proc_IP5:   /*  割込み要因IP5の場合  */ 
            /*    割込みコントローラ依存の定義があれば、そちらを実行する  */
#ifdef PROC_INT3
	PROC_INT3		/* システム依存部で定義するマクロ */
	xori    t1, zero, Cause_IP5
	j       set_ICU_IPM
	nop
#else /* PROC_INT3 */
	xori    t1, zero, Cause_IP5
	j       set_ICU_IPM
	ori     t0, zero, INTNO_IP5
#endif /* PROC_INT3 */

proc_IP6:   /*  割込み要因IP6の場合  */ 
            /*    割込みコントローラ依存の定義があれば、そちらを実行する  */
#ifdef PROC_INT4
	PROC_INT4		/* システム依存部で定義するマクロ */
	xori    t1, zero, Cause_IP6
#else /* PROC_INT4 */
	xori    t1, zero, Cause_IP6
	ori     t0, zero, INTNO_IP6
#endif /* PROC_INT4 */

/*  割込みコントローラ依存のマスク設定  */
set_ICU_IPM:

#ifdef SET_ICU_IPM
	SET_ICU_IPM	/* 割込みマスクを設定するマクロ			    */
			/*   実装を行うときには、下記にてt0、t1は利用するの */
			/*   で破壊しないように、注意しなければならない。   */
#endif /* SET_ICU_IPM */

/*
 *  原因レジスタIPビットに保持されている各種割込みの割込み要求をクリアする。
 *  t1には、割込み要求ビットを反転したものが入っている。
 */
	mfc0    t8, Cause
	and     t8, t8, t1
	mtc0    t8, Cause

/*  ステータスレジスタのマスク設定とC言語ルーチン呼び出し  */
/*    t0に割込み要因番号が設定された状態でここに来る  */
	la      t3, int_table   /*  擬似ベクタアドレス  */
	sll     t4, t0, 3       /*  割込み要因番号を8倍
				      TNT_TABLE型は、
					ハンドラのアドレス(4バイト)
					＋MIPS3コアの割込みマスク(4バイト)
				      の、合計8バイト。 */
	add     t5, t3, t4      /*  ベクタアドレスを算出  */
	lw      t6, INT_TABLE_intmask(t5)
           		    	/*  IPM(割込み許可ビット)読み出し。
				    割込みマスク以外の値は、
					IEビットはセット
					EXLビットはリセット
				    状態になっている。*/
	lw      t7, (t5)        /*  C言語ルーチン先頭アドレス読み出し  */

	jalr    ra, t7          /*  C言語ルーチン呼び出し  */
	mtc0    t6, Status      /*  割込み許可；ステータスレジスタのマスク設定*/

	mfc0    t0, Status
	ori     t0, t0, SR_EXL  /*  割込み禁止；IEビットの値は保持しなければなら
						ないのでEXLビットを用いる。*/
	mtc0    t0, Status

	/*  CP0ハザードのための時間稼ぎ  */
#ifdef NOP_FOR_CP0_HAZARD
	NOP_FOR_CP0_HAZARD
#endif /* NOP_FOR_CP0_HAZARD */

	j	join_interrupt_and_exception
	nop

