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
 *  @(#) $Id: cpu_support.S,v 1.17 2007/03/23 07:22:15 honda Exp $
 */

/*
 *	プロセッサ依存モジュール アセンブリ言語部（H8用）
 */

#define _MACRO_ONLY
#include "jsp_kernel.h"
#include "offset.h"

/*
 *  NMIを除くすべての割込みを禁止
 */
#define DISINT	orc.b  #H8INT_DIS_ALL, ccr

/*
 *  すべての割込みを許可
 */
#define ENAINT	andc.b #H8INT_ENA_ALL, ccr


	.h8300h

	.text
	.align 2

/*
 *  タスクディスパッチャ
 *
 *    　_dispatch は、割込み外ネストカウンタ = 0,割込み禁止状態
 *    で呼び出さなければならない．exit_and_dispatch も，割込みネスト
 *    カウンタ = 0・割込み禁止状態で呼び出すのが原則であるが，カーネル
 *    起動時に対応するため，割込みネストカウンタ = 1で呼び出した場合に
 *    も対応している．
 */

	.global _dispatch
_dispatch:
	push.l	er4			/*  er4〜6を保存		*/
	push.l	er5
	push.l	er6
	mov.l	@_runtsk, er0		/* er0 <- runtsk		*/
	mov.l	sp, @(TCB_sp, er0)	/* タスクスタックポインタを保存	*/
	mov.l	#dispatch_r, er1	/* 実行再開番地			*/
	mov.l	er1, @(TCB_pc, er0)
	bra     dispatcher

dispatch_r:
	pop.l	er6			/*  er4〜6を復元		*/
	pop.l	er5
	pop.l	er4
	mov.l	@_runtsk, er0
	mov.b	@(TCB_enatex, er0), r1l	/* runtsk->enatex のチェック	*/
	btst	#TCB_enatex_bit, r1l
	beq	dispatch_r_1
	mov.l	@(TCB_texptn, er0), er1	/* runtsk->texptn のチェック	*/
	beq	dispatch_r_1
	jmp	@_call_texrtn

dispatch_r_1:
	rts			/*  dispatch()の呼び出し元へリターン  */

	.global	_exit_and_dispatch
_exit_and_dispatch:
	/* 割り込み禁止で呼ばれる */
        mov.b   #0, r0l         	/* 割込みネストカウンタをクリア	*/
        mov.b   r0l, @_intnest

/*  
 *　ディスパッテャ本体
 */
dispatcher:
	/*
	 * ここには割り込み禁止で来ること
	 */

	/*
	 * ここでruntskにschedtskを代入するのは２つの意味がある。
	 * 　(1) schedtsk != NULLの場合
	 * 　　　　通常のタスク切り替えを行う。
	 * 　(2) schedtsk == NULLの場合
	 * 　　　　runtskにNULLを代入しておく。
	 * 　　　　（dispatcher_1以降の割込み待ちで割込みが入り、その中で
	 * 　　　　　iget_tid()がコールされたときに正しくTSK_NONEを返すため
	 * 　　　　　には、実行状態のタスクがない時に、runtskをNULLにして
	 * 　　　　　おく必要がある。）
	 */
	mov.l	@_schedtsk, er0		/* er0 <- schedtsk		*/
	mov.l	er0, @_runtsk		/* schedtskをruntskに		*/
			/*  実行すべきタスクがなければ、割込み待ちへ分岐  */
	beq	dispatcher_1
	mov.l	@(TCB_sp, er0), sp
	mov.l	@(TCB_pc, er0), er0	/* 実行再開番地を復帰		*/
	jmp	@er0


	/*
	 *  実行すべきタスクがない場合の割込み待ち
	 *  
	 *  ここで割込みモードに切り換えるのは，ここで発生する割込み処理
	 *  にどのスタックを使うかという問題の解決と，割込みハンドラ内で
	 *  のタスクディスパッチの防止という二つの意味がある．
	 */
dispatcher_1:
	    			/* スタックを割り込みスタックに切替え	*/
	mov.l	#STACKTOP, sp
				/*  割込みネストカウンタを１にする	*/
        mov.b   #1, r0l
        mov.b   r0l, @_intnest

	/*
	 * r1l：割込み許可時にccrに設定する値
	 *　　　割込みハンドラ内ではchg_ipmが使用できないので、
	 *　　　task_intmaskは1回読み込んでおけば、OK
	 */
#ifdef SUPPORT_CHG_IPM
        mov.b     @_task_intmask, r1l
#else	/* SUPPORT_CHG_IPM */
        mov.b     #H8INT_ENA_ALL, r1l
#endif	/* SUPPORT_CHG_IPM */

	/* CPUロックフラグをクリア */
	sub.l	er0, er0
        mov.l   er0, @_iscpulocked


dispatcher_2_enable_interrupt:
        /*
         *　ldc命令直後は割込み禁止であり、割込み要求があっても
         *　次の命令が実行される。
         *　（割込みから戻ってきてsleepするケースはない）
         */
        ldc.b     r1l, ccr              /* 割り込み許可 */
        sleep                           /* 割込み待ち */
        DISINT                          /* 割り込み禁止 */

	mov.l   @_reqflg, er0           /* reqflgのチェック */
			/* reqflgはBOOL型 -> int型(32bits) */
		/*  reqflg==NULLならば、もう1度割込み待ち  */
	beq     dispatcher_2_enable_interrupt
	sub.l	er0, er0
	mov.l   er0, @_reqflg		/* reqflgのクリア */
			/* 割込みネストカウンタをクリア	*/
	mov.b   r0l, @_intnest
	bra	dispatcher

/*
 *
 *  割込みハンドラ入口処理
 *　　割込み要因に依らず、共通な部分
 *　　（cpu_config.hのINTHDR_ENTRYからの続き）
 *
 *　　　　以下の状態でここにやってくる
 *　　　　　・割込み禁止
 *　　　　　・er0〜er3退避済み
 *　　　　　・er2：C言語ルーチンの先頭アドレス
 *　　　　　・r3l：割込み許可時に割込みマスクに設定する値
 */
        .global _common_interrupt_process
_common_interrupt_process:
	push.l	er4			/*  最適化対策でer4も退避  */
        mov.b   @_intnest, r0l          /* 割込みネストカウンタのチェック */
        bne     _interrupt_from_int     /*  intnetが0でなければ、*/
        				/*  多重割込みの処理へ  */

        /*
         *　初段の割込みの場合
         */
        mov.b   #1, r1l                 /* 割込みネストカウンタ←１ */
        mov.b   r1l, @_intnest
        mov.l   sp, er0
        mov.l   #STACKTOP, sp           /* スタック切替 */
        push.l  er0                     /* タスクスタックポインタの退避 */
        ldc.b   r3l, ccr                /* 割込み許可 */
        jsr     @er2                    /* C言語ルーチン呼び出し */
        DISINT                          /* 割込み禁止 */
        mov.b   #0, r0l                 /* 割込みネストカウンタのクリア */
        mov.b   r0l, @_intnest
        mov.l   @sp, sp                 /* スタック切替 */
        mov.l   @_reqflg, er0           /* reqflgのチェック */
        beq     _ret_to_task_int        /* reqflgがFALSEなら */
        				/* ret_to_task_intに飛ぶ */
        sub.l	er0, er0
        mov.l   er0, @_reqflg           /* reqflgをクリア */
        bra     ret_int                 /* 出口処理へジャンプ */

        /*
         *　多重割込みの場合
         *　　割込み禁止でここにやってくる
         *
         *　　割込みネストカウンタのインクリメントは以下の理由で
         *　　省略している
         *　　　・割込みが2レベルしかない
         *　　　・CPU例外をサポートしない
         *　　　　　exc_sense_context()のことを考慮しなくても良い
         */
_interrupt_from_int:
        /* 割込みネストカウンタのインクリメントは省略 */
        ldc.b   r3l, ccr                /* 割込み許可 */
        jsr     @er2                    /* C言語ルーチン呼び出し */
        DISINT                          /* 割込み禁止 */
        /* 割込みネストカウンタのディクリメントは省略 */

        /*
         *　本来はここで戻り先が、割込み待ちのsleep命令かどうかのチェッ
         *　クが必要である。（もし、割込みハンドラ内でreqflg=TRUEにして
         *　もディスパッチャが呼ばれない＝タスクが実行されない）
         *　H8の場合、sleep命令の直前にある割込み許可のldc命令の実行直
         *　後は割込み禁止であり、割込み要求があったとしても割込みは受
         *　け付けないで、sleep命令が実行される。そのため、割込みから
         *　戻った直後にsleep命令が実行されるケースは考慮しなくて良い。
         */

        /*
         *　ディスパッチャを呼ばずに、割込み元に戻る場合
         */
_ret_to_task_int:
					/* レジスタを復元 */
	pop.l	er4			/*  er4は最適化対策  */
        pop.l   er3
        pop.l   er2
        pop.l   er1
        pop.l   er0
        rte                             /* 割込み元へリターン */

/*
 * 出口処理においてスタックポインタが指している番地からCCRレジスタが
 * 積まれている番地までのオフセットの定義
 *
 *　 +0:er4
 *　 +4:er3
 *　 +8:er2
 *　+12:er1
 *　+16:er0
 *  +20:ccr ←ここをsp相対アクセスする
 */
#define OFFSET_CCR	20

/*
 *  割込みハンドラ出口処理
 *
 * 戻り先がタスクでreqflgがセットされている場合のみここにくる。
 * 割込みネストカウンタ = 0,割込み禁止状態,スクラッチレジスタを
 * 保存した状態で呼び出すこと。
 *
 */

ret_int:
	mov.l	@_enadsp, er0		/* enadspのチェック		*/
	beq	ret_int_1	/* ディスパッチ禁止ならret_int_1へ	*/

	mov.l	@_runtsk, er0		/* er0 <- runtsk		*/
	mov.l	@_schedtsk, er1		/* er1 <- schedtsk		*/
	cmp.l	er0, er1		/* runtsk と schedtsk を比較	*/
	beq	ret_int_1		/* 同じならret_int_1へ		*/

        push.l  er5                     /* レジスタ退避 */
        push.l  er6

	mov.l	sp, @(TCB_sp, e0)	/* タスクスタックポインタを保存	*/
	mov.l	#ret_int_r, er1  	/* 実行再開番地			*/
	mov.l	er1, @(TCB_pc, er0)
	bra	dispatcher:16
		/*  8ビットPC相対分岐では届かないため、16ビット指定  */

/*
 *  割込みの出口でディスパッチャからここに戻ってくる
 */
ret_int_r:
        pop.l   er6                     /* レジスタ復元 */
        pop.l   er5

        /*
         *　タスクスイッチしない場合はここから合流する
         */
ret_int_1:
					/*  タスク例外許可状態のチェック */
	mov.l	@_runtsk, er0
	mov.b	@(TCB_enatex, er0), r1l
	btst	#TCB_enatex_bit, r1l	/*  TCB.enatex=0 -> CCR.Z=1  */
	beq	ret_int_2	/*  タスク例外禁止状態ならジャンプ  */
	
					/*  タスク例外処理要求のチェック */
	mov.l	@(TCB_texptn, er0), er1
	beq	ret_int_2	/*  タスク例外処理要求がなければジャンプ */
	jsr	@_call_texrtn		/* タスク例外処理ルーチン起動	*/

ret_int_2:
        /* CPUロックフラグをクリア */
        sub.l   er0, er0
        mov.l   er0, @_iscpulocked

#ifdef SUPPORT_CHG_IPM
        /*
         *  スタックに積んであるccr（戻り先の割込みマスク）を
         *　task_intmaskにする。（下記参照）
         */
        mov.b   @(OFFSET_CCR, sp), r0l
        mov.b   @_task_intmask, r1l
        and.b   #H8INT_ENA_ALL, r0l
        or.b    r1l, r0l
        mov.b   r0l, @(OFFSET_CCR, sp)
#endif 	/* SUPPORT_CHG_IPM */
                                        /* レジスタを復元 */
        pop.l   er4                     /* 　 +0:er4 */
        pop.l   er3                     /* 　 +4:er3 */
        pop.l   er2                     /* 　 +8:er2 */
        pop.l   er1                     /* 　+12:er1 */
        pop.l   er0                     /* 　+16:er0 */
                                        /*   +20:ccr ←ここを書き換える */
        rte


/*
 *  タスク起動処理
 */

	.globl _activate_r
_activate_r:
        /* CPUロックフラグをクリア */
        sub.l   er0, er0
        mov.l   er0, @_iscpulocked

	/*  割込み許可  */
#ifdef SUPPORT_CHG_IPM
        mov.b   @_task_intmask, r0l
        ldc.b   r0l, ccr
#else 	/* SUPPORT_CHG_IPM */
        ENAINT
#endif 	/* SUPPORT_CHG_IPM */

	mov.l	#_ext_tsk, er2
	push.l	er2
	mov.l	@_runtsk, er2
	mov.l	@(TCB_tinib, er2), er2
	mov.l	@(TINIB_task, er2), er1	/*  タスク起動番地		*/
					/*  拡張情報（タスクへの引数）	*/
	mov.l	@(TINIB_exinf, er2), er0
	jmp	@er1


/**************************************************************
 *  以下、共通部とのインターフェースには含まれない独自の部分
 **************************************************************/

/*
 *  no_reg_exception()
 *  CPU例外として登録されていない例外が発生すると呼び出される。
 *  例外が発生した時点のPC,ER0〜7を出力してカーネル
 *  を停止する。
 * 
 * 　スタック構造
 *　　 +0:er0
 *　　 +4:er1
 *　　 +8:er2
 *　　+12:er3
 *　　+16:er4
 *　　+20:er5
 *　　+24:er6
 *　　+28:crr
 *　　+29:pc
 *　　+32〜:割込み前に使用されていたスタック領域
 */
	.globl _no_reg_exception
_no_reg_exception:

	push.l	er6		/*  ER0〜6を保存			*/
	push.l	er5
	push.l	er4
	push.l	er3
	push.l	er2
	push.l	er1
	push.l	er0

	mov.l	sp, er0		/*  引数設定				*/
				/*  SP は +32 で、発生時点の値		*/
	jsr	@_cpu_experr	/*  cpu_experr()の呼び出し		*/

