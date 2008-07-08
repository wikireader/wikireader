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
 *  @(#) $Id: eventflag.c,v 1.8 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	イベントフラグ機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "eventflag.h"

/*
 *  イベントフラグIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_flgid;

/*
 *  イベントフラグ初期化ブロックのエリア（kernel_cfg.c）
 */
extern const FLGINIB	flginib_table[];

/*
 *  イベントフラグ管理ブロックのエリア（kernel_cfg.c）
 */
extern FLGCB	flgcb_table[];

/*
 *  イベントフラグの数
 */
#define TNUM_FLG	((UINT)(tmax_flgid - TMIN_FLGID + 1))

/*
 *  イベントフラグIDからイベントフラグ管理ブロックを取り出すためのマクロ
 */
#define INDEX_FLG(flgid)	((UINT)((flgid) - TMIN_FLGID))
#define get_flgcb(flgid)	(&(flgcb_table[INDEX_FLG(flgid)]))

/*
 *  イベントフラグ待ち情報ブロックの定義
 *
 *  flgptn は，waiptn および wfmode と同時に使うことはないため，union 
 *  を使えばメモリを節約することが可能である．
 */
typedef struct eventflag_waiting_information {
	WINFO	winfo;		/* 標準の待ち情報ブロック */
	WOBJCB	*wobjcb;	/* 待ちオブジェクトの管理ブロック */
	FLGPTN	waiptn;		/* 待ちパターン */
	MODE	wfmode;		/* 待ちモード */
	FLGPTN	flgptn;		/* 待ち解除時のパターン */
} WINFO_FLG;

/*
 *  イベントフラグ機能の初期化
 */
#ifdef __flgini

void
eventflag_initialize(void)
{
	UINT	i;
	FLGCB	*flgcb;

	for (flgcb = flgcb_table, i = 0; i < TNUM_FLG; flgcb++, i++) {
		queue_initialize(&(flgcb->wait_queue));
		flgcb->flginib = &(flginib_table[i]);
		flgcb->flgptn = flgcb->flginib->iflgptn;
	}
}

#endif /* __flgini */

/*
 *  イベントフラグ待ち解除条件のチェック
 */
#ifdef __flgcnd

BOOL
eventflag_cond(FLGCB *flgcb, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	if ((wfmode & TWF_ORW) != 0 ? (flgcb->flgptn & waiptn) != 0
				: (flgcb->flgptn & waiptn) == waiptn) {
		*p_flgptn = flgcb->flgptn;
		if ((flgcb->flginib->flgatr & TA_CLR) != 0) {
			flgcb->flgptn = 0;
		}
		return(TRUE);
	}
	return(FALSE);
}

#endif /* __flgcnd */

/*
 *  イベントフラグのセット
 */
#ifdef __set_flg

SYSCALL ER
set_flg(ID flgid, FLGPTN setptn)
{
	FLGCB	*flgcb;
	TCB	*tcb;
	WINFO_FLG *winfo;
	ER	ercd;

	LOG_SET_FLG_ENTER(flgid, setptn);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	flgcb = get_flgcb(flgid);

	t_lock_cpu();
	flgcb->flgptn |= setptn;
	if (!(queue_empty(&(flgcb->wait_queue)))) {
		tcb = (TCB *)(flgcb->wait_queue.next);
		winfo = (WINFO_FLG *)(tcb->winfo);
		if (eventflag_cond(flgcb, winfo->waiptn,
					winfo->wfmode, &(winfo->flgptn))) {
			queue_delete(&(tcb->task_queue));
			if (wait_complete(tcb)) {
				dispatch();
			}
		}
	}
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_SET_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* __set_flg */

/*
 *  イベントフラグのセット（非タスクコンテキスト用）
 */
#ifdef __iset_flg

SYSCALL ER
iset_flg(ID flgid, FLGPTN setptn)
{
	FLGCB	*flgcb;
	TCB	*tcb;
	WINFO_FLG *winfo;
	ER	ercd;

	LOG_ISET_FLG_ENTER(flgid, setptn);
	CHECK_INTCTX_UNL();
	CHECK_FLGID(flgid);
	flgcb = get_flgcb(flgid);

	i_lock_cpu();
	flgcb->flgptn |= setptn;
	if (!(queue_empty(&(flgcb->wait_queue)))) {
		tcb = (TCB *)(flgcb->wait_queue.next);
		winfo = (WINFO_FLG *)(tcb->winfo);
		if (eventflag_cond(flgcb, winfo->waiptn,
					winfo->wfmode, &(winfo->flgptn))) {
			queue_delete(&(tcb->task_queue));
			if (wait_complete(tcb)) {
				reqflg = TRUE;
			}
		}
	}
	ercd = E_OK;
	i_unlock_cpu();

    exit:
	LOG_ISET_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* __iset_flg */

/*
 *  イベントフラグのクリア
 */
#ifdef __clr_flg

SYSCALL ER
clr_flg(ID flgid, FLGPTN clrptn)
{
	FLGCB	*flgcb;
	ER	ercd;

	LOG_CLR_FLG_ENTER(flgid, clrptn);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	flgcb = get_flgcb(flgid);

	t_lock_cpu();
	flgcb->flgptn &= clrptn; 
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_CLR_FLG_LEAVE(ercd);
	return(ercd);
}

#endif /* __clr_flg */

/*
 *  イベントフラグ待ち
 */
#ifdef __wai_flg

SYSCALL ER
wai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	FLGCB	*flgcb;
	WINFO_FLG winfo;
	ER	ercd;

	LOG_WAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn);
	CHECK_DISPATCH();
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0);
	CHECK_PAR((wfmode & ~TWF_ORW) == 0);
	flgcb = get_flgcb(flgid);

	t_lock_cpu();
	if (!(queue_empty(&(flgcb->wait_queue)))) {
		ercd = E_ILUSE;
	}
	else if (eventflag_cond(flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_OK;
	}
	else {
		winfo.waiptn = waiptn;
		winfo.wfmode = wfmode;
		wobj_make_wait((WOBJCB *) flgcb, (WINFO_WOBJ *) &winfo);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*p_flgptn = winfo.flgptn;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_WAI_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}

#endif /* __wai_flg */

/*
 *  イベントフラグ待ち（ポーリング）
 */
#ifdef __pol_flg

SYSCALL ER
pol_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn)
{
	FLGCB	*flgcb;
	ER	ercd;

	LOG_POL_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn);
	CHECK_TSKCTX_UNL();
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0);
	CHECK_PAR((wfmode & ~TWF_ORW) == 0);
	flgcb = get_flgcb(flgid);

	t_lock_cpu();
	if (!(queue_empty(&(flgcb->wait_queue)))) {
		ercd = E_ILUSE;
	}
	else if (eventflag_cond(flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

    exit:
	LOG_POL_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}

#endif /* __pol_flg */

/*
 *  イベントフラグ待ち（タイムアウトあり）
 */
#ifdef __twai_flg

SYSCALL ER
twai_flg(ID flgid, FLGPTN waiptn, MODE wfmode, FLGPTN *p_flgptn, TMO tmout)
{
	FLGCB	*flgcb;
	WINFO_FLG winfo;
	TMEVTB	tmevtb;
	ER	ercd;

	LOG_TWAI_FLG_ENTER(flgid, waiptn, wfmode, p_flgptn, tmout);
	CHECK_DISPATCH();
	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0);
	CHECK_PAR((wfmode & ~TWF_ORW) == 0);
	CHECK_TMOUT(tmout);
	flgcb = get_flgcb(flgid);

	t_lock_cpu();
	if (!(queue_empty(&(flgcb->wait_queue)))) {
		ercd = E_ILUSE;
	}
	else if (eventflag_cond(flgcb, waiptn, wfmode, p_flgptn)) {
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo.waiptn = waiptn;
		winfo.wfmode = wfmode;
		wobj_make_wait_tmout((WOBJCB *) flgcb, (WINFO_WOBJ *) &winfo,
						&tmevtb, tmout);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*p_flgptn = winfo.flgptn;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_TWAI_FLG_LEAVE(ercd, *p_flgptn);
	return(ercd);
}

#endif /* __twai_flg */
