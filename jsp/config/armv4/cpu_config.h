/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cpu_config.h,v 1.19 2004/09/17 13:45:55 honda Exp $
 */

/*
 *  プロセッサ依存モジュール（ARM4vT用）
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

/*
 *  カーネル内部識別名のリネーム
 */
#include "cpu_rename.h"

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
#define    TBIT_TCB_TSTAT       8    /* tstat フィールドのビット幅 */
#define    TBIT_TCB_PRIORITY    8    /* priority フィールドのビット幅 */

#ifndef _MACRO_ONLY
/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
    VP    sp;        /* スタックポインタ */
    FP    pc;        /* プログラムカウンタ */
} CTXB;

/*
 * 割り込みのネスト回数のカウント
 */
extern UW interrupt_count;


/*
 *  システム状態参照
 */
Inline UB
current_mode()
{
    return(current_sr() & CPSR_MODE_MASK);
}

Inline BOOL
sense_context()
{
    return(interrupt_count > 0);
}

Inline BOOL
sense_lock()
{
    return(current_sr() & CPSR_IRQ_BIT);
}

#define t_sense_lock    sense_lock
#define i_sense_lock    sense_lock


/*
 *  CPUロックとその解除
 *
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
 * 例外ベクタに書き込まれたジャンプ命令が参照するアドレス
 */
extern UW * arm_vector_add[8];


/*
 * 例外に応じたハンドラの起動番地
 */
extern UW arm_handler_add[8];


/*
 *  CPU例外ハンドラの設定
 */
extern void define_exc(EXCNO excno, FP exchdr);


Inline void
arm_install_handler(EXCNO excno, FP exchdr)
{
    *arm_vector_add[excno] = (UW)exchdr;
}


/*
 *  CPU例外ハンドラの出入口処理
 */


/*
 *  CPU例外ハンドラの出入口処理の生成マクロ
 *
 */
#define __EXCHDR_ENTRY(exchdr, stacktop)    \
extern void exchdr##_entry(VP sp);          \
asm(".text                             \n"  \
#exchdr "_entry:                       \n"  \
"       ldr   sp,.int_stack_"#exchdr"  \n"  /* スタックの切り替え        */\
"       sub   lr,lr,#4                 \n"  /* undefでもこれでいいか?    */\
"       stmfd sp!, {r0 - r2,lr}        \n"  /* 一時的にint_stackに待避   */ \
"       mrs   r1, spsr                 \n"  /* SVCモードに切り替えるため */ \
"       mov   r0, sp                   \n"  /* 保存する                  */ \
"       mov   r2,#0xd3                 \n"  /* CPSRの書き換え(SVCモードへ)*/ \
"       msr   cpsr,r2                  \n" \
"       ldr   r2,[r0,#0x0C]            \n"     /* load  PC            */\
"       stmfd sp!,{r2}                 \n"     /* Store PC            */\
"       stmfd sp!,{r3,ip,lr}           \n"     /* Store r3,ip,lr      */\
"       ldmfd r0!,{r2,ip,lr}           \n"     /* load  r0,r1,r2      */\
"       stmfd sp!,{r1,r2,ip,lr}        \n"     /* SPSR,Store r0,r1,r2 */\
"       ldr   r2, .interrupt_count_"#exchdr"\n" /* 多重割り込みか判定  */\
"       ldr   r3, [r2]                 \n" \
"       add   r0,r3,#1                 \n" \
"       str   r0, [r2]                 \n" \
"       mov   r0,sp                    \n" /* 例外ハンドラへの引数 */\
"       cmp   r3, #0x00                \n" \
"       ldreq   sp,stack_"#exchdr"     \n" /* スタックの変更       */\
"       stmeqfd sp!,{r0}               \n" /* タスクスタックの保存 */\
"       and   r2, r1, #0xc0            \n" /* 例外発生時のCPUロック状態(IRQ) */\
"       orr   r2, r2, #0x13            \n" /* とFIQを継承. SVCモード */\
"       msr   cpsr,r2                  \n" \
"       bl    "#exchdr"                \n" /* ハンドラ呼び出し     */\
"       mrs   r2, cpsr                 \n" /* FIQを継承            */\
"       and   r2, r2, #0x40            \n" /*                      */\
"       orr   r2, r2, #0x93            \n" /* 割り込み禁止         */\
"       msr   cpsr,r2                  \n" \
"       ldr   r2,.interrupt_count_"#exchdr" \n"/* 割り込み回数を   */\
"       ldr   r1, [r2]                 \n"     /* デクリメント     */\
"       sub   r3,r1,#1                 \n"\
"       str   r3, [r2]                 \n"\
"       cmp   r3,#0x00                 \n" /* 割り込みネスト数?    */\
"       bne   return_to_task_"#exchdr" \n" \
"       ldmfd sp!,{r0}                 \n" /* タスクスタックの復帰 */\
"       mov   sp, r0                   \n"\
"       ldr   r1, reqflg_"#exchdr"     \n" /* Check reqflg         */\
"       ldr   r0,[r1]                  \n"\
"       cmp   r0,#0                    \n"\
"       beq   return_to_task_"#exchdr" \n"\
"       mov   r0,#0                    \n"\
"       str   r0,[r1]                  \n" /* Clear reqflg   */\
"       b     _kernel_ret_exc          \n" /* ret_intへ      */\
"return_to_task_"#exchdr":             \n" \
"       ldmfd sp!,{r1}                 \n" /* CPSRの復帰処理 r1 <- cpsr*/\
"       mrs   r2, cpsr                 \n" /* FIQを継承            */\
"       and   r2, r2, #0x40            \n" /*                      */\
"       and   r1, r1, #~0x40           \n" /*                      */\
"       orr   r1, r1, r2               \n" /*                      */\
"       msr   spsr, r1                 \n" /* 割り込み許可   */\
"       ldmfd sp!,{r0-r3,ip,lr,pc}^    \n"\
"       .align 4                       \n"\
".int_stack_"#exchdr":                 \n"\
"       .long _kernel_int_stack + 6 * 4 \n"\
"reqflg_"#exchdr":                     \n"\
"       .long     _kernel_reqflg       \n"\
"stack_"#exchdr":                      \n"\
"       .long   " #stacktop "          \n"\
".interrupt_count_"#exchdr":            \n"\
"       .long   _kernel_interrupt_count \n")


#define _EXCHDR_ENTRY(exchdr, stacktop)    __EXCHDR_ENTRY(exchdr, stacktop)

#define EXCHDR_ENTRY(exchdr)    _EXCHDR_ENTRY(exchdr, STACKTOP)

#define EXC_ENTRY(exchdr) exchdr##_entry


/*
 *  CPU例外の発生した時のシステム状態の参照
 */

/*
 *  CPU例外の発生した時のディスパッチ
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
    return((*((UW *)p_excinf) & CPSR_IRQ_BIT) == CPSR_IRQ_BIT );
}


/*
 * 未定義の例外が入った場合
 */
extern void undef_exception();
extern void swi_exception();
extern void prefetch_exception();
extern void data_abort_exception();
extern void irq_abort_exception();
extern void fiq_abort_exception();


/*
 *  プロセッサ依存の初期化
 */
extern void    cpu_initialize(void);


/*
 *  プロセッサ依存の終了時処理
 */
extern void    cpu_terminate(void);


/*
 * CPU/割込みハンドラの出入り口処理で一時的に使用するスタック
 */
#define INT_STACK_SIZE 6
extern UW int_stack[INT_STACK_SIZE];


#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
