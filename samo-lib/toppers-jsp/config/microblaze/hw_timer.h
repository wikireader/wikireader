/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: hw_timer.h,v 1.7 2003/12/11 00:58:01 honda Exp $
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

/*
 * CPU依存タイマモジュール（MicroBlaze用）
 */ 
#include <s_services.h>
#include <microblaze.h>

/*
 * タイマ割込みハンドラの割込み番号
 */
#define INHNO_TIMER TIMINTLVL

#ifndef _MACRO_ONLY

/*
 * タイマ値の内部表現の型
 */
typedef UW CLOCK;

/*                                                                             
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換                               
 *  TIMER_CLOCK はターゲットボード毎に設定                                     
 */
#define TO_CLOCK(nume, deno)  (TIMER_CLOCK * (nume) / (deno))
#define TO_USEC(clock)      ((clock) * 1000 / TIMER_CLOCK)

/*
 *  設定できる最大タイマ周期（単位は内部表現）
 */
#define MAX_CLOCK ((CLOCK) 0xffffffff)

/*
 *  性能評価用システム時刻を取り出す際の処理時間の見積り値（単位は内部表現）
 */
#define GET_TOLERANCE	100
#define BEFORE_IREQ(clock) \
        ((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)


/*
 * タイマの起動処理
 *
 * タイマを初期化し，周期的なタイマ割込みを発生させる
 */ 
Inline void
hw_timer_initialize(void)
{
  
  CLOCK cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

  sil_wrw_mem((VP)TIMER_TCSR0,TIMER_RESET); /* リセット */
  
  assert(cyc <= MAX_CLOCK);	                /* タイマ上限値のチェック */
  sil_wrw_mem((VP)TIMER_TCCR0, cyc);        /* カウンターセット */
  
  /*
   * タイマー0イネーブル，オートリロード，ダウンカウント　割込み許可　
   */
  sil_wrw_mem((VP)TIMER_TCSR0,TIMER_ENABLE | TIMER_RELOAD | TIMER_DOWN_COUNT
                                | TIMER_ENABLE_INTR);

  /*
   *  INTC割込み許可
   */
  intc_enable_interrupt((1 << (TIMINTLVL - 1)));
}



/*
 *   タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
  sil_wrw_mem((VP)TIMER_TCSR0,sil_rew_mem((VP)TIMER_TCSR0));
  /*
   * レベル割込みのため，割込み要因を下ろした後にACK
   */
  intc_ack_interrupt(1 << (TIMINTLVL - 1));
}



/*
 *   タイマの禁止処理
 */
Inline void
hw_timer_terminate()
{
  int dummy;
  
  sil_wrw_mem((VP)TIMER_TCSR0, ~TIMER_ENABLE); /* タイマ停止         */
  sil_wrw_mem((VP)TIMER_TCSR0, sil_rew_mem((VP)TIMER_TCSR0));  /* 割込み要求のクリア */
  
  /*
   * すでに割込みが入っている場合があるので割込み要求をクリア
   */
  dummy = 1 << (TIMINTLVL - 1);
  intc_ack_interrupt(dummy);
  intc_disable_interrupt(dummy);  /* INTCの受け付け禁止 */
}



/*
 *   タイマの現在値の読み出し
 *
 *   割込み禁止区間中で呼び出すこと
 */
Inline CLOCK
hw_timer_get_current(void)
{
  return(TO_CLOCK(TIC_NUME, TIC_DENO) - sil_rew_mem((VP)TIMER_TCR0));
}



/*
 *   割込みが発生しているかの判定
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
  return(sil_rew_mem((VP)TIMER_TCSR0) & TIMER_INTERRUPT);
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
