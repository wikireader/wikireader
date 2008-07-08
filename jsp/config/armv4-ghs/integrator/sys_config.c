/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                2003      by Advanced Data Controls, Corp
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
 *  @(#) $Id: sys_config.c,v 1.4 2003/12/19 11:48:37 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール（Integrator用）
 */

#include "jsp_kernel.h"
#include <integrator.h>

/*システムログのためのポート*/
#ifndef MULTI_IO
#include <hw_sysio.h>
#else
#include <stdio.h>
#endif





/*interrupt mask*/
UW irq_mask[MAX_INT_NUM]={
    MASK_IRQ_SOFTINT,
    MASK_IRQ_UART0,
    MASK_IRQ_UART1,
    MASK_IRQ_KBDINT,
    MASK_IRQ_MOUSEINT,
    MASK_IRQ_TM0,
    MASK_IRQ_TM1,
    MASK_IRQ_TM2,
    MASK_IRQ_RTCINT,
    MASK_IRQ_EXPINT0,
    MASK_IRQ_EXPINT1,
    MASK_IRQ_EXPINT2,
    MASK_IRQ_EXPINT3,
    MASK_IRQ_PCIINT0,
    MASK_IRQ_PCIINT1,
    MASK_IRQ_PCIINT2,
    MASK_IRQ_PCIINT3,
    MASK_IRQ_LINT,
    MASK_IRQ_DEGINT,
    MASK_IRQ_ENUMINT,
    MASK_IRQ_PCILBINT,
    MASK_IRQ_EXTINT
};

extern UW int_stack[];
/*
 * 割込みハンドラ登録用テーブル
 */
FP   int_table[MAX_INT_NUM];


/*
 *  ターゲットシステム依存の初期化
 */
void
sys_initialize()
{
    int i;

    /*
     *  割込みテーブルの初期化
     */
    for(i = 0; i < MAX_INT_NUM; i++){
        int_table[i] = 0x00;        /* リセット */
   
    }

    /*
     *  ARMのベクタテーブルへ割込みハンドラを登録
     */
    arm_install_handler(IRQ_Number,IRQ_Handler);
     
    /*
     *  割込み関連の初期化
     */
    sil_wrw_mem((VP)IRQ0_ENABLECLR,INT_DISABLE_ALL);
    /*initialize system port*/
#ifndef MULTI_IO
    hw_sysio_initialize();
#endif
}


/*
 *  ターゲットシステムの終了
 */
void
sys_exit(void)
{
    syslog(LOG_EMERG, "End Kernel.....!");
    while(1);
}


/*
 *  ターゲットシステムの文字出力
 */

void
sys_putc(char c)
{
#ifndef MULTI_IO
    hw_sysio_putchar(c);
#else
    putchar(c);
#endif
}


/*
 *  割込みハンドラの設定
 *
 *  割込み番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */
void
define_inh(INHNO inhno, FP inthdr)
{
    assert(inhno < MAX_INT_NUM);
    int_table[inhno] = inthdr;

}

/*
 *  未定義の割込みが入った場合の処理
 */
void
undef_interrupt(){
    syslog(LOG_EMERG, "Unregistered Interrupt occurs.");
    while(1);
}


/*IRQレジスタがクリアされるまで待つ*/
#define INT_CLEAR_DELAY  0x70
void poll_intreg(int int_mask)
{
    int i;
    
    //while(*IRQ0_ENABLESET & int_mask);
    for(i=0;i<INT_CLEAR_DELAY;i++);
    enaint();

}

/*発生した割り込みビットを検索*/

UINT find_bit(UINT bitmap)
{
    return (ffs(bitmap)-1);
}

