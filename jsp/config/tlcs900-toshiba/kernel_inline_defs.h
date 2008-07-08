/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2006 by Witz Corporation, JAPAN
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
 *  @(#) $Id: kernel_inline_defs.h,v 1.1 2006/04/10 08:19:25 honda Exp $
 */

/*
 *	TOSHIBA開発環境向けビルトイン(inline)関数の定義
 *
 *  このインクルードファイルは，カーネル内部のcファイルからインクルード
 *  されているtool_defs.h からインクルードされる．他のファイルからは直接
 *  インクルードすることはない．他のインクルードファイルに先立って処理さ
 *  れるため，他のインクルードファイルに依存してはならない．
 */

#ifndef _KERNEL_INLINE_DEFS_H_
#define _KERNEL_INLINE_DEFS_H_

/*
 *  TOSHIBA IDEでは#pragma inline記述にてインライン関数を定義する
 *  このためカーネル内部にのみ以下の処理を適用する．
 *  tool_defs.h にてカーネル共通部のInlineおよびinlineの定義は全て
 *  無効とし，既知のインライン関数に対する#pragma inline定義本ファ
 *  イルに記載する．
 */
/* sil.h	*/
#pragma inline sil_reb_mem
#pragma inline sil_wrb_mem
#pragma inline sil_reh_mem
#pragma inline sil_wrh_mem
#pragma inline sil_reh_lem
#pragma inline sil_wrh_lem
#pragma inline sil_reh_bem
#pragma inline sil_wrh_bem
#pragma inline sil_rew_mem
#pragma inline sil_wrw_mem
#pragma inline sil_rew_lem
#pragma inline sil_wrw_lem
#pragma inline sil_rew_bem
#pragma inline sil_wrw_bem

/* t_services.h	*/
#pragma inline _t_perror
#pragma inline _t_panic

/* t_syslog.h	*/
#pragma inline _syslog_0
#pragma inline _syslog_1
#pragma inline _syslog_2
#pragma inline _syslog_3
#pragma inline _syslog_4
#pragma inline _syslog_5
#pragma inline _syslog_6

/* mailbox.c	*/
#pragma inline enqueue_msg_pri

/* task.c	*/
#pragma inline bitmap_search

/* time_event.c	*/
#pragma inline tmevtb_delete_top

/* wait.c	*/
#pragma inline make_non_wait
#pragma inline queue_insert_tpri
#pragma inline wobj_queue_insert

/* queue.h	*/
#pragma inline queue_initialize
#pragma inline queue_insert_prev
#pragma inline queue_delete
#pragma inline queue_delete_next
#pragma inline queue_empty

/* task_event.h	*/
#pragma inline tmevtb_enqueue
#pragma inline tmevtb_enqueue_evttim
#pragma inline tmevtb_dequeue

/* wait.h	*/
#pragma inline make_wait

/* serial.c	*/
#pragma inline serial_snd_chr


#endif	/* _KERNEL_INLINE_DEFS_H_	*/
