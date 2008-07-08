/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: hw_timer.h,v 1.5 2007/01/05 02:33:59 honda Exp $
 */

/*
 *	タイマドライバ（OAKS16 MINI用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <s_services.h>
#include <sil.h>
#include <oaks16mini.h>

/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define	INHNO_TIMER	       INT_TA0

/*
 *  タイマーの割り込みレベル
 */

#define INT_LEVEL_TIMER	0x05

#ifndef _MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef UH	CLOCK;

/*
 *  タイマ値の内部表現とミリ秒単位との変換
 *
 *  1msのタイマー割り込みを使用するのでそのクロックを定義する．
 */
#define TIMER_CLOCK		625
#define TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)		((clock) * 1000 / TIMER_CLOCK)

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffff)

/*
 *  性能評価用システム時刻を取り出す際の処理時間の見積り値（単位は内部表現）
 */
#define	GET_TOLERANCE	100	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize(void)
{
		/* タイマ初期化 */
	sil_wrb_mem((VP)TADR_SFR_UDF, 0x00);		/* ダウンカウント設定				*/
	sil_wrb_mem((VP)TADR_SFR_TA0MR, 0x80);		/* タイマモード	クロック：1/32		*/
	sil_wrh_mem((VP)TADR_SFR_TA0, TIMER_CLOCK-1);/* タイマ値の初期化				*/
												/* 割り込みレベルの設定				*/
	set_ic_ilvl((VP)(TADR_SFR_INT_BASE+TADR_SFR_TA0IC_OFFSET), INT_LEVEL_TIMER);
												/* カウント開始						*/
	sil_wrb_mem((VP)TADR_SFR_TABSR, TBIT_TABSR_TA0S);
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
	VB  c;

	c = sil_reb_mem((VP)TADR_SFR_TABSR);
	c &= ~TBIT_TABSR_TA0S;						/* カウント終了						*/
	sil_wrb_mem((VP)TADR_SFR_TABSR, c);
}

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
hw_timer_get_current(void)
{
	return (TIMER_CLOCK-1)-sil_reh_mem((VP)TADR_SFR_TA0);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
	return (sil_reb_mem((VP)(TADR_SFR_INT_BASE+TADR_SFR_TA0IC_OFFSET)) & TBIT_TAiIC_IR) != 0;
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
