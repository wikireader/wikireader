/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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
#include "offset.h"

	.h8300s
	.text
	.align 2

/*
 *  タスクディスパッチャ
 *  ・dispatch は、割込みネストカウンタ = 0、CPUロック状態（割込み禁止状態）で
 *    呼び出さなければならない。
 *  ・exit_and_dispatch は，割込みネストカウンタ = 0、CPUロック状態（割込み禁止
 *    状態）で呼び出すのが原則である。しかし、カーネル起動時に対応するため，
 *    割込みネストカウンタ = 1 で呼び出した場合にも対応している。
 */

	.global _dispatch
	.global	_exit_and_dispatch

/* タスクディスパッチャを明示的に呼び出す時 */
_dispatch:
	push.l	er2			/* er2〜6 を保存 */
	push.l	er3
	push.l	er4
	push.l	er5
	push.l	er6

	mov.l	@_runtsk, er0		/* er0 = runtsk */
	mov.l	sp, @(TCB_sp, er0)	/* runtsk->sp = sp
					   (タスクスタックポインタを保存) */
	mov.l	#dispatch_r, er1
	mov.l	er1, @(TCB_pc, er0)	/* runtsk->pc = dispatch_r
					   (タスクの実行再開番地を保存) */

	bra     dispatcher_1

/* 現在実行中のコンテキストを捨てて、タスクディスパッチャを呼び出す時 */
_exit_and_dispatch:
	sub.l	er0, er0
	mov.l	er0, @_intnest		/* 割込みネストカウンタ = 0 */

dispatcher_1:
	/* ここには CPUロック（割り込み禁止）で来ること */
	mov.l	@_schedtsk, er0		/* er0 = schedtsk */
	or.l	er0, er0		/* 判定部分 (実行可能タスクの有無) */
	beq	dispatcher_2		/* schedtsk が無いなら、割込み待ちへ */

	mov.l	er0, @_runtsk		/* runtsk = er0 (= schedtsk)
					   (タスクスイッチの実行) */
	mov.l	@(TCB_sp, er0), sp	/* sp = runtsk->sp
					   (タスクスタックポインタ復元) */
	mov.l	@(TCB_pc, er0), er0	/* er0 = runtsk->pc
					   (タスクの実行再開番地の取得) */
					/* 実行再開番地は以下の3通り
					   ・ディスパッチャの出口  dispatch_r
					   ・割込み／例外の出口    ret_int_r
					   ・タスク起動直後        activate_r
					 */

	jmp	@er0			/* タスクの実行再開番地へジャンプ */

	/*
	 *  実行すべきタスクが現れるまで待つ処理
	 *    ここでコンテキストを切り換えるのは，
	 *      ・ここで発生する割込み処理にどのスタックを使うかという問題の解決
	 *      ・割込みハンドラ内でのタスクディスパッチの防止
	 *    という二つの意味がある．
	 */ 
dispatcher_2:
	sub.l	er0, er0
	mov.l	er0, @_runtsk		/* runtsk = NULL */

	mov.l	#STACKTOP, sp		/* 割込みスタックに切替 */

	sub.l	er0, er0
	inc.l	#1, er0
	mov.l	er0, @_intnest		/* 割込みネストカウンタ = 1 */

dispatcher_2_enable_interrupt:
#ifdef SUPPORT_CHG_IPM
	ldc.b	@_task_intmask, exr	/* unlock_cpu相当(割込み許可) */
#else /* SUPPORT_CHG_IPM */
	ldc.b	#0, exr			/* unlock_cpu相当(割込み許可) */
#endif /* SUPPORT_CHG_IPM */
	sleep				/* 割込み待ち */
	ldc.b	#MAX_IPM, exr		/* lock_cpu 相当 (割込み禁止) */

	mov.l	@_reqflg, er0		/* er0 = reqflg  */
	or.l	er0, er0		/* 判定部分 (ディスパッチ要求) */
	beq	dispatcher_2_enable_interrupt
					/* ディスパッチ要求が無い時、
					   ループの先頭へジャンプ */

	sub.l	er0, er0		/* er0 = 0 (= NULL) */
	mov.l	er0, @_reqflg		/* reqflg = NULL (reqflgをクリア) */
	mov.l	er0, @_intnest		/* 割込みネストカウンタ = 0 */

	bra	dispatcher_1		/* ディスパッチ処理へジャンプ */

