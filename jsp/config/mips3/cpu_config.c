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

/*
 *  プロセッサ依存モジュール（MIPS3用）
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/*
 *  割込みハンドラ／割込みマスクの擬似テーブル
 */
INT_TABLE int_table[ TMAX_ALL_INTNO ];

/*
 *  例外ハンドラの擬似テーブル
 */
FP exc_table[ TMAX_CORE_EXCNO ];

/*
 *  プロセッサ依存の初期化
 */
void cpu_initialize() {

	int i;

	/* 割込みハンドラ／割込みマスクの擬似テーブル初期化 */
	for( i = 0; i < TMAX_ALL_INTNO; i++ ) {
		define_inh( i, (FP) &cpu_experr );
	}

	/* 例外ベクタの擬似テーブル初期化 */
	for( i = 0; i < TMAX_CORE_EXCNO; i++ ) {
		define_exc( i, (FP) &cpu_experr );
	}

}

/*
 *  プロセッサ依存の終了処理
 */
void cpu_terminate() {
}

/*
 *  微少時間待ち
 */
void sil_dly_nse(UINT dlytim) {

	/* $2 : v0, $3 : v1 */
	Asm("	move	$2, %0" :: "r"(dlytim) );
	Asm("	li	$3, %0" :: "g"(SIL_DLY_TIM1) );

	Asm("	sub	$2, $2, $3");	/* v0 -= v1 (dlytim -= SIL_DLY_TIM1) */
	Asm("	blez    $2, sil_dly_nse_2");	/* v0 <= 0 ならリターン */

	Asm("	li      $3, %0" :: "g"(SIL_DLY_TIM2) );

	Asm("sil_dly_nse_1:");
	Asm("	sub     $2, $2, $3");	/* v0 -= v1 (dlytim -= SIL_DLY_TIM2) */
	Asm("	bgtz    $2, sil_dly_nse_1");	/* v0 > 0 ならループ */

	Asm("sil_dly_nse_2:");
}

#ifdef SUPPORT_CHG_IPM

/*
 *  割込みマスクの変更
 *
 *  割込みマスクは、タスクディスパッチによって、新しく実行状態になったタスクに
 *  引き継がれる。そのため、タスクが実行中に、別のタスクによって割込みマスクが
 *  変更される場合がある。JSPカーネルでは、割込みマスクの変更はタスク例外処理
 *  ルーチンによっても起こるが、これによって扱いが難しくなる状況は少ないと思わ
 *  れる。割込みマスクの値によってタスクディスパッチを禁止したい場合には、
 *  dis_dsp を併用すればよい。
 *  MIPS3ターゲットでは、MIPS3コアの割込みマスクだけでなく、割込みコントローラ
 *  の割込みコントローラも扱っているので注意。
 */

SYSCALL ER chg_ipm(IPM ipm) {

	ER	ercd;

	LOG_CHG_IPM_ENTER(ipm);
	CHECK_TSKCTX_UNL();
	CHECK_IPM(ipm);

	t_lock_cpu();
	cpu_set_ipm( ipm.core );	/* MIPS3コアの割込みマスクの設定 */
	icu_set_ipm( &(ipm.icu) );	/* 外部割込みコントローラの割込みマスク
					   の設定 */
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_CHG_IPM_LEAVE(ercd);
	return(ercd);
}

/*
 *  割込みマスクの参照
 */
SYSCALL ER get_ipm(IPM *p_ipm) {

	ER	ercd;

	LOG_GET_IPM_ENTER(p_ipm);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	p_ipm->core = cpu_get_ipm();	/* MIPS3コアの割込みマスク参照 */
	icu_get_ipm(&(p_ipm->icu));	/* 外部割込みコントローラの割込みマスク
					   の参照 */
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_GET_IPM_LEAVE(ercd, *p_ipm);
	return(ercd);
}

#endif /* SUPPORT_CHG_IPM */

/*============================================================================*/
/*  共通ドキュメントにはない、独自の部分  */

/*
 * ハンドラが登録されていない割込み・例外が発生すると呼び出される
 */
void cpu_experr( EXCSTACK *sp, UW SR, UW CR) {

    syslog_0(LOG_EMERG, "Interrupt/Exception error occurs.");

    syslog_1(LOG_EMERG, "PC(EPC;CP0_r14) = 0x%08x",
    			sp->CP0_EPC);
    syslog_2(LOG_EMERG, "SR(Status;CP0_r12) = 0x%08x CR(Cause;CP0_r13) = 0x%08x",
    			SR, CR);	/* SRでも、sp->CP0_Statusでも可 */
    syslog_3(LOG_EMERG, "at(r1 ) = %08x v0(r2 ) = %08x v1(r3 ) = %08x",
    			sp->at, sp->v0, sp->v1);
    syslog_4(LOG_EMERG, "a0(r4 ) = %08x a1(r5 ) = %08x a2(r6 ) = %08x a3(r7 ) = %08x",
    			sp->a0, sp->a1, sp->a2, sp->a3);
    syslog_4(LOG_EMERG, "t0(r8 ) = %08x t1(r9 ) = %08x t2(r10) = %08x t3(r11) = %08x",
    			sp->t0, sp->t1, sp->t2, sp->t3);
    syslog_4(LOG_EMERG, "t4(r12) = %08x t5(r13) = %08x t6(r14) = %08x t7(r15) = %08x",
    			sp->t4, sp->t5, sp->t6, sp->t7);
    syslog_2(LOG_EMERG, "t8(r24) = %08x t9(r25) = %08x",
    			sp->t8, sp->t9);
    syslog_2(LOG_EMERG, "HI = %08x LO = %08x",
    			sp->hi, sp->lo);
    syslog_4(LOG_EMERG, "gp(r28) = %08x sp(r29) = %08x fp(r30) = %08x ra(r31) = %08x",
    			sp->gp, sp->sp, sp->fp, sp->ra);
    while(1);
}

/*
 *  メモリブロック操作ライブラリ（リンクスクリプト内で使用）
 *	(ItIsからの流用)
 *
 *  関数の仕様は，ANSI C ライブラリの仕様と同じ．標準ライブラリのものを
 *  使った方が効率が良い可能性がある．
 */
VP _dummy_memcpy(VP dest, VP src, UINT len) {

	VB	*d = dest;
	VB	*s = src;

	while (len-- > 0) {
		*d++ = *s++;
	}

	return(dest);
}
