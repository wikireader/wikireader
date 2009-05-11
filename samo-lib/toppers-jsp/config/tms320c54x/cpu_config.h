/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cpu_config.h,v 1.9 2003/12/04 08:25:44 honda Exp $
 */

#ifndef _CPU_CONFIG_H_
#define _CPU_CONFIG_H_

#include "api.h"

/*
 *  カーネル内部識別名のリネーム
 */
#include "cpu_rename.h"

/*
 *  プロセッサの特殊命令のインライン関数定義
 */
#ifndef _MACRO_ONLY
#include "cpu_insn.h"
#endif /* _MACRO_ONLY */

#include "c54x.h"


#define TBIT_TCB_TSTAT      6
#define TBIT_TCB_PRIORITY   4

#ifndef _MACRO_ONLY

typedef struct task_context_block{
  VP  sp;
  FP  pc;
}CTXB;

extern UH  nest;

Inline BOOL
sense_context(void)
{
  return (nest > 0);
}

Inline BOOL
sense_lock()
{
  volatile UH temp_st1;
  temp_st1 = ST1;
  return ((temp_st1 & 0x0800) > 0);
}

#define t_sense_lock sense_lock
#define i_sense_lock sense_lock

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

extern void dispatch(void);
extern void exit_and_dispatch(void);

extern FP  int_table[32];
extern const UH  int_mask_table[32];


Inline void
define_inh(INHNO inhno, FP inthdr)
{
  int_table[inhno] = inthdr;
}

#define INTHDR_ENTRY(inthdr) extern void inthdr(void)
#define INT_ENTRY(inthdr) inthdr

/*
 *  TMS320VC54x DSPにはCPU例外が存在しないので
 *  以下は適当に埋める
 */

/*
 *   CPU例外ハンドラの設定
 *   擬似ベクターテーブルに登録
 */   

Inline void
define_exc(EXCNO excno, FP exchdr)
{
}

/*
 *  CPU例外の発生した時のコンテキスト判定
 */
Inline BOOL
exc_sense_context(VP p_excinf)
{
   return(TRUE);
}

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
Inline BOOL
exc_sense_lock(VP p_excinf)
{
   return(TRUE);
}

extern void cpu_initialize(void);
extern void cpu_terminate(void);

#endif /* _MACRO_ONLY */
#endif /* _CPU_CONFIG_H_ */
