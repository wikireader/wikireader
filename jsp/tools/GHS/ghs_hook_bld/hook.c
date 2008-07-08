/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                2003      by Advanced Data Controls, Corp
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
 *  @(#) $Id: hook.c,v 1.2 2003/12/19 11:48:37 honda Exp $
 */

#ifdef GHS_HOOK
#include "jsp_kernel.h"
#include "kernel.h"
#include "task.h"
#include "ghs_hook.h"

void sys_hook0(int grp, int code)
{
    ID tskid=TSKID(runtsk);
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (tskid > TRACE_TNUM_TSKID || !(Ros_buf.hook.task_id[tskid]))
        return;
#endif
    t_lock_cpu();
    ghs_hook4(THG_SYS, tskid, code);
    t_unlock_cpu();
}

void sys_hook0_i(int grp, int code)
{
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (!(Ros_buf.hook.task_id[0]))
        return;
#endif
    t_lock_cpu();
    ghs_hook4(THG_SYS, 0, code);
    t_unlock_cpu();
}

void sys_hook1(int grp, int code, int p1)
{
    ID tskid = TSKID(runtsk);
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (tskid > TRACE_TNUM_TSKID || !(Ros_buf.hook.task_id[tskid]))
	return;
#endif
    t_lock_cpu();
    ghs_hook5(THG_SYS, tskid, code, p1);
    t_unlock_cpu();
}

void sys_hook1_i(int grp, int code, int p1)
{
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (!(Ros_buf.hook.task_id[0]))
        return;
#endif
    t_lock_cpu();
    ghs_hook5(THG_SYS, 0, code, p1);
    t_unlock_cpu();
}

void sys_hook2(int grp, int code, int p1, int p2)
{
    ID tskid = TSKID(runtsk);
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (tskid > TRACE_TNUM_TSKID || !(Ros_buf.hook.task_id[tskid]))
        return;
#endif
    t_lock_cpu();
    ghs_hook6(THG_SYS, tskid, code, p1, p2);
    t_unlock_cpu();
}

void sys_hook2_i(int grp, int code, int p1, int p2)
{
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (!(Ros_buf.hook.task_id[0]))
        return;
#endif
    t_lock_cpu();
    ghs_hook6(THG_SYS, 0, code, p1, p2);
    t_unlock_cpu();
}

void sys_hook3(int grp, int code, int p1, int p2, int p3)
{
    ID tskid = TSKID(runtsk);
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (!(Ros_buf.hook.task_id[tskid & (TRACE_TNUM_TSKID-1)]))
        return;
#endif
    t_lock_cpu();
    ghs_hook7(THG_SYS, tskid, code, p1, p2, p3);
    t_unlock_cpu();
}

void sys_hook4(int grp, int code, int p1, int p2, int p3, int p4)
{
    ID tskid = TSKID(runtsk);
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (tskid > TRACE_TNUM_TSKID || !(Ros_buf.hook.task_id[tskid]))
        return;
#endif
    t_lock_cpu();
    ghs_hook8(THG_SYS, tskid, code, p1, p2, p3, p4);
    t_unlock_cpu();
}


void trc_rtn_hook(int grp, int code, int ercd)
{
    ID	tskid = TSKID(runtsk);
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_RET))
        return;
    if (!(Ros_buf.hook.trc_grp & THG_SYS) || !(Ros_buf.hook.sys_grp & grp))
        return;
    if (tskid > TRACE_TNUM_TSKID || !(Ros_buf.hook.task_id[tskid]))
	return;
#endif
    t_lock_cpu();
    ghs_hook5(THG_RET, tskid, code, ercd);
    t_unlock_cpu();
}

/*CPUロック状態で呼ばれるため、CPUをロックする必要はない*/
void sts_hook(ID tskid)
{
	TCB	*tcb;
	int tstat;
#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_STS))
        return;
    if (tskid > TRACE_TNUM_TSKID || !(Ros_buf.hook.task_id[tskid]))
        return;
#endif
    tcb = get_tcb(tskid);
	tstat = (int)(tcb->tstat);
	if(TSTAT_DORMANT(tstat)) tstat = 0x1;	/* 0x1:Dormant */
	else if(TSTAT_RUNNABLE(tstat)) tstat = 0x0;	/* 0x0:Ready */
	else if(TSTAT_WAITING(tstat)) {
		if((tstat & (TS_WAIT_SLEEP | TS_WAIT_WOBJ)) == 0) tstat = 0x09;  /* 0x9:Delay */
		else {
			if((tstat & TS_WAIT_SLEEP) != 0) tstat = 0x08;  /* 0x8:Sleep */
			else tstat = 0x0a;  /* 0xa:Waiting */
		}
	if(TSTAT_SUSPENDED(tstat)) tstat |= 0x10;
	}
	else if(TSTAT_SUSPENDED(tstat)) tstat = 0x10;
	ghs_hook4(THG_STS, tskid, tstat);
}

void dispatch_hook(TCB	*tcb)
{
    ID tskid;

#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_DSP))
        return;
#endif
    tskid = runtsk == NULL ? 0 : TSKID(runtsk);
    t_lock_cpu();
    ghs_hook4(THG_DSP, tskid, TSKID(tcb));
}

void exception_hook(int code, int addr)
{
    int i;
    ID tskid;

#if(GHS_HOOK != 1)
    if (!(Ros_buf.hook.trc_grp & THG_INT))
        return;
#endif
    for(i = 0; i < TRACE_TNUM_EVT; i++){
        if(Ros_buf.hook.evtcode[i] == 0) return;
        if(Ros_buf.hook.evtcode[i] == code){
            tskid = runtsk == NULL ? 0 : TSKID(runtsk);
            ghs_hook5(THG_INT, tskid, code, addr);
            return;
        }
    }
}
#endif

