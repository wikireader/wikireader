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
 *  @(#) $Id: eventlog.h,v 1.7 2003/12/15 07:19:22 takayuki Exp $
 */


#ifndef __EVENTLOG_H__
#define __EVENTLOG_H__

#include <constants.h>

#include <stdarg.h>
#include "syslog.h"

	/*
	 *  ITRONデバッギングインタフェース関連の定義
	 */

	/*
	 *  ディスパッチ種別
	 */
#define DSP_NORMAL		0;	//タスクコンテキストからのディスパッチ
#define DSP_NONTSKCTX	1;	//割込み処理およびCPU例外からのディスパッチ 

	typedef	W		BITMASK;
	typedef UINT	INHNO;

	/* 割込みハンドラ */
	typedef struct t_rglog_interrupt{
		INHNO	inhno;	//割込みハンドラ番号
	} T_RGLOG_INTERRUPT;

	/* 割込みサービスハンドラ(未使用) */
	typedef struct t_rglog_isr {
		ID		isrid;	//割込みサービスルーチンID
		INHNO	inhno;	//割込みハンドラ番号
	} T_RGLOG_ISR;

	/* タイムイベントハンドラ */
	typedef struct t_rglog_timerhdr {
		UINT	type;	//タイマーの種別
		ID		hdrid;	//タイムイベントハンドラのID
		VP_INT	exinf;	//拡張情報
	} T_RGLOG_TIMERHDR;

	/* CPU例外ハンドラ */
	typedef struct t_rglog_cupexc {
		ID		tskid;	//対象となるタスクID
	} T_RGLOG_CPUEXC;

	/* タスク例外処理ルーチン */
	typedef struct t_rglog_tskexc {
		ID		tskid;	//対象となるタスクID
	} T_RGLOG_TSKEXC;

	/* タスク状態変化 */
	typedef struct t_rglog_tskstat {
		ID		tskid;		//タスクID
		STAT	tskstat;	//遷移先タスク状態
		STAT	tskwait;	//待ち状態
		ID		wobjid;		//待ち対象のオブジェクトID
	} T_RGLOG_TSKSTAT;

	/* ディスパッチャ開始 */
	typedef struct t_rglog_dispatch_enter {
		ID		tskid;		//タスクID
		UINT	disptype;	//ディスパッチ種別
	} T_RGLOG_DISPATCH_ENTER;

	/* ディスパッチャ終了 */
	typedef struct t_rglog_dispatch_leave {
		ID		tskid;		//タスクID
	} T_RGLOG_DISPATCH_LEAVE;

	/* サービスコール */
	typedef struct t_rglog_svc {
		FN		fncno;		//機能コード
		UINT	prmcnt;		//パラメータ数
		VP_INT	prmary[1];	//パラメータ
	} T_RGLOG_SVC;

	/* コメント（文字列のみのログ） */
	typedef struct t_rglog_comment {
		UINT	length;		//文字列の長さ
		char	strtext[1];	//文字列(NULL終端)-中断あり
	} T_RGLOG_COMMENT;

	typedef struct t_rglog_header {
		UINT	logtype;
		SYSTIM	logtim;
		BITMASK	valid;
		UINT	bufsz;
	} T_RGLOG_HEADER;

	typedef union t_rglog_body
	{
		T_RGLOG_INTERRUPT		interrupt;
		T_RGLOG_ISR				isr;
		T_RGLOG_TIMERHDR		timerhdr;
		T_RGLOG_CPUEXC			cpuexc;
		T_RGLOG_TSKEXC			tskexc;
		T_RGLOG_TSKSTAT			tskstat;
		T_RGLOG_DISPATCH_ENTER	dispatch_enter;
		T_RGLOG_DISPATCH_LEAVE	dispatch_leave;
		T_RGLOG_SVC				svc;
		T_RGLOG_COMMENT			comment;
	} T_RGLOG_BODY;

	typedef struct DBIFLOG
	{
		struct	t_rglog_header	header;
		union	t_rglog_body	body;
	} DBIFLOG;

	typedef struct t_rglog
	{
		UINT	logtype;
		SYSTIM	logtim;
		BITMASK	valid;
		UINT	bufsz;
		char	buf[1];
	} T_RGLOG;

#ifdef EVENTLOG_ENABLE
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

		EXTERN void event_write_syslog(const SYSLOG * str);
		EXTERN void event_write(unsigned int logtype, unsigned int valid, UINT bufsz, ... );
		EXTERN void event_write_svc_enter(int fncd, unsigned int params, ... );
		EXTERN void event_write_svc_leave(int fncd, unsigned int ercd, unsigned int retaddr);
		EXTERN void event_write_wait_unknown(int tskid);

#undef EXTERN

#endif //EVENTLOG_ENABLE

#endif //__EVENTLOG_H__

