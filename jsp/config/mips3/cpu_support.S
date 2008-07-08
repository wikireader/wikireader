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
#include "offset.h"

	.section .text
	.align 2

	.set noat
		/* atレジスタを利用したときにワーニングを出力しないよう指定 */
	.set noreorder

/*
 *  例外/割り込み出入口処理ルーチン（一般例外）
 *
 *  仕様：
 *  ・CPU例外ハンドラは、非タスクコンテキストで実行する。
 *  ・k0 レジスタを、割込み／例外ネストカウンタとして使用する。
 *  ・reqflg をチェックする前に割込みを禁止しないと、reqflg をチェック後に起動さ
 *    れた割込みハンドラ内でディスパッチが要求された場合にディスパッチされない。
 *  ・割込み許可／不許可の処理は、EXLビットを用いて行う。
 *    これは、呼出元のコンテキストの CPUロック状態を保存するためである。
 *    ただし、共通部に処理を移行する場合にはIEビットを用いて、割込み許可／不許可
 *    を実現する必要があるために、IEビットによる割込み許可／不許可に変更している
 *    事に注意。
 *    （例：遅延ディスパッチの処理に入る直前、タスク例外処理を呼出す直前）
 *
 *  C言語ルーチンでlong long型を使う場合はレジスタの待避／復元をdouble wordで行
 *  うべきだが、使用頻度が低いのと、出入口処理のオーバーヘッドを考えて、wordで待
 *  避／復元を行っている。将来的には待避するデータサイズをタスク属性で指定できる
 *  ようにする予定
 */

	.global general_exception
	.global join_interrupt_and_exception
		/* 関数呼出(proc_interrupt_sys)時に必要 */

general_exception:
	addi    sp, sp, -25*4
	sw      sp,  0*4(sp)		/* EXCSTACK構造体用(cpu_experr関数用) */
	sw      at,  1*4(sp)
	sw      v0,  2*4(sp)
	sw      v1,  3*4(sp)
	sw      a0,  4*4(sp)
	sw      a1,  5*4(sp)
	sw      a2,  6*4(sp)
	sw      a3,  7*4(sp)
	sw      t0,  8*4(sp)
	sw      t1,  9*4(sp)
	sw      t2, 10*4(sp)
	sw      t3, 11*4(sp)
	sw      t4, 12*4(sp)
	sw      t5, 13*4(sp)
	sw      t6, 14*4(sp)
	sw      t7, 15*4(sp)
	sw      t8, 16*4(sp)
	sw      t9, 17*4(sp)
	sw      gp, 18*4(sp)
	sw      fp, 19*4(sp)
	sw      ra, 20*4(sp)

	mfc0    a1, Status		/* a1 -> 例外処理で使う */
	mfc0    t1, EPC

	sw      a1, 21*4(sp)		/* Status */
	sw      t1, 22*4(sp)		/* EPC */

	mfhi    t2			/* 乗除算レジスタ HI の待避 */
	mflo    t3			/* 乗除算レジスタ LO の待避 */

	sw      t2, 23*4(sp)		/* HI */
	sw      t3, 24*4(sp)		/* LO */

	move	a0, sp			/* 例外ハンドラの引数設定 */
		/* 引数は、例外発生時のデータを退避したスタック領域へのポインタ
		   割込みコントローラのデータを含めると複雑になるため、
		   対象はMIPS3コアのレジスタ部分のみとする。 */

	PUSH_ICU_IPM			/* 割込みコントローラの割込みマスクの
					   待避マクロ(システム依存部で定義) */

	bgtz    k0, multipul_exception  /*  初段の例外と多重例外の判別 */
	addi    k0, k0, 1               /* ネストカウンタのインクリメント */

