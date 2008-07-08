/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: wait.h,v 1.6 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	待ち状態管理モジュール
 */

#ifndef _WAIT_H_
#define _WAIT_H_

#include "task.h"
#include "time_event.h"

/*
 *  待ち状態への移行
 *
 *  実行中のタスクを待ち状態に移行させる．具体的には，実行中のタスクを
 *  レディキューから削除し，TCB の winfoフィールド，WINFO の tmevtbフ
 *  ィールドを設定する．
 */
Inline void
make_wait(WINFO *winfo)
{
	make_non_runnable(runtsk);
	runtsk->winfo = winfo;
	winfo->tmevtb = NULL;
}

/*
 *  待ち状態への移行（タイムアウト指定）
 *
 *  実行中のタスクを，タイムアウト指定付きで待ち状態に移行させる．具体
 *  的には，実行中のタスクをレディキューから削除し，TCB の winfoフィー
 *  ルド，WINFO の tmevtbフィールドを設定する．また，タイムイベントブ
 *  ロックを登録する．
 */
extern void	make_wait_tmout(WINFO *winfo, TMEVTB *tmevtb, TMO tmout);

/*
 *  待ち解除
 *
 *  tcb で指定されるタスクの待ち状態を解除する．具体的には，タイムイベ
 *  ントブロックが登録されていれば，それを登録解除する．また，タスク状
 *  態を更新し，待ち解除したタスクからの返値を E_OK とする．待ちキュー
 *  からの削除は行わない．待ち解除したタスクへのディスパッチが必要な場
 *  合には TRUE を返す．
 */
extern BOOL	wait_complete(TCB *tcb);

/*
 *  タイムアウトに伴う待ち解除
 *
 *  tcb で指定されるタスクが，待ちキューにつながれていれば待ちキューか
 *  ら削除し，タスク状態を更新する．また，待ち解除したタスクからの返値
 *  を，wait_tmoutでは E_TMOUT，wait_tmout_ok では E_OK とする．待ち解
 *  除したタスクへのディスパッチが必要な時は，reqflg を TRUE にする．
 *  wait_tmout_ok は，dly_tsk で使うためのもので，待ちキューから削除す
 *  る処理を行わない．
 *  いずれの関数も，タイムイベントのコールバック関数として用いるための
 *  もので，割込みハンドラから呼び出されることを想定している．
 */
extern void	wait_tmout(TCB *tcb);
extern void	wait_tmout_ok(TCB *tcb);

/*
 *  待ち状態の強制解除
 *
 *  tcb で指定されるタスクの待ち状態を強制的に解除する．具体的には，タ
 *  スクが待ちキューにつながれていれば待ちキューから削除し，タイムイベ
 *  ントブロックが登録されていればそれを登録解除する．
 *  wait_cancel は，タスクの状態は更新しない．
 *  wait_release は，タスクの状態を更新し，待ち解除したタスクからの返
 *  値を E_RLWAI とする．また，待ち解除したタスクへのディスパッチが必
 *  要な場合には TRUE を返す．
 */
extern void	wait_cancel(TCB *tcb);
extern BOOL	wait_release(TCB *tcb);

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分操作ルーチン
 *
 *  同期・通信オブジェクトの初期化ブロックと管理ブロックの先頭部分は共
 *  通になっている．以下は，その共通部分を扱うための型およびルーチン群
 *  である．
 *  複数の待ちキューを持つ同期・通信オブジェクトの場合，先頭以外の待ち
 *  キューを操作する場合には，これらのルーチンは使えない．また，オブジェ
 *  クト属性の TA_TPRI ビットを参照するので，このビットを他の目的に使っ
 *  ている場合も，これらのルーチンは使えない．
 */

/*
 *  同期・通信オブジェクトの初期化ブロックの共通部分
 */
typedef struct wait_object_initialization_block {
	ATR	wobjatr;	/* オブジェクト属性 */
} WOBJINIB;

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分
 */
typedef struct wait_object_control_block {
	QUEUE		wait_queue;	/* 待ちキュー */
	const WOBJINIB	*wobjinib;	/* 初期化ブロックへのポインタ */
} WOBJCB;

/*
 *  同期・通信オブジェクト待ち情報ブロックの定義
 */
typedef struct wait_object_waiting_information {
	WINFO	winfo;		/* 標準の待ち情報ブロック */
	WOBJCB	*wobjcb;	/* 待ちオブジェクトの管理ブロック */
} WINFO_WOBJ;

/*
 *  同期・通信オブジェクトに対する待ち状態への移行
 *  
 *  実行中のタスクを待ち状態に移行させ，同期・通信オブジェクトの待ちキュー
 *  につなぐ．また，待ち情報ブロック（WINFO）の wobjcb を設定する．
 *  wobj_make_wait_tmout は，タイムイベントブロックの登録も行う．
 */
extern void	wobj_make_wait(WOBJCB *wobjcb, WINFO_WOBJ *winfo);
extern void	wobj_make_wait_tmout(WOBJCB *wobjcb, WINFO_WOBJ *winfo,
					TMEVTB *tmevtb, TMO tmout);

/*
 *  タスク優先度変更時の処理
 *
 *  同期・通信オブジェクトに対する待ち状態にあるタスクの優先度が変更さ
 *  れた場合に，待ちキューの中でのタスクの位置を修正する．
 */
extern void	wobj_change_priority(WOBJCB *wobjcb, TCB *tcb);

#endif /* _WAIT_H_ */
