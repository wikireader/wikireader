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
 *  @(#) $Id: jsp_kernel.h,v 1.10 2003/07/01 13:31:35 hiro Exp $
 */

/*
 *	JSPカーネル 標準インクルードファイル
 *
 *  このインクルードファイルは，カーネルを構成するプログラムのソースファ
 *  イルで必ずインクルードするべき標準インクルードファイルである．
 *
 *  アセンブリ言語のソースファイルやシステムコンフィギュレーションファ
 *  イルからこのファイルをインクルードする時は，_MACRO_ONLY を定義して
 *  おくことで，マクロ定義以外の記述を除くことができる．
 */

#ifndef _JSP_KERNEL_H_
#define _JSP_KERNEL_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include "jsp_rename.h"

/*
 *  アプリケーションと共通のインクルードファイル
 */
#include <kernel.h>

/*
 *  ターゲット依存情報の定義
 */
#include <t_config.h>

/*
 *  オブジェクトIDの最小値の定義
 */
#define	TMIN_TSKID	1	/* タスクIDの最小値 */
#define	TMIN_SEMID	1	/* セマフォIDの最小値 */
#define	TMIN_FLGID	1	/* フラグIDの最小値 */
#define	TMIN_DTQID	1	/* データキューIDの最小値 */
#define	TMIN_MBXID	1	/* メールボックスIDの最小値 */
#define	TMIN_MPFID	1	/* 固定長メモリプールIDの最小値 */
#define	TMIN_CYCID	1	/* 周期ハンドラIDの最小値 */

/*
 *  優先度の段階数の定義
 */
#define	TNUM_TPRI	(TMAX_TPRI - TMIN_TPRI + 1)
#define	TNUM_MPRI	(TMAX_MPRI - TMIN_MPRI + 1)

/*
 *  カーネル内で用いるマクロの定義
 */
#define SYSCALL

/*
 *  ヘッダファイルを持たないモジュールの関数・変数の宣言
 */
#ifndef _MACRO_ONLY

/*
 *  各モジュールの初期化（kernel_cfg.c）
 */
extern void	object_initialize(void);

/*
 *  初期化ルーチンの実行（kernel_cfg.c）
 */
extern void	call_inirtn(void);

/*
 *  終了処理ルーチンの実行（kernel_cfg.c）
 */
extern void	call_terrtn(void);

/*
 *  起動メッセージの表示（banner.c）
 */
extern void	print_banner(void);

/*
 *  カーネルの終了処理（startup.c）
 */
extern void	kernel_exit(void);

/*
 *  カーネル動作状態フラグ（startup.c）
 */
extern BOOL	iniflg;

#endif /* _MACRO_ONLY */
#endif /* _JSP_KERNEL_H_ */
