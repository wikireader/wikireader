/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006 by Monami Software Limited Partnership, JAPAN	
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cpu_config.h,v 1.11 2007/05/30 03:56:47 honda Exp $
 */

//#include <sys_config.h>

/*
 *	プロセッサ依存モジュール（M32R用）
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

#include <cpu_rename.h>

#include "m32r.h"

/*
 *  chg_ipm/get_ipm をサポートするかどうかの定義
 */
/*#define	SUPPORT_CHG_IPM*/

/*
 *  TCB 関連の定義
 *
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */

/*
 *  TCB 中のフィールドのビット幅の定義
 */
#define	TBIT_TCB_PRIORITY	8	/* priority フィールドのビット幅 */
#define	TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅 */

#ifndef _MACRO_ONLY
/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	unsigned long sp;		/* スタックポインタ */
	FP	pc;					/* プログラムカウンタ */
} CTXB;

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除
 *
 *  CPUロックはCPUが持つ割込み要求許可/禁止に対応付ける。PSWのIEビット
 *  を操作する。
 */

#define t_lock_cpu		lock_cpu
#define i_lock_cpu		lock_cpu
#define t_unlock_cpu	unlock_cpu
#define i_unlock_cpu	unlock_cpu
#define ena_int			lock_cpu
#define dis_int			unlock_cpu

Inline void
lock_cpu()
{
	__asm("mvfc r0, psw        \n"
	      "and3 r0, r0, 0xffbf \n"
	      "mvtc r0, psw " : : : "r0");
}

Inline void
unlock_cpu()
{
	__asm("mvfc r0, psw       \n"
	      "or3  r0, r0, 0x40  \n"
	      "mvtc r0, psw " : : : "r0");
}

/*
 *  sense_context
 *  
 *  ユーザスタックを使っているならタスクコンテキスト
 */

Inline BOOL
sense_context(void)
{
	int psw;
	__asm("mvfc	%0, psw" : "=r"(psw));
	return (psw & 0x80) != 0 ? FALSE : TRUE;
}

/*
 *  sense_lock
 *  
 *  割込み禁止ならCPUロック状態
 */
Inline BOOL
sense_lock(void)
{
	int psw;
	__asm("mvfc	%0, psw" : "=r"(psw));
	return (psw & 0x40) != 0 ? FALSE : TRUE;
}

/*
 *  タスクディスパッチャ
 */

/*
 *  最高優先順位タスクへのディスパッチ（cpu_support.S）
 *
 *  dispatch は，タスクコンテキストから呼び出されたサービスコール処理
 *  内で，CPUロック状態で呼び出さなければならない．
 */
extern void	dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ（cpu_support.S）
 *
 *  exit_and_dispatch は，CPUロック状態で呼び出さなければならない．
 */
extern void	exit_and_dispatch(void);

/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */

extern FP InterruptHandlerEntry[NUM_INTERRUPT];
extern FP ExceptionHandlerEntry[NUM_EXCEPTION];

/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */
Inline void
define_inh(INHNO inhno, FP inthdr)
{
	InterruptHandlerEntry[inhno-1] = inthdr;
}

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 */
Inline void
define_exc(EXCNO excno, FP exchdr)
{
	ExceptionHandlerEntry[excno-1] = exchdr;
}

/*
 *  割込みハンドラ／CPU例外ハンドラの出入口処理
 */


/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 *  reqflg をチェックする前に割込みを禁止しないと，reqflg をチェック後
 *  に起動された割込みハンドラ内でディスパッチが要求された場合に，ディ
 *  スパッチされない．
 */
#define INT_ENTRY(hdr) hdr
#define INTHDR_ENTRY(entry)	extern void entry();

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 *  CPU例外ハンドラは，非タスクコンテキストで実行する．そのため，CPU例
 *  外ハンドラを呼び出す前に割込みモードに移行し，リターンしてきた後に
 *  元のモードに戻す．元のモードに戻すために，割込みモードに移行する前
 *  の SR を割込みスタック上に保存する．CPU例外がタスクコンテキストで
 *  発生し，reqflg が TRUE になった時に，ret_exc へ分岐する．
 *  reqflg をチェックする前に割込みを禁止しないと，reqflg をチェック後
 *  に起動された割込みハンドラ内でディスパッチが要求された場合に，ディ
 *  スパッチされない．
 */
#define EXC_ENTRY(hdr) hdr
#define EXCHDR_ENTRY(entry)	extern void entry();

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のコンテキスト
 *
 *  非タスクコンテキストの場合にTRUEを返す．p_excinfに例外ハンドラを呼
 *  び出す際のスタックポインタが格納されている（cpu_support.S）ので，
 *  そこから例外発生時のpswを取得しコンテキストを判断する．
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
	return (((unsigned int*)p_excinf)[1] & 0x8000) == 0;
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	return (((unsigned int*)p_excinf)[1] & 0x4000) == 0;
}

/*
 *  プロセッサ依存の初期化
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void	cpu_terminate(void);

/*
 * 現在処理中の割込み優先度
 */
extern ER get_ipr(IPR *);

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
