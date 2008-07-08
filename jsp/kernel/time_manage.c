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
 *  @(#) $Id: time_manage.c,v 1.11 2005/11/12 14:56:15 hiro Exp $
 */

/*
 *	システム時刻管理機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "time_event.h"

/*
 *  システム時刻の設定
 */
#ifdef __set_tim

SYSCALL ER
set_tim(const SYSTIM *p_systim)
{
	ER	ercd;

	LOG_SET_TIM_ENTER(p_systim);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	systim_offset = *p_systim - current_time;
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_SET_TIM_LEAVE(ercd);
	return(ercd);
}

#endif /* __set_tim */

/*
 *  システム時刻の参照
 */
#ifdef __get_tim

SYSCALL ER
get_tim(SYSTIM *p_systim)
{
	ER	ercd;

	LOG_GET_TIM_ENTER(p_systim);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_systim = systim_offset + current_time;
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_GET_TIM_LEAVE(ercd, *p_systim);
	return(ercd);
}

#endif /* __get_tim */

/*
 *  性能評価用システム時刻の参照
 */
#ifdef __vxget_tim
#ifdef SUPPORT_VXGET_TIM
#include <hw_timer.h>

SYSCALL ER
vxget_tim(SYSUTIM *p_sysutim)
{
	SYSUTIM	utime;
	SYSTIM	time;
#if TIC_DENO != 1
	INT	subtime;
#endif /* TIC_DENO != 1 */
	CLOCK	clock;
	BOOL	ireq;
	BOOL	locked;
	ER	ercd;

	LOG_VXGET_TIM_ENTER(p_sysutim);
	CHECK_TSKCTX();

	locked = sense_lock();
	if (!(locked)) {
		t_lock_cpu();
	}
	time = systim_offset + next_time;
#if TIC_DENO != 1
	subtime = (INT) next_subtime;
#endif /* TIC_DENO != 1 */
	clock = hw_timer_get_current();
	ireq = hw_timer_fetch_interrupt();
	if (!(locked)) {
		t_unlock_cpu();
	}

	utime = ((SYSUTIM) time) * 1000;
#if TIC_DENO != 1
	utime += subtime * 1000 / TIC_DENO;
#endif /* TIC_DENO != 1 */
	if (!(ireq && !(BEFORE_IREQ(clock)))) {
		utime -= TIC_NUME * 1000 / TIC_DENO;
	}
	utime += TO_USEC(clock);
	*p_sysutim = utime;
	ercd = E_OK;

    exit:
	LOG_VXGET_TIM_LEAVE(ercd, *p_sysutim);
	return(ercd);
}

#endif /* SUPPORT_VXGET_TIM */
#endif /* __vxget_tim */
