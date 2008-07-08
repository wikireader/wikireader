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
 *  @(#) $Id: dataqueue.h,v 1.5 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	データキュー機能
 */

#ifndef _DATAQUEUE_H_
#define _DATAQUEUE_H_

#include "queue.h"

/*
 *  データキュー初期化ブロック
 */
typedef struct dataqueue_initialization_block {
	ATR	dtqatr;		/* データキュー属性 */
	UINT	dtqcnt;		/* データキュー領域の容量 */
	VP	dtq;		/* データキュー領域の先頭番地 */
} DTQINIB;

/*
 *  データキュー管理ブロック
 */
typedef struct dataqueue_control_block {
	QUEUE	swait_queue;	/* データキュー送信待ちキュー */
	const DTQINIB *dtqinib;	/* データキュー初期化ブロックへのポインタ */
	QUEUE	rwait_queue;	/* データキュー受信待ちキュー */
	UINT	count;		/* データキュー中のデータの数 */
	UINT	head;		/* 最初のデータの格納場所 */
	UINT	tail;		/* 最後のデータの格納場所の次 */
} DTQCB;

/*
 *  データキュー機能の初期化
 */
extern void	dataqueue_initialize(void);

/*
 *  データキュー領域にデータを格納
 */
extern BOOL	enqueue_data(DTQCB *dtqcb, VP_INT data);

/*
 *  データキュー領域にデータを強制格納
 */
extern void	force_enqueue_data(DTQCB *dtqcb, VP_INT data);

/*
 *  データキュー領域からデータを取出し
 */
extern BOOL	dequeue_data(DTQCB *dtqcb, VP_INT *p_data);

/*
 *  受信待ちキューの先頭タスクへのデータ送信
 */
extern TCB	*send_data_rwait(DTQCB *dtqcb, VP_INT data);

/*
 *  送信待ちキューの先頭タスクからのデータ受信
 */
extern TCB	*receive_data_swait(DTQCB *dtqcb, VP_INT *p_data);

#endif /* _DATAQUEUE_H_ */
