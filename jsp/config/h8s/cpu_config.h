/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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

/*
 *  プロセッサ依存モジュール(H8S用)
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include <cpu_rename.h>

/*
 *  プロセッサ関連の定義
 */
#include <h8s.h>
#include <h8s_sil.h>

/*
 *  設定可能なカーネル管理下の最高許可割込みビットパターン
 *  (エクステンドレジスタへ設定;cpu_insn.h 内で利用するので、先に定義する。)
 */
#ifndef GDB_STUB
#define  MAX_IPM	7	/* = EXR_I2|EXR_I1|EXR_I0 (最高レベル) */
#else  /* GDB_STUB */
#define  MAX_IPM	6	/* レベル７の割り込みは、GDB_STUB が利用する */
#endif /* GDB_STUB */

#define  str_MAX_IPM	TO_STRING(MAX_IPM)
			/* 割込み入口処理INTHDR_ENTRYマクロ用 (cpu_config.h) */

/*
 *  プロセッサの特殊命令のインライン関数定義
 *    内部で、MAX_IPMを利用しているので、定義後に読み込む
 */
#include <cpu_insn.h>

/*
 *  chg_ipm/get_ipm をサポートするかどうかの定義
 */
#define	SUPPORT_CHG_IPM

/*
 *  タスクコンテキストブロックの定義
 */
#ifndef _MACRO_ONLY

typedef struct task_context_block {
	VP	sp;		/* スタックポインタ */
	FP	pc;		/* プログラムカウンタ */
} CTXB;

#endif /* _MACRO_ONLY */

/*
 *  システム状態参照
 */
#ifndef _MACRO_ONLY

/*
 *  割込みネストカウンタ
 *    ネストカウンタの値でコンテキストの判断を行うので、コンテキスト参照時に
 *    利用する。
 */

extern UW	intnest;

/*
 *  コンテキスト参照
 *    割込みネストカウンタを読み出した直後に割込みが発生しても、戻ったときには
 *    コンテキストも元に戻っている
 */

Inline BOOL
sense_context(void)
{
	return( intnest > 0 );
}

Inline BOOL
sense_lock(void)
{
	return( current_intmask() == MAX_IPM );
}

#endif /* _MACRO_ONLY */

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

#ifndef _MACRO_ONLY

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *
 *  task_intmask は、chg_ipm をサポートするための変数。chg_ipm をサポート
 *  しない場合には、t_unlock_cpu 中の task_intmask は 0 に置き換えてよい。
 */

#ifdef SUPPORT_CHG_IPM
extern IPM	task_intmask;	/* タスクコンテキストでの割込みマスク */
#endif /* SUPPORT_CHG_IPM */

Inline void
t_lock_cpu(void)
{
	disint();		/* cpu_insn.h */
}

Inline void
t_unlock_cpu(void)
{
#ifdef SUPPORT_CHG_IPM
	/*
	 *  t_unlock_cpu が呼び出されるのは CPUロック状態のみであるため。
	 *  処理の途中で task_intmask が書き換わることはない。
	 */
	set_intmask(task_intmask);
#else /* SUPPORT_CHG_IPM */
	enaint();		/* cpu_insn.h */
#endif /* SUPPORT_CHG_IPM */
}

/*
 *  CPUロックとその解除（非タスクコンテキスト用）
 */

extern IPM	int_intmask;	/* 非タスクコンテキストでの割込みマスク */

Inline void
i_lock_cpu(void)
{
	IPM	intmask;

	/*
	 *  一時変数 intmask を使っているのは、current_intmask()を呼んだ直後に
	 *  割込みが発生し、起動された割込みハンドラ内で int_intmask が変更
	 *  される可能性があるためである。
	 */
	intmask = current_intmask();
	disint();		/* cpu_insn.h */
	int_intmask = intmask;
}

Inline void
i_unlock_cpu(void)
{
	set_intmask(int_intmask);
}

/*
 *  タスクディスパッチャ
 */

/*
 *  最高優先順位タスクへのディスパッチ（cpu_support.S）
 *    dispatch は，タスクコンテキストから呼び出されたサービスコール処理内で、
 *    CPUロック状態で呼び出さなければならない。
 */
extern void	dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ（cpu_support.S）
 *    exit_and_dispatch は，CPUロック状態で呼び出さなければならない。
 */
extern void	exit_and_dispatch(void);

/*
 *  割込みハンドラの設定
 *    H8Sでは、割込みハンドラテーブルの設定はROMに直接行うため使用しない。
 */

Inline void define_inh(INHNO inhno, FP inthdr) {
}

