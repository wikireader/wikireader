/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: cpu_config.h,v 1.18 2007/03/23 07:22:15 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（H8用）
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

#include <h8.h>

/*
 *  カーネルの内部識別名のリネーム
 */

#include <cpu_rename.h>

/*
 *  プロセッサの特殊命令のインライン関数定義
 */

#ifndef _MACRO_ONLY
#include <cpu_insn.h>

/*
 *  タスクコンテキストブロックの定義
 */

typedef struct task_context_block {
	VP	sp;		/* スタックポインタ */
	FP	pc;		/* プログラムカウンタ */
} CTXB;

/*
 *  割込みマスク操作ライブラリ
 */

/*
 *  現在の割込みマスクの読出し
 */

Inline UB
current_intmask(void)
{
	return(current_ccr() & H8INT_MASK_ALL);
}

/*
 *  割込みマスクの設定
 */

Inline void
set_intmask(UB intmask)
{
	set_ccr((current_ccr() & ~H8INT_MASK_ALL) | intmask);
}

/*
 *  割込みネストカウンタ
 */

extern volatile UB intnest;

/*
 *  コンテキスト参照
 *
 *  割込みネストカウンタを読み出した直後に割込みが発生しても、戻ったと
 *  きにはコンテキストも元に戻っている
 */

Inline BOOL
sense_context(void)
{
	BOOL ret;
	
	ret = (intnest != 0) ? TRUE : FALSE;
	return(ret);
}

/*
 *  CPUロック状態の参照
 */

/*
 *  CPUロック状態を表すフラグ
 */
extern volatile BOOL    iscpulocked;

#define sense_lock()    iscpulocked
#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *
 *  task_intmask は、chg_ipm をサポートするための変数。chg_ipm をサポート
 *  しない場合には、t_unlock_cpu 中の task_intmask は 0 に置き換えてよい。
 */

#ifdef SUPPORT_CHG_IPM
extern volatile UB       task_intmask;   /* タスクコンテキストでの割込みマスク */
#endif /* SUPPORT_CHG_IPM */

Inline void
t_lock_cpu(void)
{
        disint();
        iscpulocked = TRUE;
}

Inline void
t_unlock_cpu(void)
{
        iscpulocked = FALSE;
#ifdef SUPPORT_CHG_IPM
        /*
         *  t_unlock_cpu が呼び出されるのは CPUロック状態のみであるため、
         *  処理の途中で task_intmask が書き換わることはない。
         */
        set_intmask(task_intmask);
#else /* SUPPORT_CHG_IPM */
        enaint();               /* cpu_insn.h */
#endif /* SUPPORT_CHG_IPM */
}

/*
 *  CPUロックとその解除（非タスクコンテキスト用）
 */

extern volatile UB int_intmask;    /* 非タスクコンテキストでの割込みマスク */

Inline void
i_lock_cpu(void)
{
        UB      intmask = current_intmask();
        /*
         *  一時変数 intmask を使っているのは，current_intmask()を呼ん
         *  だ直後に割込みが発生し，起動された割込みハンドラ内で
         *  int_intmask が変更される可能性があるためである．
         */

        disint();
        int_intmask = intmask;
        iscpulocked = TRUE;
}

Inline void
i_unlock_cpu(void)
{
        iscpulocked = FALSE;
        set_intmask(int_intmask);
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
 *
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 *
 */

#define JMP_OPECODE		0x5a000000u

Inline void
define_inh(INHNO inhno, FP inthdr)
{
#ifdef REDBOOT
   UW *addr = (UW *)VECTOR_TABLE_ADDR;
   addr[inhno] = JMP_OPECODE | (UW)inthdr;
#endif	/*  #ifdef REDBOOT  */
}

/*
 *   CPU例外ハンドラの設定
 */

Inline void
define_exc(EXCNO excno, FP exchdr)
{
}

#endif	/*  _MACRO_ONLY  */

/*
 *  割込みハンドラの出入口処理
 *
 */

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *		（主に入口処理）
 */

/*  
 * C言語ルーチンの関数名から入口処理のラベルを生成 するマクロ
 */

/*  C言語用のラベル生成  */
#define	INT_ENTRY(inthdr)	_kernel_##inthdr##_entry
#define	EXC_ENTRY(exchdr)	INT_ENTRY(exchdr)

/*  アセンブラ用のラベル生成  */
#define	INT_ENTRY_ASM(inthdr)	__kernel_##inthdr##_entry



/*
 *　　割込みの入口処理を生成するマクロの定義
 *　　　（割込み要因毎に異なる部分）
 *
 *　　　パラメータ
 *	　　entry：入口処理のラベル
 *　　　　　inthdr：C言語ルーチンの関数名（先頭の'_'は付けない）
 *　　　　　intmask：　割込み許可時に割込みマスクに設定する値
 *　　　　　　　　　　　IPM_LEVEL1、IPM_LEVEL2のいずれかにすること
 *
 *　　　H8では割込み受付直後はハードウェア的に割込み禁止になっている
 *
 *　　　　レジスタ割り当て
 *　　　　　・er0〜er3：退避
 *　　　　　・er2：C言語ルーチンの先頭アドレス
 *　　　　　・r3l：割込み許可時に割込みマスクに設定する値
 *
 */
#define	INTHDR_ENTRY2(entry, inthdr, intmask)	 \
	_INTHDR_ENTRY2(entry, inthdr, intmask)

#define	_INTHDR_ENTRY2(entry, inthdr, intmask)	 \
asm(".text					\n"\
"	.align 2				\n"\
"	.global "#entry"			\n"\
#entry":					\n"\
	/*  必要最小限のレジスタをスタックに待避  */\
"	push.l	er0				\n"\
"	push.l	er1				\n"\
"	push.l	er2				\n"\
"	push.l	er3				\n"\
	/*  C言語ルーチンの先頭アドレス  */	 \
"	mov.l   #_"#inthdr", er2		\n"\
	/*  割込み許可時に設定する割込みマスクの値  */\
"	mov.b   #"#intmask", r3l		\n"\
"	jmp	@__kernel_common_interrupt_process"\
)

/*  _INTHDR_ENTRY()マクロ　ここまで  */

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 */

#define INTHDR_ENTRY(inthdr)				\
	extern void INT_ENTRY(inthdr)(void) throw();	\
	INTHDR_ENTRY2(INT_ENTRY_ASM(inthdr), inthdr, inthdr##_intmask)

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 */

#define	EXCHDR_ENTRY(exchdr)	INTHDR_ENTRY(exchdr)


#ifndef _MACRO_ONLY

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のコンテキスト判定
 */

Inline BOOL
exc_sense_context(VP p_excinf)
{
        /* H8版では呼ばれない。 */
        return(TRUE);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */

Inline BOOL
exc_sense_lock(VP p_excinf)
{
        /* H8版では呼ばれない。 */
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

/*
 *  未定義割込み発生時のエラー出力時のスタック構造の定義
 */

typedef struct exc_stack {
	UW	er0;
	UW	er1;
	UW	er2;
	UW	er3;
	UW	er4;
	UW	er5;
	UW	er6;
	UW	pc;	/*  プログラム・カウンタ  	*/
} EXCSTACK;

/*
 * 未定義割込み発生時のエラー出力 (cpu_config.c, cpu_support.S)
 */

extern void     cpu_experr(EXCSTACK *sp) throw();

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．
 */

extern void cpu_putc(char c);

#endif /* _MACRO_ONLY_ */

#endif /* _CPU_CONFIG_H_ */