/* 初段の例外の場合 */
first_exception:
	la      t3, _stack_top		/* _stack_top は、システム依存部
					   (リンカスクリプト推奨)定義 */
					/* スタックポインタの減算(t3=t3-4)は、
					   _stack_top 設定時に行っているので、
					   ここでは不要 */
	sw      sp, (t3)		/* タスクスタックポインタの保存 */
	move    sp, t3			/* スタック切り替え */

	jal     proc_interrupt_and_exception	/* 割込みと例外の判別処理へ */
	nop

	/*  割込み禁止で戻ってくる  */
	la      t2, reqflg
	lw      t3, (t2)		/* t3 = reqflg */
	lw      sp, (sp)		/* スタック切り替え */
	beq     t3, zero, ret_to_task_int	/* ディスパッチ要求がない場合 */
	nop

	j       ret_int			/* 割込み／例外の出口処理へ */
	sw      zero, (t2)		/* reqflg のクリア */

/* 多重例外（多重割込み）の場合 (例外ハンドラの引数は既に a0 に設定済み) */
multipul_exception:
	jal     proc_interrupt_and_exception	/* 割込みと例外の判別処理へ */
	nop

/* ディスパッチャを呼ばずに単純に割込み元（例外発生元）に戻る場合 */
ret_to_task_int:
	POP_ICU_IPM			/* 割込みコントローラの割込みマスクの
					   復帰マクロ(システム依存部で定義) */

	lw      t0, 23*4(sp)		/* HI */
	lw      t1, 24*4(sp)		/* LO */

	mthi    t0			/* 乗除算レジスタ HI の復元 */
	mtlo    t1			/* 乗除算レジスタ LO の復元 */

/*  保留：long long型を使用した場合  */
	lw      t0, 21*4(sp)    	/* Status */
	lw      t1, 22*4(sp)		/* EPC */

	lw      at,  1*4(sp)
	lw      v0,  2*4(sp)
	lw      v1,  3*4(sp)
	lw      a0,  4*4(sp)
	lw      a1,  5*4(sp)
	lw      a2,  6*4(sp)
	lw      a3,  7*4(sp)

	/*
	 *  多重例外時なので、タスクスイッチがない場合なので、タスクコンテキスト
	 *  の割込みマスクが変更されている心配はない。
	 *  （ステータスレジスタは元の値をそのまま書き戻す）
	 */
	mtc0    t0, Status
	mtc0    t1, EPC

	lw      t0,  8*4(sp)
	lw      t1,  9*4(sp)
	lw      t2, 10*4(sp)
	lw      t3, 11*4(sp)
	lw      t4, 12*4(sp)
	lw      t5, 13*4(sp)
	lw      t6, 14*4(sp)
	lw      t7, 15*4(sp)
	lw      t8, 16*4(sp)
	lw      t9, 17*4(sp)
	lw      gp, 18*4(sp)
	lw      fp, 19*4(sp)
	lw      ra, 20*4(sp)

	addi    sp, sp, 25*4		/* ERET 命令は、直後の命令を実行しない
					   ので遅延スロットには入れられない。*/

	eret				/* 例外からの復帰 */
	nop

/*
 *  割込みと例外の判別処理関数
 *    以下のレジスタに値が代入済み
 *      a0 : 例外ハンドラの引数, a1 : ステータスレジスタ
 */
proc_interrupt_and_exception:
	addi    sp, sp, -4
	sw      ra, 0(sp)

	mfc0    a2, Cause
	andi    t2, a2, ExcCode_mask
	beq     t2, zero, proc_interrupt        /* 割込みと例外の判別 */
		/* ここから直接proc_interrupt_sysへ飛びたいが、届かない
		   可能性があるので、一度proc_interruptに飛ぶ。 */
	nop

