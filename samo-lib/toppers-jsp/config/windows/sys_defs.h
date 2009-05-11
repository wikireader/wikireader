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
 *  @(#) $Id: sys_defs.h,v 1.17 2003/12/15 07:37:50 takayuki Exp $
 */


#ifndef __SYS_DEFS_H
#define __SYS_DEFS_H

#include <cpu_rename.h>

    /*
     *  使用するディスパッチャを選択
     */
#define WIN32_THREADMODEL               //スレッド = タスク
//#define WIN32_NATIVEDISPATCHER        //x86命令ディスパッチャ(PUSHAD, FSAVE)  ※選択不可
//#define WIN32_EXCEPTIONDISPATCHER     //Windows構造化例外ディスパッチャ ※選択不可
//#define WIN32_VIRTUALMACHINE          //仮想マシンクラスライブラリ ※選択不可

    /*
     * Windows依存部拡張機能の選択
     */
//#define DEBUG_ENABLE       /* デバッグ出力コンソールを有効にするかどうか */

//#define DEVICE_ENABLE      /* デバイスエミュレーション機能を使用するかどうか */

//#define EVENTLOG_ENABLE    /* カーネル動作ログ取得機構を有効にするかどうか */

	/*
     *  シミュレーションに用いるタイマーの種別 
     */
//#define TIMER_NOTHING         // 使用しない (外部から供給する)
#define TIMER_MULTIMEDIATIMER	// マルチメディアタイマ
//#define TIMER_WINDOWSTIMER	// 標準のタイマ (SetTimerとかKillTimerとか)
//#define TIMER_WAITABLETIMER   // NT4のWaitableTimerを使用するバージョン

    /*
     *  その他の機能選択
     */
//#define KERNEL_DEBUG_MODE    /* カーネルのデバッグログを出す(Windows依存部デバッグ用) */


#ifndef _MACRO_ONLY
/*
 *  システムアボートハンドラ
 */
extern void kernel_abort(void);

#endif /* _MACRO_ONLY */

#endif


