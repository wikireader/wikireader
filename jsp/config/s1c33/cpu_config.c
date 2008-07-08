/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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
 *	プロセッサ依存モジュール(S1C33用)
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "itron.h"
#include "s1c33.h"
#include "sys_config.h"

/*
 *  CPUロック解除時に復元する
 */
UINT	tps_OrgIntLevel;

/*
 *  割込みネストカウンタ
 */
UINT	tps_IntNestCnt;

/*
 *  割込み制御レジスタ 位置特定処理
 */
ER	get_Offset(INTNO);
ER	get_BitFlag(INTNO);

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize()
{
	tps_IntNestCnt = 0;			/* 割込みネストカウントの初期化 */
	tps_OrgIntLevel = TPS_INIT_INTLEVEL;

	/*
	 *  割り込み許可フラグを設定し、OS管理下の割り込みをマスクする
	 */
	set_psr((TPS_CPULOCK_LEV << 8) | S1C33_PSR_FLAG_IE);

}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate()
{

}

/*
 *  タスク起動処理
 */
void
activate_r(void)
{
	unlock_cpu();
	Asm("xld.w %r0, ext_tsk");		/* 戻り位置をext_tskに設定 */
	Asm("pushn %r0");
	Asm("ld.w %%r6, %0": : "r"(runtsk->tinib->exinf));
	set_pc(runtsk->tinib->task);
}

/*
 *  タスクディスパッチャ
 *
 *  dispatchはタスクコンテキスト・CPUロック状態で呼び出す。
 *  exit_and_dispatchも同様だが、カーネル起動時に対応するため
 *  非タスクコンテキスト・CPUロック解除状態の呼び出しにも対応
 *  している。
 */
void
dispatch(void)
{
	TCB * task;

	Asm("pushn %r3");
	task = runtsk;
	task->tskctxb.sp = get_sp();
	task->tskctxb.pc = && dispatch_1;	/* gcc拡張機能を使用する */
	Asm("xjp _kernel_exit_and_dispatch");
dispatch_1:
	Asm("popn %r3");
	task = (TCB * volatile) runtsk;
	if ((task->enatex != FALSE) && (task->texptn != 0)) {
		Asm("xjp _kernel_calltex");	/* 例外ハンドラ起動	*/
	}
}

void
exit_and_dispatch(void)
{
	register UW	ulPsr;

	lock_cpu();			/* CPUをロックする	*/
	runtsk = schedtsk;
	if (runtsk != NULL) {
		set_sp(runtsk->tskctxb.sp);
		set_pc(runtsk->tskctxb.pc);
	}

	set_sp(STACKTOP);			/* 実行可能なタスクがない場合	*/
	ulPsr = get_psr();			/* 割り込みを許可してイベント	*/
	ulPsr |= S1C33_PSR_FLAG_IE;	/* 発生を待つ					*/
	set_psr(ulPsr);
	unlock_cpu();

	while (1) {
		Asm("halt");
	}
}

/*
 *  割込みハンドラ/CPU例外ハンドラ出口処理
 */
void
ret_int(void)
{
	static	TCB * task;

	reqflg = FALSE;
	task = runtsk;

	if (task == NULL) {			/* アイドル処理へジャンプする */
		Asm("xjp _kernel_exit_and_dispatch");
	}

	if ((enadsp != FALSE) && (task != schedtsk)) {
#ifndef	__c33adv
		Asm("pushn %r1");
#endif	/* __c33adv */
		task->tskctxb.sp = get_sp();
		task->tskctxb.pc = && ret_int_2;/* gcc拡張機能を使用 */
						/* ディスパッチ処理へジャンプ */
		Asm("xjp _kernel_exit_and_dispatch");
ret_int_2:
#ifndef	__c33adv
		Asm("popn %r1");
#endif	/* __c33adv */
	}
	task = (TCB * volatile) runtsk;
	if ((task->enatex != FALSE) && (task->texptn != 0)) {
		call_texrtn();			/* 例外ハンドラ起動	*/
	}
#ifdef	__c33adv
	Asm("pops %sor");
#else	/* __c33adv */
	Asm("ld.w %alr, %r1");
	Asm("ld.w %ahr, %r0");
#endif	/* __c33adv */

#if TPS_DAREA_CNT == 4				/* データエリアポインタ数に	*/
	Asm("popn %r11");			/* 応じたpop処理を行う		*/
#elif TPS_DAREA_CNT == 3
	Asm("popn %r12");
#elif TPS_DAREA_CNT == 2
	Asm("popn %r13");
#elif TPS_DAREA_CNT == 1
	Asm("popn %r14");
#else
	Asm("popn %r15");
#endif /* TPS_DAREA */

	Asm("reti");				/* CPUロック状態は復帰した	*/
						/* タスクのサービスコールから	*/
						/* リターンする時に解除される	*/
}


