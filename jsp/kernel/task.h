/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: task.h,v 1.12 2007/03/26 06:14:05 honda Exp $
 */

/*
 *	タスク管理モジュール
 */

#ifndef _TASK_H_
#define _TASK_H_

#include "queue.h"
#include "time_event.h"

/*
 *  タスク優先度の内部表現・外部表現変換マクロ
 */
#define INT_PRIORITY(x)		((UINT)((x) - TMIN_TPRI))
#define EXT_TSKPRI(x)		((PRI)(x) + TMIN_TPRI)

/*
 *  タスク状態の内部表現
 *
 *  TCB 中のタスク状態では，実行状態（RUNNING）と実行可能状態（READY）
 *  は区別しない．両状態を総称して，実行できる状態（RUNNABLE）と呼ぶ．
 *  二重待ち状態は，(TS_WAITING | TS_SUSPENDED) で表す．TS_WAIT_???? 
 *  は，待ち状態に伴う付属状態を表し，待ち状態（二重待ち状態を含む）の
 *  場合にのみ設定される．
 */
#define	TS_DORMANT	0x00u	/* 休止状態 */
#define	TS_RUNNABLE	0x01u	/* 実行できる状態 */
#define	TS_WAITING	0x02u	/* 待ち状態 */
#define	TS_SUSPENDED	0x04u	/* 強制待ち状態 */

#define	TS_WAIT_SLEEP	0x08u	/* 起床待ち状態 */
#define	TS_WAIT_WOBJ	0x10u	/* 同期・通信オブジェクトに対する待ち状態 */
#define	TS_WAIT_WOBJCB	0x20u	/* 共通部分の待ちキューにつながっている */

/*
 *  タスク状態判別マクロ
 *
 *  TSTAT_DORMANT はタスクが休止状態であるかどうかを，TSTAT_RUNNABLE 
 *  はタスクが実行できる状態であるかどうかを判別する．TSTAT_WAITING は
 *  待ち状態と二重待ち状態のいずれかであるかどうかを，TSTAT_SUSPENDED 
 *  は強制待ち状態と二重待ち状態のいずれかであるかどうかを判別する．
 */
#define TSTAT_DORMANT(tstat)	((tstat) == TS_DORMANT)
#define TSTAT_RUNNABLE(tstat)	(((tstat) & TS_RUNNABLE) != 0)
#define TSTAT_WAITING(tstat)	(((tstat) & TS_WAITING) != 0)
#define TSTAT_SUSPENDED(tstat)	(((tstat) & TS_SUSPENDED) != 0)

/*
 *  待ち情報ブロック（WINFO）の定義
 *
 *  タスクが待ち状態の間は，TCB およびそこから指される WINFO を次のよ
 *  うに設定しなければならない．
 *
 *  (a) TCB のタスク状態を待ち状態にする．その際に，待ち状態に伴う付属
 *  状態（TS_WAIT_????）も設定する．
 *
 *  (b) タイムアウトを監視するために，タイムイベントブロックを登録する．
 *  登録するタイムイベントブロックは，待ちに入るサービスコール処理関数
 *  のローカル変数として確保し，それへのポインタを WINFO の tmevtb に
 *  記憶する．タイムアウトの監視が必要ない場合（永久待ちの場合）には，
 *  tmevtb を NULL にする．
 *
 *  同期・通信オブジェクトに対する待ち状態の場合には，標準の WINFO に 
 *  wobjcbフィールドを追加した構造体（WINFO_WOBJ，wait.h で定義）を使
 *  い，タスク状態に TS_WAIT_WOBJ を設定する．また，以下の (c)〜(e) の
 *  設定を行う必要がある．同期・通信オブジェクトに関係しない待ち（起床
 *  待ち，時間経過待ち）の場合には，(c)〜(e) は必要ない．
 *
 *  (c) TCB を待ち対象の同期・通信オブジェクトの待ちキューにつなぐ．待
 *  ちキューにつなぐために，task_queue を使う．TCB を同期・通信オブジェ
 *  クトの管理ブロックの共通部分（WOBJCB）の待ちキューにつないだ場合に
 *  は，タスク状態に TS_WAIT_WOBJCB を設定する．
 *
 *  (d) 待ち対象の同期・通信オブジェクトの管理ブロックへのポインタを，
 *  WINFO_WOBJ の wobjcb に記憶する．
 *  JSPカーネルで wobjcb を使うのは，優先度順の待ちキューにつながれて
 *  いるタスクの優先度が変更された場合のみであるが，デバッグ情報を取り
 *  出しやすいように，待ちキューが優先度順でない場合にも wobjcb を設定
 *  する．
 *
 *  (e) 待ち対象の同期・通信オブジェクトに依存して記憶することが必要な
 *  情報がある場合には，WINFO_WOBJ に必要な情報のためのフィールドを追加
 *  した構造体を定義し，WINFO_WOBJ の代わりに用いる．
 *
 *  待ち状態を解除する際には，待ち解除したタスクからの返値を WINFO の
 *  wercd に設定する．wercd を使うのは待ち解除以降であるのに対して，
 *  tmevtb は待ち解除後は使わないため，メモリ節約のために共用体（union）
 *  を使っている．
 */
