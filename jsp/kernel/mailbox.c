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
 *  @(#) $Id: mailbox.c,v 1.8 2003/07/01 13:31:08 hiro Exp $
 */

/*
 *	メールボックス機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "mailbox.h"

/*
 *  メールボックスIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_mbxid;

/*
 *  メールボックス初期化ブロックのエリア（kernel_cfg.c）
 */
extern const MBXINIB	mbxinib_table[];

/*
 *  メールボックス管理ブロックのエリア（kernel_cfg.c）
 */
extern MBXCB	mbxcb_table[];

/*
 *  メールボックスの数
 */
#define TNUM_MBX	((UINT)(tmax_mbxid - TMIN_MBXID + 1))

/*
 *  メールボックスIDからメールボックス管理ブロックを取り出すためのマクロ
 */
#define INDEX_MBX(mbxid)	((UINT)((mbxid) - TMIN_MBXID))
#define get_mbxcb(mbxid)	(&(mbxcb_table[INDEX_MBX(mbxid)]))

/*
 *  メールボックス待ち情報ブロックの定義
 */
typedef struct mailbox_waiting_information {
	WINFO	winfo;		/* 標準の待ち情報ブロック */
	WOBJCB	*wobjcb;	/* 待ちオブジェクトの管理ブロック */
	T_MSG	*pk_msg;	/* 受信したメッセージ */
} WINFO_MBX;

/* 
 *  メールボックス機能の初期化
 */
#ifdef __mbxini

void
mailbox_initialize()
{
	UINT	i;
	MBXCB	*mbxcb;

	for (mbxcb = mbxcb_table, i = 0; i < TNUM_MBX; mbxcb++, i++) {
		queue_initialize(&(mbxcb->wait_queue));
		mbxcb->mbxinib = &(mbxinib_table[i]);
		mbxcb->head = NULL;
	}
}

#endif /* __mbxini */

/*
 *  メッセージ優先度の取出し
 */
#define	MSGPRI(pk_msg)	(((T_MSG_PRI *) pk_msg)->msgpri)

/*
 *  優先度順メッセージキューへの挿入
 */
Inline void
enqueue_msg_pri(T_MSG **p_prevmsg_next, T_MSG *pk_msg)
{
	T_MSG	*pk_nextmsg;

	while ((pk_nextmsg = *p_prevmsg_next) != NULL) {
		if (MSGPRI(pk_nextmsg) > MSGPRI(pk_msg)) {
			break;
		}
		p_prevmsg_next = &(pk_nextmsg->next);
	}
	pk_msg->next = pk_nextmsg;
	*p_prevmsg_next = pk_msg;
}

/*
 *  メールボックスへの送信
 */
#ifdef __snd_mbx

SYSCALL ER
snd_mbx(ID mbxid, T_MSG *pk_msg)
{
	MBXCB	*mbxcb;
	TCB	*tcb;
	ER	ercd;
    
	LOG_SND_MBX_ENTER(mbxid, pk_msg);
	CHECK_TSKCTX_UNL();
	CHECK_MBXID(mbxid);
	mbxcb = get_mbxcb(mbxid);
	CHECK_PAR((mbxcb->mbxinib->mbxatr & TA_MPRI) == 0
		|| (TMIN_MPRI <= MSGPRI(pk_msg)
			&& MSGPRI(pk_msg) <= mbxcb->mbxinib->maxmpri));

	t_lock_cpu();
	if (!(queue_empty(&(mbxcb->wait_queue)))) {
		tcb = (TCB *) queue_delete_next(&(mbxcb->wait_queue));
		((WINFO_MBX *)(tcb->winfo))->pk_msg = pk_msg;
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if ((mbxcb->mbxinib->mbxatr & TA_MPRI) != 0) {
		enqueue_msg_pri(&(mbxcb->head), pk_msg);
		ercd = E_OK;
	}
	else {
		pk_msg->next = NULL;
		if (mbxcb->head != NULL) {
			mbxcb->last->next = pk_msg;
		}
		else {
			mbxcb->head = pk_msg;
		}
		mbxcb->last = pk_msg;
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_SND_MBX_LEAVE(ercd);
	return(ercd);
}

#endif /* __snd_mbx */

/*
 *  メールボックスからの受信
 */
#ifdef __rcv_mbx

SYSCALL ER
rcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
	MBXCB	*mbxcb;
	WINFO_MBX winfo;
	ER	ercd;
    
	LOG_RCV_MBX_ENTER(mbxid, ppk_msg);
	CHECK_DISPATCH();
	CHECK_MBXID(mbxid);
	mbxcb = get_mbxcb(mbxid);
    
	t_lock_cpu();
	if (mbxcb->head != NULL) {
		*ppk_msg = mbxcb->head;
		mbxcb->head = (*ppk_msg)->next;
		ercd = E_OK;
	}
	else {
		wobj_make_wait((WOBJCB *) mbxcb, (WINFO_WOBJ *) &winfo);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*ppk_msg = winfo.pk_msg;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_RCV_MBX_LEAVE(ercd, *ppk_msg);
	return(ercd);
}

#endif /* __rcv_mbx */

/*
 *  メールボックスからの受信（ポーリング）
 */
#ifdef __prcv_mbx

SYSCALL ER
prcv_mbx(ID mbxid, T_MSG **ppk_msg)
{
	MBXCB	*mbxcb;
	ER	ercd;
    
	LOG_PRCV_MBX_ENTER(mbxid, ppk_msg);
	CHECK_TSKCTX_UNL();
	CHECK_MBXID(mbxid);
	mbxcb = get_mbxcb(mbxid);
    
	t_lock_cpu();
	if (mbxcb->head != NULL) {
		*ppk_msg = mbxcb->head;
		mbxcb->head = (*ppk_msg)->next;
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

    exit:
	LOG_PRCV_MBX_LEAVE(ercd, *ppk_msg);
	return(ercd);
}

#endif /* __prcv_mbx */

/*
 *  メールボックスからの受信（タイムアウトあり）
 */
#ifdef __trcv_mbx

SYSCALL ER
trcv_mbx(ID mbxid, T_MSG **ppk_msg, TMO tmout)
{
	MBXCB	*mbxcb;
	WINFO_MBX winfo;
	TMEVTB	tmevtb;
	ER	ercd;
    
	LOG_TRCV_MBX_ENTER(mbxid, ppk_msg, tmout);
	CHECK_DISPATCH();
	CHECK_MBXID(mbxid);
	CHECK_TMOUT(tmout);
	mbxcb = get_mbxcb(mbxid);
    
	t_lock_cpu();
	if (mbxcb->head != NULL) {
		*ppk_msg = mbxcb->head;
		mbxcb->head = (*ppk_msg)->next;
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		wobj_make_wait_tmout((WOBJCB *) mbxcb, (WINFO_WOBJ *) &winfo,
						&tmevtb, tmout);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*ppk_msg = winfo.pk_msg;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_TRCV_MBX_LEAVE(ercd, *ppk_msg);
	return(ercd);
}

#endif /* __trcv_mbx */