/*
 *  タスクコンテキストへの復帰処理（ディスパッチャ出口処理）
 */
dispatch_r:
	pop.l	er6			/* er2〜6 を復元 */
	pop.l	er5
	pop.l	er4
	pop.l	er3
	pop.l	er2

	/* タスク例外処理要求のチェック */
	mov.l	@_runtsk, er0		/* er0 = runtsk */

	mov.b	@(TCB_enatex, er0), r1l	/* r1l = runtsk->enatex */
	btst	#TCB_enatex_bit, r1l	/* TCB_enatex == 1 か？ */
	beq	dispatch_r_1		/* タスク例外処理禁止の時、ジャンプ */

	mov.l	@(TCB_texptn, er0), er1	/* er1 = runtsk->texptn */
	beq	dispatch_r_1		/* タスク例外要求が無い時、ジャンプ */

	jmp	@_call_texrtn		/* タスク例外処理ルーチンの呼出 */

dispatch_r_1:
        rts				/* dispatch 呼び出し個所へ戻る */

/*============================================================================*/

/*
 *  割込みハンドラ出入口処理
 *    ここに来るときは、CPU ロック状態で、
 *      er0(r0l) : 割込み要求後の割り込みレベルの値
 *      er1      : 割込みハンドラのアドレス
 *    が代入されている。（er0, er1 はスタックに保存済み）
 */

	.global ret_main
	.global	_ret_int_r	/* monitor 向け */

ret_main:
	push.l	er2			/* 残りのレジスタを保存 */
	push.l  er3
	push.l	er4
	push.l	er5
	push.l	er6
		/* もしかすると、H8S/2600 における macl, mach レジスタの保存も
		   ここで行う必要があるかも知れません。（復帰時も同様。）
		   開発環境の制限により、確認できていません。 */

	mov.l	@_intnest, er2		/* er2 = intnest */
	mov.l	er2, er3		/* er3(= er2)は、次の処理で利用する。 */
	inc.l	#1, er2			/* 割込みネストカウンタ += 1 */
	mov.l	er2, @_intnest

	or.l	er3, er3		/* 判定部分
					   (割込み発生時のコンテキスト) */
	beq	first_exception		/* 初段の割込みの時、ジャンプ */

/* 多重割込みの処理 */
multipul_exception:
	ldc.b	r0l, exr		/* (現在の割込みレベル以上)割込み許可 */

	jsr	@er1			/* 割込みハンドラ呼出 */

	ldc.b	#MAX_IPM, exr		/* lock_cpu 相当 (割込み禁止) */

	mov.l	@_intnest, er0		/* er0 = intnest */
	dec.l	#1, er0			/* 割込みネストカウンタ -= 1 */
	mov.l	er0, @_intnest

	jmp	ret_to_task_int

/* 初段の割込みの場合 */
first_exception:

	mov.l	sp, er2			/* er2 = タスクスタックポインタ */
	mov.l	#STACKTOP, sp		/* 割込みスタックに切替 */
	push.l	er2			/* タスクスタックポインタを
					   割込みスタックに退避 */

	ldc.b	r0l, exr		/* (現在の割込みレベル以上)割込み許可 */

	jsr	@er1			/* 割込みハンドラ呼出 */

	ldc.b	#MAX_IPM, exr		/* lock_cpu 相当 (割込み禁止) */

	mov.l	@sp, sp			/* タスクスタックに切替 */

	mov.l	@_intnest, er0		/* er0 = intnest */
	dec.l	#1, er0			/* 割込みネストカウンタ -= 1 */
	mov.l	er0, @_intnest

	/* ディスパッチ要求/タスク例外処理要求(reqflg)の判定 */
	mov.l	@_reqflg, er0		/* er0 = reqflg */
	or.l	er0, er0		/* 判定部分 */
	bne	occur_dispatch_or_tsk_exception
					/* reqflg = TRUE の時、ジャンプ */
					/* 下記の条件が、OR で成立の時
					   ・遅延ディスパッチが発生
					   ・タスク例外処理要求が発生 */

	/* レジスタの復元 */
