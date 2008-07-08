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
 *  @(#) $Id: mempfix.h,v 1.5 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	固定長メモリプール機能
 */

#ifndef _MEMPFIX_H_
#define _MEMPFIX_H_

#include "queue.h"

/*
 *  固定長メモリプール初期化ブロック
 */
typedef struct fixed_memorypool_initialization_block {
	ATR	mpfatr;		/* 固定長メモリプール属性 */
	UINT	blksz;		/* メモリブロックのサイズ（丸めた値） */
	VP	mpf;		/* 固定長メモリプール領域の先頭番地 */
	VP	limit;		/* 固定長メモリプール領域の上限番地 */
} MPFINIB;

/*
 *  空ブロックリストの定義
 */
typedef struct free_list {
	struct free_list *next;
} FREEL;

/*
 *  固定長メモリプール管理ブロック
 */
typedef struct fixed_memorypool_control_block {
	QUEUE	wait_queue;	/* 固定長メモリプール待ちキュー */
	const MPFINIB *mpfinib;	/* 固定長メモリプール初期化ブロック */
	VP	unused;		/* 未使用領域の先頭番地 */
	FREEL	*freelist;	/* 空きブロックのリスト */
} MPFCB;

/*
 *  固定長メモリプール機能の初期化
 */
extern void	mempfix_initialize(void);

/*
 *  固定長メモリプールからブロックを獲得
 */
extern BOOL	mempfix_get_block(MPFCB *mpfcb, VP *p_blk);

#endif /* _MEMPFIX_H_ */
