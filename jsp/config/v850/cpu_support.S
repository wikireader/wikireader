/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
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
 *  @(#) $Id: cpu_support.S,v 1.10 2007/01/05 02:10:17 honda Exp $
 */

/*
 *	プロセッサ依存モジュール アセンブリ言語部（V850ES用）
 */

#define	_MACRO_ONLY
#include "v850asm.inc"
#include "jsp_kernel.h"
#include "offset.h"

/*
 *  タスクディスパッチャ
 */
Function	__kernel_dispatch

	/*
	 * コンテキストの退避 
	 */
	Push	ep
	addi	-120, sp, sp
	mov	sp, ep
	sst.w	r1, 116[ep]
	sst.w	r2, 112[ep]
	sst.w	r3, 108[ep]
	sst.w	r4, 104[ep]
	sst.w	r5, 100[ep]
	sst.w	r6,  96[ep]
	sst.w	r7,  92[ep]
	sst.w	r8,  88[ep]
	sst.w	r9,  84[ep]
	sst.w	r10, 80[ep]
	sst.w	r11, 76[ep]
	sst.w	r12, 72[ep]
	sst.w	r13, 68[ep]
	sst.w	r14, 64[ep]
	sst.w	r15, 60[ep]
	sst.w	r16, 56[ep]
	sst.w	r17, 52[ep]
	sst.w	r18, 48[ep]
	sst.w	r19, 44[ep]
	sst.w	r20, 40[ep]
	sst.w	r21, 36[ep]
	sst.w	r22, 32[ep]
	sst.w	r23, 28[ep]
	sst.w	r24, 24[ep]
	sst.w	r25, 20[ep]
	sst.w	r26, 16[ep]
	sst.w	r27, 12[ep]
	sst.w	r28,  8[ep]
	sst.w	r29,  4[ep]
	sst.w	r31,  0[ep]

	/* スタックと次の起動番地を保存 */
	Lea	__kernel_runtsk, ep
	sld.w	0[ep], ep
	sst.w	sp, TCB_sp[ep]
	Lea	dispatch_r, r10
	sst.w	r10, TCB_pc[ep]
	stsr	psw, r10
	sst.w	r10, TCB_psw[ep]

	/*
	 * 次のタスクの準備 __kernel_dispatch_1
	 *  ここは割り込み禁止状態で来る必要がある。
	 */
Label	__kernel_dispatch_1
	/* runtsk = schedtsk */
	Lea	__kernel_schedtsk, r10
	ld.w	0[r10], r10
	Lea	__kernel_runtsk, r11
	st.w	r10, 0[r11]
	cmp	r10,r0
	bne	1f

	/*
	 * 起動すべきタスクはない...アイドルループ
	 */

	/* 割り込みスタックに切り替え */
	Lea	STACKTOP, sp
	/* intnestを１にする */
	mov	r0, r10
	add	1, r10
	Lea	_intnest, r11
	st.w	r10, 0[r11]

	/* 割り込み待ち */
	ei
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	di

	/* intnestをクリアする */
	Lea	_intnest, r10
	st.w	r0, 0[r10]

	br	__kernel_dispatch_1

1:
	/* スタックと次の起動番地を復帰 */
	mov	r10, ep
	sld.w	TCB_pc[ep], r31
	sld.w	TCB_sp[ep], sp
	sld.w	TCB_psw[ep], r10
	ldsr	r10, psw
	jmp	r31

	/* ディスパッチャの復帰ルーチン */
Label	dispatch_r
	Lea	__kernel_runtsk, r10
	ld.w	0[r10], r10
	ld.w	TCB_enatex[r10], r11
	tst1	TCB_enatex_bit, 0[r11]
	be	1f
	ld.w	TCB_texptn[r10], r11
	cmp	r0, r11
	be	1f

	/* タスク例外 */
	jarl	__kernel_calltex, r31
1:
	mov	sp, ep
	sld.w	116[ep], r1
	sld.w	112[ep], r2
	sld.w	108[ep], r3
	sld.w	104[ep], r4
	sld.w	100[ep], r5
	sld.w	 96[ep], r6
	sld.w	 92[ep], r7
	sld.w	 88[ep], r8
	sld.w	 84[ep], r9
	sld.w	 80[ep], r10
	sld.w	 76[ep], r11
	sld.w	 72[ep], r12
	sld.w	 68[ep], r13
	sld.w	 64[ep], r14
	sld.w	 60[ep], r15
	sld.w	 56[ep], r16
	sld.w	 52[ep], r17
	sld.w	 48[ep], r18
	sld.w	 44[ep], r19
	sld.w	 40[ep], r20
	sld.w	 36[ep], r21
	sld.w	 32[ep], r22
	sld.w	 28[ep], r23
	sld.w	 24[ep], r24
	sld.w	 20[ep], r25
	sld.w	 16[ep], r26
	sld.w	 12[ep], r27
	sld.w	  8[ep], r28
	sld.w	  4[ep], r29
	sld.w	  0[ep], r31
	addi	120, ep, sp
	Pop     ep
	jmp	r31

/*
 *  タスク起動時処理
 */
Function	__kernel_activate_r

	Lea	__kernel_runtsk, ep
	ld.w	0[ep], ep
	ld.w	TCB_tinib[ep], ep
	ld.w	TINIB_exinf[ep], r6
	ld.w	TINIB_task[ep], r10
	Lea	_ext_tsk, r31
	
	ei

	jmp	r10

/*
 *  現在のコンテキストを捨ててディスパッチ
 *    CPUロック状態で呼び出さなければならない
 */
Function	__kernel_exit_and_dispatch
	di
	Lea	_intnest, r10
	st.w	r0, 0[r10]
	br	__kernel_dispatch_1


.extern _InterruptHandlerEntry
.extern __kernel_runtsk
.extern __kernel_calltex

/*
 * 割込みハンドラ
 */
Function	vector_handler

	/* 作業領域の作成 */
	addi	-16,  sp, sp
	st.w	 ep, 12[sp]
	st.w	r10,  8[sp]
	st.w	r11,  4[sp]
	st.w	r12,  0[sp]

	/*
	 * ハンドラ起動番地の読み出し
	 *    R10 : ハンドラの番地
	 */
	stsr	ecr, r10
	shr	2, r10
	add	-4, r10
	Lea	_InterruptHandlerEntry, r11
	add	r10, r11
	ld.w	0[r11], r10

	/* ハンドラが無ければ即終了 */
	cmp	r0,r10
	bne	1f

	/* 割り込みハンドラが登録されていない場合：
	   ここでr6を破壊して、Ｃの関数を呼んでいるから他のレジスタも破壊
	   される可能性が強いが、たいてい、cpu_experr()の中でhaltさせて
	   いるか無限ループしているのでこの関数からは戻ってこない。戻って
	   くるようにしたいなら他のレジスタも保存したほうがよい */
	stsr	ecr, r6
	jarl	_cpu_experr, r31
	br	end_handler

1:
	/*
	 * 残りのレジスタ退避 
	 */
	addi	-116, sp, ep
	mov	ep, sp			/* スタックに反映 */
	sst.w	r1, 112[ep]
	sst.w	r2, 108[ep]
	sst.w	r3, 104[ep]
	sst.w	r4, 100[ep]
	sst.w	r5,  96[ep]
	sst.w	r6,  92[ep]
	sst.w	r7,  88[ep]
	sst.w	r8,  84[ep]
	sst.w	r9,  80[ep]
	sst.w	r13, 76[ep]
	sst.w	r14, 72[ep]
	sst.w	r15, 68[ep]
	sst.w	r16, 64[ep]
	sst.w	r17, 60[ep]
	sst.w	r18, 56[ep]
	sst.w	r19, 52[ep]
	sst.w	r20, 48[ep]
	sst.w	r21, 44[ep]
	sst.w	r22, 40[ep]
	sst.w	r23, 36[ep]
	sst.w	r24, 32[ep]
	sst.w	r25, 28[ep]
	sst.w	r26, 24[ep]
	sst.w	r27, 20[ep]
	sst.w	r28, 16[ep]
	sst.w	r29, 12[ep]
	sst.w	r31,  8[ep]
	stsr	eipc,   r6		/* 多重割込み対策 */
	sst.w	r6 ,  4[ep]
	stsr	eipsw,  r6
	sst.w	r6 ,  0[ep]

	/* 多重割り込みチェックとネストカウンタをインクリメント */
	Lea	_intnest, r11
	ld.w	0[r11], r6
	add	1, r6
	st.w	r6, 0[r11]
	cmp	1, r6
	ble	1f

	/* 多重割込み...すでに割り込みスタックになっている */
	jr	2f
1:
	/* 多重割込みでない */
	/* スタック切り替え */
	mov	sp, r11
	Lea	STACKTOP, sp
	Push	r11

	Lea	end_int, r31
	/* 割り込み許可 */
	ei
	/* ハンドラ起動 */
	jmp	r10

	/*
	 * 割り込みからの復帰処理
	 */
Label	end_int
	/* 割り込み禁止 */
	di

	/* スタックの復帰 */
	ld.w	0[sp], sp

	/* intnestをディクリメント */
	Lea	_intnest, r11
	ld.w	0[r11], r10
	mov	1, r11
	sub	r11, r10
	Lea	_intnest, r11
	st.w	r10, 0[r11]

	/* 
	 * タスクディスパッチャ要求があるか？
	 */
	Lea	__kernel_reqflg, r10
	ld.w	0[r10], r11
	cmp	r0, r11
	bz	recover_from_int
	st.w	r0, 0[r10]
	jr	ret_int

2:
	Lea	end_int_nest, r31
	/* 割り込み許可 */
	ei
	/* ハンドラ起動 */
	jmp	r10

	/*
	 * 割り込みからの復帰処理
	 */
Label	end_int_nest
	/* 割り込み禁止 */
	di

	/* intnestをディクリメント */
	Lea	_intnest, r11
	ld.w	0[r11], r10
	mov	1, r11
	sub	r11, r10
	Lea	_intnest, r11
	st.w	r10, 0[r11]

recover_from_int:
	/*
	 * レジスタ復帰 
	 */
	mov	sp, ep

	sld.w	112[ep], r1
	sld.w	108[ep], r2
	sld.w	104[ep], r3
	sld.w	100[ep], r4
	sld.w	 96[ep], r5
	sld.w	 92[ep], r6
	sld.w	 88[ep], r7
	sld.w	 84[ep], r8
	sld.w	 80[ep], r9
	sld.w	 76[ep], r13
	sld.w	 72[ep], r14
	sld.w	 68[ep], r15
	sld.w	 64[ep], r16
	sld.w	 60[ep], r17
	sld.w	 56[ep], r18
	sld.w	 52[ep], r19
	sld.w	 48[ep], r20
	sld.w	 44[ep], r21
	sld.w	 40[ep], r22
	sld.w	 36[ep], r23
	sld.w	 32[ep], r24
	sld.w	 28[ep], r25
	sld.w	 24[ep], r26
	sld.w	 20[ep], r27
	sld.w	 16[ep], r28
	sld.w	 12[ep], r29
	sld.w	  8[ep], r31
	sld.w	  4[ep], r10
	ldsr	    r10, eipc
	sld.w	  0[ep], r10
	ldsr	    r10, eipsw

	addi	116, ep, sp


end_handler:
	/*
	 * 作業領域の復帰
	 */
	addi	 16, sp, sp
	ld.w	 -4[sp], ep
	ld.w	 -8[sp], r10
	ld.w	-12[sp], r11
	ld.w	-16[sp], r12
	reti


/*
 * 割り込みにともなうタスクディスパッチの判定
 */
ret_int:
	/* ディスパッチ禁止？ */
	Lea	__kernel_enadsp, r10
	ld.w	0[r10], r10
	cmp	r0, r10
	be	ret_int_r

	/* タスク切り替えの必要があるか？ */
	Lea	__kernel_schedtsk, r10
	ld.w	0[r10], r10
	Lea	__kernel_runtsk, r11
	ld.w	0[r11], r11
	cmp	r10,r11
	be	ret_int_r

	/* TCBの中身を修正して復帰情報を設定 */
	mov	ep, r11
	Lea	__kernel_runtsk, ep
	sld.w	0[ep], ep			/* ep = _kernel_runtsk */

	sst.w	sp, TCB_sp[ep]
	Lea	ret_int_r, r10
	sst.w	r10, TCB_pc[ep]
	stsr	psw, r10
	sst.w	r10, TCB_psw[ep]
	mov	r11, ep

	/* タスクディスパッチャへ */
	stsr	eipsw, r10
	ori	0x20, r10, r10
	ldsr	r10, eipsw
	Lea	__kernel_dispatch_1, r10
	ldsr	r10, eipc
	reti

ret_int_r:
	di

	Lea	__kernel_runtsk, r10
	ld.w	0[r10], r10
	ld.w	TCB_enatex[r10], r11
	tst1	TCB_enatex_bit, 0[r11]
	be	1f
	ld.w	TCB_texptn[r10], r11
	cmp	r0, r11
	be	1f

	/* タスク例外 */
	jarl	__kernel_calltex, r31
1:
	/* おおもとの復帰処理へ飛ばす */
	jr	recover_from_int

/*
 * CPUロック状態の取得
 *
 *   割込み禁止ならCPUロック状態
 */
Function	_sense_lock
	stsr	psw, r10
	shr	5, r10
	andi	1, r10, r10
	jmp	r31


