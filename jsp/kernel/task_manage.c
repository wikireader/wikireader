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
 *  @(#) $Id: task_manage.c,v 1.9 2007/04/22 18:04:06 hiro Exp $
 */

/*
 *	タスク管理機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "wait.h"

/*
 *  タスクの起動
 */
#ifdef __act_tsk

SYSCALL ER
act_tsk(ID tskid)
{
	TCB	*tcb;
	ER	ercd;

	LOG_ACT_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(tcb->tstat)) {
		if (make_active(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (!(tcb->actcnt)) {
		tcb->actcnt = TRUE;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	t_unlock_cpu();

    exit:
	LOG_ACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __act_tsk */

/*
 *  タスクの起動（非タスクコンテキスト用）
 */
#ifdef __iact_tsk

SYSCALL ER
iact_tsk(ID tskid)
{
	TCB	*tcb;
	ER	ercd;

	LOG_IACT_TSK_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	tcb = get_tcb(tskid);

	i_lock_cpu();
	if (TSTAT_DORMANT(tcb->tstat)) {
		if (make_active(tcb)) {
			reqflg = TRUE;
		}
		ercd = E_OK;
	}
	else if (!(tcb->actcnt)) {
		tcb->actcnt = TRUE;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	i_unlock_cpu();

    exit:
	LOG_IACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __iact_tsk */

/*
 *  タスク起動要求のキャンセル
 */
#ifdef __can_act

SYSCALL ER_UINT
can_act(ID tskid)
{
	TCB	*tcb;
	ER_UINT	ercd;

	LOG_CAN_ACT_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	tcb = get_tcb_self(tskid);

	t_lock_cpu();
	ercd = tcb->actcnt ? 1 : 0;
	tcb->actcnt = FALSE;
	t_unlock_cpu();

    exit:
	LOG_CAN_ACT_LEAVE(ercd);
	return(ercd);
}

#endif /* __can_act */

/*
 *  自タスクの終了
 */
#ifdef __ext_tsk

SYSCALL void
ext_tsk(void)
{
	LOG_EXT_TSK_ENTER();

#ifdef ACTIVATED_STACK_SIZE
	/*
	 *  create_context と activate_context で，使用中のスタック領
	 *  域を破壊しないように，スタック上にダミー領域を確保する．
	 */
	(void) alloca(ACTIVATED_STACK_SIZE);
#endif /* ACTIVATED_STACK_SIZE */

	if (sense_context()) {
		/*
		 *  非タスクコンテキストから ext_tsk が呼ばれた場合，
		 *  システムログにエラーを記録し，そのまま実行を続ける
		 *  が, 動作は保証されない．
		 */
		syslog_0(LOG_EMERG,
			"ext_tsk is called from non-task contexts.");
	}
	if (sense_lock()) {
		/*
		 *  CPUロック状態で ext_tsk が呼ばれた場合は，CPUロック
		 *  を解除してからタスクを終了する．実装上は，サービス
		 *  コール内でのCPUロックを省略すればよいだけ．
		 */
		syslog_0(LOG_WARNING,
			"ext_tsk is called from CPU locked state.");
	}
	else {
		if (sense_context()) {
			i_lock_cpu();
		}
		else  {
			t_lock_cpu();
		}
	}
	if (!(enadsp)) {
		/*
		 *  ディスパッチ禁止状態で ext_tsk が呼ばれた場合は，
		 *  ディスパッチ許可状態にしてからタスクを終了する．
		 */
		syslog_0(LOG_WARNING,
			"ext_tsk is called from dispatch disabled state.");
		enadsp = TRUE;
	}
	exit_task();
}

#endif /* __ext_tsk */

/*
 *  タスクの強制終了
 */
#ifdef __ter_tsk

SYSCALL ER
ter_tsk(ID tskid)
{
	TCB	*tcb;
	UINT	tstat;
	ER	ercd;

	LOG_TER_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	tcb = get_tcb(tskid);
	CHECK_NONSELF(tcb);

	t_lock_cpu();
	if (TSTAT_DORMANT(tstat = tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		if (TSTAT_RUNNABLE(tstat)) {
			make_non_runnable(tcb);
		}
		else if (TSTAT_WAITING(tstat)) {
			wait_cancel(tcb);
		}
		make_dormant(tcb);
		if (tcb->actcnt) {
			tcb->actcnt = FALSE;
			if (make_active(tcb)) {
				dispatch();
			}
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_TER_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* __ter_tsk */

/*
 *  タスク優先度の変更
 */
#ifdef __chg_pri

SYSCALL ER
chg_pri(ID tskid, PRI tskpri)
{
	TCB	*tcb;
	UINT	newpri;
	UINT	tstat;
	ER	ercd;

	LOG_CHG_PRI_ENTER(tskid, tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	CHECK_TPRI_INI(tskpri);
	tcb = get_tcb_self(tskid);
	newpri = (tskpri == TPRI_INI) ? tcb->tinib->ipriority
					: INT_PRIORITY(tskpri);

	t_lock_cpu();
	if (TSTAT_DORMANT(tstat = tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if (TSTAT_RUNNABLE(tstat)) {
		if (change_priority(tcb, newpri)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		tcb->priority = newpri;
		if ((tstat & TS_WAIT_WOBJCB) != 0) {
			wobj_change_priority(((WINFO_WOBJ *)(tcb->winfo))
							->wobjcb, tcb);
		}
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_CHG_PRI_LEAVE(ercd);
	return(ercd);
}

#endif /* __chg_pri */

/*
 *  タスク優先度の参照
 */
#ifdef __get_pri

SYSCALL ER
get_pri(ID tskid, PRI *p_tskpri)
{
	TCB	*tcb;
	ER	ercd;

	LOG_GET_PRI_ENTER(tskid, p_tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	tcb = get_tcb_self(tskid);

	t_lock_cpu();
	if (TSTAT_DORMANT(tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		*p_tskpri = EXT_TSKPRI(tcb->priority);
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_GET_PRI_LEAVE(ercd, *p_tskpri);
	return(ercd);
}

#endif /* __get_pri */
