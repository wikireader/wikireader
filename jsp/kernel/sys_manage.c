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
 *  @(#) $Id: sys_manage.c,v 1.8 2003/07/01 13:30:35 hiro Exp $
 */

/*
 *	システム状態管理機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/*
 *  タスクの優先順位の回転
 */
#ifdef __rot_rdq

SYSCALL ER
rot_rdq(PRI tskpri)
{
	UINT	pri;
	ER	ercd;

	LOG_ROT_RDQ_ENTER(tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TPRI_SELF(tskpri);

	t_lock_cpu();
	pri = (tskpri == TPRI_SELF) ? runtsk->priority : INT_PRIORITY(tskpri);
	if (rotate_ready_queue(pri)) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_ROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __rot_rdq */

/*
 *  タスクの優先順位の回転（非タスクコンテキスト用）
 */
#ifdef __irot_rdq

SYSCALL ER
irot_rdq(PRI tskpri)
{
	ER	ercd;

	LOG_IROT_RDQ_ENTER(tskpri);
	CHECK_INTCTX_UNL();
	CHECK_TPRI(tskpri);

	i_lock_cpu();
	if (rotate_ready_queue(INT_PRIORITY(tskpri))) {
		reqflg = TRUE;
	}
	ercd = E_OK;
	i_unlock_cpu();

    exit:
	LOG_IROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __irot_rdq */

/*
 *  実行状態のタスクIDの参照
 */
#ifdef __get_tid

SYSCALL ER
get_tid(ID *p_tskid)
{
	ER	ercd;

	LOG_GET_TID_ENTER(p_tskid);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_tskid = TSKID(runtsk);
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_GET_TID_LEAVE(ercd, *p_tskid);
	return(ercd);
}

#endif /* __get_tid */

/*
 *  実行状態のタスクIDの参照（非タスクコンテキスト用）
 */
#ifdef __iget_tid

SYSCALL ER
iget_tid(ID *p_tskid)
{
	ER	ercd;

	LOG_IGET_TID_ENTER(p_tskid);
	CHECK_INTCTX_UNL();

	i_lock_cpu();
	*p_tskid = (runtsk == NULL) ? TSK_NONE : TSKID(runtsk);
	ercd = E_OK;
	i_unlock_cpu();

    exit:
	LOG_IGET_TID_LEAVE(ercd, *p_tskid);
	return(ercd);
}

#endif /* __iget_tid */

/*
 *  CPUロック状態への移行
 */
#ifdef __loc_cpu

SYSCALL ER
loc_cpu(void)
{
	ER	ercd;

	LOG_LOC_CPU_ENTER();
	CHECK_TSKCTX();

	if (!(t_sense_lock())) {
		t_lock_cpu();
	}
	ercd = E_OK;

    exit:
	LOG_LOC_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* __loc_cpu */

/*
 *  CPUロック状態への移行（非タスクコンテキスト用）
 */
#ifdef __iloc_cpu

SYSCALL ER
iloc_cpu(void)
{
	ER	ercd;

	LOG_ILOC_CPU_ENTER();
	CHECK_INTCTX();

	if (!(i_sense_lock())) {
		i_lock_cpu();
	}
	ercd = E_OK;

    exit:
	LOG_ILOC_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* __iloc_cpu */

/*
 *  CPUロック状態の解除
 *
 *  CPUロック中は，ディスパッチが必要となるサービスコールを呼び出すこ
 *  とはできないため，CPUロック状態の解除時にディスパッチャを起動する
 *  必要はない．
 */
#ifdef __unl_cpu

SYSCALL ER
unl_cpu(void)
{
	ER	ercd;

	LOG_UNL_CPU_ENTER();
	CHECK_TSKCTX();

	if (t_sense_lock()) {
		t_unlock_cpu();
	}
	ercd = E_OK;

    exit:
	LOG_UNL_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* __unl_cpu */

/*
 *  CPUロック状態の解除（非タスクコンテキスト用）
 *
 *  CPUロック中は，ディスパッチが必要となるサービスコールを呼び出すこ
 *  とはできないため，CPUロック状態の解除時にディスパッチャの起動を要
 *  求する必要はない．
 */
#ifdef __iunl_cpu

SYSCALL ER
iunl_cpu(void)
{
	ER	ercd;

	LOG_IUNL_CPU_ENTER();
	CHECK_INTCTX();

	if (i_sense_lock()) {
		i_unlock_cpu();
	}
	ercd = E_OK;

    exit:
	LOG_IUNL_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* __iunl_cpu */

/*
 *  ディスパッチの禁止
 */
#ifdef __dis_dsp

SYSCALL ER
dis_dsp(void)
{
	ER	ercd;

	LOG_DIS_DSP_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	enadsp = FALSE;
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_DIS_DSP_LEAVE(ercd);
	return(ercd);
}

#endif /* __dis_dsp */

/*
 *  ディスパッチの許可
 */
#ifdef __ena_dsp

SYSCALL ER
ena_dsp(void)
{
	ER	ercd;

	LOG_ENA_DSP_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	enadsp = TRUE;
	if (runtsk != schedtsk) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_ENA_DSP_LEAVE(ercd);
	return(ercd);
}

#endif /* __ena_dsp */

/*
 *  コンテキストの参照
 */
#ifdef __sns_ctx

SYSCALL BOOL
sns_ctx(void)
{
	BOOL	state;

	LOG_SNS_CTX_ENTER();
	state = sense_context() ? TRUE : FALSE;
	LOG_SNS_CTX_LEAVE(state);
	return(state);
}

#endif /* __sns_ctx */

/*
 *  CPUロック状態の参照
 */
#ifdef __sns_loc

SYSCALL BOOL
sns_loc(void)
{
	BOOL	state;

	LOG_SNS_LOC_ENTER();
	state = sense_lock() ? TRUE : FALSE;
	LOG_SNS_LOC_LEAVE(state);
	return(state);
}

#endif /* __sns_loc */

/*
 *  ディスパッチ禁止状態の参照
 */
#ifdef __sns_dsp

SYSCALL BOOL
sns_dsp(void)
{
	BOOL	state;

	LOG_SNS_DSP_ENTER();
	state = !(enadsp) ? TRUE : FALSE;
	LOG_SNS_DSP_LEAVE(state);
	return(state);
}

#endif /* __sns_dsp */

/*
 *  ディスパッチ保留状態の参照
 */
#ifdef __sns_dpn

SYSCALL BOOL
sns_dpn(void)
{
	BOOL	state;

	LOG_SNS_DPN_ENTER();
	state = (sense_context() || sense_lock() || !(enadsp)) ? TRUE : FALSE;
	LOG_SNS_DPN_LEAVE(state);
	return(state);
}

#endif /* __sns_dpn */

/*
 *  カーネル動作状態の参照
 */
#ifdef __vsns_ini

SYSCALL BOOL
vsns_ini(void)
{
	BOOL	state;

	LOG_VSNS_INI_ENTER();
	state = !(iniflg) ? TRUE : FALSE;
	LOG_VSNS_INI_LEAVE(state);
	return(state);
}

#endif /* __vsns_ini */
