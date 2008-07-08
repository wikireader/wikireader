/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                     2003         by Advanced Data Controls, Corp
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
 *  @(#) $Id: cpu_config.h,v 1.5 2004/09/04 16:38:40 honda Exp $
 */


/*
 *  プロセッサ依存モジュール（SH3/4用）
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_


/*
 *  カーネル内部識別名のリネーム
 */
#include <cpu_rename.h>


#ifdef SH4
#include "sh4.h"        
#else  /* SH3 */
#include "sh3.h"        
#endif /* SH4 */


/*
 *  設定可能な最高優先度
 */
#ifdef GHS_STUB
#define MAX_IPM  0xe
#else
#define MAX_IPM  0xf 
#endif 


/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#ifndef _MACRO_ONLY
#include <cpu_insn.h>
#endif /* _MACRO_ONLY */

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

/*
 *  TCB 中のフィールドのビット幅の定義
 */
#define	TBIT_TCB_PRIORITY	8	/* priority フィールドのビット幅 */
#define	TBIT_TCB_TSTAT		8	/* tstat フィールドのビット幅 */
#define	TBIT_TCB_TWAIT		8	/* twait フィールドのビット幅 */

#ifndef _MACRO_ONLY
/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
        VP	sp;             /* スタックポインタ */
        FP  pc;             /* プログラムカウンタ */
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
current_intmask()
{
	return(current_sr() & 0x000000f0);
}


/*
 *  割込みマスクの設定
 */
Inline void
set_intmask(UW intmask)
{
	set_sr((current_sr() & ~0x000000f0) | intmask);
}


/*
 *  システム状態参照
 */

asm unsigned int _context()
{
%
    stc     r7_bank,r0
}

#define sense_context() (_context() > 0)


Inline BOOL
sense_lock()
{
	return(current_intmask() == MAX_IPM << 4);
}


#define t_sense_lock	sense_lock
#define i_sense_lock	sense_lock


/*
 *  CPUロックとその解除（タスクコンテキスト用）
 *
 *  task_intmask は，chg_ipm をサポートするための変数．chg_ipm をサポー
 *  トしない場合には，t_unlock_cpu 中の task_intmask は 0 に置き換えて
 *  よい．
 */

#ifdef SUPPORT_CHG_IPM
extern UW	task_intmask;	/* タスクコンテキストでの割込みマスク */
#endif /* SUPPORT_CHG_IPM */


Inline void
t_lock_cpu()
{
	disint();
}


Inline void
t_unlock_cpu()
{
#ifdef SUPPORT_CHG_IPM
	/*
	 *  t_unlock_cpu が呼び出されるのは CPUロック状態のみであるた
	 *  め，処理の途中で task_intmask が書き換わることはない．
	 */
	set_intmask(task_intmask);
#else /* SUPPORT_CHG_IPM */
	enaint();
#endif /* SUPPORT_CHG_IPM */
}


/*
 *  CPUロックとその解除（非タスクコンテキスト用）
 */

extern UW	int_intmask;	/* 非タスクコンテキストでの割込みマスク */

Inline void
i_lock_cpu()
{
	UW	intmask;

	/*
	 *  一時変数 intmask を使っているのは，current_intmask()を呼ん
	 *  だ直後に割込みが発生し，起動された割込みハンドラ内で
	 *  int_intmask が変更される可能性があるためである．
	 */
	intmask = current_intmask();
	disint();
	int_intmask = intmask;
}


Inline void
i_unlock_cpu()
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


/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */

/*
 *  ベクタベースの定義
 */
extern void   BASE_VBR(void);


/*
 *  例外ベクタテーブルの構造の定義
 */
typedef struct exc_vector_entry {
	FP	exchdr;			/* 例外ハンドラの起動番地 */
} EXCVE;



/*
 *  割り込みハンドラの疑似テーブル
 *  SH3以降はベクタテーブルを持たないため割り込み処理で例外要因を
 *  オフセットにこれら疑似テーブルよりハンドラの実行番地及び
 *  割り込みマスクの値を取得する
 */
extern FP	int_table[0x50];
extern VW   int_plevel_table[0x50];


/*
 *  CPU例外ハンドラの疑似テーブル
 */
extern FP	exc_table[(0x1E0 >> 5) + 1];


/*
 *
 *  割込みハンドラの設定
 *
 *  ベクトル番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 *  疑似テーブルに登録する
 *  stubを使う場合はstub呼び出しによりstubに登録する
 *  
 */

extern FP general_exception();
extern FP interrupt();


Inline void
define_inh(INHNO inhno, FP inthdr)
{
        int_table[inhno >> 5] = inthdr;

}


/*
 *   CPU例外ハンドラの設定
 *   擬似ベクターテーブルに登録
 */   
Inline void
define_exc(EXCNO excno, FP exchdr)
{
        exc_table[excno >> 5] = exchdr;

}


/*
 *  割り込みレベルの設定
 */
Inline void
define_int_plevel(UINT dintno, UW plevel)
{
    int_plevel_table[dintno >> 5] = (plevel << 4) | 0x40000000;
}


/*
 *  割込みハンドラ／CPU例外ハンドラの出入口処理
 *  
 */

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

/*
 *  CPU例外の発生した時のコンテキスト判定
 */

Inline BOOL
exc_sense_context(VP p_excinf)
{
	return(_context() > 1);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
	return((*((UW *)p_excinf + 11) & 0x00000f0) == MAX_IPM << 4);
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
 * 未登録の割込み/例外発生時のログ出力
 */
extern void     cpu_expevt(VW,VW,VW,VW);
extern void     cpu_interrupt(VW,VW,VW,VW);
extern void     no_reg_interrupt(void);


/*
 *  シリアルコントローラのボーレート設定時の待ち時間の設定
 *  バスステートコントローラのリフレッシュカウンタを使う
 */
#define WAIT_RFCR_FOR_SCI    200


#endif /* _MACRO_ONLY_ */
#endif /* _CPU_CONFIG_H_ */
