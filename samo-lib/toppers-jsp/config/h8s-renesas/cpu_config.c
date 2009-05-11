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
 *  プロセッサ依存モジュール(H8S用)
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/*
 *  タスクコンテキストでの割込みマスク
 */
#ifdef SUPPORT_CHG_IPM
volatile IPM	task_intmask = 0;		/* IPM -> UB：符号無し8ビット */
#endif /* SUPPORT_CHG_IPM */

/*
 *  非タスクコンテキストでの割込みマスク
 */
volatile IPM	int_intmask = 0;

/*
 *  割込みネストカウンタ
 */
volatile UB	intnest = 1;

/*
 *  CPUロック状態を表すフラグ
 *  　・割込み禁止（カーネル管理下の割込みのみ）
 *  　　　かつ
 *  　・iscpulocked == TRUE
 *  　　　のときCPUロック状態とする。
 */
volatile BOOL     iscpulocked = TRUE;

/*
 *  プロセッサ依存の初期化
 */
void cpu_initialize(void) {

        /*
         *  以下の4つはスタートアップルーチンで初期化が済んでいる。
         *  　　・割込みネストカウンタ intnest = 1;
         *  　　・非タスクコンテキストの割込みマスク int_intmask = 0;
         *  　　・タスクコンテキストの割込みマスク task_intmask = 0;
         *  　　　　（chg_ipmをサポートする場合）
         *  　　・CPUロックフラグ iscpulocked = TRUE;
         */

	/* 割込み制御モードの設定 */
	h8s_wrb_reg(SYSCR, SYS_SYSCR);

	/* 割込みレベルの初期化 */
	h8s_wrb_reg(IPRA, 0);
	h8s_wrb_reg(IPRB, 0);
	h8s_wrb_reg(IPRC, 0);
	h8s_wrb_reg(IPRD, 0);
	h8s_wrb_reg(IPRE, 0);
	h8s_wrb_reg(IPRF, 0);
	h8s_wrb_reg(IPRG, 0);
	h8s_wrb_reg(IPRH, 0);
	h8s_wrb_reg(IPRI, 0);
	h8s_wrb_reg(IPRJ, 0);
	h8s_wrb_reg(IPRK, 0);
}

/*
 *  プロセッサ依存の終了処理
 */
void cpu_terminate(void) {
}

/*
 * 微少時間待ち
 * 　　注意事項：
 * 　　　標準ではdlytimはUINT型だが16ビットしかないので、
 * 　　　UW型に変更している。
 * 　　　sil_dly_nse()は内部でsil_dly_nse_long()を呼び出す。
 */
void sil_dly_nse(UINT dlytim) {
	sil_dly_nse_long((UW)dlytim);
}

#ifdef SUPPORT_CHG_IPM

/*
 *  割込みマスクの変更
 *
 *　IPMに設定できる値は0〜MAX_IPMである。
 *　割込みプライオリティレベルが(MAX_IPM+1)以上の割込みはカーネル管理外
 *　扱いである。
 *
 *  IPM が 0 以外の時にも，タスクディスパッチは保留されない．
 *  ディスパッチも禁止したい場合には，loc_cpu によりCPUロック状態に
 *  すればよい．IPM は，
 *  タスクディスパッチによって，新しく実行状態になったタスクへ引き継が
 *  れる．そのため，タスクが実行中に，別のタスクによって IPM が変更さ
 *  れる場合がある．JSPカーネルでは，IPM の変更はタスク例外処理ルーチ
 *  ンによっても起こるのが，これによって扱いが難しくなる状況は少ないと
 *  思われる．
 *　IPM の値によってタスクディスパッチを禁止したい場合には，dis_dspを
 *  併用すればよい．
 */
SYSCALL ER
chg_ipm(IPM ipm)
{
        ER      ercd = E_OK;

        LOG_CHG_IPM_ENTER(ipm);
        CHECK_TSKCTX_UNL();
        CHECK_PAR(ipm <= MAX_IPM);

        t_lock_cpu();
        task_intmask = ipm;
        t_unlock_cpu();

    exit:
        LOG_CHG_IPM_LEAVE(ercd)
        return(ercd);
}

