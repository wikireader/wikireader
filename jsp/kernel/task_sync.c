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
 *  @(#) $Id: task_sync.c,v 1.7 2003/12/20 08:12:24 hiro Exp $
 */

/*
 *	タスク付属同期機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "wait.h"

/*
 *  起床待ち
 */
#ifdef __slp_tsk

SYSCALL ER
slp_tsk()
{
	WINFO	winfo;
	ER	ercd;

	LOG_SLP_TSK_ENTER();
	CHECK_DISPATCH();

	t_lock_cpu();
	if (runtsk->wupcnt) {
		runtsk->wupcnt = FALSE;
		ercd = E_OK;
	}
	else {
		runtsk->tstat = (TS_WAITING | TS_WAIT_SLEEP);
		make_wait(&winfo);
		LOG_TSKSTAT(runtsk);
		dispatch();
		ercd = winfo.wercd;
	}
	t_unlock_cpu();

    exit:
	LOG_SLP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __slp_tsk */

/*
 *  起床待ち（タイムアウトあり）
 */
#ifdef __tslp_tsk

SYSCALL ER
tslp_tsk(TMO tmout)
{
	WINFO	winfo;
	TMEVTB	tmevtb;
	ER	ercd;

	LOG_TSLP_TSK_ENTER(tmout);
	CHECK_DISPATCH();
	CHECK_TMOUT(tmout);

	t_lock_cpu();
	if (runtsk->wupcnt) {
		runtsk->wupcnt = FALSE;
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		runtsk->tstat = (TS_WAITING | TS_WAIT_SLEEP);
		make_wait_tmout(&winfo, &tmevtb, tmout);
		LOG_TSKSTAT(runtsk);
		dispatch();
		ercd = winfo.wercd;
	}
	t_unlock_cpu();

    exit:
	LOG_TSLP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __tslp_tsk */

/*
 *  タスクの起床
 */
#ifdef __wup_tsk

SYSCALL ER
wup_tsk(ID tskid)
{
	TCB	*tcb;
	UINT	tstat;
	ER	ercd;

	LOG_WUP_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(tstat = tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if ((tstat & TS_WAIT_SLEEP) != 0) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (!(tcb->wupcnt)) {
		tcb->wupcnt = TRUE;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	t_unlock_cpu();

    exit:
	LOG_WUP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __wup_tsk */

/*
 *  タスクの起床（非タスクコンテキスト用）
 */
#ifdef __iwup_tsk

SYSCALL ER
iwup_tsk(ID tskid)
{
	TCB	*tcb;
	UINT	tstat;
	ER	ercd;

	LOG_IWUP_TSK_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	tcb = get_tcb(tskid);

	i_lock_cpu();
	if (TSTAT_DORMANT(tstat = tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if ((tstat & TS_WAIT_SLEEP) != 0) {
		if (wait_complete(tcb)) {
			reqflg = TRUE;
		}
		ercd = E_OK;
	}
	else if (!(tcb->wupcnt)) {
		tcb->wupcnt = TRUE;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	i_unlock_cpu();

    exit:
	LOG_IWUP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __iwup_tsk */

/*
 *  タスク起床要求のキャンセル
 */
#ifdef __can_wup

SYSCALL ER_UINT
can_wup(ID tskid)
{
	TCB	*tcb;
	ER_UINT	ercd;

	LOG_CAN_WUP_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		ercd = tcb->wupcnt ? 1 : 0;
		tcb->wupcnt = FALSE;
	}
	t_unlock_cpu();

    exit:
	LOG_CAN_WUP_LEAVE(ercd);
	return(ercd);
}

#endif /* __can_wup */

/*
 *  待ち状態の強制解除
 */
#ifdef __rel_wai

SYSCALL ER
rel_wai(ID tskid)
{
	TCB	*tcb;
	ER	ercd;

	LOG_REL_WAI_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	tcb = get_tcb(tskid);

	t_lock_cpu();
	if (!(TSTAT_WAITING(tcb->tstat))) {
		ercd = E_OBJ;
	}
	else {
		if (wait_release(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_REL_WAI_LEAVE(ercd);
	return(ercd);
}

#endif /* __rel_wai */

/*
 *  待ち状態の強制解除（非タスクコンテキスト用）
 */
#ifdef __irel_wai

SYSCALL ER
irel_wai(ID tskid)
{
	TCB	*tcb;
	ER	ercd;

	LOG_IREL_WAI_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	tcb = get_tcb(tskid);

	i_lock_cpu();
	if (!(TSTAT_WAITING(tcb->tstat))) {
		ercd = E_OBJ;
	}
	else {
		if (wait_release(tcb)) {
			reqflg = TRUE;
		}
		ercd = E_OK;
	}
	i_unlock_cpu();

    exit:
	LOG_IREL_WAI_LEAVE(ercd);
	return(ercd);
}

#endif /* __irel_wai */

/*
 *  強制待ち状態への移行
 */
#ifdef __sus_tsk

SYSCALL ER
sus_tsk(ID tskid)
{
	TCB	*tcb;
	UINT	tstat;
	ER	ercd;

	LOG_SUS_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (tcb == runtsk && !(enadsp)) {
		ercd = E_CTX;
	}
	else if (TSTAT_DORMANT(tstat = tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if (TSTAT_RUNNABLE(tstat)) {
		/*
		 *  実行できる状態から強制待ち状態への遷移
		 */
		tcb->tstat = TS_SUSPENDED;
		LOG_TSKSTAT(tcb);
		if (make_non_runnable(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (TSTAT_SUSPENDED(tstat)) {
		ercd = E_QOVR;
	}
	else {
		/*
		 *  待ち状態から二重待ち状態への遷移
		 */
		tcb->tstat |= TS_SUSPENDED;
		LOG_TSKSTAT(tcb);
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_SUS_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __sus_tsk */

/*
 *  強制待ち状態からの再開
 */
#ifdef __rsm_tsk

SYSCALL ER
rsm_tsk(ID tskid)
{
	TCB	*tcb;
	UINT	tstat;
	ER	ercd;

	LOG_RSM_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	tcb = get_tcb(tskid);

	t_lock_cpu();
	if (!(TSTAT_SUSPENDED(tstat = tcb->tstat))) {
		ercd = E_OBJ;
	}
	else if (!(TSTAT_WAITING(tstat))) {
		/*
		 *  強制待ち状態から実行できる状態への遷移
		 */
		if (make_runnable(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		/*
		 *  二重待ち状態から待ち状態への遷移
		 */
		tcb->tstat &= ~TS_SUSPENDED;
		LOG_TSKSTAT(tcb);
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_RSM_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __rsm_tsk */

/*
 *  強制待ち状態からの強制再開
 *
 *  JSPカーネルでは，frsm_tsk と rsm_tsk は同一の処理となる．frsm_tsk 
 *  が呼ばれると，frsm_tsk と rsm_tsk の両方のサービスコールのトレース
 *  ログが出力される．ログ取得後に rsm_tsk のトレースログを削除するこ
 *  とが必要である．rsm_tsk のトレースログを正しく削除するためには，タ
 *  スクディスパッチのログと，タスク例外処理のログも取得することが必要
 *  となるので，注意が必要である．
 */
#ifdef __frsm_tsk

SYSCALL ER
frsm_tsk(ID tskid)
{
	ER	ercd;

	LOG_FRSM_TSK_ENTER(tskid);
	ercd = rsm_tsk(tskid);
	LOG_FRSM_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __frsm_tsk */

/*
 *  自タスクの遅延
 */
#ifdef __dly_tsk

SYSCALL ER
dly_tsk(RELTIM dlytim)
{
	WINFO	winfo;
	TMEVTB	tmevtb;
	ER	ercd;

	LOG_DLY_TSK_ENTER(dlytim);
	CHECK_DISPATCH();
	CHECK_PAR(dlytim <= TMAX_RELTIM);

	t_lock_cpu();
	runtsk->tstat = TS_WAITING;
	make_non_runnable(runtsk);
	runtsk->winfo = &winfo;
	winfo.tmevtb = &tmevtb;
	tmevtb_enqueue(&tmevtb, dlytim, (CBACK) wait_tmout_ok, (VP) runtsk);
	LOG_TSKSTAT(runtsk);
	dispatch();
	ercd = winfo.wercd;
	t_unlock_cpu();

    exit:
	LOG_DLY_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __dly_tsk */
