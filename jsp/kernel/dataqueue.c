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
 *  @(#) $Id: dataqueue.c,v 1.9 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	データキュー機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "dataqueue.h"

/*
 *  データキューIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_dtqid;

/*
 *  データキュー初期化ブロックのエリア（kernel_cfg.c）
 */
extern const DTQINIB	dtqinib_table[];

/*
 *  データキューの数
 */
#define TNUM_DTQ	((UINT)(tmax_dtqid - TMIN_DTQID + 1))

/*
 *  データキュー管理ブロックのエリア（kernel_cfg.c）
 */
extern DTQCB	dtqcb_table[];

/*
 *  データキューIDからデータキュー管理ブロックを取り出すためのマクロ
 */
#define INDEX_DTQ(dtqid)	((UINT)((dtqid) - TMIN_DTQID))
#define get_dtqcb(dtqid)	(&(dtqcb_table[INDEX_DTQ(dtqid)]))

/*
 *  データキュー待ち情報ブロックの定義
 *
 *  データキューへの送信待ちとデータキューからの受信待ちで，同じ待ち情
 *  報ブロックを使う．
 */
typedef struct dataqueue_waiting_information {
	WINFO	winfo;		/* 標準の待ち情報ブロック */
	WOBJCB	*wobjcb;	/* 待ちオブジェクトの管理ブロック */
	VP_INT	data;		/* 送受信データ  */
} WINFO_DTQ;

/*
 *  データキュー機能の初期化
 */
#ifdef __dtqini

void
dataqueue_initialize(void)
{
	UINT	i;
	DTQCB	*dtqcb;

	for (dtqcb = dtqcb_table, i = 0; i < TNUM_DTQ; dtqcb++, i++) {
		queue_initialize(&(dtqcb->swait_queue));
		dtqcb->dtqinib = &(dtqinib_table[i]);
		queue_initialize(&(dtqcb->rwait_queue));
		dtqcb->count = 0;
		dtqcb->head = 0;
		dtqcb->tail = 0;
	}
}

#endif /* __dtqini */

/*
 *  データキュー領域にデータを格納
 */
#ifdef __dtqenq

BOOL
enqueue_data(DTQCB *dtqcb, VP_INT data)
{
	if (dtqcb->count < dtqcb->dtqinib->dtqcnt) {
		*((VP_INT *)(dtqcb->dtqinib->dtq) + dtqcb->tail) = data;
		dtqcb->count++;
		dtqcb->tail++;
		if (dtqcb->tail >= dtqcb->dtqinib->dtqcnt) {
			dtqcb->tail = 0;
		}
		return(TRUE);
	}
	return(FALSE);
}

#endif /* __dtqenq */

/*
 *  データキュー領域にデータを強制格納
 */
#ifdef __dtqfenq

void
force_enqueue_data(DTQCB *dtqcb, VP_INT data)
{
	*((VP_INT *)(dtqcb->dtqinib->dtq) + dtqcb->tail) = data;
	dtqcb->tail++;
	if (dtqcb->tail >= dtqcb->dtqinib->dtqcnt) {
		dtqcb->tail = 0;
	}
	if (dtqcb->count < dtqcb->dtqinib->dtqcnt) {
		dtqcb->count++;
	}
	else {
		dtqcb->head = dtqcb->tail;
	}
}

#endif /* __dtqfenq */

/*
 *  データキュー領域からデータを取出し
 */
#ifdef __dtqdeq

BOOL
dequeue_data(DTQCB *dtqcb, VP_INT *p_data)
{
	if (dtqcb->count > 0) {
		*p_data = *((VP_INT *)(dtqcb->dtqinib->dtq) + dtqcb->head);
		dtqcb->count--;
		dtqcb->head++;
		if (dtqcb->head >= dtqcb->dtqinib->dtqcnt) {
			dtqcb->head = 0;
		}
		return(TRUE);
	}
	return(FALSE);
}

#endif /* __dtqdeq */

/*
 *  受信待ちキューの先頭タスクへのデータ送信
 */
#ifdef __dtqsnd

TCB *
send_data_rwait(DTQCB *dtqcb, VP_INT data)
{
	TCB	*tcb;

	if (!(queue_empty(&(dtqcb->rwait_queue)))) {
		tcb = (TCB *) queue_delete_next(&(dtqcb->rwait_queue));
		((WINFO_DTQ *)(tcb->winfo))->data = data;
		return(tcb);
	}
	return(NULL);
}

#endif /* __dtqsnd */

/*
 *  送信待ちキューの先頭タスクからのデータ受信
 */
#ifdef __dtqrcv

TCB *
receive_data_swait(DTQCB *dtqcb, VP_INT *p_data)
{
	TCB	*tcb;

	if (!(queue_empty(&(dtqcb->swait_queue)))) {
		tcb = (TCB *) queue_delete_next(&(dtqcb->swait_queue));
		*p_data = ((WINFO_DTQ *)(tcb->winfo))->data;
		return(tcb);
	}
	return(NULL);
}

#endif /* __dtqrcv */

/*
 *  データキューへの送信
 */
#ifdef __snd_dtq

SYSCALL ER
snd_dtq(ID dtqid, VP_INT data)
{
	DTQCB	*dtqcb;
	WINFO_DTQ winfo;
	TCB	*tcb;
	ER	ercd;

	LOG_SND_DTQ_ENTER(dtqid, data);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if ((tcb = send_data_rwait(dtqcb, data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (enqueue_data(dtqcb, data)) {
		ercd = E_OK;
	}
	else {
		winfo.data = data;
		wobj_make_wait((WOBJCB *) dtqcb, (WINFO_WOBJ *) &winfo);
		dispatch();
		ercd = winfo.winfo.wercd;
	}
	t_unlock_cpu();

    exit:
	LOG_SND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __snd_dtq */

/*
 *  データキューへの送信（ポーリング）
 */
#ifdef __psnd_dtq

SYSCALL ER
psnd_dtq(ID dtqid, VP_INT data)
{
	DTQCB	*dtqcb;
	TCB	*tcb;
	ER	ercd;

	LOG_PSND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if ((tcb = send_data_rwait(dtqcb, data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (enqueue_data(dtqcb, data)) {
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

    exit:
	LOG_PSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __psnd_dtq */

/*
 *  データキューへの送信（ポーリング，非タスクコンテキスト用）
 */
#ifdef __ipsnd_dtq

SYSCALL ER
ipsnd_dtq(ID dtqid, VP_INT data)
{
	DTQCB	*dtqcb;
	TCB	*tcb;
	ER	ercd;

	LOG_IPSND_DTQ_ENTER(dtqid, data);
	CHECK_INTCTX_UNL();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);

	i_lock_cpu();
	if ((tcb = send_data_rwait(dtqcb, data)) != NULL) {
		if (wait_complete(tcb)) {
			reqflg = TRUE;
		}
		ercd = E_OK;
	}
	else if (enqueue_data(dtqcb, data)) {
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	i_unlock_cpu();

    exit:
	LOG_IPSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __ipsnd_dtq */

/*
 *  データキューへの送信（タイムアウトあり）
 */
#ifdef __tsnd_dtq

SYSCALL ER
tsnd_dtq(ID dtqid, VP_INT data, TMO tmout)
{
	DTQCB	*dtqcb;
	WINFO_DTQ winfo;
	TMEVTB	tmevtb;
	TCB	*tcb;
	ER	ercd;

	LOG_TSND_DTQ_ENTER(dtqid, data, tmout);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	CHECK_TMOUT(tmout);
	dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if ((tcb = send_data_rwait(dtqcb, data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (enqueue_data(dtqcb, data)) {
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo.data = data;
		wobj_make_wait_tmout((WOBJCB *) dtqcb, (WINFO_WOBJ *) &winfo,
						&tmevtb, tmout);
		dispatch();
		ercd = winfo.winfo.wercd;
	}
	t_unlock_cpu();

    exit:
	LOG_TSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __tsnd_dtq */

/*
 *  データキューへの強制送信
 */
#ifdef __fsnd_dtq

SYSCALL ER
fsnd_dtq(ID dtqid, VP_INT data)
{
	DTQCB	*dtqcb;	
	TCB	*tcb;
	ER	ercd;

	LOG_FSND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);
	CHECK_ILUSE(dtqcb->dtqinib->dtqcnt > 0);

	t_lock_cpu();
	if ((tcb = send_data_rwait(dtqcb, data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
	}
	else {
		force_enqueue_data(dtqcb, data);
	}
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_FSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __fsnd_dtq */

/*
 *  データキューへの強制送信（非タスクコンテキスト用）
 */
#ifdef __ifsnd_dtq

SYSCALL ER
ifsnd_dtq(ID dtqid, VP_INT data)
{
	DTQCB	*dtqcb;
	TCB	*tcb;
	ER	ercd;

	LOG_IFSND_DTQ_ENTER(dtqid, data);
	CHECK_INTCTX_UNL();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);
	CHECK_ILUSE(dtqcb->dtqinib->dtqcnt > 0);

	i_lock_cpu();
	if ((tcb = send_data_rwait(dtqcb, data)) != NULL) {
		if (wait_complete(tcb)) {
			reqflg = TRUE;
		}
	}
	else {
		force_enqueue_data(dtqcb, data);
	}
	ercd = E_OK;
	i_unlock_cpu();

    exit:
	LOG_IFSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* __ifsnd_dtq */

/*
 *  データキューからの受信
 */
#ifdef __rcv_dtq

SYSCALL ER
rcv_dtq(ID dtqid, VP_INT *p_data)
{
	DTQCB	*dtqcb;
	WINFO_DTQ winfo;
	TCB	*tcb;
	VP_INT	data;
	ER	ercd;

	LOG_RCV_DTQ_ENTER(dtqid, p_data);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (dequeue_data(dtqcb, p_data)) {
		if ((tcb = receive_data_swait(dtqcb, &data)) != NULL) {
			enqueue_data(dtqcb, data);
			if (wait_complete(tcb)) {
				dispatch();
			}
		}
		ercd = E_OK;
	}
	else if ((tcb = receive_data_swait(dtqcb, p_data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		runtsk->tstat = (TS_WAITING | TS_WAIT_WOBJ);
		make_wait(&(winfo.winfo));
		queue_insert_prev(&(dtqcb->rwait_queue),
					&(runtsk->task_queue));
		winfo.wobjcb = (WOBJCB *) dtqcb;
		LOG_TSKSTAT(runtsk);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo.data;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_RCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* __rcv_dtq */

/*
 *  データキューからの受信（ポーリング）
 */
#ifdef __prcv_dtq

SYSCALL ER
prcv_dtq(ID dtqid, VP_INT *p_data)
{
	DTQCB	*dtqcb;
	TCB	*tcb;
	VP_INT	data;
	ER	ercd;

	LOG_PRCV_DTQ_ENTER(dtqid, p_data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (dequeue_data(dtqcb, p_data)) {
		if ((tcb = receive_data_swait(dtqcb, &data)) != NULL) {
			enqueue_data(dtqcb, data);
			if (wait_complete(tcb)) {
				dispatch();
			}
		}
		ercd = E_OK;
	}
	else if ((tcb = receive_data_swait(dtqcb, p_data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

    exit:
	LOG_PRCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* __prcv_dtq */

/*
 *  データキューからの受信（タイムアウトあり）
 */
#ifdef __trcv_dtq

SYSCALL ER
trcv_dtq(ID dtqid, VP_INT *p_data, TMO tmout)
{
	DTQCB	*dtqcb;
	WINFO_DTQ winfo;
	TMEVTB	tmevtb;
	TCB	*tcb;
	VP_INT	data;
	ER	ercd;

	LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout);
	CHECK_DISPATCH();
	CHECK_DTQID(dtqid);
	CHECK_TMOUT(tmout);
	dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	if (dequeue_data(dtqcb, p_data)) {
		if ((tcb = receive_data_swait(dtqcb, &data)) != NULL) {
			enqueue_data(dtqcb, data);
			if (wait_complete(tcb)) {
				dispatch();
			}
		}
		ercd = E_OK;
	}
	else if ((tcb = receive_data_swait(dtqcb, p_data)) != NULL) {
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		runtsk->tstat = (TS_WAITING | TS_WAIT_WOBJ);
		make_wait_tmout(&(winfo.winfo), &tmevtb, tmout);
		queue_insert_prev(&(dtqcb->rwait_queue),
					&(runtsk->task_queue));
		winfo.wobjcb = (WOBJCB *) dtqcb;
		LOG_TSKSTAT(runtsk);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo.data;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_TRCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* __trcv_dtq */