/*
 *  割込みマスクの参照
 */
SYSCALL ER
get_ipm(IPM *p_ipm)
{
        ER      ercd = E_OK;

        LOG_GET_IPM_ENTER(p_ipm);
        CHECK_TSKCTX_UNL();

        t_lock_cpu();
        *p_ipm = task_intmask;
        t_unlock_cpu();

    exit:
        LOG_GET_IPM_LEAVE(ercd, *p_ipm);
        return(ercd);
}


#endif /* SUPPORT_CHG_IPM */

/*============================================================================*/
/*  共通ドキュメントにはない、独自の部分  */

/*
 * 登録されていない割り込みが発生すると呼び出される
 */
void 
cpu_experr(EXCSTACK *sp)
{
    UW sp2, pc, ccr, tmp;
    
    sp2 = (UW)sp + OFFSET_SP;
    tmp = sp->pc;
    ccr = (tmp >> 24U) & 0xff;	/*  上位1バイト  */
    pc = tmp & 0x00ffffffUL;	/*  下位3バイト  */
    
    syslog(LOG_EMERG, "Unexpected interrupt.");
    syslog(LOG_EMERG, "PC  = 0x%08lx SP  = 0x%08lx CCR  = 0x%02x",
                       (VP)pc, (VP)sp2, (INT)ccr);
    syslog(LOG_EMERG, "EXR  = 0x%02x", (INT)(sp->exr));
    syslog(LOG_EMERG, "ER0 = 0x%08lx ER1 = 0x%08lx ER2 = 0x%08lx ER3 = 0x%08lx",
           (VP)(sp->er0), (VP)(sp->er1), (VP)(sp->er2), (VP)(sp->er3));
    syslog(LOG_EMERG, "ER4 = 0x%08lx ER5 = 0x%08lx ER6 = 0x%08lx",
                       (VP)(sp->er4), (VP)(sp->er5), (VP)(sp->er6));
    while(1)
    	;
}


/*============================================================================*/
/*  デバッグ用コード  */

#ifdef TEST_CPU_INSN

volatile UB ccr, exr;
volatile IPM intmask;

void test_cpu_insn(void)
{
	ccr = current_ccr();
	set_ccr(0xf);
	ccr = current_ccr();

	exr = current_exr();
	set_exr(0x7);
	exr = current_exr();

	intmask = current_intmask();
	set_intmask(0x3);
	intmask = current_intmask();
	
	disint();
	enaint();
	
	_disint_();
}

#endif /* TEST_CPU_INSN */


#ifdef TEST_CPU_CONFIG

volatile BOOL b;
volatile ER err;
volatile IPM ipm;

void dummy(void)
{
}

void test_cpu_config(void)
{
	b = sense_context();
	dummy();
	intnest = 1;
	dummy();
	b = sense_context();
	dummy();
	intnest = 0;
	dummy();
	b = sense_context();
	dummy();

	b = t_sense_lock();
	dummy();
	t_lock_cpu();
	dummy();
	b = t_sense_lock();
	dummy();
	t_unlock_cpu();
	dummy();
	b = t_sense_lock();
	dummy();

	i_lock_cpu();
	dummy();
	b = i_sense_lock();
	dummy();
	i_unlock_cpu();
	dummy();
	b = i_sense_lock();
	dummy();
	
	err = chg_ipm(6);
	dummy();
	err = get_ipm(&ipm);
	dummy();
	err = chg_ipm(3);
	dummy();
	err = get_ipm(&ipm);
	dummy();
	err = chg_ipm(8);
	dummy();
}

#endif /* TEST_CPU_CONFIG */

#ifdef TEST_H8S_SIL

volatile UB ddr;

void test_h8s_sil(void)
{
	ddr = sil_reb_ddr(IO_PORT7);
	sil_wrb_ddr(IO_PORT7, 0xff);
	ddr = sil_reb_ddr(IO_PORT7);
	sil_anb_ddr(IO_PORT7, 0xf);
	ddr = sil_reb_ddr(IO_PORT7);
	sil_orb_ddr(IO_PORT7, 0x80);
	ddr = sil_reb_ddr(IO_PORT7);
}

#endif /* TEST_H8S_SIL */



