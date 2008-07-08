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
 *  @(#) $Id: stdafx.h,v 1.2 2003/06/30 15:58:39 takayuki Exp $
 */

// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。

#if !defined(AFX_STDAFX_H__D427C5C9_3DF2_4C06_BC74_B35DCF7062FC__INCLUDED_)
#define AFX_STDAFX_H__D427C5C9_3DF2_4C06_BC74_B35DCF7062FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
// CComModule クラスから派生したクラスを使用して、オーバーライドする場合
// _Module の名前は変更しないでください。
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>


/*
 *  ITRONデバッギングインタフェース関連の定義
 */

/*
 *  ディスパッチ種別
 */
#define DSP_NORMAL		0;	//タスクコンテキストからのディスパッチ
#define DSP_NONTSKCTX	1;	//割込み処理およびCPU例外からのディスパッチ 

typedef	short		 BITMASK;
typedef unsigned int INHNO;

/* 割込みハンドラ */
typedef struct t_rglog_interrupt{
	int inhno;	//割込みハンドラ番号
} T_RGLOG_INTERRUPT;

/* 割込みサービスハンドラ(未使用) */
typedef struct t_rglog_isr {
	int		isrid;	//割込みサービスルーチンID
	int     inhno;	//割込みハンドラ番号
} T_RGLOG_ISR;

/* タイムイベントハンドラ */
typedef struct t_rglog_timerhdr {
	unsigned int	type;	//タイマーの種別
	int             hdrid;	//タイムイベントハンドラのID
	void *          exinf;	//拡張情報
} T_RGLOG_TIMERHDR;

/* CPU例外ハンドラ */
typedef struct t_rglog_cupexc {
	int		tskid;	//対象となるタスクID
} T_RGLOG_CPUEXC;

/* タスク例外処理ルーチン */
typedef struct t_rglog_tskexc {
	int		tskid;	//対象となるタスクID
} T_RGLOG_TSKEXC;

/* タスク状態変化 */
typedef struct t_rglog_tskstat {
	int		tskid;		//タスクID
	int		tskstat;	//遷移先タスク状態
	int		tskwait;	//待ち状態
	int		wobjid;		//待ち対象のオブジェクトID
} T_RGLOG_TSKSTAT;

/* ディスパッチャ開始 */
typedef struct t_rglog_dispatch_enter {
	int				tskid;		//タスクID
	unsigned int	disptype;	//ディスパッチ種別
} T_RGLOG_DISPATCH_ENTER;

/* ディスパッチャ終了 */
typedef struct t_rglog_dispatch_leave {
	int		tskid;		//タスクID
} T_RGLOG_DISPATCH_LEAVE;

/* サービスコール */
typedef struct t_rglog_svc {
	int				fncno;		//機能コード
	unsigned int	prmcnt;		//パラメータ数
	void *			prmary[1];	//パラメータ
} T_RGLOG_SVC;

/* コメント（文字列のみのログ） */
typedef struct t_rglog_comment {
	unsigned int	length;		//文字列の長さ
	char			strtext[1];	//文字列(NULL終端)-中断あり
} T_RGLOG_COMMENT;

typedef struct t_rglog_header {
	unsigned int	logtype;
	unsigned int	logtim;
	unsigned int	valid;
	unsigned int	bufsz;
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
	unsigned int logtype;
	unsigned int logtim;
	unsigned int valid;
	unsigned int bufsz;
	char		 buf[1];
} T_RGLOG;

/*
 *  ログ情報の種別の定義
 */
#define LOG_TYP_INTERRUPT	0x01	/* 割込みハンドラ */
#define LOG_TYP_ISR			0x02	/* 割込みサービスハンドラ */
#define LOG_TYP_TIMERHDR	0x03	/* タイムイベントハンドラ */
#define LOG_TYP_CPUEXC		0x04	/* CPU例外ハンドラ */
#define LOG_TYP_TSKEXC		0x05	/* タスク例外処理ルーチン */
#define LOG_TYP_TSKSTAT		0x06	/* タスク状態変化 */
#define LOG_TYP_DISPATCH	0x07	/* ディスパッチャ */
#define LOG_TYP_SVC			0x08	/* サービスコール */
#define LOG_TYP_COMMENT		0x09	/* コメント（文字列のみのログ） */
#define LOG_ENTER			0x00	/* 入口／開始 */
#define LOG_LEAVE			0x80	/* 出口／終了 */

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_STDAFX_H__D427C5C9_3DF2_4C06_BC74_B35DCF7062FC__INCLUDED)