typedef union waiting_information {
	ER	wercd;		/* 待ち解除時のエラーコード */
	TMEVTB	*tmevtb;	/* 待ち状態用のタイムイベントブロック */
} WINFO;

/*
 *  タスク初期化ブロック
 *
 *  タスクに関する情報を，値が変わらないためにROMに置ける部分（タスク
 *  初期化ブロック）と，値が変化するためにRAMに置かなければならない部
 *  分（タスク管理ブロック，TCB）に分離し，TCB内に対応するタスク初期化
 *  ブロックを指すポインタを入れる．タスク初期化ブロック内に対応する
 *  TCBを指すポインタを入れる方法の方が，RAMの節約の観点からは望ましい
 *  が，実行効率が悪くなるために採用していない．他のオブジェクトについ
 *  ても同様に扱う．
 *  タスク初期化ブロックには，DEF_TEX で定義されるタスク例外処理ルーチ
 *  ンに関する情報も含む．
 */
typedef struct task_initialization_block {
	ATR	tskatr;		/* タスク属性 */
	VP_INT	exinf;		/* タスクの拡張情報 */
	FP	task;		/* タスクの起動番地 */
	UINT	ipriority;	/* タスクの起動時優先度（内部表現） */
	SIZE	stksz;		/* スタック領域のサイズ（丸めた値） */
	VP	stk;		/* スタック領域の先頭番地 */

	ATR	texatr;		/* タスク例外処理ルーチン属性 */
	FP	texrtn;		/* タスク例外処理ルーチンの起動番地 */
} TINIB;

/*
 *  TCB 中のフィールドのビット幅の定義
 *
 *  TCB 中のフィールドの配置は，性能に大きく影響すると思われるため，ター
 *  ゲット依存にフィールドのビット幅を変更することを許している．
 */
#ifndef TBIT_TCB_TSTAT
#define	TBIT_TCB_TSTAT		6	/* tstat フィールドのビット幅 */
#endif /* TBIT_TCB_TSTAT */

#ifndef TBIT_TCB_PRIORITY
#define	TBIT_TCB_PRIORITY	4	/* priority フィールドのビット幅 */
#endif /* TBIT_TCB_PRIORITY */

/*
 *  タスク管理ブロック（TCB）
 *
 *  JSPでは，タスクの起動要求キューイング数の最大値（TMAX_ACTCNT）と起
 *  床要求キューイング数の最大値（TMAX_WUPCNT）は 1 に固定されているた
 *  め，キューイングされているかどうかの真偽値で表現することができる．
 *  また，強制待ち要求ネスト数の最大値（TMAX_SUSCNT）が 1 に固定されて
 *  いるので，強制待ち要求ネスト数（suscnt）は必要ない．
 */
typedef struct task_control_block {
	QUEUE	task_queue;	/* タスクキュー */
	const TINIB *tinib;	/* タスク初期化ブロックへのポインタ */

	unsigned int	tstat : TBIT_TCB_TSTAT;	/* タスク状態（内部表現）*/
	unsigned int	priority : TBIT_TCB_PRIORITY;
						/* 現在の優先度（内部表現）*/
	unsigned int	actcnt : 1;		/* 起動要求キューイング */
	unsigned int	wupcnt : 1;		/* 起床要求キューイング */
	unsigned int	enatex : 1;		/* タスク例外処理許可状態 */

	TEXPTN	texptn;		/* 保留例外要因 */
	WINFO	*winfo;		/* 待ち情報ブロックへのポインタ */
	CTXB	tskctxb;	/* タスクコンテキストブロック */
} TCB;

