/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 * 
 *  @(#) $Id: cpu_config.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（PowerPC用）
 *　　　　　カーネル内部で使用する定義
 *　　　　　　データ型、マクロ、関数のプロトタイプ宣言
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
 *  PowerPCプロセッサの内部レジスタの定義
 */
#include <powerpc.h>


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
#define	TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅 */
#define	TBIT_TCB_PRIORITY	8	/* priority フィールドのビット幅 */

/*
 *  スタックに必要なマージン
 *  
 *  PowerPC EABIで規定されているコーリング・コンベンションでは関数呼び
 *  出し時のフレームスタックポインタが指している8バイト先(sp+4)〜
 *  (sp+7)番地に呼ばれた側の情報が待避される。そのため、アセンブラルー
 *  チンからC言語ルーチンを呼び出す箇所（特に割込みコントローラ依存
 *  部）では注意が必要である。
 *  具体的には以下の箇所が該当する。
 *   　(1) スタートアップルーチン
 *   　(2) タスク起動
 *   　  (2-1) タスク例外処理ルーチン起動
 *   　(3) 割込みハンドラ起動
 *   　(4) CPU例外ルーチン起動
 *  
 *  　　　　　　　　　　　　　スタックが伸びる方向
 *  　　　　　　　　　　　　　　　　　↑
 *  　 関数呼び出し前のsp→  ---------------------- 
 *  　 　　　　　           |                      |
 *  　　　　　　         +4  ----------------------　
 *  　　　　　　            |                      |←C言語ルーチンの呼び出し
 *  　　　　　　         +8  ----------------------   により書き込まれる
 *  　
 *  　備考
 *  　　64ビット化の際には注意が必要である。
 *  　
 */
#define STACK_MARGIN	8


#ifndef _MACRO_ONLY
/*
 *  タスクコンテキストブロックの定義
 *　　spが4バイト境界であることを明示するため、
 *　　UW*型で宣言している。
 */

typedef struct task_context_block {
        UW	*sp;            /* スタックポインタ */
        FP	pc;             /* プログラムカウンタ */
} CTXB;

/*
 *  システム状態参照
 */

/*
 *  コンテキスト参照
 *  
 *    戻り値
 *    　TRUE ：非タスクコンテキスト
 *    　FALSE：タスクコンテキスト
 *
 *　SPRG0を割込みネストカウンタに用いている
 *  割込みネストカウンタを読み出した直後に割込みが発生しても、
 *  戻ったときにはコンテキストも元に戻っている
 *
 */
Inline BOOL
sense_context()
{
    UW intnest;
    
    intnest = current_sprg0();    	/*  intnest ← SPRG0  */
    return(intnest > 0);
}

/*
 *  CPUロック状態参照
 *  
 *    戻り値
 *    　TRUE ：CPUロック状態
 *    　FALSE：CPUロック解除状態
 *
 *  MSRを読み出した直後に割込みが発生しても、戻ったときには
 *  MSRも元に戻っている
 *
 */
Inline BOOL
sense_lock()
{
    return(!(current_msr() & MSR_EE));
}

#define t_sense_lock    sense_lock
#define i_sense_lock    sense_lock



/*
 *  CPUロックとその解除
 *
 *  　MSRのEEビットでCPUロックを実現している。
 *  　割込みマスクは割込みコントローラにある。
 *  　割込み全禁止と割込みマスクが独立しているため、
 *  　単純に実装できる。
 */

/*  タスクコンテキスト用  */
#define t_lock_cpu      disint      /*  cpu_insn.h  */
#define t_unlock_cpu    enaint      /*  cpu_insn.h  */

/*  非タスクコンテキスト用  */
#define i_lock_cpu      disint      /*  cpu_insn.h  */
#define i_unlock_cpu    enaint      /*  cpu_insn.h  */


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

/*
 *  割込みハンドラの擬似テーブル
 */
extern FP int_table[];

/*
 *  例外ベクタの擬似テーブル
 */
extern FP exc_table[];


Inline void
define_inh(INHNO intno, FP inthdr)
{
    int_table[intno] = inthdr;

#ifdef GDB_STUB     /*  保留  */

#endif
}

/*
 *   CPU例外ハンドラの設定
 */

Inline void
define_exc(EXCNO excno, FP exchdr)
{
    exc_table[excno] = exchdr;

#ifdef GDB_STUB     /*  保留  */

#endif
}


/*
 *  割込みハンドラの出入口処理の生成マクロ
 */

#define INTHDR_ENTRY(inthdr)  extern void inthdr(void)

#define INT_ENTRY(inthdr) inthdr

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 */
#define EXCHDR_ENTRY(exchdr)  extern void exchdr(VP sp)

#define EXC_ENTRY(exchdr)     exchdr

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のコンテキスト判別
 *  
 *    戻り値
 *    　TRUE ：CPU例外発生時に非タスクコンテキスト
 *    　FALSE：CPU例外発生時にタスクコンテキスト
 *
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
    UW  intnest;
    
    intnest = current_sprg0();    	/*  intnest ← SPRG0  */

    /*
     * １と比較するのは、現在実行中のCPU例外の分
     * 割込みネストカウンタがインクリメントされているため
     */
    return(intnest > 1);
}


/*
 * 例外ハンドラの引数p_excinfが指し示す番地から
 * 　　　　　　　　　　　　SSR1レジスタまでのオフセット
 * 
 * 　　MSRのコピーSSR1レジスタは、スタックに(OFFSET_SSR1+1)番目に
 * 　　積まれている。
 * 　　support.SのSAVE_GPR0_12_SPRGを参照 
 */
#define OFFSET_SSR1	12

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 *  
 *    戻り値
 *    　TRUE ：CPU例外発生時にCPUロック状態
 *    　FALSE：CPU例外発生時にCPUロック解除状態
 *  
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
    UW  ssr1;

    ssr1 = *((volatile UW *)p_excinf + OFFSET_SSR1);
    return( !(ssr1 & MSR_EE) );
}

/*
 *  プロセッサ依存の初期化
 */
extern void cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void cpu_terminate(void);

/*
 *  下記のログ出力時のスタック構造の定義
 */
typedef struct exc_stack {
	UW	r0;
	UW	r3;
	UW	r4;
	UW	r5;
	UW	r6;
	UW	r7;
	UW	r8;
	UW	r9;
	UW	r10;
	UW	r11;
	UW	r12;
	UW	srr0;	/*   待避／復帰レジスタ0　*/
	UW	srr1;	/*   待避／復帰レジスタ1　*/
	UW	lr;	/*   リンク・レジスタ 	*/
	UW	ctr;	/*   カウント・レジスタ	*/
	UW	cr;	/*   コンディション・レジスタ 	*/
	UW	xer;	/*   整数オペレーションの条件識別レジスタ　*/
			/*  　（キャリーやオーバフローなど）  */
	UW	exc_no;	/*   例外番号 	*/
} EXCSTACK;

/*  スタック上のデータの表示  */
void syslog_data_on_stack(EXCSTACK *sp);

/*
 * 登録されていない例外発生時のログ出力 (cpu_config.c)
 */
extern void no_reg_exception(EXCSTACK *);



#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */

/*  end of file  */
