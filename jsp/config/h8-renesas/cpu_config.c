/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Katsuhiro Amano
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
 *  @(#) $Id: cpu_config.c,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

/*
 *  プロセッサ依存モジュール（H8用）
 *　　　カーネル内部で使用する定義
 *　　　　C言語関数の実体
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include "sil.h"
#include <hw_serial.h>  /*  SCI_putchar_pol()  */

#ifdef SUPPORT_CHG_IPM
/*
 *  タスクコンテキストでの割込みマスク
 */
volatile UB      task_intmask = 0;
#endif /* SUPPORT_CHG_IPM */

/*
 *  非タスクコンテキストでの割込みマスク
 */
volatile UB      int_intmask = 0;

/*
 *  割込みネストカウンタ
 */
volatile UB      intnest = 0;

/*
 *  CPUロック状態を表すフラグ
 */
volatile BOOL    iscpulocked = TRUE;

/*
 *  タイマのプライオリティレベル設定用のデータ
 *      本来はhw_timer.hに記述するべきだが、そうすると
 *      hw_timer.hをインクルードしたファイルですべて実体化されて
 *　　　メモリ領域を占有してしまうため、実体はここに記述する。
 */
const IRC TIMER_IRC = {(UB*)SYSTEM_TIMER_IPR,
                        SYSTEM_TIMER_IP_BIT,
                        SYSTEM_TIMER_IPM
};

/*
 *  SYSCRの初期値
 *      $CPUディレクトリで定義する内容
 *      　ビット7 SSBY=0：sleep命令でスリープモードへ移行
 *      　ビット3 UE=1：CCR.UIを割込みマスクビットとして利用
 *      　ビット1 1：リザーブビット
 *      　ビット0 RAME=1：内蔵RAM有効
 *      $SYSディレクトリでSYSCR_SYSとして定義する内容
 *      　ビット6-4 STS：スタンバイ・タイマ・セレクト
 *      　ビット2 NMIEG：NMIエッジ
 */
#define SYSCR_INI	(H8SYSCR_UE | BIT1 | H8SYSCR_RAME | SYSCR_SYS)

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize(void)
{
        /*
         *  以下の３つはスタートアップルーチンで初期化が済んでいる。
         *  　　・割込みネストカウンタ intnest
         *  　　・非タスクコンテキストの割込みマスク int_intmask
         *  　　・タスクコンテキストの割込みマスク task_intmask
         *  　　　　（chg_ipmをサポートする場合）
         */

        /*
         *  SYSCRの設定
         */
        sil_wrb_mem((VP)H8SYSCR, (VB)SYSCR_INI);
         
        /*  未完成  */
        /*  sleep命令で遷移する省電力状態の設定  */

        /*  低レベル出力用シリアルポートの初期化  */
        SCI_initialize(SCI_LOW_PORTID);
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate(void)
{
}

#ifdef SUPPORT_CHG_IPM
/*
 *  割込みマスクの変更
 *
 *　IPMに設定できる値としてIPM_LEVEL0、IPM_LEVEL1がマクロ定義されている。
 *
 *
 *　IPM_LEVEL0：レベル０　すべての割込みを受け付ける
 *　IPM_LEVEL1：レベル１　NMIおよびプライオリティレベル１の割込みのみを
 *　　　　　　　　　　　　受け付ける
 *
 *  chg_ipm を使って IPM をレベル2（NMI 以外のすべての割込みを禁止）に
 *  変更することはできない．NMI 以外のすべての割込みを禁止したい場合
 *  には，loc_cpu によりCPUロック状態にすればよい．
 *  IPM が 0 以外の時にも，タスクディスパッチは保留されない．IPM は，
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
        CHECK_PAR( (ipm == IPM_LEVEL0) || (ipm == IPM_LEVEL1) || (ipm == IPM_LEVEL2));

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

/*
 * 微少時間待ち
 * 　　注意事項：
 * 　　　標準ではdlytimはUINT型だが16ビットしかないので、
 * 　　　UW型に変更している。
 * 　　　sil_dly_nse()は内部でsil_dly_nse2()を呼び出す。
 */
void sil_dly_nse(UINT dlytim) {
	sil_dly_nse_long((UW)dlytim);
}

/*****以下、共通部とのインターフェースに含まれない部分*********/

/*
 * 登録されていない割り込みのデフォルト処理
 */

/*
 * スタックの先頭から各レジスタのまでのオフセット
 * 
 * 　スタック構造
 *　　 +0:er0
 *　　 +4:er1
 *　　 +8:er2
 *　　+12:er3
 *　　+16:er4
 *　　+20:er5
 *　　+24:er6
 *　　+28:crr
 *　　+29:pc
 *　　+32〜:割込み前に使用されていたスタック領域
 */

/*
 * 割込み発生直前のスタックポインタまでのオフセット
 */
#define OFFSET_SP	32

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
    
	/*
	 *  %x（整数型）では2バイトしか表示できないので
	 *  %p（ポインタ型）を用いて4バイト表示している。
	 */
    syslog(LOG_EMERG, "Unexpected interrupt.");
    syslog(LOG_EMERG, "PC  = 0x%08p SP  = 0x%08p CCR  = 0x%02x",
                       (VP)pc, (VP)sp2, (INT)ccr);
    syslog(LOG_EMERG, "ER0 = 0x%08p ER1 = 0x%08p ER2 = 0x%08p ER3 = 0x%08p",
           (VP)(sp->er0), (VP)(sp->er1), (VP)(sp->er2), (VP)(sp->er3));
    syslog(LOG_EMERG, "ER4 = 0x%08p ER5 = 0x%08p ER6 = 0x%08p",
                       (VP)(sp->er4), (VP)(sp->er5), (VP)(sp->er6));
    while(1)
    	;
}

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．
 */
void
cpu_putc(char c)
{
    if (c == '\n') {
        SCI_putchar_pol('\r');
    }
    SCI_putchar_pol((UB)c);
}