/*
 *  実行状態のタスク
 *
 *  実行状態のタスク（＝プロセッサがコンテキストを持っているタスク）の
 *  TCB を指すポインタ．実行状態のタスクがない場合は NULL にする．
 *  サービスコールの処理中で，自タスク（サービスコールを呼び出したタス
 *  ク）に関する情報を参照する場合は runtsk を使う．runtsk を書き換え
 *  るのは，ディスパッチャ（と初期化処理）のみである．
 */
extern TCB	*runtsk;

/*
 *  最高優先順位のタスク
 *
 *  実行できるタスクの中で最高優先順位のタスクの TCB を指すポインタ．実
 *  行できるタスクがない場合は NULL にする．
 *  ディスパッチ禁止状態など，ディスパッチが保留されている間は，runtsk
 *  と一致しているとは限らない．
 */
extern TCB	*schedtsk;

/*
 *  ディスパッチ／タスク例外処理ルーチン起動要求フラグ
 *
 *  割込みハンドラ／CPU例外ハンドラの出口処理に，ディスパッチまたは
 *  タスク例外処理ルーチンの起動を要求することを示すフラグ．
 */
extern BOOL	reqflg;

/*
 *  ディスパッチ許可状態
 *
 *  ディスパッチ許可状態である（すなわち，ディスパッチ禁止状態でない）
 *  ことを示すフラグ．
 */
extern BOOL	enadsp;

/*
 *  レディキュー
 *
 *  レディキューは，実行できる状態のタスクを管理するためのキューである．
 *  実行状態のタスクも管理しているため，レディ（実行可能）キューという
 *  名称は正確ではないが，レディキューという名称が定着しているため，こ
 *  の名称で呼ぶことにする．
 *
 *  レディキューは，優先度ごとのタスクキューで構成されている．タスクの
 *  TCBは，該当する優先度のキューに登録される．
 */
extern QUEUE	ready_queue[TNUM_TPRI];

/*
 *  レディキューサーチのためのビットマップ
 *
 *  レディキューのサーチを効率よく行うために，優先度ごとのタスクキュー
 *  にタスクが入っているかどうかを示すビットマップを用意している．ビッ
 *  トマップを使うことで，メモリアクセスの回数を減らすことができるが，
 *  ビット操作命令が充実していないプロセッサで，優先度の段階数が少ない
 *  場合には，ビットマップ操作のオーバーヘッドのために，逆に効率が落ち
 *  る可能性もある．
 */
extern UINT	ready_primap;

/*
 *  タスクIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_tskid;

/*
 *  タスク初期化ブロックのエリア（kernel_cfg.c）
 */
extern const TINIB	tinib_table[];

/*
 *  タスク生成順序テーブル（kernel_cfg.c）
 */
extern const ID	torder_table[];

/*
 *  TCBのエリア（kernel_cfg.c）
 */
extern TCB	tcb_table[];

/*
 *  タスクの数
 */
#define TNUM_TSK	((UINT)(tmax_tskid - TMIN_TSKID + 1))

/*
 *  タスクIDからTCBを取り出すためのマクロ
 */
#define INDEX_TSK(tskid)	((UINT)((tskid) - TMIN_TSKID))
#define get_tcb(tskid)		(&(tcb_table[INDEX_TSK(tskid)]))
#define get_tcb_self(tskid)	((tskid) == TSK_SELF ? runtsk : get_tcb(tskid))

/*
 *  TCBからタスクIDを取り出すためのマクロ
 */
#define	TSKID(tcb)	((ID)(((tcb) - tcb_table) + TMIN_TSKID))

/*
 *  タスク管理モジュールの初期化
 */
extern void	task_initialize(void);

/*
 *  最高優先順位タスクのサーチ
 *
 *  レディキュー中の最高優先順位のタスクをサーチし，そのTCBへのポインタ
 *  を返す．レディキューが空の場合には，この関数を呼び出してはならない．
 */
extern TCB	*search_schedtsk(void);

/*
 *  実行できる状態への移行
 *
 *  tcb で指定されるタスクの状態を実行できる状態とし，レディキューに挿
 *  入する．実行できる状態になったタスクの優先度が，最高優先順位のタス
 *  クの優先度よりも高い場合は，最高優先順位のタスクを更新し，ディスパッ
 *  チ許可状態であれば TRUE を返す．そうでない場合は FALSE を返す．
 */