/* 割込み以外の例外の処理 */
	la      t4, exc_table
         	              		/* a1 = Status  */
	ori     t5, a1, SR_EXL
	xori    t5, t5, SR_EXL		/* EXLビットクリア */
		/* 割込み以外の例外はCPUロック状態でも受け付けるので、CPUロック
		   状態を保存する必要がある。そのためIEビットは操作せずに、割り
		   込み禁止・許可はEXLビットを用いて実現する */
	add     t6, t4, t2		/* 例外コード対応アドレスの設定 */
	lw      t7, (t6)		/* C言語ルーチンの呼出アドレス取得 */
	jalr    t7			/* C言語ルーチン呼び出し */
	mtc0    t5, Status		/* 割込み許可 (EXLビット = 0) */

	mfc0    t0, Status
	ori     t0, t0, SR_EXL
	mtc0    t0, Status              /* 割込み禁止 (EXLビット = 1) */

	NOP_FOR_CP0_HAZARD		/* CP0ハザードのための時間稼ぎマクロ
					   (このマクロはシステム依存部定義) */

	j       join_interrupt_and_exception	/* 例外・割込みの合流地点へ */
	nop

/* 割込みの場合の処理 */
proc_interrupt:

/*
 *  割込みコントローラの多様性に対応するため、実装はシステム依存部で行う。
 *    実装方法は、
 *	マクロ（PROC_INTERRUPT_SYS）または、関数呼出(proc_interrupt_sys)
 *    とする。
 *
 *  ・呼出時にセットされている引数
 *	a1 : ステータスレジスタ
 *	a2 : 原因レジスタ
 *
 *  ・実装時には、raレジスタを破壊してはならない。
 *
 *  ・実装する具体的な処理の流れ：
 *	1. 割込み要因の判別
 *	2. 割込みマスク（コア、割込みコントローラ）の設定
 *	3. 割込み許可
 *	4. Ｃ言語ルーチンの呼出
 *	5. 割込み禁止
 *	6. 割込みコントローラの割込み要求のクリア
 *     (7. join_interrupt_and_exceptionに戻る。; 関数呼出の場合)
 */

#ifdef PROC_INTERRUPT_SYS
	PROC_INTERRUPT_SYS
#else /* PROC_INTERRUPT_SYS */
	j	proc_interrupt_sys
	nop
#endif /* PROC_INTERRUPT_SYS */

/* 例外・割込み処理の合流地点 */
join_interrupt_and_exception:
	addi	k0, k0, -1		/* ネストカウンタ ディクリメント */
	lw      ra, (sp)		/* ra を復元 */
	jr      ra			/* proc_interrupt_and_exeption の呼出元
					   に戻る */
	addi    sp, sp, 4

/*
 *  割り込みハンドラ/CPU例外ハンドラ出口処理
 *
 *  戻り先がタスクコンテキストでreqflgがセットされている場合のみ、ここにくる。
 *  ・k0 = 0、割込み禁止状態、汎用レジスタを保存した状態で呼び出すこと。
 *  ・t1にruntskの値を入れてからret_int_1にジャンプすること。
 */

ret_int:
	POP_ICU_IPM			/* 割込みコントローラの割込みマスクの
					   復帰マクロ(システム依存部で定義) */

	lw      t1, 21*4(sp)		/* Status */
	ori	t1, t1, SR_EXL		/* 割込み処理に入るときの、CPUロック
					   状態を保持したいので、割込み禁止は
					   EXLビットを用いて行う。 */
	mtc0    t1, Status		/* MIPS3コアの割込みマスクの復帰
					    + EXLビットによる割込み禁止 */

	la      t0, enadsp
	la      t3, runtsk
	lw      t2, (t0)		/* t2 = enadsp */
	lw      t1, (t3)		/* t1 = runtsk */
 	beq     t2, zero, ret_int_1	/* ディスパッチ禁止ならジャンプ */
	nop
	la      t4, schedtsk
	lw      t5, (t4)		/* t5 = schedtsk */
	beq     t1, t5, ret_int_1	/* runtsk = schedtsk ならジャンプ */
	nop

