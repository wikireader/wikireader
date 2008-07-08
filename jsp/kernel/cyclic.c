/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cyclic.c,v 1.11 2006/02/12 05:29:32 hiro Exp $
 */

/*
 *	周期ハンドラ機能
 */

#include "jsp_kernel.h"
#include "check.h"
#include "cyclic.h"

/*
 *  周期ハンドラIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_cycid;

/*
 *  周期ハンドラ初期化ブロックのエリア（kernel_cfg.c）
 */
extern const CYCINIB	cycinib_table[];

/*
 *  周期ハンドラ管理ブロックのエリア（kernel_cfg.c）
 */
extern CYCCB	cyccb_table[];

/*
 *  周期ハンドラの数
 */
#define TNUM_CYC	((UINT)(tmax_cycid - TMIN_CYCID + 1))

/*
 *  周期ハンドラIDから周期ハンドラ管理ブロックを取り出すためのマクロ
 */
#define INDEX_CYC(cycid)	((UINT)((cycid) - TMIN_CYCID))
#define get_cyccb(cycid)	(&(cyccb_table[INDEX_CYC(cycid)]))

/*
 *  引数まで定義した周期ハンドラの型
 */
typedef void	(*CYCHDR)(VP_INT exinf);

/*
 *  周期ハンドラ機能の初期化
 */
#ifdef __cycini

void
cyclic_initialize()
{
	UINT	i;
	CYCCB	*cyccb;

	for (cyccb = cyccb_table, i = 0; i < TNUM_CYC; cyccb++, i++) {
		cyccb->cycinib = &(cycinib_table[i]);
		if ((cyccb->cycinib->cycatr & TA_STA) != 0) {
			cyccb->cycsta = TRUE;
			tmevtb_enqueue_cyc(cyccb,
					(EVTTIM)(cyccb->cycinib->cycphs));
		}
		else {
			cyccb->cycsta = FALSE;
		}
	}
}

#endif /* __cycini */

/*
 *  周期ハンドラ起動のためのタイムイベントブロックの登録
 */
#ifdef __cycenq

void
tmevtb_enqueue_cyc(CYCCB *cyccb, EVTTIM evttim)
{
	tmevtb_enqueue_evttim(&(cyccb->tmevtb), evttim,
				(CBACK) call_cychdr, (VP) cyccb);
	cyccb->evttim = evttim;
}

#endif /* __cycenq */

/*
 *  周期ハンドラの動作開始
 */
#ifdef __sta_cyc

SYSCALL ER
sta_cyc(ID cycid)
{
	CYCCB	*cyccb;
	ER	ercd;

	LOG_STA_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	cyccb = get_cyccb(cycid);

	t_lock_cpu();
	if (cyccb->cycsta) {
		tmevtb_dequeue(&(cyccb->tmevtb));
	}
	else {
		cyccb->cycsta = TRUE;
	}
	tmevtb_enqueue_cyc(cyccb, base_time + cyccb->cycinib->cyctim);
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_STA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* __sta_cyc */

/*
 *  周期ハンドラの動作停止
 */
#ifdef __stp_cyc

SYSCALL ER
stp_cyc(ID cycid)
{
	CYCCB	*cyccb;
	ER	ercd;

	LOG_STP_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	cyccb = get_cyccb(cycid);

	t_lock_cpu();
	if (cyccb->cycsta) {
		cyccb->cycsta = FALSE;
		tmevtb_dequeue(&(cyccb->tmevtb));
	}
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_STP_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* __stp_cyc */

/*
 *  周期ハンドラ起動ルーチン
 */
#ifdef __cyccal

void
call_cychdr(CYCCB *cyccb)
{
	/*
	 *  次回の起動のためのタイムイベントブロックを登録する．
	 *
	 *  同じタイムティックで周期ハンドラを再度起動すべき場合には，
	 *  この関数から isig_tim に戻った後に，再度この関数が呼ばれる
	 *  ことになる．
	 */
	tmevtb_enqueue_cyc(cyccb, cyccb->evttim + cyccb->cycinib->cyctim);

	/*
	 *  周期ハンドラを，CPUロック解除状態で呼び出す．
	 */
	i_unlock_cpu();
	LOG_CYC_ENTER(cyccb);
	(*((CYCHDR)(cyccb->cycinib->cychdr)))(cyccb->cycinib->exinf);
	LOG_CYC_LEAVE(cyccb);
	i_lock_cpu();
}

#endif /* __cyccal */
