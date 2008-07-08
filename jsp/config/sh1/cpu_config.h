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
 *  @(#) $Id: cpu_config.h,v 1.18 2005/11/14 08:00:44 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（SH1用）
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
 *  SH1プロセッサの内部レジスタの定義
 */
#include <sh1.h>

/*
 *  数値データ文字列化用マクロの定義
 */
#include <util.h>

/*
 *  ユーザー定義情報
 */
#include <user_config.h>

/*
 *  設定可能な最高優先度
 */
#ifdef GDB_STUB
#define MAX_IPM  0xe	/* スタブありの場合は優先度14でCPUロック   */
#else
#define MAX_IPM  0xf	/* スタブなしの場合は最高優先度でCPUロック */
#endif /*  GDB_STUB  */

#define str_MAX_IPM  		TO_STRING(MAX_IPM)

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

#ifndef _MACRO_ONLY
/*
 *  タスクコンテキストブロックの定義
 *  
 *  　spをUW *型としているのは4バイト境界を意識しているため
 *  
 */
typedef struct task_context_block {
        UW 	*sp;            /* スタックポインタ */
        FP	pc;             /* プログラムカウンタ */
} CTXB;

/*
 *  割込みマスク操作ライブラリ
 *
 *  割込みマスク（intmask）は，IPM（Interrupt Priority Mask）を4ビット
 *  左にシフトしたものである．
 */

/*
 *  現在の割込みマスクの読出し
 */
Inline UW
current_intmask(void)
{
	return(current_sr() & 0x000000f0u);
}

/*
 *  割込みマスクの設定
 */
Inline void
set_intmask(UW intmask)
{
	set_sr((current_sr() & ~0x000000f0u) | intmask);
}


/*
 *  割込み／CPU例外ネストカウンタ
 */
extern UW	intnest;

/*
 *  システム状態参照
 */


/*
 *  コンテキスト参照
 *
 *  割込みネストカウンタを読み出した直後に割込みが発生しても、
 *  戻ったときにはコンテキストも元に戻っている
 */
Inline BOOL
sense_context(void)
{
	return(intnest > 0);
}

Inline BOOL
sense_lock(void)
{
	return(current_intmask() == (MAX_IPM << 4));
}

#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock

/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *
 *  task_intmask は，chg_ipm をサポートするための変数．chg_ipm をサポー
 *  トしない場合には，task_intmask が常に 0 になっていると考えればよい．
 */

#ifdef SUPPORT_CHG_IPM
extern UW	task_intmask;	/* タスクコンテキストでの割込みマスク */
#endif /* SUPPORT_CHG_IPM */

Inline void
t_lock_cpu(viod)
{
	disint();		/*  cpu_insn.h  */
}

Inline void
t_unlock_cpu(void)
{
#ifdef SUPPORT_CHG_IPM
	/*
	 *  t_unlock_cpu が呼び出されるのは CPUロック状態のみであるた
	 *  め，処理の途中で task_intmask が書き換わることはない．
	 */
	set_intmask(task_intmask);
#else /* SUPPORT_CHG_IPM */
	enaint();		/*  cpu_insn.h  */
#endif /* SUPPORT_CHG_IPM */
}

/*
 *  CPUロックとその解除（非タスクコンテキスト用）
 */

extern UW	int_intmask;	/* 非タスクコンテキストでの割込みマスク */

Inline void
i_lock_cpu(void)
{
	UW	intmask;

	/*
	 *  一時変数 intmask を使っているのは，current_intmask() を呼
	 *  んだ直後に割込みが発生し，起動された割込みハンドラ内で
	 *  int_intmask が変更される可能性があるためである．
	 */
	intmask = current_intmask();
	disint();
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

#endif /* _MACRO_ONLY */

/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */

/*
 *  例外ベクタテーブルの定義
 *  　　マクロの説明
 *  　　　KERNEL_HAS_A_VECTOR_TABLE
 *  　　　　カーネルが例外ベクタテーブルを持つ
 *  　　　SIO_RESERVED
 *  　　　　シリアルデバイスがデバッガによって使用されている
 */
#ifndef _MACRO_ONLY

#ifdef KERNEL_HAS_A_VECTOR_TABLE
extern FP vector_table[];	/*  例外ベクタテーブル  */
#endif /* KERNEL_HAS_A_VECTOR_TABLE */


/*
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */
Inline void
define_inh(INHNO inhno, FP inthdr)
{
#ifdef GDB_STUB
	/*  スタブ呼び出し  */
	Asm("mov   #0x08,r0;	\
	     mov   %0,r4;	\
	     mov   %1,r5;	\
	     trapa #0x21"
               : /* no output */
               : "r"(inhno),"r"(inthdr)
               : "r0","r4","r5");

#else	/*  GDB_STUB  */

#ifdef KERNEL_HAS_A_VECTOR_TABLE

#ifdef SIO_RESERVED	/*  シリアル割り込みは避ける  */
	if ((inhno != RXI0) && (inhno != TXI0))
#endif /* SIO_RESERVED */
		vector_table[inhno] = inthdr;

#endif /* KERNEL_HAS_A_VECTOR_TABLE */

#endif	/*  GDB_STUB  */
}

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 */
extern void define_exc(EXCNO excno, FP exchdr)  throw();


/*
 *  割込みハンドラ／CPU例外ハンドラの入口処理
 */

/*  C言語ルーチンの関数名から入口処理のラベルを生成  
 *     kernel_cfg.cで使用
 */
#define	INT_ENTRY(inthdr)	_kernel_##inthdr##_entry
#define	EXC_ENTRY(exchdr)	_kernel_##exchdr##_entry

/*
 *  割込みハンドラ／CPU例外ハンドラの入口処理の共通部分
 *
 *	SH1では、割込みとCPU例外の扱いがほとんど同じなので、
 *	入口処理のマクロも共通に定義している
 *
 *　　引数
 *	label：入口処理のラベル
 *	inthdr：C言語ルーチンの先頭アドレス
 *	common_routine：cpu_support.S内の分岐先アドレス
 *			interrupt_entry：割込みの場合
 *			cpu_exception_entry：CPU例外の場合
 *
 *　　レジスタ割当
 *　　　　r1：割込み受付直後のSRのコピー
 *　　　　r0：interrupt_entry
 *　　　　r2：C言語ルーチンの先頭アドレス
 *
 *	割込みとCPU例外の相違点
 *	　　・CPU例外ハンドラに引数を与える
 *	　　・CPU例外では、例外発生時のIPMと同じ値で割込みを許可する
 *		（CPU例外により、IPMを変化させない）
 *
 *	　　・割込みハンドラの場合				
 *	　　  	割込み受付時にハードウェアがIPMを設定する	
 *	　　  	割込み許可にはこのIPMの値を用いる		
 *	　　・CPU例外ハンドラの場合				
 *	　　  	CPU例外受付時にIPMの値は変化しない		
 *	　　  	割込み許可にはCPU例外発生時のIPMの値を用いる	
 */
#define	HANDLER_ENTRY_PROC(label, handler, common_routine)		   \
asm(".text;								  "\
"	.align 2;							  "\
"	.global _"#label";						  "\
"_"#label":;								  "\
			/*  必要最小限のレジスタをスタックに待避  */	   \
"	mov.l	r0,@-r15;						  "\
"	mov.l	r1,@-r15;						  "\
				/*  割込み禁止とSR復元の準備  		*/ \
"	stc	sr,r1;							  "\
	/*  割込み禁止  						*/ \
	/*     割込みを禁止する前に別の割込みが入った場合の注意点は 	*/ \
	/*     cpu_suppourt.Sのret_intを参照 				*/ \
"	mov.l	_mask_ipm_"#handler",r0;				  "\
"	ldc	r0,sr;							  "\
"	mov.l	r2,@-r15;						  "\
"	mov.l	_common_routine_"#handler",r0;				  "\
"	mov.l	_c_routine_"#handler",r2;" /* C言語ルーチンの先頭アドレス*/\
"	jmp	@r0;		"/*  interrupt_entryへジャンプ 		*/ \
"	nop;				"	/*  遅延スロット  	*/ \
	/* 備考 							*/ \
	/* 　遅延スロットにPC相対アドレッシングのロード命令を 		*/ \
	/* 　入れると誤動作する。 					*/ \
	/*  　（スロット不当命令例外が発生しないので発見が難しい。） 	*/ \
									   \
"	.align 2;							  "\
"_mask_ipm_"#handler":;	    "	/*  割込み禁止用マスク  		*/ \
"	.long  "str_MAX_IPM" << 4; "	/*  ipm以外のビットはゼロで良い	*/ \
"_c_routine_"#handler":;	    					  "\
"	.long  _"#handler";   "	/* C言語ルーチンの先頭アドレス  	*/ \
"_common_routine_"#handler":; "	/* cpu_support.S内の分岐先アドレス	*/ \
"	.long  __kernel_"#common_routine"; "		  		   \
)

/*
 *  割込みハンドラの入口処理の生成マクロ
 *
 */
#define INTHDR_ENTRY(inthdr)	INTHDR_ENTRY2(INT_ENTRY(inthdr), inthdr)

#define INTHDR_ENTRY2(entry, inthdr)					\
	extern void entry(void);					\
	HANDLER_ENTRY_PROC(entry, inthdr, interrupt_entry)


/*
 *  CPU例外ハンドラの入口処理の生成マクロ
 */
#define	EXCHDR_ENTRY(exchdr)	EXCHDR_ENTRY2(EXC_ENTRY(exchdr), exchdr)

#define EXCHDR_ENTRY2(entry, exchdr)					\
	extern void entry(void);					\
	HANDLER_ENTRY_PROC(entry, exchdr, cpu_exception_entry)
	/*
	 *  void entry()は単なるエントリのラベルなので、
	 *  引数は付けない
	 */

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
	 *  １と比較するのは、現在実行中のCPU例外の分
	 *  割込みネストカウンタがインクリメントされているため 	
	 *  
	 *  CPU例外の入口処理中に別のCPU例外は発生しないと仮定
	 *  している
	 */
	return(intnest > 1);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	UW sr = *(UW *)p_excinf; 
	return((sr& 0x00000f0u) == (MAX_IPM << 4));
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
 *  下記のログ出力時のスタック構造の定義
 */
typedef struct exc_stack {
	VW	r0;
	VW	r1;
	VW	r2;
	VW	r3;
	VW	r4;
	VW	r5;
	VW	r6;
	VW	r7;
	VW	r8;
	VW	r9;
	VW	r10;
	VW	r11;
	VW	r12;
	VW	r13;
	VW	r14;
	VW	r15;
	VW	pr;	/*  プロシージャ・レジスタ  	*/
	VW	pc;	/*  プログラム・カウンタ  	*/
	VW	sr;	/*  ステータス・レジスタ  	*/
} EXCSTACK;


/*
 * 例外発生時のログ出力 (cpu_config.c, cpu_support.S)
 */
extern void     cpu_experr(EXCSTACK *);

#endif /* _MACRO_ONLY */


/*
 *  例外ベクタに設定するデフォルトの値
 *  	以下の例外要因でデフォルトとは異なる例外ベクタを定義
 *	する場合は、sys_config.hで該当するマクロを定義する。
 */
#define RESET_VECTOR 	start		/*  リセットベクタ  */
#define INIT_STACK  	STACKTOP	/*  スタックポインタの初期値  */

#define RESERVED_VECTOR	RESET_VECTOR	/*  システム予約のリセットベクタ  */
					/*  実際には使用されない  */

#ifndef GII_VECTOR	/*  一般不当命令  */
#define GII_VECTOR	RESET_VECTOR
#endif /* GII_VECTOR */

#ifndef SII_VECTOR	/*  スロット不当命令  */
#define SII_VECTOR	RESET_VECTOR
#endif /* SII_VECTOR */

#ifndef CAE_VECTOR	/*  CPUアドレスエラー  */
#define CAE_VECTOR	RESET_VECTOR
#endif /* CAE_VECTOR */

#ifndef DAE_VECTOR	/*  DMAアドレスエラー  */
#define DAE_VECTOR	RESET_VECTOR
#endif /* DAE_VECTOR */

#ifndef NMI_VECTOR	/*  NMI  */
#define NMI_VECTOR	RESET_VECTOR
#endif /* NMI_VECTOR */


#endif /* _CPU_CONFIG_H_ */
