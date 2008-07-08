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

/*
 *  プロセッサ依存モジュール(H8S用)
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/*
 *  タスクコンテキストでの割込みマスク
 */
#ifdef SUPPORT_CHG_IPM
IPM	task_intmask;		/* IPM -> UB：符号無し8ビット */
#endif /* SUPPORT_CHG_IPM */

/*
 *  非タスクコンテキストでの割込みマスク
 */
IPM	int_intmask;

/*
 *  割込みネストカウンタ
 */
UW	intnest;

/*
 *  プロセッサ依存の初期化
 */
void cpu_initialize() {

#ifdef SUPPORT_CHG_IPM
	/* タスクコンテキストでの割込みマスクの初期化 */
	task_intmask = 0x00;
#endif /* SUPPORT_CHG_IPM */

	/* 割込み／CPU例外ネストカウンタの初期化 */
	intnest = 1;

	/* 割込みレベルの初期化 */
	icu_set_ilv( (VP) IPRA, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRA, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRB, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRB, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRC, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRC, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRD, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRD, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRE, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRE, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRF, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRF, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRG, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRG, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRH, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRH, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRI, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRI, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRJ, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRJ, IPR_LOW, 0 );
	icu_set_ilv( (VP) IPRK, IPR_UPR, 0 );
	icu_set_ilv( (VP) IPRK, IPR_LOW, 0 );
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
	/* er0 = dlytim */
	Asm("		sub.l	%0, er0" :: "g"(SIL_DLY_TIM1) );
	Asm("		ble	sil_dly_nse_2");
				/* dlytim < SIL_DLY_TIM1 ならループ終了 */
	Asm("	sil_dly_nse_1:");
	Asm("		sub.l	%0, er0" :: "g"(SIL_DLY_TIM2) );
	Asm("		bgt	sil_dly_nse_1");
				/* dlytim > SIL_DLY_TIM2 ならループ続行 */
	Asm("	sil_dly_nse_2:");
}

#ifdef SUPPORT_CHG_IPM

/*
 *  割込みマスクの変更
 *
 *  ・chg_ipm を使って、割込みマスクを MAX_IPM （NMI スタブリモートブレーク 以外
 *    のすべての割込みを禁止）以上に変更することはできません。NMI スタブリモート
 *    ブレーク以外のすべての割込みを禁止したい場合には、loc_cpuによりCPUロック
 *    状態にしてください。
 *  ・割込みマスクが 0 以外の時であっても、タスクディスパッチは保留されません。
 *    このとき、割込みマスクの状態は、タスクディスパッチによって新しく実行状態に
 *    なったタスクへ、その都度、引き継がれます。
 *    このため、タスクが実行中に、割込みが発生し、ディスパッチした後の別のタスク
 *    によって割込みマスクが変更される場合があります。
 *    TOPPERS/JSP カーネルでは、割込みマスクの変更はタスク例外処理ルーチンによっ
 *    ても起こるので，これによって扱いが難しくなる状況は少ないと思われます。
 *  ・割込みマスクの値によってタスクディスパッチを禁止したい場合には、dis_dsp を
 *    併用してください。
 */
SYSCALL ER
chg_ipm(IPM ipm)
{
	ER	ercd;

	LOG_CHG_IPM_ENTER(ipm);
	CHECK_TSKCTX_UNL();
	CHECK_PAR( /*(0 <= ipm) &&*/ (ipm <= MAX_IPM) );
	/* IPM型 は、unsigned で定義しているので、コメントアウトして
	   いるところは、常に成立しているため、Warnning が発生する。
	   しかし、可読性の問題から、明示的に記述しておく。 */

	t_lock_cpu();
	task_intmask = ipm;
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_CHG_IPM_LEAVE(ercd)
	return(ercd);
}

/*
 *  割込みマスクの参照
 */
SYSCALL ER
get_ipm(IPM *p_ipm)
{
	ER	ercd;

	LOG_GET_IPM_ENTER(p_ipm);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_ipm = task_intmask;
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
 * 登録されていない割り込みが発生すると呼び出される
 */
void cpu_experr(EXCSTACK *sp)
{
	syslog( LOG_EMERG, "Unexpected interrupt." );
	syslog( LOG_EMERG, "SP  = %08x PC  = %08x CCR = %08x",
				sp - 34, sp->pc,  sp->ccr );
	syslog( LOG_EMERG, "ER0 = %08x ER1 = %08x ER2 = %08x ER3 = %08x",
				sp->er0, sp->er1, sp->er2, sp->er3 );
	syslog( LOG_EMERG, "ER4 = %08x ER5 = %08x ER6 = %08x ",
				sp->er4, sp->er5, sp->er6 );

	while(1);
}

/*
 *  メモリブロック操作ライブラリ
 *	(ItIsからの流用)
 *
 *  関数の仕様は，ANSI C ライブラリの仕様と同じ．標準ライブラリのものを
 *  使った方が効率が良い可能性がある．
 *
 */
VP _dummy_memcpy(VP dest, VP src, UINT len)
{
	VB	*d = (VB *)dest;
	VB	*s = (VB *)src;

	while (len-- > 0) {
		*d++ = *s++;
	}

	return(dest);
}
