/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005 by Freelines CO.,Ltd
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
 *  @(#) $Id: cpu_config.c,v 1.9 2007/01/05 02:10:17 honda Exp $
 */

/*
 *	プロセッサ依存モジュール（V850用）
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include <hw_serial.h>

/*
 *  タスクコンテキストでの割込みマスク
 */
#ifdef SUPPORT_CHG_IPM
UH	task_intmask;
#endif /* SUPPORT_CHG_IPM */

/*
 *  非タスクコンテキストでの割込みマスク
 */
UH	int_intmask;


/*
 *  割込みネストカウンタ
 */

UW	intnest = 0;

/*
 *  割込みハンドラテーブル
 */
INTVE InterruptHandlerEntry[NUM_INTERRUPT];

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize(void)
{
	hw_port_initialize(SERIAL_PORT);
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate(void)
{
}

/*
 * 登録されていない割り込みが発生すると呼び出される
 */

void cpu_experr(unsigned int intno)
{
    syslog(LOG_EMERG, "!Unexpected interrupt. [INTNO = %d]", intno);
    __asm("halt");
}

/*
 *   システム文字出力先の指定
 */

void
cpu_putc(char c)
{
    if (c == '\n') {
        hw_port_wait_putchar(SERIAL_PORT, '\r');
    }
    hw_port_wait_putchar(SERIAL_PORT, c);
}

void *
local_memcpy (void *out, const void *in, unsigned int n)
{
    char *o = out;
    const char *i = in;

    while (n -- > 0)
        *o ++ = *i ++;
    return out;
}
