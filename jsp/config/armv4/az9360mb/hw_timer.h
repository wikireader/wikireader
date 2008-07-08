/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005-2007 by Y.D.K.Co.,LTD Technologies company
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
 *  @(#) $Id: hw_timer.h,v 1.1 2007/05/11 06:06:35 honda Exp $
 */

/*
 *  タイマモジュール
 *  TIMER0を使用
 */

#ifndef	_HW_TIMER_H_
#define	_HW_TIMER_H_

#include	<s_services.h>
#ifndef	_NS9360_H_
#include	<ns9360.h>
#endif

/*
 *  タイマ割込みの割込み番号(タイマーにセットする優先度)
 */
#define		INHNO_TIMER		INTLV_TIM0

#ifndef	_MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef	UW		CLOCK;

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 *  TIMER_CLOCK はターゲットボード毎に設定
 */
#define	TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)			((clock) * 1000 / TIMER_CLOCK)

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffffffff)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE	100
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize()
{
	CLOCK    cyc = TO_CLOCK(TIC_NUME, TIC_DENO);
	    
	sil_wrw_mem((VP)TIMER0_CR,0x00);  			/* タイマ停止,割込みクリア,割込み禁止 */
	assert(cyc <= MAX_CLOCK);					/* タイマ上限値のチェック */
	sil_wrw_mem((VP)TIMER0_CR,0x00000007);		/* タイマ停止,割込みクリア,割込み禁止 */
	sil_wrw_mem((VP)TIMER0_RLC_REG,cyc);		/* カウント値セット       */
	/*
	 *  割込み関連の設定
	 */
	/* 割込みマスククリア割込み許可 */
	sil_wrw_mem((VP)INT_CFG4_7_REG,sil_rew_mem((VP)INT_CFG4_7_REG)|0x80000000);	/* IRQ4 IntEnable */

	/* 割込み許可,タイマスタート */
	sil_wrw_mem((VP)TIMER0_CR,0x0000800f); 
}



/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
	UW	n;

	n = (sil_rew_mem((VP)TIMER0_CR));
	sil_wrw_mem((VP)TIMER0_CR, n | 0x200);
	sil_wrw_mem((VP)TIMER0_CR, n & ~0x200);
}


/*
 *  タイマの停止処理
 */
Inline void
hw_timer_terminate()
{
	/* タイマ停止,割込みクリア,割込み禁止 */
	sil_wrw_mem((VP)TIMER0_CR,0x00000107);
	/* 割込みマスクセット割込み禁止 */
	sil_wrw_mem((VP)INT_CFG4_7_REG,sil_rew_mem((VP)INT_CFG4_7_REG)&~(0x80000000));	/* IRQ4 IntDisable */
}


/*
 *  タイマの現在値の読みだし
 *
 *  割込み禁止区間中で呼び出すこと．
 */
Inline CLOCK
hw_timer_get_current(void)
{
	return(TO_CLOCK(TIC_NUME, TIC_DENO) -
		sil_rew_mem((VP)TIMER0_RR));
}


Inline BOOL
hw_timer_fetch_interrupt(void)
{
	return(sil_rew_mem((VP)TIS_REG) & 1);
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
