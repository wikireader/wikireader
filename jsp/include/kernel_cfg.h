/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: kernel_cfg.h,v 1.5 2003/12/06 13:09:46 hiro Exp $
 */

/*
 *	kernel_cfg.c 用インクルードファイル
 */

#ifndef _KERNEL_CFG_H_
#define _KERNEL_CFG_H_

/*
 *  カーネル標準インクルードファイル
 */
#include <../kernel/jsp_kernel.h>

/*
 *  カーネルの各インクルードファイル
 */
#include <../kernel/task.h>
#include <../kernel/semaphore.h>
#include <../kernel/eventflag.h>
#include <../kernel/dataqueue.h>
#include <../kernel/mailbox.h>
#include <../kernel/mempfix.h>
#include <../kernel/cyclic.h>
#include <../kernel/interrupt.h>
#include <../kernel/exception.h>
#include <../kernel/time_event.h>

/*
 *  カーネルのワークエリアを定義するためのデータ型
 *
 *  スタック領域と固定長メモリプール領域を，VP型のサイズよりも大きい単
 *  位でアラインさせる必要がある場合には，__STK_UNIT と__MPF_UNIT を，
 *  それぞれアラインさせる単位のデータ型に定義する．
 *
 *  以下の定義は，__STK_UNIT および __MPF_UNIT のサイズが 2の巾乗であ
 *  ることを仮定している．
 */

#ifndef __STK_UNIT
typedef VP	__STK_UNIT;
#endif /* __STK_UNIT */

#ifndef __MPF_UNIT
typedef VP	__MPF_UNIT;
#endif /* __MPF_UNIT */

#define __TROUND_STK_UNIT(sz) \
		(((sz) + sizeof(__STK_UNIT) - 1) & ~(sizeof(__STK_UNIT) - 1))
#define __TCOUNT_STK_UNIT(sz) \
		(((sz) + sizeof(__STK_UNIT) - 1) / sizeof(__STK_UNIT))

#define __TROUND_MPF_UNIT(sz) \
		(((sz) + sizeof(__MPF_UNIT) - 1) & ~(sizeof(__MPF_UNIT) - 1))
#define __TCOUNT_MPF_UNIT(sz) \
		(((sz) + sizeof(__MPF_UNIT) - 1) / sizeof(__MPF_UNIT))

/*
 *  リネームしたシンボルを元に戻すためのインクルードファイル
 */
#include <../kernel/jsp_unrename.h>
#include <sys_unrename.h>
#include <cpu_unrename.h>

#endif /* _KERNEL_CFG_H_ */