/*
 *  CPU例外ハンドラの設定
 *    H8Sでは、割込みハンドラテーブルの設定はROMに直接行うため使用しない。
 */

Inline void define_exc(EXCNO excno, FP exchdr) {
}

#endif /* _MACRO_ONLY */

/*
 *  割込みハンドラの出入口処理
 */

/*
 *  割込みハンドラの入口処理
 *    この処理に来るまでに、ハードウェアにて、当該割込みレベルまでの割込みを
 *    禁止する処理が行われている。
 *    entry：入口処理のラベル
 *    inthdr：C言語ルーチンの先頭アドレス
 */
#define	_INTHDR_ENTRY(entry, inthdr)		    \
asm("	.text					\n" \
"	.align 2				\n" \
"	.global _"#entry"			\n" \
"						\n" \
"_"#entry":					\n" \
"	push.l	er0				\n" \
			/* CPUロックに必要なレジスタをタスクスタックに待避 */ \
"	stc.b	exr, r0l			\n" \
			/* exr を r0l (er0) にコピー \
"			  (_kernel_ret_main 内で、割込み禁止許可制御に使う) */ \
"						\n" \
"	ldc.b	#"str_MAX_IPM", exr		\n" \
			/* lock_cpu 相当 (割込み禁止) */ \
"						\n" \
"	push.l	er1				\n" \
			/* er1 をタスクスタックに待避 */ \
"	mov.l	#_"#inthdr", er1		\n" \
			/* er1 = 割込みハンドラのアドレス */ \
"	jmp	_kernel_ret_main		\n" \
			/* 割込み入口処理へジャンプ */ \
)

/*
 *  割込みハンドラの出入口処理の生成マクロ
 */
#define INTHDR_ENTRY(inthdr)			\
	extern void inthdr##_entry(void);	\
	_INTHDR_ENTRY(inthdr##_entry, inthdr)	/* この処理は上記マクロで定義 */
#define	INT_ENTRY(inthdr)	inthdr##_entry

/* ベクタテーブル(sys_support.S)記述用ラベル取得マクロ */
#define	__INT_ENTRY(inthdr)	_##inthdr##_entry
#define	_INT_ENTRY(inthdr)	__INT_ENTRY(inthdr)

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 */
#define	EXCHDR_ENTRY(exchdr)	INTHDR_ENTRY(exchdr)
#define	EXC_ENTRY(exchdr)	exchdr##_entry

/* ベクタテーブル(sys_support.S)記述用ラベル取得マクロ */
#define	__EXC_ENTRY(exchdr)	_##exchdr##_entry
#define	_EXC_ENTRY(exchdr)	__EXC_ENTRY(exchdr)

/*
 *  CPU例外の発生した時のシステム状態の参照
 */
#ifndef _MACRO_ONLY

/*
 *  CPU例外の発生した時のコンテキスト判定
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
	return(intnest > 1);
		/* 1と比較するのは、現在実行中のCPU例外の分割込みネスト
		   カウンタがインクリメントされているため */
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 *    H8S版では、呼ばれることは無いが、ソースを比較する場合向けに設定する。
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	return(TRUE);
}

/*
 *  プロセッサ依存の初期化
 */
extern void	cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void	cpu_terminate(void);

#endif /* _MACRO_ONLY_ */

/*============================================================================*/
/*  共通ドキュメントにはない、独自の部分  */

/*
 *  割込みレベル設定用マクロ（もしくは、インライン関数）
 *  ・x : インタラプトプライオリティレジスタ	( IPRA - IPRK )
 *  ・y : IPR設定用マスク（上位か下位の選択）	( IPR_UPR, IPR_LOW )
 *  ・z : 割込みレベル				( 0 - 7 )
 */
#ifndef _MACRO_ONLY

Inline void icu_set_ilv( VP ipr, BOOL mask, INT level ) {

	if( mask == IPR_UPR ) {
		h8s_wrb_mem( ipr, ( h8s_reb_mem( ipr ) & ~IPR_UPR_MASK ) | TO_HI4((UB) level) );
	} else if( mask == IPR_LOW ) {
		h8s_wrb_mem( ipr, ( h8s_reb_mem( ipr ) & ~IPR_LOW_MASK ) | TO_LO4((UB) level) );
	}
}

#endif /* _MACRO_ONLY_ */

#ifndef _MACRO_ONLY
/*
 *  未定義割込み発生時のエラー出力 (cpu_config.c, cpu_support.S)
 *    構造体 EXCSTACK は、h8s.h で定義
 */
extern void	cpu_experr(EXCSTACK *sp);

#endif /* _MACRO_ONLY_ */

#endif /* _CPU_CONFIG_H_ */
