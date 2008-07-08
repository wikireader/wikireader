/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: cpu_support.S,v 1.16 2005/11/12 14:58:46 hiro Exp $
 */

/*
 *	プロセッサ依存モジュール アセンブリ言語部（M68040用）
 */

#define	_MACRO_ONLY
#include "jsp_kernel.h"
#include "offset.h"

/*
 *  タスクディスパッチャ
 *
 *  dispatch は，マスタモード・割込み禁止状態で呼び出さなければならな
 *  い．exit_and_dispatch も，マスタモード・割込み禁止状態で呼び出すの
 *  が原則であるが，カーネル起動時に対応するため，割込みモードで呼び出
 *  した場合にも対応している．
 */
	.text
	.globl dispatch
	.globl exit_and_dispatch
dispatch:
	movem.l %d2-%d7/%a2-%a6, -(%sp)	/* レジスタを保存 */
	move.l runtsk, %a0		/* A0 を runtsk に */
	move.l %sp, TCB_msp(%a0)	/* タスクスタックを保存 */
	move.l #dispatch_r, TCB_pc(%a0)	/* 実行再開番地を保存 */
	jbra dispatcher

dispatch_r:
	movem.l (%sp)+, %d2-%d7/%a2-%a6	/* レジスタを復帰 */
	btst.b #TCB_enatex_bit, TCB_enatex(%a0)
	jbeq dispatch_r_1		/* enatex が FALSE ならリターン */
	tst.l TCB_texptn(%a0)		/* texptn が 0 でなければ           */
	jbne call_texrtn		/*   タスク例外処理ルーチンの呼出し */
dispatch_r_1:
	rts

exit_and_dispatch:
	or.w #0x1000, %sr		/* マスタモード */
dispatcher:
	/*
	 *  ここではマスタモード・割込み禁止状態でなければならない．
	 */
	move.l schedtsk, %a0
	move.l %a0, runtsk		/* schedtsk を runtsk に */
	jbeq dispatcher_1		/* runtsk があるか？ */
	move.l TCB_msp(%a0), %sp	/* タスクスタックを復帰 */
	move.l TCB_pc(%a0), %a1		/* 実行再開番地を復帰 */
	jmp (%a1)
dispatcher_1:
	stop #0x2000			/* 割込み待ち（割込みモード） */
	/*
	 *  ここで割込みモードに切り換えるのは，ここで発生する割込み処理
	 *  にどのスタックを使うかという問題の解決と，割込みハンドラ内で
	 *  のタスクディスパッチの防止という2つの意味がある．
	 *
	 *  この stop命令は，IPM を 0 にするが，本来は task_intmask に
	 *  設定すべきである．M68040 では，stop 命令のパラメータに定数
	 *  しかとれないため，やむをえず 0 にしている（stop 命令を 8つ
	 *  並べて，task_intmask の値で飛び分ける手はあるが，そこまで
	 *  やる意義はないと考えた）．
	 *
	 *  プロセッサを待ちモードに移行させる処理と，割込み許可とは，
	 *  不可分に行なう必要がある（M68040 では stop命令で両方行なう
	 *  ので問題ない）．これを不可分に行なわない場合，割込みを許可
	 *  した直後に割込みが入り，その中でタスクが実行可能状態になる
	 *  と，実行すべきタスクがあるにもかかわらずプロセッサが待ちモー
	 *  ドになってしまう．
	 *
	 *  割込みを待つ間は，runtsk を NULL（=0）に設定しなければなら
	 *  ない．このように設定しないと，割込みハンドラから iget_tid 
	 *  を呼び出した際の動作が仕様に合致しなくなる．
	 */
	or.w #0x1700, %sr		/* マスタモード・割込み禁止 */
	tst.l reqflg			/* reqflg が FALSE なら */
	jbeq dispatcher_1		/*      dispatcher_1 へ */
	clr.l reqflg			/* reqflg を FALSE に */
	jbra dispatcher
	
/*
 *  タスク起動時処理
 */
	.text
	.globl activate_r
activate_r:
	/*
	 *  タスク起動直後はタスク例外処理が禁止されているため，ここでタ
	 *  スク例外処理ルーチンを呼び出す条件は成り立たない．
	 */
#ifdef SUPPORT_CHG_IPM			/* t_unlock_cpu 相当の処理 */
	move.w %sr, %d0			/* 割込みマスクを task_intmask に */
	and.w #~0x0700, %d0
	or.w task_intmask, %d0
	move.w %d0, %sr
#else /* SUPPORT_CHG_IPM */
	and.w #~0x0700, %sr		/* 割込み許可 */
#endif /* SUPPORT_CHG_IPM */
	move.l (%sp)+, %a0		/* タスクの起動番地を a0 に */
	jmp (%a0)

/*
 *  割込みハンドラ／CPU例外ハンドラ出口処理
 *
 *  ret_int は割込みモード・割込み禁止状態で，ret_exc はマスタモード・
 *  割込み禁止状態で呼び出さなければならない．また ret_exc は，スクラッ
 *  チレジスタを保存した状態で呼び出すこと．
 */
	.text
	.globl ret_int
	.globl ret_exc
ret_int:
	addq.l #8, %sp			/* スローアウェイフレームを捨てる */
	or.w #0x1000, %sr		/* マスタモード */
	movem.l %d0-%d1/%a0-%a1, -(%sp)	/* スクラッチレジスタを保存 */
ret_exc:
	clr.l reqflg			/* reqflg を FALSE に */
	move.l runtsk, %a0		/* A0 ← runtsk */
	tst.l enadsp			/* enadsp が FALSE なら */
	jbeq ret_int_1			/*         ret_int_1 へ */
	cmp.l schedtsk, %a0		/* runtsk と schedtsk が同じなら */
	jbeq ret_int_1			/*                  ret_int_1 へ */
	movem.l %d2-%d7/%a2-%a6, -(%sp)	/* 残りのレジスタを保存 */
	move.l %sp, TCB_msp(%a0)	/* タスクスタックを保存 */
	move.l #ret_int_r, TCB_pc(%a0)	/* 実行再開番地を保存 */
	jbra dispatcher

ret_int_r:
	movem.l (%sp)+, %d2-%d7/%a2-%a6	/* レジスタを復帰 */
ret_int_1:
	btst.b #TCB_enatex_bit, TCB_enatex(%a0)
	jbeq ret_int_2			/* enatex が FALSE ならリターン */
	tst.l TCB_texptn(%a0)		/* texptn が 0 ならリターン */
	jbeq ret_int_2
	jsr call_texrtn			/* タスク例外処理ルーチンの呼出し */
ret_int_2:
#ifdef SUPPORT_CHG_IPM
	move.w 16(%sp), %d0		/* 戻り先の割込みマスクを */
	and.w #~0x0700, %d0		/*        task_intmask に */
	or.w task_intmask, %d0
	move.w %d0, 16(%sp)
#endif /* SUPPORT_CHG_IPM */
	movem.l (%sp)+, %d0-%d1/%a0-%a1	/* スクラッチレジスタを復帰 */
	rte

/*
 *  微少時間待ち
 */
	.globl _sil_dly_nse
_sil_dly_nse:
	subi.l #SIL_DLY_TIM1, %d0	/* D0 から SIL_DLY_TIM1 を引く */
	jbhi _sil_dly_nse_1		/* 結果が 0 以下ならリターン */
	rts
_sil_dly_nse_1:
	subi.l #SIL_DLY_TIM2, %d0	/* D0 から SIL_DLY_TIM2 を引く */
	jbhi _sil_dly_nse_1		/* 結果が 0 より大きければループ */
	rts
