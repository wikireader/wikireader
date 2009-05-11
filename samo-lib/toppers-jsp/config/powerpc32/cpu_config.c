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
 *  @(#) $Id: cpu_config.c,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（PowerPC用）
 *　　　　　カーネル内部で使用する定義
 *　　　　　　C言語関数の実体
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include <sil.h>


/*
 *  割込みハンドラ／割込みマスクの擬似テーブル
 */
FP int_table[TMAX_INTNO];


/*
 *  例外ベクタの擬似テーブル
 */
FP exc_table[TMAX_EXCNO];

/*
 *  エラーメッセージのデータテーブル
 *
 *  インプリメンテーション固有の例外0x1000はカーネルが用意する出
 *  入口処理を経由しないので、このテーブルの0x10番目が使用される
 *  ことはない。
 */
static const char *(exe_list[]) = {
	"",				/*  0x0  */
	"System Reset",			/*  0x1  */
	"Machine Check",		/*  0x2  */
	"DSI",				/*  0x3  */
	"ISI",				/*  0x4  */
	"External Interrupt",		/*  0x5  */
	"Alignment",			/*  0x6  */
	"Program",			/*  0x7  */
	"Floating Point Unavailable",	/*  0x8  */
	"Decrementer",			/*  0x9  */
	"Implement Exception 0xa00",	/*  0xa  */
	"",				/*  0xb  */
	"System Call",			/*  0xc  */
	"Trace",			/*  0xd  */
	"Floating Point Assist",	/*  0xe  */
};


/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize()
{
    	INHNO i;
    	EXCNO j;

    	/* 
    	 * 割込み擬似ベクタテーブル初期化
    	 *
    	 *　　0で初期化して未登録扱いにしているので、
    	 *　　割込み処理のICU依存部分PROC_ICUでは、
    	 *　　未登録の場合の処理を入れておくこと
         */
    	for( i = 0; i < TMAX_INTNO; i++ ) {
        	define_inh( i, (FP)0 );
    	}

    	/* CPU例外擬似ベクタテーブル初期化 */
    	for( j = 0; j < TMAX_EXCNO; j++ ) {
        	define_exc( j, (FP)&no_reg_exception );
    	}
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate()
{
}

#ifdef SUPPORT_CHG_IPM

/*
 *  割込みマスクの変更
 *
 *  PowerPCコアにはIPMがなく、割込みコントローラのIPMのみ扱って
 *  いる点に注意
 *
 *  chg_ipm を使って NMI以外のすべての割込みを禁止できるかどうか
 *  は割込みコントローラに依存する。
 *  NMI以外のすべての割込みを禁止したい場合には，loc_cpuによりCPU
 *  ロック状態にすればよい．
 *  IPMによって一部の割込みがマスクされている時にも，タスクディス
 *  パッチは保留されない．IPMは，タスクディスパッチによって，新し
 *  く実行状態になったタスクへ引き継がれる．そのため，タスクが実
 *  行中に，別のタスクによって IPMが変更される場合がある．JSPカー
 *  ネルでは，IPMの変更はタスク例外処理ルーチンによっても起こるが，
 *  これによって扱いが難しくなる状況は少ないと思われる．IPM の値
 *  によってタスクディスパッチを禁止したい場合には，dis_dspを併用
 *  すればよい．
 */
SYSCALL ER
chg_ipm(IPM ipm)
{
	ER	ercd;

	LOG_CHG_IPM_ENTER(ipm);	/*  ICU依存  */
	CHECK_TSKCTX_UNL();
	CHECK_IPM(ipm);		/*  ICU依存  */

	t_lock_cpu();
	CHG_IPM(ipm);		/*  ICU依存  */
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

	LOG_GET_IPM_ENTER(p_ipm);		/*  ICU依存  */
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	GET_IPM(p_ipm);				/*  ICU依存  */
	ercd = E_OK;
	t_unlock_cpu();

    exit:
	LOG_GET_IPM_LEAVE(ercd, *p_ipm);	/*  ICU依存  */
	return(ercd);
}

#endif /* SUPPORT_CHG_IPM */


/*  CPU例外／割込み発生時のスタック上のデータ表示  */
void syslog_data_on_stack(EXCSTACK *sp)
{
    syslog(LOG_EMERG, "PC = 0x%08x SR = 0x%08x",
    			sp->srr0, sp->srr1);

    syslog(LOG_EMERG, "r0 = 0x%08x ", sp->r0);
    syslog(LOG_EMERG, "r3 = 0x%08x r4 = 0x%08x r5 = 0x%08x",
    			sp->r3, sp->r4, sp->r5);
    syslog(LOG_EMERG, "r6 = 0x%08x r7 = 0x%08x r8 = 0x%08x",
    			sp->r6, sp->r7, sp->r8);
    syslog(LOG_EMERG, "r9 = 0x%08x r10 = 0x%08x r11 = 0x%08x",
    			sp->r9, sp->r10, sp->r11);
    syslog(LOG_EMERG, "r12 = 0x%08x ", sp->r12);


    syslog(LOG_EMERG, "LR = 0x%08x CTR = 0x%08x",
    			sp->lr, sp->ctr );
    syslog(LOG_EMERG, "CR = 0x%08x XER = 0x%08x",
    			sp->cr, sp->xer );
}


/*
 * 登録されていない例外が発生すると呼び出される
 */

void no_reg_exception(EXCSTACK *sp)
{
    syslog(LOG_EMERG, "Exception error occurs.");
    syslog(LOG_EMERG, " [0x%02x]: %s", 
    	   sp->exc_no, exe_list[sp->exc_no]);

    /*  スタック上のデータの表示  */
    syslog_data_on_stack(sp);

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
VP
_dummy_memcpy(VP dest, VP src, UINT len)
{
	VB *d = (VB *)dest;
	VB *s = (VB *)src;

	while (len-- > 0) {
		*d++ = *s++;
	}
	return(dest);
}

/*  end of file  */