/* ディスパッチャを経由する場合の処理 (t1 = runtsk に注意) */
	/* ここから、割込み禁止／許可を、EXLビットを用いて実現する方法に変更 */
	mfc0	t0, Status
	ori	t0, t0, SR_EXL_IE
	xori	t0, t0, SR_EXL_IE	/* lock_cpu 相当 (割込み禁止)
					    + EXLビットクリア (割込み許可) */
	mtc0	t0, Status

	addi    sp, sp, -8*4		/* スクラッチレジスタの保存 */
	sw      s0, 0*4(sp)
	sw      s1, 1*4(sp)
	sw      s2, 2*4(sp)
	sw      s3, 3*4(sp)
	sw      s4, 4*4(sp)
	sw      s5, 5*4(sp)
	sw      s6, 6*4(sp)
	sw      s7, 7*4(sp)
					/* t1 = runtsk に注意  */
	sw      sp, TCB_sp(t1)		/* runtsk->sp = sp
					   (タスクスタックポインタの保存) */
	la      t2, ret_int_r

	j       dispatcher		/* ディスパッチャ呼び出し */
	sw      t2, TCB_pc(t1)		/* runtsk->pc = ret_int_r
					   (実行再開番地の保存) */

/*
 * 割込み・例外の出口処理
 *   ・ディスパッチャ処理において、t1 = runtsk となっていることに注意
 *   ・スタックポインタも設定済み
 */
/* ディスパッチャ呼び出し後のタスクコンテキストからの合流地点 */
ret_int_r:
	lw      s0, 0*4(sp)		/* スクラッチレジスタの復元 */
	lw      s1, 1*4(sp)
	lw      s2, 2*4(sp)
	lw      s3, 3*4(sp)
 	lw      s4, 4*4(sp)
	lw      s5, 5*4(sp)
	lw      s6, 6*4(sp)
	lw      s7, 7*4(sp)
	addi    sp, sp, 8*4

	mfc0	t0, Status
	ori	t0, t0, SR_EXL_IE	/* unlock_cpu 相当 (割込み許可)
					    + EXLビットセット (割込み禁止) */
	mtc0	t0, Status

/* ディスパッチャを経由しない場合の合流地点 (t1 = runtsk に注意) */
ret_int_1:

	/* タスク例外処理要求のチェック */
	lb      t2, TCB_enatex(t1)
	andi    t4, t2, TCB_enatex_mask /* t3 = runtsk->enatex  */
	beq     t4, zero, ret_int_2     /* タスク例外処理禁止の時、ジャンプ */
	nop

	lw      t5, TCB_texptn(t1)      /* t5 = runtsk->texptn */
	beq     t5, zero, ret_int_2	/* タスク例外要求が無い時、ジャンプ */
	nop

	mfc0	t0, Status
	ori	t0, t0, SR_EXL_IE
	xori	t0, t0, SR_EXL_IE	/* lock_cpu相当 (割込み禁止)
					    + EXLビットクリア (割込み許可) */
	mtc0	t0, Status

	jal     call_texrtn             /* タスク例外処理ルーチン(ターゲット非
					   依存部定義)の呼び出し */
	nop
		/* call_texrtn()(task.c)では、実際のタスク例外処理ルーチン呼び
		   出し前後で、unlock_cpu、lock_cpu処理を行っているので、状態は
		   保持される。 */

	mfc0	t0, Status
	ori	t0, t0, SR_EXL_IE	/* unlock_cpu 相当 (割込み許可)
					    + EXLビットセット (割込み禁止) */
	mtc0	t0, Status
	/* ここまでが、割込み禁止／許可を、EXLビットを用いて実現する方法 */

ret_int_2:
	/* レジスタの復帰 (Statusレジスタは、既に復元済み) */
	lw      t1, 22*4(sp)		/* EPC */
	mtc0    t1, EPC

	lw      t2, 23*4(sp)		/* HI */
	lw      t3, 24*4(sp)		/* LO */

	mthi    t2			/* 乗除算レジスタ HI の復元 */
	mtlo    t3			/* 乗除算レジスタ LO の復元 */

	lw      at,  1*4(sp)
	lw      v0,  2*4(sp)
	lw      v1,  3*4(sp)
	lw      a0,  4*4(sp)
	lw      a1,  5*4(sp)
	lw      a2,  6*4(sp)
	lw      a3,  7*4(sp)
	lw      t0,  8*4(sp)
	lw      t1,  9*4(sp)
	lw      t2, 10*4(sp)
	lw      t3, 11*4(sp)
	lw      t4, 12*4(sp)
	lw      t5, 13*4(sp)
	lw      t6, 14*4(sp)
	lw      t7, 15*4(sp)
	lw      t8, 16*4(sp)
	lw      t9, 17*4(sp)

	lw      gp, 18*4(sp)
	lw      fp, 19*4(sp)
	lw      ra, 20*4(sp)

	addi    sp, sp, 25*4

	eret				/* 割込み/例外処理からの復帰 */
	nop

