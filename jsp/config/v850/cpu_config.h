/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005 by Freelines CO.,Ltd
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
 *  @(#) $Id: cpu_config.h,v 1.14 2007/01/05 02:10:17 honda Exp $
 */

#include "sys_config.h"

/*
 *	プロセッサ依存モジュール（V850用）
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */

#include <cpu_rename.h>

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
	unsigned long psw;		/* プログラムステータスワード */
	FP	pc;					/* プログラムカウンタ */
} CTXB;

/*
 *  割込みマスク操作ライブラリ
 *
 *  割込みマスク（intmask）は，IPM（Interrupt Priority Mask）を8ビット
 *  左にシフトしたものである．
 */

BOOL sense_lock(void);

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  現在の割込みマスクの読出し
 */

Inline UB
current_intmask(void)
{
	return (UB)sense_lock();
}

/*
 *  割込みマスクの設定
 */

Inline void
set_intmask(UB intmask)
{
	if (intmask) {
		asm("di");
	} else {
		asm("ei");
	}
}

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 */

#define t_lock_cpu		lock_cpu
#define i_lock_cpu		lock_cpu
#define t_unlock_cpu	unlock_cpu
#define i_unlock_cpu	unlock_cpu

Inline void
lock_cpu(void)
{	asm("di");	}

Inline void
unlock_cpu(void)
{	asm("ei");	}


/*
 *  割込みネストカウンタ
 */

extern UW	intnest;

Inline BOOL
sense_context(void)
{
	return (intnest > 0);
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

typedef struct int_vector_entry {
	FP	inthdr;
} INTVE;

extern INTVE InterruptHandlerEntry[NUM_INTERRUPT];

/*
 *  例外ベクタテーブルの構造の定義
 */
typedef struct exc_vector_entry {
	FP	exchdr;			/* 例外ハンドラの起動番地 */
} EXCVE;

/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */
Inline void
define_inh(INHNO inhno, FP inthdr)
{
	InterruptHandlerEntry[inhno-1].inthdr = inthdr;
}

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 */
Inline void
define_exc(EXCNO excno, FP exchdr)
{
	define_inh(excno, exchdr);
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
 *  CPU例外の発生した時のディスパッチ
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
	/* １と比較するのは、現在実行中のCPU例外の分 		*/
	/*  割込みネストカウンタがインクリメントされているため 	*/
	return(intnest > 0);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	return((*((UH *) p_excinf) & 0x0700) == 0x0700);
}

/*
 *  プロセッサ依存の初期化
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void	cpu_terminate(void);

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
