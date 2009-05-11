/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cpu_config.h,v 1.3 2005/07/06 01:48:21 honda Exp $
 */


/*
 *  プロセッサ依存モジュール（NiosII用）
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
#ifndef _MACRO_ONLY
#include <cpu_insn.h>
#endif /* _MACRO_ONLY */


/*
 *  TCB 関連の定義
 *
 *  cpu_context.h に入れる方がエレガントだが，参照の依存性の関係で，
 *  cpu_context.h には入れられない．
 */

/*
 *  TCB 中のフィールドのビット幅の定義
 */
#define    TBIT_TCB_PRIORITY    8    /* priority フィールドのビット幅 */
#define    TBIT_TCB_TSTAT       8    /* tstat フィールドのビット幅 */


#ifndef _MACRO_ONLY
/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
        VP    sp;             /* スタックポインタ */
        FP  pc;             /* プログラムカウンタ */
} CTXB;


/*
 * 割り込みのネスト回数のカウント
 */
extern UW interrupt_count;

/*
 *  システム状態参照
 */
Inline BOOL
sense_context()
{
    return(interrupt_count > 0);    
}

Inline BOOL
sense_lock()
{
    return((current_status() & STATUS_PIE) == 0);
}

#define t_sense_lock    sense_lock
#define i_sense_lock    sense_lock


/*
 *  CPUロックとその解除
 */
#define t_lock_cpu      lock_cpu
#define i_lock_cpu      lock_cpu
#define t_unlock_cpu    unlock_cpu
#define i_unlock_cpu    unlock_cpu

Inline void
lock_cpu()
{
    disint();
}

Inline void
unlock_cpu()
{
    enaint();
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
extern void    dispatch(void);

/*
 *  現在のコンテキストを捨ててディスパッチ（cpu_support.S）
 *
 *  exit_and_dispatch は，CPUロック状態で呼び出さなければならない．
 */
extern void    exit_and_dispatch(void);


/*
 *  割込みハンドラ／CPU例外ハンドラの設定
 */

/*
 *  割り込みハンドラ/ビットマスクのテーブル
 *  int_handler_table[] は割込みハンドラを登録するテーブル
 *  int_bit_table[]は割込みの優先順位に応じたビットのテーブルであり，
 *  
 */
extern FP   int_handler_table[MAX_INT_NUM];
extern UW   int_bit_table[MAX_INT_NUM];

/*
 *  CPU例外ハンドラの疑似テーブル
 */
extern FP    exc_handler_table[];

/*
 *
 *  割込みハンドラの設定
 *
 *  割込み優先度 inhno の割込みハンドラの起動番地を inthdr に設定する．
 *  疑似テーブルに登録する
 *  
 */

Inline void
define_inh(INHNO inhno, FP inthdr)
{
    int_handler_table[inhno] = inthdr;
}

/*
 *   CPU例外ハンドラの設定
 *   擬似ベクターテーブルに登録
 */   
Inline void
define_exc(EXCNO excno, FP exchdr)
{
    exc_handler_table[excno] = exchdr;
}


/*
 *  割込みハンドラ／CPU例外ハンドラの出入口処理
 *  
 */


/*
 *  割込みハンドラの出入口処理の生成マクロ
 */

#define INTHDR_ENTRY(inthdr)  extern void inthdr(void);

#define INT_ENTRY(inthdr) inthdr

/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 */
#define    EXCHDR_ENTRY(exchdr)  extern void exchdr(VP sp);

#define    EXC_ENTRY(exchdr)     exchdr

/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のコンテキスト判定
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
    return(interrupt_count > 1); 
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
    return((*(UW*)p_excinf & STATUS_PIE) == 0);
}


/*
 *  プロセッサ依存の初期化
 */
extern void    cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void    cpu_terminate(void);


/*
 * 未登録の割込み/例外発生時のログ出力
 */
extern void     no_reg_interrupt(void);
extern void     no_reg_exception(void);

#endif /* _MACRO_ONLY_ */

/*
 *  vxget_tim をサポートするかどうかの定義
 */
#define    SUPPORT_VXGET_TIM

/*                                                                             
 *  シリアルポート番号に関する定義      
 */
#define TNUM_PORT   1   /* サポートするシリアルポートの数 */
#define TNUM_SIOP   1   /* サポートするシリアルI/Oポートの数 */

/*
 *  システムタスクに関する定義
 */
#define    LOGTASK_PORTID    1    /* システムログを出力するシリアルポート番号 */


#endif /* _CPU_CONFIG_H_ */
