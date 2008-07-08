/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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
 * 
 *  @(#) $Id: cpu_config.c,v 1.16 2005/11/14 08:00:44 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（SH1用）
 *　　　　　カーネル内部で使用する定義
 *　　　　　　C言語関数の実体
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include <sil.h>

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
 *  割込み／CPU例外ネストカウンタ
 */
UW	intnest;

/*
 *  オリジナルのベクタベースレジスタの保存領域
 */
#ifdef KERNEL_HAS_A_VECTOR_TABLE
static VP org_vbr;
#endif /* KERNEL_HAS_A_VECTOR_TABLE */

#ifdef SUPPORT_CPU_EXC_ENTRY_CHECK

/*
 *  CPU例外の要因数
 */
#ifndef NUM_EXC
#define NUM_EXC	(6 + 2)
#endif	/*  NUM_EXC  */

/*
 *  登録されたCPU例外の要因数
 */
static UW num_exc;

/*
 *  CPU例外の入口処理の先頭アドレス
 */
static FP exc_entries[NUM_EXC];

#endif /* SUPPORT_CPU_EXC_ENTRY_CHECK */

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize(void)
{
	/*
	 *  タスクコンテキストでの割込みマスクの初期化
	 */
#ifdef SUPPORT_CHG_IPM
	task_intmask = 0x0000u;
#endif /* SUPPORT_CHG_IPM */

	/*
	 *  割込み／CPU例外ネストカウンタの初期化
	 */
	intnest = 1u;

#ifndef GDB_STUB

	/*
	 * 割り込みコントローラの初期化
	 */
	sil_wrh_mem(IPRA, 0x0000);
	sil_wrh_mem(IPRB, 0x0000);
	sil_wrh_mem(IPRC, 0x0000);
	sil_wrh_mem(IPRD, 0x0000);
	sil_wrh_mem(IPRE, 0x0000);

#endif	/*  GDB_STUB  */

#ifdef KERNEL_HAS_A_VECTOR_TABLE
	/*
	 *  ベクタテーブルは初期値付き変数（配列）にしたので、
	 *  スタートアップルーチンでdataセクションをコピーする際に
	 *  初期化される。
	 *  （ここで初期化する必要はない。）
	 */
	
	/*
	 *  ベクタベースレジスタの初期化
	 */
	org_vbr = current_vbr();
	set_vbr((VP)vector_table);
#endif /* KERNEL_HAS_A_VECTOR_TABLE */

#ifdef SUPPORT_CPU_EXC_ENTRY_CHECK
	num_exc = 0;
#endif /* SUPPORT_CPU_EXC_ENTRY_CHECK */

}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate(void)
{
#ifdef KERNEL_HAS_A_VECTOR_TABLE
	set_vbr(org_vbr);
#endif /* KERNEL_HAS_A_VECTOR_TABLE */
}

/*
 *  CPU例外ハンドラの設定
 *
 *  ベクトル番号 excno のCPU例外ハンドラの起動番地を exchdr に設定する．
 */
void
define_exc(EXCNO excno, FP exchdr)
{
	/*  SH1は割込みもCPU例外も同じ形式  */
	define_inh((INHNO)excno, exchdr);

#ifdef SUPPORT_CPU_EXC_ENTRY_CHECK
#if 0
	CHECK_PAR(num_exc < NUM_EXC);
#endif
	exc_entries[num_exc++] = exchdr;
#endif /* SUPPORT_CPU_EXC_ENTRY_CHECK */
}

#ifdef SUPPORT_CPU_EXC_ENTRY_CHECK
/*
 *  CPU例外の入口処理で割込み禁止するまでの命令数
 */
#define NUM_INST_DIS_INT	5


/*
 *  割込みからの戻り先のチェック
 *
 *  　引数
 *  　　VP pc：スタック上に積まれた戻り番地
 *  　戻り値
 *  　　TRUE：戻り先がCPU例外の入口処理（割込み禁止する前）
 *  　　FALSE：戻り先が上記以外
 *  　備考
 *  　　割込みの出口処理で割込み禁止で呼ばれる。
 */

/* cpu_support.Sのret_intだけから呼ばれるので 	*/
/* ヘッダーファイルには含めない。 		*/
BOOL check_cpu_exc_entry(VP pc)   throw();

BOOL check_cpu_exc_entry(VP pc)
{
	UW i;
	UH *entry;	/* 2バイト長命令へのポインタ */
	
	for(i = 0; i < num_exc; i++) {
		entry = (UH *)exc_entries[i];
		if (((UW)entry <= (UW)pc) &&
		     ((UW)pc <= (UW)(entry + NUM_INST_DIS_INT)) ) {
			return TRUE;
		}
	}
	return FALSE;
}


#endif /* SUPPORT_CPU_EXC_ENTRY_CHECK */



#ifdef SUPPORT_CHG_IPM

/*
 *  割込みマスクの変更
 *
 *  chg_ipm を使って IPM を0xf（NMI 以外のすべての割込みを禁止）に変更
 *  することはできない．NMI 以外のすべての割込みを禁止したい場合には，
 *  loc_cpu によりCPUロック状態にすればよい．
 *  IPM が 0 以外の時にも，タスクディスパッチは保留されない．IPM は，
 *  タスクディスパッチによって，新しく実行状態になったタスクへ引き継が
 *  れる．そのため，タスクが実行中に，別のタスクによって IPM が変更さ
 *  れる場合がある．JSPカーネルでは，IPM の変更はタスク例外処理ルーチ
 *  ンによっても起こるが，これによって扱いが難しくなる状況は少ないと
 *  思われる．IPM の値によってタスクディスパッチを禁止したい場合には，
 *  dis_dsp を併用すればよい．
 */
SYSCALL ER
chg_ipm(IPM ipm)
{
	ER	ercd;

	LOG_CHG_IPM_ENTER(ipm);
	CHECK_TSKCTX_UNL();
	CHECK_PAR(0 <= ipm && ipm <= (MAX_IPM - 1) );

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
	ER	ercd;

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

/*
 * 登録されていない例外が発生すると呼び出される
 */
void cpu_experr(EXCSTACK *sp)
{
    syslog(LOG_EMERG, "Exception error occurs.");

    syslog(LOG_EMERG, "PC = 0x%08x SR = 0x%08x PR = 0x%08x",
    			sp->pc, sp->sr, sp->pr);

    syslog(LOG_EMERG, "r0 = 0x%08x r1 = 0x%08x r2 = 0x%08x",
    			sp->r0, sp->r1, sp->r2);
    syslog(LOG_EMERG, "r3 = 0x%08x r4 = 0x%08x r5 = 0x%08x",
    			sp->r3, sp->r4, sp->r5);
    syslog(LOG_EMERG, "r6 = 0x%08x r7 = 0x%08x r8 = 0x%08x",
    			sp->r6, sp->r7, sp->r8);
    syslog(LOG_EMERG, "r9 = 0x%08x r10 = 0x%08x r11 = 0x%08x",
    			sp->r9, sp->r10, sp->r11);
    syslog(LOG_EMERG, "r12 = 0x%08x r13 = 0x%08x r14 = 0x%08x",
    			sp->r12, sp->r13, sp->r14);
    			/*  例外発生直前のスタックポインタの値  */
    syslog(LOG_EMERG, "r15 = 0x%08x", (sp->r15)+19*4);

    while(1);
}


/*
 *  メモリブロック操作ライブラリ
 *	(ItIsからの流用)
 *
 *  関数の仕様は，ANSI C ライブラリの仕様と同じ．標準ライブラリのものを
 *  使った方が効率が良い可能性がある．
 *
 */
void *
_dummy_memcpy(void *dest, const void *src, size_t len)
{
	VB	*d = (VB *)dest;
	VB	*s = (VB *)src;

	while (len-- > 0) {
		*d++ = *s++;
	}
	return(dest);
}