/*
 *  タスクディスパッチャ
 *  ・dispatch は、k0 = 0、CPUロック(割込み禁止)状態で呼び出さなければならない。
 *  ・exit_and_dispatch は，k0 = 0、CPUロック(割込み禁止)状態で呼び出すのが原則
 *    しかし、カーネル起動処理のため，k0 = 1で呼び出した場合にも対応している。
 */

	.global dispatch
	.global exit_and_dispatch

/* タスクディスパッチャを明示的に呼び出す時 */
dispatch:
	addi    sp, sp, -13*4
	sw      at,  1*4(sp)		/* スクラッチレジスタの待避 */
	sw      s0,  2*4(sp)
	sw      s1,  3*4(sp)
	sw      s2,  4*4(sp)
	sw      s3,  5*4(sp)
	sw      s4,  6*4(sp)
	sw      s5,  7*4(sp)
	sw      s6,  8*4(sp)
	sw      s7,  9*4(sp)
	sw      gp, 10*4(sp)
	sw      fp, 11*4(sp)
	sw      ra, 12*4(sp)

	la      t0, runtsk
	la      t2, dispatch_r
	lw      t1, (t0)		/* t1 = runtsk */
	sw      sp, TCB_sp(t1)		/* runtsk->sp = sp */
	j       dispatcher
	sw      t2, TCB_pc(t1)		/* runtsk->pc = dispatch_r */

/* 現在実行中のコンテキストを捨てて、タスクディスパッチャを呼び出す時 */
exit_and_dispatch:
	move    k0, zero		/* ネストカウンタをクリア */

dispatcher:
	/* ここには CPUロック（割り込み禁止）で来ること */
	la      t0, schedtsk
	lw      t1, (t0)		/* t1 = schedtsk */
	beq     t1, zero, dispatcher_2	/* schedtsk が無ければ割込み待ちへ */
	nop

dispatcher_1:
	la      t0, runtsk
	sw      t1, (t0)		/* runtsk = t1 (= schedtsk)
					   (タスクスイッチの実行) */
	lw      t2, TCB_pc(t1)		/* 実行再開番地を取得する。
					   実行再開番地は以下の3通り
					     ・ディスパッチャの出口  dispatch_r
					     ・割込み／例外の出口    ret_int_r
					     ・タスク起動直後        activate_r
					   いずれの場合も t1=runtsk として呼び
					   出していることに注意。 */
	jr      t2			/* 実行再開番地へジャンプ */
	lw      sp, TCB_sp(t1)		/* スタックポインタを復元 */

	/*
	 *  実行すべきタスクが現れるまで待つ処理
	 *
	 *  ここでコンテキストを切り換えるのは，
	 *    ・ここで発生する割込み処理にどのスタックを使うかという問題の解決
	 *    ・割込みハンドラ内でのタスクディスパッチの防止
	 *  という二つの意味がある．
	 */ 
dispatcher_2:
	la	t2, runtsk
	sw	zero, (t2)		/* runtsk = NULL
					   これは、iget_id が、TSK_NONEを返す
					   場合に対応するため。*/

	la      sp, _stack_top		/* スタック切り替え */
					/* _stack_top は、システム依存部
					   (リンカスクリプト推奨)定義 */
	li      k0, 1			/* ネストカウンタ＝1 */

	/* ループ内で使う定数の準備 */
	la      t2, reqflg              /* reqflg のアドレス */
	li      t3, ~SR_ERL_EXL         /* Status の割込み関連ビット */
	li      t4, ~SR_IE              /* Status の割込み許可ビット */

