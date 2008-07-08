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

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include "s_services.h"

/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define INHNO_TIMER		S1C33_INHNO_P16TIMER1B

#define S1C33_P16TIMER_CLOCK	40000000

#ifndef _MACRO_ONLY
/*
 *  外部関数の参照
 */
extern	ER	ena_int(INHNO);
extern	ER	dis_int(INHNO);

/*
 *  16ビットタイマ1の初期化
 */
Inline void
hw_timer_initialize(void)
{
	/*
	 *  16ビットタイマ1B 割込み禁止
	 */
	dis_int(INHNO_TIMER);

	/*
	 *  16ビットタイマ1B 割込み要因クリア
	 */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntFactor[2] |= 0x40;

	/*
	 *  16bitタイマ1設定
	 *
	 *  タイマクロックON  16分周  16ビットタイマ1B 周期 1ms
	 */
	(*(s1c33TimerControl_t *) S1C33_TIMER_CONTROL_BASE).bP16ClkCtrl[1]
		= 0x0b;
	(*(s1c33P16Timer_t *) S1C33_P16TIMER_BASE).stChannel[1].uwComp[1]
		= ((S1C33_P16TIMER_CLOCK * 1) / (16 * 1000));

	/*
	 *  16ビットタイマリセット
	 */
	(*(s1c33P16Timer_t *) S1C33_P16TIMER_BASE).stChannel[1].bControl = 0x02;

	/*
	 *  16ビットタイマ1B 割込み許可
	 */
	ena_int(INHNO_TIMER);


	/*
	 *  16ビットタイマ1 動作開始
	 */
	(*(s1c33P16Timer_t *) S1C33_P16TIMER_BASE).stChannel[1].bControl = 0x01;

	return;
}

/*
 *  16ビットタイマ1割込み要因のクリア
 */
Inline void
hw_timer_int_clear(void)
{
	/*
	 *  16ビットタイマ1B 割込み要因クリア
	 */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntFactor[2] |= 0x40;

	return;
}

/*
 *  16ビットタイマ1の停止
 */
Inline void
hw_timer_terminate(void)
{
	/*
	 *  16ビットタイマ1B 割込み禁止
	 */
	dis_int(INHNO_TIMER);

	/*
	 *  16ビットタイマ1停止
	 */
	(*(s1c33P16Timer_t *) S1C33_P16TIMER_BASE).stChannel[1].bControl = 0x00;

	return;
}

#endif /* _MACRO_ONLY  */
#endif /* _HW_TIMER_H_ */
