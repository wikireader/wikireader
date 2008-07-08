/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 */

/*
 *  プロセッサ依存モジュール（Xstormy16用）
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
 *  chg_ipm/get_ipm はサポートしない
 */
#undef  SUPPORT_CHG_IPM

/*
 *  calltex は使用しない
 */
#define OMIT_CALLTEX

/*
 *  シリアルポート数の定義
 */
#define TNUM_PORT   2   /* サポートするシリアルポートの数 */

/*
 *  シリアルポート番号に関する定義
 */
#define LOGTASK_PORTID  2   /* システムログに用いるシリアルポート番号 */

/*
 *  ログタスクの必要スタックサイズ
 */
#define LOGTASK_STACK_SIZE 256

/*
 *  ログバッファのサイズ
 */
#define TCNT_SYSLOG_BUFFER  8

/*
 *  TCB 中のフィールドのビット幅の定義
 */
#define TBIT_TCB_TSTAT      8   /* tstat フィールドのビット幅 */
#define TBIT_TCB_PRIORITY   4   /* priority フィールドのビット幅 */

#ifndef _MACRO_ONLY

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
    unsigned short sp;          /* スタックポインタ */
    unsigned short mode;        /* dispatcherの復帰先 */
} CTXB;

/*
 *  タスク/非タスク参照用グローバル変数
 */
extern unsigned short _kernel_intnest ;

/*
 *  システム状態参照
 */
#define sense_context() ( _kernel_intnest > 0)
#define sense_lock() (!( __PSW & 0x0080 ))
#define t_sense_lock sense_lock
#define i_sense_lock sense_lock
#define t_lock_cpu() ({__asm__( "clr1 psw,#7" );})
#define t_unlock_cpu() ({__asm__( "set1 psw,#7" );})
#define i_lock_cpu t_lock_cpu
#define i_unlock_cpu t_unlock_cpu

/*
 *  CPU例外の発生した時のコンテキストの参照
 */
#define exc_sense_context(p_excinf) ( _kernel_intnest > 1 )

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
#define exc_sense_lock(p_excinf) (!(((unsigned short*)p_excinf)[-1] & 0x0080))

/*
 *  割込みハンドラ生成マクロ
 */
#define INTHDR_ENTRY(inthdr) extern void inthdr(void)
#define INT_ENTRY(inthdr) inthdr

/*
 *  CPU例外生成マクロ
 */
#define EXCHDR_ENTRY(exchdr) extern void exchdr(VP sp)
#define EXC_ENTRY(exchdr) exchdr

/*
 *  プロセッサ依存の初期化
 */
extern void cpu_initialize(void);

/*
 *  プロセッサ依存の終了時処理
 */
extern void cpu_terminate(void);

/*
 *  割込みハンドラの設定
 */
extern void define_inh(INHNO inhno, FP inthdr) ;

/*
 *  CPU例外ハンドラの設定
 */
extern void define_exc(EXCNO excno, FP exchdr) ;

/*
 *  ターゲットシステムの文字出力
 */
extern void sys_putc(char c);

/*
 *  タスクディスパッチャ
 *  最高優先順位タスクへのディスパッチ（cpu_support.S内）
 */
extern void dispatch(void);

/*
 *  タスクディスパッチャ
 *  現在のコンテキストを捨ててディスパッチ（cpu_support.S内）
 */
extern void exit_and_dispatch(void);

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
