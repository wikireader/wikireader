/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: hw_timer.h,v 1.2 2007/01/05 02:10:17 honda Exp $
 */

/*
 *	ターゲット依存タイマモジュール（V850ES/SG2内蔵タイマP-ch0(TMP0)用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <s_services.h>

#ifndef _MACRO_ONLY

/* システムタイマーはTMP0を使用 */
#define	TIMER_INT_PRIORITY		0		/* 割り込み優先度(0が最高で7が最低) */
#define TIMER_CLKDIV			1		/* fx/2=10MHz */
#define	TIMER_COMPAREVALUE		(10000-1)	/* fx/2で1msecを作る */

/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define	INHNO_TIMER	       (0x17)

/*
 *  タイマ値の内部表現の型
 */
typedef UW	CLOCK;

#define TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define TO_USEC(clock)      ((clock) * 1000 / TIMER_CLOCK)

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize(void)
{
	/* タイマ初期化 */
	sil_wrb_mem((VP)TP0CTL0, 1);		/* fx/2 (count stop) */
	sil_wrb_mem((VP)TP0CTL1, 0);		/* interval mode */
	sil_wrb_mem((VP)TP0IOC0, 1);		/* TOP00 clock out */
	sil_wrh_mem((VP)TP0CCR0, TIMER_COMPAREVALUE);
	sil_wrb_mem((VP)TP0CTL0, 0x81);		/* Count Start */

	/* 割込み要求を解除しつつ、割込み許可にして、優先度を設定する */
	sil_wrb_mem((VP)TP0CCIC0, TIMER_INT_PRIORITY);
}

/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear(void)
{
}

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
Inline void
hw_timer_terminate(void)
{
	/* 割込みをマスクする */
	sil_wrb_mem((VP)TP0CCIC0, sil_reb_mem((VP)TP0CCIC0) | 0x47);
}

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
hw_timer_get_current(void)
{
	return sil_rew_mem((VP)TP0CNT);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
	return (sil_reb_mem((VP)TP0CCIC0) & 0x80) != 0;
}

#endif /* _MACRO_ONLY */

#endif /* _HW_TIMER_H_ */
