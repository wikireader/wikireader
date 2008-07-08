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
 *  @(#) $Id: startup.c,v 1.7 2003/12/13 05:09:54 hiro Exp $
 */

/*
 *	カーネルの初期化と終了処理
 */

#include "jsp_kernel.h"
#include "time_event.h"
#include "syslog.h"

/*
 *  カーネル動作状態フラグ
 */
BOOL	iniflg;

/*
 *  カーネルの初期化
 */
void
kernel_start()
{
	/*
	 *  ターゲット依存の初期化
	 */
	cpu_initialize();
	sys_initialize();
	tool_initialize();

	/*
	 *  システムログ機能の初期化
	 */
	syslog_initialize();

	/*
	 *  起動メッセージの表示
	 */
	print_banner();

	/*
	 *  各モジュールの初期化
	 *
	 *  タイムイベント管理モジュールは他のモジュールより先に初期化
	 *  する必要がある．
	 */
	tmevt_initialize();
	object_initialize();

	/*
	 *  初期化ルーチンの実行
	 */ 
	call_inirtn();

	/*
	 *  カーネル動作の開始
	 */
	iniflg = TRUE;
	exit_and_dispatch();
}

/*
 *  カーネルの終了処理
 */
void
kernel_exit()
{
	/*
	 *  割込みを禁止
	 */
	if (!sense_lock()) {
		if (sense_context()) {
			i_lock_cpu();
		}
		else  {
			t_lock_cpu();
		}
	}
	iniflg = FALSE;

	/*
	 *  終了処理ルーチンの実行
	 */
	call_terrtn();

	/*
	 *  atexit の処理とデストラクタの実行
	 */
	call_atexit();

	/*
	 *  システムログ機能の終了処理
	 */
	syslog_terminate();

	/*
	 *  ターゲット依存の終了処理
	 */
	cpu_terminate();
	sys_exit();
}