extern BOOL	make_runnable(TCB *tcb);

/*
 *  実行できる状態から他の状態への移行
 *
 *  tcb で指定されるタスクをレディキューから削除する．tcb で指定したタ
 *  スクが最高優先順位のタスクであった場合には，最高優先順位のタスクを
 *  設定しなおし，ディスパッチ許可状態であれば TRUE を返す．そうでない
 *  場合は FALSE を返す．タスクの状態は更新しない．
 */
extern BOOL	make_non_runnable(TCB *tcb);

/*
 *  休止状態への移行
 *
 *  tcb で指定されるタスクの状態を休止状態とする．また，タスクの起動時
 *  に初期化すべき変数の初期化と，タスク起動のためのコンテキストを設定
 *  する．
 */
extern void	make_dormant(TCB *tcb);

/*
 *  休止状態から実行できる状態への移行
 *
 *  tcb で指定されるタスクの状態を休止状態から実行できる状態とする．実
 *  行できる状態に移行したタスクへのディスパッチが必要な場合は TRUE，
 *  そうでない場合は FALSE を返す．
 */
extern BOOL	make_active(TCB *tcb);

/*
 *  実行状態のタスクの終了
 *
 *  exit_task を ext_tsk に埋め込まずに別の関数にしているのは，
 *  create_context と activate_context が自タスクの終了処理で使用する
 *  スタックを破壊しないようにするための対策の一環である．ext_tsk とは
 *  別関数にしておくことで，ローカル変数も含めて，使用するスタック領域
 *  が重ならないようにできる．また，コンパイラが自動的にインライン展開
 *  するのを避けるために，ext_tsk とは別のファイルに入れている．
 */
extern void	exit_task(void);

/*
 *  レディキュー中のタスクの優先度の変更
 *
 *  tcb で指定されるレディキュー中のタスクの優先度を newpri（内部表現）
 *  に変更する．また，必要な場合には最高優先順位のタスクを更新し，ディ
 *  スパッチ許可状態であれば TRUE を返す．そうでない場合は FALSE を返
 *  す．
 */
extern BOOL	change_priority(TCB *tcb, UINT newpri);

/*
 *  レディキューの回転
 *
 *  レディキュー中の，pri で指定される優先度のタスクキューを回転させる．
 *  また，必要な場合には最高優先順位のタスクを変更し，ディスパッチが保
 *  留されていなければ TRUE を返す．そうでない場合は FALSE を返す．
 */
extern BOOL	rotate_ready_queue(UINT pri);

/*
 *  タスク例外処理ルーチンの呼出し
 *
 *  タスク例外処理ルーチンを呼び出す．呼び出す前に，実行状態のタスクの
 *  保留例外要因をクリアし，タスク例外処理禁止状態にし，CPUロックを解
 *  除する．
 *  タスク例外処理ルーチンから戻ると，まずCPUロック状態に戻し，その間
 *  に保留例外要因が 0 でなくなっていれば，再びタスク例外処理ルーチン
 *  を呼び出す．保留例外要因が 0 の場合には，例外処理許可状態にして関
 *  数からリターンする．
 *  この関数は，実行状態のタスクが，タスク例外処理許可状態（enatex が 
 *  TRUE）で，保留例外要因が 0 でない（texptn が 0 でない）場合に呼び
 *  出すことを想定している．この関数は，CPUロック状態で呼び出さなけれ
 *  ばならない．
 */
extern void	call_texrtn(void);

/*
 *  タスク例外処理ルーチンの起動
 *
 *  実行状態のタスクがタスク例外処理ルーチンの起動条件を満たしていれば，
 *  タスク例外処理ルーチンを呼び出す．CPU例外処理ルーチンを呼び出す時
 *  は，一時的にCPUロックを解除する．
 *  この関数は，ディスパッチャや割込みハンドラ／CPU例外ハンドラの出口
 *  処理から呼び出されることを想定しているが，同等の処理をターゲット依
 *  存部で記述してもよい．その場合には，OMIT_CALLTEX をマクロ定義すれ
 *  ばよい．
 */
extern void	calltex(void);

#endif /* _TASK_H_ */
