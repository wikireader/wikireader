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
 *  @(#) $Id: cpu_config.c,v 1.18 2007/03/23 07:22:15 honda Exp $
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

#include <hw_serial.h>

/*
 *  プロセッサ依存モジュール（H8用）
 */

#ifdef SUPPORT_CHG_IPM
/*
 *  タスクコンテキストでの割込みマスク
 */
volatile UB task_intmask = 0;
#endif /* SUPPORT_CHG_IPM */

/*
 *  非タスクコンテキストでの割込みマスク
 */

volatile UB int_intmask = 0;

/*
 *  割込みネストカウンタ
 */

volatile UB intnest = 0;

/*
 *  CPUロック状態を表すフラグ
 *　　TRUE ：CPUロック状態
 *　　FALSE：CPUロック解除状態
 */
volatile BOOL iscpulocked = TRUE;

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
 *  ベクタテーブルの初期化（for RedBoot）
 */
#ifdef REDBOOT

/*  VECTOR_TABLE_ADDRはsys_config.hで定義する  */
extern void	vector(void);

static void
copy_vector_table(void)
{
	UW n;
	UW *dst = (UW *)VECTOR_TABLE_ADDR;	/* ベクタテーブルコピー先 */
	UW *src = (UW *)vector;			/* ベクタテーブルコピー元 */
	TMP_VECTOR tmp_vector;

	load_vector(&tmp_vector);
	for (n = 0; n < VECTOR_SIZE; n++) {
		*dst = JMP_OPECODE | (*src);	/* jmp命令の付加 */
		++dst;
		++src;
	}
	save_vector(&tmp_vector);
}
#endif	/* of #ifdef REDBOOT */

/*
 *  プロセッサ依存の初期化
 */

#ifndef H8IPRA_INI
#define H8IPRA_INI	0
#endif  /*  H8IPRA_INI  */

#ifndef H8IPRB_INI
#define H8IPRB_INI	0
#endif  /*  H8IPRB_INI  */

void
cpu_initialize(void)
{
	/* 
	 *　CCR のUIビットを割り込みマスクビットとして使用する。
	 *　　SYSCR.UE←0
	 */
	bitclr((UB*)H8SYSCR, H8SYSCR_UE_BIT);
	
	/* 
	 *　すべての割込みプライオリティをレベル０にする
	 *
	 *　　・モニタやスタブが割込みを使用する場合
	 *　　　　H8IPRA_INIとH8IPRB_INIを定義して、設定内容を保持する。
	 *　　　　IERレジスタはカーネル側で変更していないので、そのままで
	 *　　　　よい。
	 */
	sil_wrb_mem((VP)H8IPRA, H8IPRA_INI);
	sil_wrb_mem((VP)H8IPRB, H8IPRB_INI);
	
	SCI_initialize(SYSTEM_PORTID);

#ifdef REDBOOT
	copy_vector_table();
#endif	/* of #ifdef REDBOOT */
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
 *　IPMに設定できる値としてIPM_LEVEL0、IPM_LEVEL1、IPM_LEVEL2がマクロ
 *　定義されている。
 *
 *
 *　IPM_LEVEL0：レベル０　すべての割込みを受け付ける
 *　IPM_LEVEL1：レベル１　NMIおよびプライオリティレベル１の割込みのみを
 *　　　　　　　　　　　　受け付ける
 *　IPM_LEVEL2：レベル２　NMI以外の割込みを受け付けない
 *
 *  IPM がレベル0以外の時にも，タスクディスパッチは保留されない．IPM は，
 *  タスクディスパッチによって，新しく実行状態になったタスクへ引き継が
 *  れる．そのため，タスクが実行中に，別のタスクによって IPM が変更さ
 *  れる場合がある．JSPカーネルでは，IPM の変更はタスク例外処理ルーチ
 *  ンによっても起こるので，別のタスクによって IPM が変更されることに
 *  よって，扱いが難しくなる状況は少ないと思われる．IPM の値によってタ
 *  スクディスパッチを禁止したい場合には，dis_dsp を併用すればよい．
 */
SYSCALL ER
chg_ipm(IPM ipm)
{
        ER      ercd = E_OK;

        LOG_CHG_IPM_ENTER(ipm);
        CHECK_TSKCTX_UNL();
        CHECK_PAR( (ipm == IPM_LEVEL0) || (ipm == IPM_LEVEL1) || (ipm == IPM_LEVEL2) );

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

/*****以下、共通部とのインターフェースに含まれない部分*********/

/*
 * 未定義割込み発生時のエラー出力
 * 　　登録されていない割込みが発生すると呼び出される
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
#define OFFSET_SP	32u

void cpu_experr(EXCSTACK *sp)
{
    UW sp2, pc, ccr, tmp;
    
    sp2 = (UW)sp + OFFSET_SP;
    tmp = sp->pc;
    ccr = (tmp >> 24u) & 0xffu;	/*  上位1バイト  */
    pc = tmp & 0x00ffffffu;	/*  下位3バイト  */
    
    syslog(LOG_EMERG, "Unexpected interrupt.");
    syslog(LOG_EMERG, "PC  = 0x%08x SP  = 0x%08x CCR  = 0x%02x",
                       pc, sp2, ccr);
    syslog(LOG_EMERG, "ER0 = 0x%08x ER1 = 0x%08x ER2 = 0x%08x ER3 = 0x%08x",
                       sp->er0, sp->er1, sp->er2, sp->er3);
    syslog(LOG_EMERG, "ER4 = 0x%08x ER5 = 0x%08x ER6 = 0x%08x",
                       sp->er4, sp->er5, sp->er6);
    while(1)
    	;
}

/*
 *   システム文字出力先の指定
 */

void
cpu_putc(char c)
{
    if (c == '\n') {
        SCI_wait_putchar(SYSTEM_SCI, '\r');
    }
    SCI_wait_putchar(SYSTEM_SCI, c);
}

/*
 *  local_memcpy
 *
 *    標準 C ライブラリの memcpy と同じ、低水準コピー関数
 *    リンク時にエラーが発生するため、名前を変えている。
 *    また、リンクスクリプトの最後の行に
 *
 *       PROVIDE(_memcpy = _local_memcpy);
 *
 *    を追加した。
 *
 *    eepmov.w命令を使う方が効率的だが、データ転送中は（長い時間）
 *    割込み禁止になってしまうので、使用していない。
 */

void *
local_memcpy (void *out, const void *in, size_t n)
{
    char *o = out;
    const char *i = in;

    while (n > 0) {
        *o = *i;
        ++o;
        ++i;
        --n;
    }
    return out;
}