ret_to_task_int:
	pop.l	er6			/*  +0:er6 */
	pop.l	er5			/*  +4:er5 */
	pop.l	er4			/*  +8:er4 */
	pop.l	er3			/* +12:er3 */
	pop.l	er2			/* +16:er2 */
	pop.l	er1			/* +20:er1 */
	pop.l	er0			/* +24:er0 */
					/* +28:EXR -> EXR_offset = 28 */
	rte				/* 割込み発生場所に戻る */
			/* EXR の復元(unlock_cpu 相当)はハードウェアが行う。 */

/*
 *  割込み出入口処理における遅延ディスパッチ/タスク例外に関する処理
 */
occur_dispatch_or_tsk_exception:

/* 割込み発生個所の判定 */
	mov.b	@(EXR_offset, sp), r0l	/* r01 = スタック上のEXR */
	and.b	#EXR_I_MASK, r0l	/* r01 = スタック上の割り込みマスク */

#ifdef SUPPORT_CHG_IPM			/* r1l = タスクコンテキストの
					         割込みマスク */
	mov.b	@_task_intmask, r1l
#else /* SUPPORT_CHG_IPM */
	mov.b	#0, r1l
#endif /* SUPPORT_CHG_IPM */

	cmp.b	r0l, r1l		/* 判定部分 (割込み発生個所) */

	bne	ret_to_task_int		/* 割込み発生場所が、タスクコンテキスト
					   でない時、ジャンプ */
		/* 論理的な分岐先は、to_int_routine における SUPPORT_CHG_IPM
		   マクロで囲まれた個所の直後である。コストの問題から、本質的
		   に同じなので、ret_to_task_int と記述した。 */

	sub.l	er0, er0		/* er0 = NULL (= 0) */
	mov.l	er0, @_reqflg		/* reqflg = NULL (reqflgをクリア) */

/* 遅延ディスパッチに関する判定 */
	/* ディスパッチ禁止/許可の判定 */
	mov.l	@_enadsp, er0		/* er0 = enadsp */
	or.l	er0, er0		/* 判定部分 */
	beq	to_tsk_exc		/* ディスパッチ禁止ならジャンプ */

	/* スケジューリング結果の判定 */
	mov.l	@_runtsk, er0		/* er0 = runtsk	*/
	mov.l	@_schedtsk, er1		/* er1 = schedtsk */
	cmp.l	er0, er1		/* 判定部分 */
	beq	to_tsk_exc		/* runtsk == schedtsk ならジャンプ */

/* 遅延ディスパッチが発生した場合 */
occur_dispatch:
#ifdef H8S_2600				/* H8S_2600はコンパイルオプション定義 */
	/* 積和レジスタの退避 */
	/* er0 は下記にて利用するので破壊できない */
	stmac	mach, er1
	push.l	er1
	stmac	macl, er1
	push.l	er1
#endif /* H8S_2600 */

	mov.l	sp, @(TCB_sp, er0)	/* runtsk->sp = sp
					   (タスクスタックポインタを保存) */

	mov.l	#ret_int_r, er1
	mov.l	er1, @(TCB_pc, er0)	/* runtsk->pc = ret_int_r
					   (タスクの実行再開番地を保存) */

	jmp	dispatcher_1		/* ディスパッテャ呼出 */

/* 遅延ディスパッチによってディスパッチしたタスクコンテキストの開始地点 */
_ret_int_r:	/* monitor 向け */
ret_int_r:

#ifdef H8S_2600				/* H8S_2600はコンパイルオプション定義 */
	/* 積和レジスタの復元 */
	pop.l	er0
	ldmac	er0, macl
	pop.l	er0
	ldmac	er0, mach
