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
 *  @(#) $Id: ghs_hook.c,v 1.2 2003/12/19 11:48:37 honda Exp $
 */
#ifdef GHS_HOOK
#include "jsp_kernel.h"
#include "kernel.h"
#include "time_event.h"
#include "task.h"
#include "ghs_hook.h"


#define	THG_ALL	THG_SYS|THG_RET|THG_DSP|THG_STS|THG_INT
#define	THF_ALL	THF_TSK|THF_TSKS|THF_TEX|THF_SEM|THF_FLG|THF_DTQ|THF_MBX|THF_MPF|THF_TIM
#ifndef GRP
#define	GRP	THG_ALL
#endif
#ifndef SEV
#define	SEV	THF_ALL  
#endif
#ifndef TSK
#define	TSK	TRACE_TNUM_TSKID
#endif


#define MAX_TSK  256

T_ROS Ros_buf;

struct GHS_TASKS {
	B	sts;			
	B	pri;			
} ghs_tasks[MAX_TSK];

ghs_tasks_initialize()
{
	INT	i;
	TCB	*tcb;

	for (tcb = tcb_table, i = 0; i < tmax_tskid; tcb++, i++) {
		if(i > sizeof(ghs_tasks) / sizeof(ghs_tasks[0]))
			return;
		if(TSTAT_RUNNABLE(tcb->tstat)) ghs_tasks[i].sts = TTS_RDY;
		else ghs_tasks[i].sts = TTS_DMT;
		ghs_tasks[i].pri = EXT_TSKPRI(tcb->priority);
	}
}

int SetEndian()
{
    union testme {
        char    a[2];
        short   b;
    	} testme;

    testme.b    = 0;
    testme.a[0] = 1;
    if (testme.b== 1)
        return(0);
    else
        return(1);
}

void vinit_hook(void)
{
    int i;

    Ros_buf.trace.bigendian = SetEndian();
    Ros_buf.trace.dummy = 0;
    Ros_buf.trace.size = BUFF_SIZE-1;
    Ros_buf.trace.putp = 0;
    Ros_buf.trace.getp = 0;
    for(i = 0; i < TRACE_TNUM_TSKID-1; i++)
        Ros_buf.hook.task_id[i] = 0;
    for(i = 0; i < TRACE_TNUM_EVT-1; i++)
        Ros_buf.hook.evtcode[i] = 0;
    Ros_buf.hook.max_tid = tmax_tskid;
    Ros_buf.hook.tnum_tskid = TRACE_TNUM_TSKID;
    Ros_buf.hook.tnum_evt = TRACE_TNUM_EVT;

    Ros_buf.hook.trc_grp = GRP;
    Ros_buf.hook.sys_grp = SEV;
    for(i = 0; i < TSK; i++)
        Ros_buf.hook.task_id[i] = 1;    
    
}

void ghs_hook4(int sts, int p1, int p2)
{
    int putp;

        while(BUFF_SIZE-(Ros_buf.trace.putp-Ros_buf.trace.getp) < 4)
        {
	    Ros_buf.trace.getp += Ros_buf.syscall[Ros_buf.trace.getp & Ros_buf.trace.size] & 0x0F;
        }
        putp = Ros_buf.trace.putp;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = (sts << 4) | 4;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = current_time;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p1;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p2;
        Ros_buf.trace.putp = putp;
}

void ghs_hook5(int sts, int p1, int p2, int p3)
{
    int putp;

        while(BUFF_SIZE-(Ros_buf.trace.putp-Ros_buf.trace.getp) < 5)
        {   Ros_buf.trace.getp += Ros_buf.syscall[Ros_buf.trace.getp & Ros_buf.trace.size] & 0x0F;
        }
	putp = Ros_buf.trace.putp;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = (sts << 4) | 5;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = current_time;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p1;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p2;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p3;
        Ros_buf.trace.putp = putp;
}

void ghs_hook6(int sts, int p1, int p2, int p3, int p4)
{
    int putp;

        while(BUFF_SIZE-(Ros_buf.trace.putp-Ros_buf.trace.getp) < 6)
        {   Ros_buf.trace.getp += Ros_buf.syscall[Ros_buf.trace.getp & Ros_buf.trace.size] & 0x0F;
        }

        putp = Ros_buf.trace.putp;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = (sts << 4) | 6;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = current_time;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p1;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p2;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p3;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p4;
        Ros_buf.trace.putp = putp;
}

void ghs_hook7(int sts, int p1, int p2, int p3, int p4, int p5)
{
    int putp;

        while(BUFF_SIZE-(Ros_buf.trace.putp-Ros_buf.trace.getp) < 7)
        {   Ros_buf.trace.getp += Ros_buf.syscall[Ros_buf.trace.getp & Ros_buf.trace.size] & 0x0F;
        }

        putp = Ros_buf.trace.putp;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = (sts << 4) | 7;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = current_time;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p1;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p2;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p3;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p4;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p5;
        Ros_buf.trace.putp = putp;
}

void ghs_hook8(int sts, int p1, int p2, int p3, int p4, int p5, int p6)
{
    int putp;

        while(BUFF_SIZE-(Ros_buf.trace.putp-Ros_buf.trace.getp) < 8)
        {   Ros_buf.trace.getp += Ros_buf.syscall[Ros_buf.trace.getp & Ros_buf.trace.size] & 0x0F;
        }
        putp = Ros_buf.trace.putp;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = (sts << 4) | 8;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = current_time;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p1;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p2;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p3;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p4;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p5;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p6;
        Ros_buf.trace.putp = putp;
}

void ghs_hook9(int sts, int p1, int p2, int p3, int p4, int p5, int p6, int p7)
{
    int putp;

        while(BUFF_SIZE-(Ros_buf.trace.putp-Ros_buf.trace.getp) < 9)
        {   Ros_buf.trace.getp += Ros_buf.syscall[Ros_buf.trace.getp & Ros_buf.trace.size] & 0x0F;
        }

        putp = Ros_buf.trace.putp;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = (sts << 4) | 9;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = current_time;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p1;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p2;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p3;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p4;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p5;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p6;
        Ros_buf.syscall[putp++ & Ros_buf.trace.size] = p7;
        Ros_buf.trace.putp = putp;
}

#endif

