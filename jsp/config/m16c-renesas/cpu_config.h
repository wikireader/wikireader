/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
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
 *  @(#) $Id: cpu_config.h,v 1.4 2006/08/03 04:15:57 honda Exp $
 */


/*
 *  プロセッサ依存モジュール（M16C用）
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

#ifdef NEED_API
#include "api.h"
#endif /* NEED_API */

/*
 *  カーネルの内部識別名のリネーム
 */
#include <cpu_rename.h>

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#ifndef _MACRO_ONLY
#include <cpu_insn.h>
#endif /* _MACRO_ONLY */

/*
 *  TCB 中のフィールドのビット幅の定義
 *
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */
#define TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅 */
#define TBIT_TCB_PRIORITY	4	/* priority フィールドのビット幅 */

#ifndef _MACRO_ONLY

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	void	*sp;	/* スタックポインタ(near固定) */
	FP		pc;	/* プログラムカウンタ */
} CTXB;

/* 
 *  割込み/CPU例外ネストカウンタ
 */
extern char	intnest;

/*
 *  システム状態の参照
 */

/*
 *  現在のコンテキストを返す関数.
 *  非タスクコンテキストならTRUEを返す.
 */
Inline BOOL
sense_context(void)
{
	/*  ネストカウンタ0より大なら非タスクコンテキスト  */
	return(intnest > 0);
}

/*
 *  現在のCPUロック状態を返す関数.
 *  CPUロック状態ならTRUEを返す.
 */
Inline BOOL
sense_lock(void)
{
	return((current_flgreg() & FLG_I_MASK) == 0);
}

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除
 */
Inline void
lock_cpu(void)
{
	disint();
}

Inline void
unlock_cpu(void)
{
	enaint();
}

#define t_lock_cpu	lock_cpu
#define i_lock_cpu	lock_cpu
#define t_unlock_cpu	unlock_cpu
#define i_unlock_cpu	unlock_cpu

/*
 *  タスクディスパッチャ
 */

/*
 *  最高優先順位タスクへのディスパッチ（cpu_support.a30）
 *
 *  dispatch は，タスクコンテキストから呼び出された
 *  サービスコール処理の中から，CPUロック状態で呼び出さなければならない．
 */
extern void	dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ(cpu_support.a30)
 *  exit_and_dispatch は，CPUロック状態で呼び出さなければならない．
 */
extern void	exit_and_dispatch(void);

/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */

/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 *  割込みハンドラを登録するベクタテーブルはROM上に取り,
 *  そこに書き込む. そのためここでは何もしない．
 */

Inline void
define_inh(INHNO inhno, FP inthdr)
{
}

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 *  割込みハンドラを登録するベクタテーブルはROM上に取り,
 *  そこに書き込む. そのためここでは何もしない．
 */
Inline void
define_exc(EXCNO excno, FP exchdr)
{
}

/*
 *  割込みハンドラ／CPU例外ハンドラの出入口処理
 *
 *  割込み／CPU例外が発生時のCPUの状態は次のとおり.
 *
 *  FLGレジスタのIビットは 0(割込み禁止) 
 *  FLGレジスタのIPLは受け付けた割込みのIPL
 *  FLGレジスタのUビットは, 割込みの種類によって異なるが,
 *  カーネルではISPのみを使用し, USPを使用していないため,
 *  ここでは問題とならない.
 *
 *  割込み/例外発生時には, 自動的に FLG, PCがスタックへ保存される.
 *  出入り口処理ではまず残りのレジスタをスタックに保存する.
 *  初段の割込みの場合はタスクのスタックに,
 *  多重割り込みの場合は割込み用のスタックに, それぞれ保存される.
 *
 *  その後, スタックポインタの値を r1 に, 
 *  割込みハンドラのアドレスを a1 と a0 に格納した状態で
 *  割込み出入口処理の後半へジャンプする.
 */

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 *  割込み発生時、実行中のレジスタの値をスタックに退避する．その後、ス
 *  タックの値をR1レジスタにセットしアセンブラで記述されたハンドリング
 *  プログラムに分岐する．
 */

#define INTHDR_ENTRY(inthdr)\
extern void inthdr##_entry(void);\
asm("	.glb _" #inthdr "						");\
asm("	.glb __kernel_interrupt						");\
asm("	.section program, code, align					");\
asm("_" #inthdr "_entry:						");\
asm("	pushm	r0,r1,r2,r3,a0,a1,sb,fb	; レジスタをタスクスタックへ退避");\
asm("	stc	isp, r1			; スタックポインタを取り出し	");\
asm("	mov.w	#_" #inthdr "&0ffffh, a0; 割込みハンドラの下位2バイト	");\
asm("	mov.w	#_" #inthdr ">>16, a1	; 割込みハンドラの上位2バイト	");\
asm("	jmp	__kernel_interrupt	; 割込み処理ルーチン後半へ	");

#define INT_ENTRY(inthdr)	inthdr##_entry

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 *  CPU例外ハンドラの基本的な処理は割込みハンドラと共有する．割込みハ
 *  ンドラとの違いはスタック値を退避したR1レジスタの値をp_excinfとして
 *  使用するか、しないかの差異のみです．
 */

#define EXCHDR_ENTRY(exchdr)\
extern void exchdr##_entry(void);\
asm("	.glb $" #exchdr "		 				");\
asm("	.glb __kernel_interrupt		 				");\
asm("	.section program, code, align					");\
asm("_" #exchdr "_entry:						");\
asm("	pushm	r0,r1,r2,r3,a0,a1,sb,fb	; レジスタをタスクスタックへ退避");\
asm("	stc	isp, r1			; スタックポインタを取り出し	");\
asm("	mov.w	#$" #exchdr "&0ffffh, a0; 割込みハンドラの下位2バイト	");\
asm("	mov.w	#$" #exchdr ">>16, a1	; 割込みハンドラの上位2バイト	");\
asm("	jmp	__kernel_interrupt	; 割込み処理ルーチン後半へ	");

#define EXC_ENTRY(exchdr) exchdr##_entry

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のコンテキストの参照
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
	/* 
	 *  ネストカウンタが1より大なら非タスクコンテキスト
	 */
	return( intnest > 1);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	/*
	 *  例外発生前のFLGレジスタIビットが0ならCPUロック状態
	 *
	 *  下式の定数18はCPU例外入り口処理でレジスタがスタックにどう
	 *  保存されているかに依存して決定する数値
	 */
	return( ( *(char *)((char *)p_excinf+18) & FLG_I_MASK ) == 0);
}

/*
 *  プロセッサ依存の初期化(cpu_config.c)
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理(cpu_config.c)
 */
extern void	cpu_terminate(void);

/*
 * 割り込み制御レジスタ割り込み優先度変更
 */
extern void set_ic_ilvl(VP addr, UB val);

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
