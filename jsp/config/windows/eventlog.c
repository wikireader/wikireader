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
 *  @(#) $Id: eventlog.c,v 1.7 2003/12/15 07:19:22 takayuki Exp $
 */


#include <s_services.h>

#ifdef EVENTLOG_ENABLE

#include "jsp_kernel.h"
#include "task.h"
#include "semaphore.h"
#include "dataqueue.h"
#include "eventflag.h"
#include "mempfix.h"
#include "mailbox.h"

//#include <kernel_debug.h>

extern SEMCB 	semcb_table[1];
extern UINT		tmax_semid;
extern FLGCB	flgcb_table[1];
extern UINT		tmax_flgid;
extern DTQCB 	dtqcb_table[1];
extern UINT		tmax_dtqid;
extern MBXCB 	mbxcb_table[1];
extern UINT		tmax_mbxid;
extern MPFCB 	mpfcb_table[1];
extern UINT		tmax_mpfid;

/*
 *  イベントログ出力用 タスク状態取得関数
 *    (com_support.cはC++になるので、ここに置く)
 */

Inline BOOL is_inside_of(void * src, void * min, void * sup)
{	return ((long)src >= (long)min) && ((long)src <  (long)sup);	}

static void decode_waitstatus(TCB * tcb, unsigned int * objtype, int * objid)
{
	QUEUE * queue;

	if((tcb->tstat & TS_WAIT_SLEEP) != 0)
	{
		*objtype = TTW_SLP;
	}else
	{
		if((tcb->tstat & TS_WAIT_WOBJCB) != 0)
		{
			do {
				queue = tcb->task_queue.next;
			} while( queue != &tcb->task_queue && is_inside_of(queue, tcb_table, &tcb_table[TNUM_TSK]) );

			if(queue == &tcb->task_queue)
				*objtype = 0;

				/* セマフォ */
			else if(is_inside_of(queue, semcb_table, &semcb_table[tmax_semid-1]))
			{
				*objtype = TTW_SEM;
				*objid   = ((long)queue - (long)semcb_table) / sizeof(SEMCB) + 1;
			}
				/* イベントフラグ */
			else if(is_inside_of(queue, flgcb_table, &flgcb_table[tmax_flgid-1]))
			{
				*objtype = TTW_FLG;
				*objid   = ((long)queue - (long)flgcb_table) / sizeof(FLGCB) + 1;
			}
				/* データキュー */
			else if(is_inside_of(queue, dtqcb_table, &dtqcb_table[tmax_dtqid-1]))
			{
				if(((long)queue % sizeof(DTQCB)) == (long)(&((DTQCB *)0)->rwait_queue))
					*objtype = TTW_RDTQ;
				else
					*objtype = TTW_SDTQ;
				*objid   = ((long)queue - (long)dtqcb_table) / sizeof(DTQCB) + 1;
			}
				/* メールボックス */
			else if(is_inside_of(queue, mbxcb_table, &mbxcb_table[tmax_mbxid-1]))
			{
				*objtype = TTW_MBX;
				*objid   = ((long)queue - (long)mbxcb_table) / sizeof(MBXCB) + 1;
			}
				/* 固定長メモリプール */
			else if(is_inside_of(queue, mpfcb_table, &mpfcb_table[tmax_mpfid-1]))
			{
				*objtype = TTW_MPL;
				*objid   = ((long)queue - (long)mpfcb_table) / sizeof(MPFCB) + 1;
			}
		}else
		{
				/* slp_tskではなく、オブジェクト待ちキューにつながれていないタイムアウト付きの待ち -> dly_tsk */
			if(tcb->winfo->tmevtb != NULL)
				*objtype = TTW_DLY;
		}
	}
}

void decode_taskstatus(TCB * tcb, int * tskid, unsigned int * tskstat, unsigned int * objtype, int * objid)
{
	int stat = tcb->tstat;

		/* タスクID */
	*tskid = TSKID(tcb);

		/* タスク状態 */
	*tskstat = TTS_DMT;
	if((stat & TS_RUNNABLE) != 0)
	{
		*tskstat = TTS_RDY;
	}else
	{
		if((stat & TS_WAITING) != 0)
		{
				/* 待ち情報のデコード */
			decode_waitstatus((TCB *)tcb, objtype, objid);

			if((stat & TS_SUSPENDED) != 0)
				*tskstat = TTS_WAS;
			else
				*tskstat = TTS_WAI;
		}else
		{
			if((stat & TS_SUSPENDED) != 0)
				*tskstat = TTS_SUS;
		}
	}
}

#endif

