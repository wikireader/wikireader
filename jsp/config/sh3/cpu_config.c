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
 *  @(#) $Id: cpu_config.c,v 1.24 2006/04/10 09:58:15 honda Exp $
 */

/*
 *  プロセッサ依存モジュール（SH3/4用）
 */
#include <s_services.h>
#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/*
 *  タスクコンテキストでの割込みマスク
 */
#ifdef SUPPORT_CHG_IPM
UW	task_intmask;
#endif /* SUPPORT_CHG_IPM */


/*
 *  非タスクコンテキストでの割込みマスク
 */
UW	int_intmask;


/*
 * 割り込みハンドラ/優先度の疑似テーブル
 */
FP  int_table[(MAX_INTIVT>>5) + 1];
VW  int_plevel_table[(MAX_INTIVT>>5) + 1];


/*
 * CPU例外ハンドラの疑似テーブル
 */
FP	exc_table[(0x1E0 >> 5) + 1];


/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize()
{
    int i;
    
    /*
     *  タスクコンテキストでの割込みマスクの初期化
     */
#ifdef SUPPORT_CHG_IPM
    task_intmask = 0x0000;    
#endif /* SUPPORT_CHG_IPM */
    
    /*
     * int_table[],int_plevel_table[]の初期化
     * 未登録の割込み発生時にcpu_interrup()が呼び出されるように
     * no_reg_interrup()を登録する
     * ディフォルトのint_plevel_table[]の割込み優先度はMAX_IPM - 1
     * とする．
     */
    for(i = 0; i < ((MAX_INTIVT>>5) + 1); i++){
        int_table[i] = no_reg_interrupt;
        int_plevel_table[i] = 0x40000000 | (MAX_IPM - 1) << 4;
    }
    
#ifndef GDB_STUB
    /*
     * 割り込みコントローラの初期化 
     */
#if defined(SH7750)
    sil_wrh_mem((VP)ICR, 0x0000);
    sil_wrh_mem((VP)IPRA, 0x0000);
    sil_wrh_mem((VP)IPRB, 0x0000);
    sil_wrh_mem((VP)IPRC, 0x0000);
#else /* SH7708 || SH7709 || SH7709A || SH7729R || SH7727 */   
    sil_wrh_mem((VP)ICR0, 0x0000);
    sil_wrh_mem((VP)IPRA, 0x0000);     
    sil_wrh_mem((VP)IPRB, 0x0000);
#if defined(SH7709) || defined(SH7709A) || defined(SH7729R) || defined(SH7727) 
    sil_wrh_mem((VP)ICR1, 0x4000);
    sil_wrh_mem((VP)ICR2, 0x0000);     
    sil_wrh_mem((VP)PINTER, 0x0000);
    sil_wrh_mem((VP)IPRC, 0x0000);
    sil_wrh_mem((VP)IPRD, 0x0000);
    sil_wrh_mem((VP)IPRE, 0x0000);
    sil_wrb_mem((VP)IRR0, 0x0000);   
    sil_wrb_mem((VP)IRR1, 0x0000);
    sil_wrb_mem((VP)IRR2, 0x0000);
#endif /* SH7709 || SH7709A  */   
#endif /* SH7750 */
    
    /*
     *  ベクタベースレジスターの初期化
     */
    set_vbr(BASE_VBR);
    
#endif /* GDB_STUB */
}


/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate()
{
}


/*
 * Trapa以外の例外で登録されていない例外が発生すると呼び出される
 */
void
cpu_expevt(VW expevt,VW spc,VW ssr,VW pr)
{
    syslog(LOG_EMERG, "Unregistered Expevt error occurs.");
    syslog(LOG_EMERG, "Expevt = %08x SPC = %08x SR = %08x PR=%08X",
           expevt,spc,ssr,pr);
    while(1);
}

/*
 * 未登録の割込みが発生した場合に呼び出される
 */
void
cpu_interrupt(VW intevt, VW intevt2,VW spc,VW ssr)
{
    syslog(LOG_EMERG, "Unregistered Interrupt occurs.");
#if defined(SH7709) || defined(SH7709A) || defined(SH7729R) || defined(SH7727) 
    syslog(LOG_EMERG, "INTEVT = %08x INTEVT2 = %08x SPC = %08x SR = %08x ",
           intevt,intevt2,spc,ssr);
#else /* SH7708 || SH7750 */   
    syslog(LOG_EMERG, "INTEVT = %08x  SPC = %08x SR = %08x ",intevt,spc,ssr);
#endif    
    while(1);
}



#ifdef SUPPORT_CHG_IPM

/*
 *  割込みマスクの変更
 *
 *  chg_ipm を使って IPM を MAX_IPM （NMI スタブリモートブレーク 以外
 *  のすべての割込みを禁止）以上に変更することはできない．NMI スタブリ
 *  モートブレーク以外のすべての割込みを禁止したい場合には、loc_cpu に
 *  よりCPUロック状態にすればよい．IPM が 0 以外の時にも，タスクディス
 *  パッチは保留されない．IPM は，タスクディスパッチによって，新しく実
 *  行状態になったタスクへ引き継がれる．そのため，タスクが実行中に，別
 *  のタスクによって IPM が変更される場合がある．JSPカーネルでは，IPM
 *  の変更はタスク例外処理ルーチンによっても起こるので，これによって扱
 *  いが難しくなる状況は少ないと思われる．IPM の値によってタスクディス
 *  パッチを禁止したい場合には，dis_dsp を併用すればよい．
 */

SYSCALL ER
chg_ipm(IPM ipm)
{
    ER  ercd;
    
    LOG_CHG_IPM_ENTER(ipm);
	CHECK_TSKCTX_UNL();
	CHECK_PAR(0 <= ipm && ipm <= MAX_IPM - 1);

	t_lock_cpu();
	task_intmask = (ipm << 4);
    ercd = E_OK;
	t_unlock_cpu();

  exit:
    LOG_CHG_IPM_LEAVE(ercd);
    return(ercd);
}


/*
 *  割込みマスクの参照
 */
SYSCALL ER
get_ipm(IPM *p_ipm)
{
    ER  ercd;

    LOG_GET_IPM_ENTER(p_ipm);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_ipm = (task_intmask >> 4);
    ercd = E_OK;
	t_unlock_cpu();

  exit:
    LOG_GET_IPM_LEAVE(ercd, *p_ipm);
    return(ercd);
}

#endif /* SUPPORT_CHG_IPM */
