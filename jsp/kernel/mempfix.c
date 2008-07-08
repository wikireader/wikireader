/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: mempfix.c,v 1.12 2005/11/12 14:55:23 hiro Exp $
 */

/*
 *	固定長メモリプール機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "mempfix.h"

/*
 *  固定長メモリプールIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_mpfid;

/*
 *  固定長メモリプール初期化ブロックのエリア（kernel_cfg.c）
 */
extern const MPFINIB	mpfinib_table[];

/*
 *  固定長メモリプール管理ブロックのエリア（kernel_cfg.c）
 */
extern MPFCB	mpfcb_table[];

/*
 *  固定長メモリプールの数
 */
#define TNUM_MPF	((UINT)(tmax_mpfid - TMIN_MPFID + 1))

/*
 *  固定長メモリプールIDから固定長メモリプール管理ブロックを取り出すた
 *  めのマクロ
 */
#define INDEX_MPF(mpfid)	((UINT)((mpfid) - TMIN_MPFID))
#define get_mpfcb(mpfid)	(&(mpfcb_table[INDEX_MPF(mpfid)]))

/*
 *  固定長メモリプール待ち情報ブロックの定義
 */
typedef struct fixed_memorypool_waiting_information {
	WINFO	winfo;		/* 標準の待ち情報ブロック */
	WOBJCB	*wobjcb;	/* 待ちオブジェクトの管理ブロック */
	VP	blk;		/* 獲得したメモリブロック */
} WINFO_MPF;

/* 
 *  固定長メモリプール機能の初期化
 */
#ifdef __mpfini

void
mempfix_initialize()
{
	UINT	i;
	MPFCB	*mpfcb;

	for (mpfcb = mpfcb_table, i = 0; i < TNUM_MPF; mpfcb++, i++) {
		queue_initialize(&(mpfcb->wait_queue));
		mpfcb->mpfinib = &(mpfinib_table[i]);
		mpfcb->unused = mpfcb->mpfinib->mpf;
		mpfcb->freelist = NULL;
	}
}

#endif /* __mpfini */

/*
 *  固定長メモリプールからブロックを獲得
 */
#ifdef __mpfget

BOOL
mempfix_get_block(MPFCB *mpfcb, VP *p_blk)
{
	FREEL	*free;

	if (mpfcb->freelist != NULL) {
		free = mpfcb->freelist;
		mpfcb->freelist = free->next;
		*p_blk = (VP) free;
		return(TRUE);
	}
	else if (mpfcb->unused < mpfcb->mpfinib->limit) {
		*p_blk = mpfcb->unused;
		mpfcb->unused = (VP)((char *)(mpfcb->unused)
						+ mpfcb->mpfinib->blksz);
		return(TRUE);
	}
	return(FALSE);
}

#endif /* __mpfget */

/*
 *  固定長メモリブロックの獲得
 */
#ifdef __get_mpf

SYSCALL ER
get_mpf(ID mpfid, VP *p_blk)
{
	MPFCB	*mpfcb;
	WINFO_MPF winfo;
	ER	ercd;

	LOG_GET_MPF_ENTER(mpfid, p_blk);
	CHECK_DISPATCH();
	CHECK_MPFID(mpfid);
	mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	if (mempfix_get_block(mpfcb, p_blk)) {
		ercd = E_OK;
	}
	else {
		wobj_make_wait((WOBJCB *) mpfcb, (WINFO_WOBJ *) &winfo);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*p_blk = winfo.blk;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_GET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

#endif /* __get_mpf */

/*
 *  固定長メモリブロックの獲得（ポーリング）
 */
#ifdef __pget_mpf

SYSCALL ER
pget_mpf(ID mpfid, VP *p_blk)
{
	MPFCB	*mpfcb;
	ER	ercd;

	LOG_PGET_MPF_ENTER(mpfid, p_blk);
	CHECK_TSKCTX_UNL();
	CHECK_MPFID(mpfid);
	mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	if (mempfix_get_block(mpfcb, p_blk)) {
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	t_unlock_cpu();

    exit:
	LOG_PGET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

#endif /* __pget_mpf */

/*
 *  固定長メモリブロックの獲得（タイムアウトあり）
 */
#ifdef __tget_mpf

SYSCALL ER
tget_mpf(ID mpfid, VP *p_blk, TMO tmout)
{
	MPFCB	*mpfcb;
	WINFO_MPF winfo;
	TMEVTB	tmevtb;
	ER	ercd;

	LOG_TGET_MPF_ENTER(mpfid, p_blk, tmout);
	CHECK_DISPATCH();
	CHECK_MPFID(mpfid);
	CHECK_TMOUT(tmout);
	mpfcb = get_mpfcb(mpfid);

	t_lock_cpu();
	if (mempfix_get_block(mpfcb, p_blk)) {
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		wobj_make_wait_tmout((WOBJCB *) mpfcb, (WINFO_WOBJ *) &winfo,
						&tmevtb, tmout);
		dispatch();
		ercd = winfo.winfo.wercd;
		if (ercd == E_OK) {
			*p_blk = winfo.blk;
		}
	}
	t_unlock_cpu();

    exit:
	LOG_TGET_MPF_LEAVE(ercd, *p_blk);
	return(ercd);
}

#endif /* __tget_mpf */

/*
 *  固定長メモリブロックの返却
 */
#ifdef __rel_mpf

SYSCALL ER
rel_mpf(ID mpfid, VP blk)
{
	MPFCB	*mpfcb;
	TCB	*tcb;
	FREEL	*free;
	ER	ercd;
    
	LOG_REL_MPF_ENTER(mpfid, blk);
	CHECK_TSKCTX_UNL();
	CHECK_MPFID(mpfid);
	mpfcb = get_mpfcb(mpfid);
	CHECK_PAR(mpfcb->mpfinib->mpf <= blk
			&& blk < mpfcb->mpfinib->limit
			&& ((char *)(blk) - (char *)(mpfcb->mpfinib->mpf))
					% mpfcb->mpfinib->blksz == 0);

	t_lock_cpu();
	if (!(queue_empty(&(mpfcb->wait_queue)))) {
		tcb = (TCB *) queue_delete_next(&(mpfcb->wait_queue));
		((WINFO_MPF *)(tcb->winfo))->blk = blk;
		if (wait_complete(tcb)) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		free = (FREEL *) blk;
		free->next = mpfcb->freelist;
		mpfcb->freelist = free;
		ercd = E_OK;
	}
	t_unlock_cpu();

    exit:
	LOG_REL_MPF_LEAVE(ercd);
	return(ercd);
}

#endif /* __rel_mpf */
