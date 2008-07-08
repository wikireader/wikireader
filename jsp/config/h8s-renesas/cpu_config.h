/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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
 *      プロセッサ依存モジュール（H8S用）
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
 *  プロセッサ関連の定義
 */
#include <h8s.h>
#include <h8s_sil.h>


/*
 *  プロセッサの特殊命令のインライン関数定義
 *    内部で、MAX_IPMを利用しているので、定義後に読み込む
 */
#include <cpu_insn.h>

/*
 *  TCB 中のフィールドのビット幅の定義
 *
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */
#define TBIT_TCB_TSTAT          8       /* tstat フィールドのビット幅 */
#define TBIT_TCB_PRIORITY       8       /* priority フィールドのビット幅 */

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

extern volatile UB	intnest;

/*
 *  コンテキスト参照
 *    割込みネストカウンタを読み出した直後に割込みが発生しても、戻ったときには
 *    コンテキストも元に戻っている
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
 *  　・割込み禁止（カーネル管理下の割込みのみ）
 *  　　　かつ
 *  　・iscpulocked == TRUE
 *  　　　のときCPUロック状態とする。
 */
extern volatile BOOL     iscpulocked;

#define sense_lock()    iscpulocked

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
extern volatile IPM	task_intmask;	/* タスクコンテキストでの割込みマスク */
#endif /* SUPPORT_CHG_IPM */

Inline void
t_lock_cpu(void)
{
	disint();		/* cpu_insn.h */
        iscpulocked = TRUE;
}

Inline void
t_unlock_cpu(void)
{
        iscpulocked = FALSE;
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

extern volatile IPM	int_intmask;	/* 非タスクコンテキストでの割込みマスク */

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
 *  最高優先順位タスクへのディスパッチ（cpu_support.src）
 *    dispatch は，タスクコンテキストから呼び出されたサービスコール処理内で、
 *    CPUロック状態で呼び出さなければならない。
 */
extern void	dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ（cpu_support.src）
 *    exit_and_dispatch は，CPUロック状態で呼び出さなければならない。
 */
extern void	exit_and_dispatch(void);

/*
 *  割込みハンドラの設定
 *    H8Sでは、割込みベクタテーブルをROM上に直接に配置するため、
 *　　何も処理しない。
 *　（ベクタテーブルをperlスクリプトで生成する）
 */

Inline void define_inh(INHNO inhno, FP inthdr) {
}

/*
 *  CPU例外ハンドラの設定
 *    H8Sでは、CPU例外の機構がないため、何も処理しない。
 */

Inline void define_exc(EXCNO excno, FP exchdr) {
}

#endif /* _MACRO_ONLY */

/*
 *  割込みハンドラの出入口処理
 *　　H8C用H8S依存部では割込みの出入口処理をアセンブラソース内に
 *　　生成するため、本来はC言語中で定義する必要がない。
 *　　ここでは共通部とのインターフェースを合わせるため、
 *　　必要なマクロ定義のみしている。
 */

/*
 *  C言語ルーチンの関数名から入口処理のラベルを生成
 *　　H8C用H8依存部では入口処理がvector.src内のローカル・シンボルに
 *　　できるので、グローバルに公開する必要がない。
 *　　ここでは共通部とのインターフェースを合わせるため、
 *　　ダミーのラベルを与えている。
 */
#define INT_ENTRY(inthdr) _kernel_common_interrupt_process
#define EXC_ENTRY(exchdr) _kernel_common_interrupt_process

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *　　　C言語上ではうまく生成できないため、アセンブラソース内で
 *　　　別途用意する。
 */
#define INTHDR_ENTRY(inthdr) extern void INT_ENTRY(inthdr)(void)

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 */
#define EXCHDR_ENTRY(exchdr) extern void EXC_ENTRY(exchdr)(void)

/*
 *  CPU例外の発生した時のシステム状態の参照
 */
#ifndef _MACRO_ONLY

/*
 *  CPU例外の発生した時のコンテキスト判定
 *    H8S版では、呼ばれることは無いが、インターフェースをそろえるため、
 *　　実装している。
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
        return(TRUE);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 *    H8S版では、呼ばれることは無いが、インターフェースをそろえるため、
 *　　実装している。
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
 *  ・ipr : インタラプトプライオリティレジスタ( IPRA - IPRK ) 下位16ビット
 *  ・mask : IPR設定用マスク（上位か下位の選択）( IPR_UPR, IPR_LOW )
 *  ・level : 割込みレベル( 0 - 7 )
 */
#ifndef _MACRO_ONLY

Inline void icu_set_ilv( UH ipr, BOOL mask, INT level ) {
	UB val;
	
	if( mask == IPR_UPR ) {
		val = (UB)(( h8s_reb_reg( ipr ) & ~IPR_UPR_MASK ) | TO_HI4((UB) level));
		h8s_wrb_reg( ipr, val );
	} else if( mask == IPR_LOW ) {
		val = (UB)(( h8s_reb_reg( ipr ) & ~IPR_LOW_MASK ) | TO_LO4((UB) level));
		h8s_wrb_reg( ipr, val );
	}
}

/*
 *  未定義割込み発生時のエラー出力 (cpu_config.c, cpu_support.src)
 */

/*
 * スタックの先頭から各レジスタのまでのオフセット
 * 
 * 　前提条件
 * 　　・アドバンスドモード
 * 　　・割込み制御モード２
 * 
 * 　スタック構造
 *　　 +0:er0
 *　　 +4:er1
 *　　 +8:er2
 *　　+12:er3
 *　　+16:er4
 *　　+20:er5
 *　　+24:er6
 *　　+28:exr
 *　　+29:リザーブ
 *　　+30:crr
 *　　+31:pc
 *　　+34〜:割込み前に使用されていたスタック領域
 */

#ifndef __2000A__
#error support Advanced mode only.(Normal mode isn't supported)
#endif

/*
 * 割込み発生直前のスタックポインタまでのオフセット
 * 　（未定義割込み発生時）
 */
#define OFFSET_SP	34

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
	UB	exr;
	UB	dummy;
	UW	pc;	/*  プログラム・カウンタ  	*/
} EXCSTACK;


extern void	cpu_experr(EXCSTACK *sp);

#endif /* _MACRO_ONLY_ */

#endif /* _CPU_CONFIG_H_ */
