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
 *  @(#) $Id: linux_sigio.c,v 1.7 2003/06/04 01:48:33 hiro Exp $
 */


#include <t_services.h>
#include "../kernel/jsp_kernel.h"
#include "../kernel/queue.h"
#include "kernel_id.h"
#include <signal.h>
#include <linux_sigio.h>


/*
 *  SIGIO通知イベントキュー
 */

static QUEUE	sigio_queue;

/*
 *  SIGIO通知ハンドラ
 *
 *  SIGIO通知処理タスクを起床する．
 */
void
linux_sigio_handler()
{
	syscall(iwup_tsk(TSK_LINUX_SIGIO));
}

/*
 *  ノンブロッキングI/O モジュール起動ルーチン
 *
 *  SIGIO通知イベントキューを初期化．SIGIO通知処理タスクとセマフォを生
 *  成．SIGIO通知ハンドラを設定．
 */
static int	initflag = 0;		/* 初期化済フラグ */

void
linux_sigio_initialize(VP_INT exinf)
{
	if (!initflag) {
		queue_initialize(&sigio_queue);
		initflag = 1;
	}
}

/*
 *  SIGIO通知処理タスク本体
 */
void
linux_sigio_task(void)
{
	QUEUE	*q;
	SIGIOEB	*event;

	while (slp_tsk() == E_OK) {
		/*
		 *  SIGIO通知イベントキュー中の各イベントブロックのコー
		 *  ルバックルーチンを呼びだす．コールバックルーチンが
		 *  0 以外を返した場合，そのイベントブロックを削除する．
		 */
		syscall(wai_sem(SEM_LINUX_SIGIO));
		q = sigio_queue.next;
		while (q != &sigio_queue) {
			event = ((SIGIOEB *) q);
			q = q->next;
			if ((*(event->callback))(event->arg)) {
				queue_delete((QUEUE *) &(event->queue));
			}
		}
		syscall(sig_sem(SEM_LINUX_SIGIO));
	}
	ext_tsk();
}

/*
 *  SIGIO通知イベントをSIGIO通知イベントキューへ挿入
 */
ER
enqueue_sigioeb(SIGIOEB *event)
{
	syscall(wai_sem(SEM_LINUX_SIGIO));
	queue_insert_prev(&sigio_queue , (QUEUE *) &(event->queue));
	syscall(sig_sem(SEM_LINUX_SIGIO));
	return(E_OK);
}

/*
 *   システム起動時登録用
 *   システム起動時、初期化ルーチンは非タスクコンテキストから
 *   呼ばれるためenqueue_sigioeb()でセマフォが使用できない
 */
ER
enqueue_sigioeb_initialize(SIGIOEB *event)
{
	queue_insert_prev(&sigio_queue , (QUEUE *) &(event->queue));
	return(E_OK);
}
