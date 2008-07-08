/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
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
 *  @(#) $Id: cpu_config.c,v 1.4 2006/08/03 04:15:57 honda Exp $
 */

/*
 *  プロセッサ依存モジュール（M16C用）
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/* 
 *  割込み/CPU例外ネストカウンタ
 */
char	intnest;

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize(void)
{
	/* 割込みネストカウント初期化(タスクコンテキスト) */
	intnest = 0;
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate(void)
{
}

/*
 * software_init_hookの仮定義
 */
#ifndef EXTERNAL_SOFT_HOOK
const VP_INT software_init_hook = 0;
#endif  /* EXTERNAL_SOFT_HOOK */


/*
 * 割り込み制御レジスタ割り込み優先度変更
 *
 * 割り込み制御レジスタは，そのレジスタに対応する割込み要求が発生しない箇所で
 * 変更する必要がある．そのため，割込みを禁止した後，変更する必要がある．
 * 割込みを禁止して割込み制御レジスタを変更する場合は，使用する命令に注意する
 * 必要がある．
 * IRビット以外を変更する場合には，AND,OR,BCLR,BSET命令を使用すること．
 * IRビットはMOV命令を使用すること．
 * また，Iフラグを用いて割込みを禁止する場合は，許可する前に数命令おくこと．
 * 詳細は，M16Cハードウェアマニュアルの
 *       "使用上の注意->割り込み->割込み制御レジスタの変更"
 * を参照のこと． 
 */
void
set_ic_ilvl(VP addr, UB val){
    BOOL    locked;
    
    locked = sense_lock();
    if (!(locked)) {
        sense_context() ? i_lock_cpu() : t_lock_cpu();
    }
    
    _asm("PUSH.W A0");
    _asm("PUSH.B R0L");
    _asm("MOV.W $$[FB], A0", addr);
    _asm("MOV.B $$[FB], R0L", val);
    _asm("AND.B #08H, [A0]");
    _asm("OR.B  R0L, [A0]");
    _asm("NOP");
    _asm("NOP");    
    _asm("POP.B R0L");
    _asm("POP.W A0");

    if (!(locked)) {
        sense_context() ? i_unlock_cpu() : t_unlock_cpu();
    }
    
}