/*
 *  割込みレベル変更処理
 */
ER
chg_ilv(ILEVNO ilevno)
{
	UW	ulPsr;
	ER	ercd;

	LOG_CHG_ILV_ENTER(ilevno)
	CHECK_TSKCTX_UNL();
	if(ilevno > TPS_INTLEV_MAX){
		ercd = E_PAR;
	} else {
		ulPsr = get_psr();			/* 現在のPSR値を取得	*/
		ulPsr = (ulPsr & ~S1C33_PSR_MASK_IL) | (ilevno << 8);
		set_psr(ulPsr);				/* PSR値を更新		*/
		ercd = E_OK;
	}
exit:
	LOG_CHG_ILV_LEAVE(ercd)				/* Set New IL		*/
	return(ercd);
}

/*
 *  割込みレベル取得処理
 */
ER
get_ilv(ILEVNO *p_ilevno)
{
	UW	ulPsr;
	ER	ercd;

	LOG_GET_ILV_ENTER(p_ilevno)
	CHECK_TSKCTX_UNL();
	if(p_ilevno == NULL){
		ercd = E_PAR;
	} else {
		ulPsr = get_psr();			/* 現在の割込みレベルを取得 */
		*p_ilevno = (ulPsr & S1C33_PSR_MASK_IL) >> 8;
		ercd = E_OK;
	}
exit:
	LOG_GET_ILV_LEAVE(ercd, piLevno)
	return(ercd);
}

#ifndef __c33pe
/*
 *  割込み発生許可処理
 */
ER
ena_int(INTNO intno)
{
	INT	iOffset, iBitFlag;

	iOffset = get_Offset(intno);			/* レジスタ位置を取得する */
	iBitFlag = get_BitFlag(intno);

	if(iOffset == E_PAR || iBitFlag == E_PAR){
		return (E_PAR);
	}
							/* 割り込みを許可する */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntEnable[iOffset] |= (UB)iBitFlag;

	return (E_OK);
}

/*
 *  割込み発生禁止処理
 */
ER
dis_int(INTNO intno)
{
	INT	iOffset, iBitFlag;

	iOffset = get_Offset(intno);			/* レジスタ位置を取得する  */
	iBitFlag = get_BitFlag(intno);

	if(iOffset == E_PAR || iBitFlag == E_PAR){
		return (E_PAR);
	}
							/* 割り込みを禁止する */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntEnable[iOffset] &= ~((UB)iBitFlag);

	return (E_OK);
}

/*
 *  割込み発生要因クリア
 */
ER
clr_int(INTNO intno)
{
	INT	iOffset, iBitFlag;

	iOffset = get_Offset(intno);			/* レジスタ位置を取得する */
	iBitFlag = get_BitFlag(intno);

	if(iOffset == E_PAR || iBitFlag == E_PAR){
		return (E_PAR);
	}
#ifdef	__c33adv
	if(intno > S1C33_INHNO_SERIAL3TX){		/* 許可/禁止レジスタと要因    */
		iOffset -= 0x0d;			/* フラグレジスタのオフセット */
	}						/* 位置が一致しないため       */
#endif /* __c33adv */
							/* 割り込み要因をクリアする */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntFactor[iOffset] = ((UB)iBitFlag);

	return (E_OK);
}
#else  /* __c33pe */
#error "*_int() are valid for C33 Standard/Advanced macro."
#endif /* __c33pe */