#endif /* H8S_2600 */

/* 遅延ディスパッチが発生しなかった場合の開始地点 */

/* タスク例外処理に関する判定 */
to_tsk_exc:
	mov.l	@_runtsk, er0		/* er0 = runtsk */

	/* タスク例外処理禁止/許可の判定 */
	mov.b	@(TCB_enatex, er0), r1l	/* r1l = runtsk->enatex */
	btst	#TCB_enatex_bit, r1l	/* TCB_enatex == 1 か？ */
	beq	to_int_routine		/* タスク例外処理禁止の時、ジャンプ */

	/* タスク例外要求の有無の判定 */
	mov.l	@(TCB_texptn, er0), er1	/* er1 = runtsk->texptn */
	beq	to_int_routine		/* タスク例外要求が無い時、ジャンプ */

	jsr	@_call_texrtn		/* タスク例外処理ルーチンの呼出 */

to_int_routine:

/* chg_ipm の話は、遅延ディスパッチ先のタスクの問題なので、ここに記述する。 */
#ifdef SUPPORT_CHG_IPM
	/* 割り込みハンドラ終了時に別のタスクへスイッチしたときに、
	   割り込みマスクを整合させる処理 */
	/* 例：ディスパッチ順序：
		タスクＡ->割り込みハンドラ->タスクＢ->タスクＡ
	       動作：
		この時にタスクＢ内で、割込みマスクの変更が行われた場合に、
		この割込みマスクに関する情報を、タスクＢからタスクＡへと
		引き継ぐために必要。*/
	mov.b	@_task_intmask, r1l	/* r1l = task_intmask */
		/* task_intmask：CHG_IPM により変更された可能性のある
				 割込みマスク */
	mov.b	r1l, @(EXR_offset, sp)
		/* スタック上の EXR に上書きをする。 */
		/* 制限事項：EXR のトレースビット T は保存されない。 */
#endif /* SUPPORT_CHG_IPM */

	jmp	ret_to_task_int		/* タスクコンテキストへの復帰 */

/*============================================================================*/

/*
 *  タスク起動処理
 */

	.global _activate_r

_activate_r:
	/* ここに来るときは、CPUロックになっている。 */

	mov.l	#_ext_tsk, er0
	push.l	er0				/* タスクからの戻り番地を設定 */

	/* タスク関係の設定準備 */
	mov.l	@_runtsk, er2			/* er2 = runtsk */
	mov.l	@(TCB_tinib, er2), er2		/* er2 = runtsk->tinib */

	/* タスク起動番地の設定 */
	mov.l	@(TINIB_task, er2), er1		/* er1 = runtsk->tinib->task */

	/* タスクへの引数（拡張情報）設定 */
	mov.l	@(TINIB_exinf, er2), er0	/* er0 = runtsk->tinib->exinf */

#ifdef SUPPORT_CHG_IPM
	ldc.b	@_task_intmask, exr		/* unlock_cpu相当(割込み許可) */
#else /* SUPPORT_CHG_IPM */
	ldc.b	#0, exr				/* unlock_cpu相当(割込み許可) */
#endif /* SUPPORT_CHG_IPM */

	jmp	@er1				/* タスク起動 */

/*============================================================================*/
/*  共通ドキュメントにはない、独自の部分  */

/*
 *  no_reg_exception()
 *    CPU例外として登録されていない例外が発生すると呼び出される。
 *    例外が発生した時点の PC、er0〜7 を出力してカーネルを停止する。
 */
	.global no_reg_exception

no_reg_exception:

	push.l	er0			/* er0〜6を保存	*/
	push.l	er1
	push.l	er2
	push.l	er3
	push.l	er4
	push.l	er5
	push.l	er6

	mov.l	sp, er0			/* 引数にスタックポインタを設定	*/
	/* CPU例外発生時点の SP は、
	   34(= 4*7(er) + 1(exr) + 1(予約) + 1(ccr) + 3(pc) ) を引く */

	jsr	@_cpu_experr		/* cpu_experr()の呼び出し */

