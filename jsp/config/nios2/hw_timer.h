/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: hw_timer.h,v 1.4 2004/09/10 14:38:21 honda Exp $
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

/*
 * CPU依存タイマモジュール（Nios2用）
 */ 
#include <s_services.h>
#include <nios2.h>

/*
 * タイマ割込みハンドラの割込み番号
 */
#define INHNO_TIMER TIM_INTLVL

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

    /* タイマーストップ */
    sil_wrw_mem((VP)TIM_CONTROL,TIM_CONTROL_STOP);
    /* タイムアウトステータスクリア */
    sil_wrw_mem((VP)TIM_STATUS, 0x00);
    
      
    assert(cyc <= MAX_CLOCK);                          /* タイマ上限値のチェック */
    sil_wrw_mem((VP)TIM_PERIODL, (cyc & 0xffff)); /* カウンターセット 下位16bit */
    sil_wrw_mem((VP)TIM_PERIODH, (cyc >> 16));    /* カウンターセット 上位16bit */
  
    /*
     * タイマースタート，オートリロード，割込み許可
     */
    sil_wrw_mem((VP)TIM_CONTROL, TIM_CONTROL_START
                                 |TIM_CONTROL_COUNT|TIM_CONTROL_ITO);

    /*
     * 割込みコントローラの許可
     */
    ena_int(TIM_INTLVL);
}


/*
 *   タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
    /* TOビットのクリア */
    sil_wrw_mem((VP)TIM_STATUS,0x00);
}


/*
 *   タイマの禁止処理
 */
Inline void
hw_timer_terminate()
{
    /* タイマ停止 */
    sil_wrw_mem((VP)TIM_CONTROL, TIM_CONTROL_STOP);

    /* 割込み要求のクリア */
    sil_wrw_mem((VP)TIM_STATUS, 0x00);
}


/*
 *   タイマの現在値の読み出し
 *
 *   割込み禁止区間中で呼び出すこと
 */
Inline CLOCK
hw_timer_get_current(void)
{
    CLOCK count;

    /* スナップレジスタに書き込むと値をホールドする */
    sil_wrw_mem((VP)TIM_SNAPL, 0x00);

    /* カウント値を読み込む */
    count = sil_rew_mem((VP)TIM_SNAPL);
    count |= sil_rew_mem((VP)TIM_SNAPH) << 16; 
    
    return(TIMER_CLOCK - count);
}


/*
 *   割込みが発生しているかの判定
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
    return(sil_rew_mem((VP)TIM_STATUS) & TIM_STATUS_TO);
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