dispatcher_3:
	mfc0    t1, Status
	and     t1, t1, t3		/* ERL, EXL ビットクリア */
	ori     t1, t1, SR_IE		/* IEビットセット */
	mtc0    t1, Status		/* 割込み許可 : unlock_cpu 相当 */
	and     t1, t1, t4		/* IEビットリセット */
	mtc0    t1, Status		/* 割込み禁止 : lock_cpu 相当 */

	NOP_FOR_CP0_HAZARD		/* CP0ハザードのための時間稼ぎマクロ
					   (このマクロはシステム依存部定義) */

	lw      t5, (t2)		/* t5 = reqflg */

	beq     t5, zero, dispatcher_3	/* ディスパッチ要求がなければループの
					   先頭へ */
	nop
	sw      zero, (t2)		/* reqflg = NULL (reqflgをクリア) */
	j       dispatcher		/* 実行再開番地の取得処理へジャンプ */
	move    k0, zero		/* ネストカウンタをクリア */

/*
 *  ディスパッチャ出口処理
 *  ・ディスパッチャ処理において、t1 = runtsk となっていることに注意
 *  ・スタックポインタも設定済み
 */
dispatch_r: 
	lw      at,  1*4(sp)		/* スクラッチレジスタを復帰 */
	lw      s0,  2*4(sp)
	lw      s1,  3*4(sp)
	lw      s2,  4*4(sp)
	lw      s3,  5*4(sp)
	lw      s4,  6*4(sp)
	lw      s5,  7*4(sp)
	lw      s6,  8*4(sp)
	lw      s7,  9*4(sp)
	lw      gp, 10*4(sp)
	lw      fp, 11*4(sp)
	lw      ra, 12*4(sp)
	addi    sp, sp, 13*4

	/* タスク例外処理要求のチェック */
	lb      t2, TCB_enatex(t1)
	andi    t4, t2, TCB_enatex_mask	/* t4 = runtsk->enatex */
	beq     t4, zero, dispatch_r_1	/* タスク例外処理禁止の時、ジャンプ */
	nop

	lw      t5, TCB_texptn(t1)	/* t5 = runtsk->texptn */
	beq     t5, zero, dispatch_r_1	/* タスク例外要求が無い時、ジャンプ */
	nop

	j       call_texrtn		/* タスク例外処理ルーチン(ターゲット非
					   依存部定義)の呼び出し */
	nop
		/* call_texrtn()(task.c)では、実際のタスク例外処理ルーチン呼び
		   出し前後で、unlock_cpu、lock_cpu処理を行っているので、状態は
		   保持される。 */

dispatch_r_1:
	jr      ra			/* dispatch 呼び出し元へ戻る */
	nop

/*
 *  タスク起動処理
 *  ・ディスパッチャ処理において、t1 = runtsk となっていることに注意
 *  ・スタックポインタも設定済み
 */

	.globl activate_r

activate_r:
	/* ここに来るときは、CPUロックになっている（IEビット = 0) */

	/* 割り込み許可の準備 */
	mfc0    t0, Status
	ori     t0, t0, SR_ERL_EXL_IE   /* xorを使うため、1度ビットセットする */
	xori    t0, t0, SR_ERL_EXL      /* ERL, EXLビットをクリア
					   (IEビットは残す)  */

	/* タスク関係の設定準備 */
					/* t1 = runtsk (ディスパッチャで設定) */
	lw      t2, TCB_tinib(t1)       /* t2 = runtsk->tinib */

	/* タスク起動番地の設定 */
	lw      t3, TINIB_task(t2)      /* t3 = runtsk->tinib->task */

	/* タスクへの引数（拡張情報）設定 */
	lw      a0, TINIB_exinf(t2)     /* a0 = runtsk->tinib->exinf */

	mtc0    t0, Status              /* unlock_cpu 相当 (割込み許可) */

	la      ra, ext_tsk             /* タスクからの戻り番地を設定 */

	j       t3                      /* タスク起動 */
	nop
