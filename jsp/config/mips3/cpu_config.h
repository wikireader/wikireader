/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include <cpu_rename.h>

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#include <cpu_insn.h>

/*
 *  chg_ipm/get_ipm をサポートするかどうかの定義
 */
#define	SUPPORT_CHG_IPM

/*
 *  TCB 関連の定義
 *
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */
#ifndef _MACRO_ONLY

/*  タスクコンテキストブロックの定義  */
typedef struct task_context_block {
        VP	sp;             /* スタックポインタ */
        FP	pc;             /* プログラムカウンタ */
} CTXB;

#endif /* _MACRO_ONLY */

/*  TCB 中のフィールドのビット幅の定義  */
#define	TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅 */
#define	TBIT_TCB_PRIORITY	8	/* priority フィールドのビット幅 */

/*
 *  システム状態参照
 */

#ifndef _MACRO_ONLY

/*
 *  コンテキスト参照
 *    割込みネストカウンタを読み出した直後に割込みが発生しても、戻ったときには
 *    コンテキストも元に戻っている。
 */
Inline BOOL sense_context() {

	UW intnest;

	Asm("move %0, "str_k0 :"=r"(intnest));

	return(intnest > 0);
}

Inline BOOL sense_lock() {

	return(!(current_sr() & SR_IE));
}

#endif /* _MACRO_ONLY */

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *    CPUロック状態の判断は、ステータスレジスタのIEビットを用いて判断している。
 */

#ifndef _MACRO_ONLY

Inline void t_lock_cpu() {

	disint();		/* cpu_insn.h */
}

Inline void t_unlock_cpu() {

	enaint();		/* cpu_insn.h */
}

#endif /* _MACRO_ONLY */

/*
 *  CPUロックとその解除（非タスクコンテキスト用）
 */

#define i_lock_cpu	t_lock_cpu
#define i_unlock_cpu	t_unlock_cpu

/*
 *  タスクディスパッチャ
 */

#ifndef _MACRO_ONLY

/*
 *  最高優先順位タスクへのディスパッチ（cpu_support.S）
 *    dispatch は、タスクコンテキストから呼び出されたサービスコール処理内で、
 *    CPUロック状態で呼び出さなければならない。
 */
extern void	dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ（cpu_support.S）
 *    exit_and_dispatch は、CPUロック状態で呼び出さなければならない。
 */
extern void	exit_and_dispatch(void);

/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */

/*  割込みハンドラ／割込みマスク擬似テーブル  */
extern INT_TABLE int_table[ TMAX_ALL_INTNO ];

/*  例外ハンドラ擬似テーブル  */
extern FP	 exc_table[ TMAX_CORE_EXCNO ];

/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号(割込み要因番号)intno の割込みハンドラの起動番地をinthdrに設定。
 */

Inline void define_inh(INTNO intno, FP inthdr) {

	int_table[intno].inthdr = inthdr;
}

/*
 *   CPU例外ハンドラの設定
 */

Inline void define_exc(EXCNO excno, FP exchdr) {

	exc_table[excno] = exchdr;
}

#endif /* _MACRO_ONLY */

/*
 *  割込みハンドラの出入口処理の生成マクロ
 */

#define	INTHDR_ENTRY(inthdr)  extern void inthdr(void)
#define INT_ENTRY(inthdr) inthdr

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 */
#define	EXCHDR_ENTRY(exchdr)  extern void exchdr(VP sp)
#define	EXC_ENTRY(exchdr)     exchdr

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

#ifndef _MACRO_ONLY

/*
 *  CPU例外の発生した時のコンテキスト判別
 */
Inline BOOL exc_sense_context(VP p_excinf) {

	UW  nest;

	Asm( "move %0, "str_k0 : "=r"(nest) );

	return( nest > 1 );
		/* 1と比較するのは、現在実行中のCPU例外の分だけ割込みネスト
		   カウンタがインクリメントされているため */
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL exc_sense_lock(VP p_excinf) {

	/* Statusレジスタは、スタックに22番目に積まれている。
	   exc_stack(mips3.h), cpu_support.S 参照 */
	return( !( *((UW *)p_excinf + 21) & SR_IE) );
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

/*============================================================================*/
/*  共通ドキュメントにはない、独自の部分  */

/*  プロセッサコア＋割込みコントローラに設定する割込みマスクのチェック  */
#define CHECK_IPM(ipm)	     CHECK_CORE_IPM( (ipm.core >> 8) & 0xff );	\
			     CHECK_ICU_IPM( ipm.icu )

#ifndef _MACRO_ONLY

/*
 *  割り込みレベルの設定（共通部からは呼び出されない）
 */

Inline void all_set_ilv(INTNO intno, IPM *ipm) {

	/*  MIPSコアの intmask テーブルの設定  */
	int_table[intno].intmask = SR_BEV | (ipm->core) | SR_IE;
			/* 割込みベクタを、kseg1に持ってくるため、BEV=1 */

	/*  割込みコントローラの割込みマスクテーブルの設定  */
	/*  （割込みレベルの設定）  */
	icu_set_ilv( intno, &(ipm->icu) );
}

#endif /* _MACRO_ONLY */

/*
 *  ログ出力用の関数
 */

#ifndef _MACRO_ONLY

/* 登録されていない割込み・例外発生時のログ出力 (cpu_config.c, cpu_support.S) */
extern void     cpu_experr( EXCSTACK *sp, UW SR, UW CR );

#endif /* _MACRO_ONLY */

#endif /* _CPU_CONFIG_H_ */
