/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
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
 */

/*
 *  タイマドライバ（Xstormy16用）
 */

#include <kernel.h>
#include <s_services.h>
#include <hw_timer.h>
#include <cpu_sfrs.h>

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
void hw_timer_initialize(void)
{
                                /*
                                 *  タイムティック周期の設定
                                 */
    _kernel_tic_nume = 5u ;
                                /*
                                 *  内臓RC発信を用い5msの割り込みを発生
                                 */
    __SFR_BITCLR( __OCR0, 4 ) ;         /* start internal-RC                 */
    __SFRW(__T0LR) = 5000-1 ;           /* Internl-RC is almost 1MHz,        */
                                        /*  so set 5000-1 for 5ms            */
    __SFR(__T0PR) = 0 ;                 /* Prescaler = 1/1                   */
    __SFR(__T0CNT) = 0x0d ;             /* Select internal-RC,               */
                                        /*  start timer and enable interrupt */
                                /*
                                 *  デフォルトのタイマー割り込みハンドラを登録する
                                 */
    _kernel_define_inh( INHNO_TIMER, hw_timer_handler ) ;
                                /*
                                 *  タイマー割り込みをLevel1に設定
                                 */
    __SFR_BITCLR( __IL1L, 5 ) ;
    __SFR_BITSET( __IL1L, 4 ) ;
}

/*
 *  タイマ割込み要求のクリア
 */
void hw_timer_int_clear(void)
{
    __SFR_BITCLR( __T0CNT, 1 ) ;
}

/*
 *  タイマの停止処理
 */
void hw_timer_terminate(void)
{
                                /*
                                 *  タイマの動作を停止させる．
                                 */
    __SFR(__T0CNT) = 0x00 ;
                                /*
                                 *  タイマー割り込みを禁止
                                 */
    __SFR_BITCLR( __IL1L, 5 ) ;
    __SFR_BITCLR( __IL1L, 4 ) ;
}

/*
 *  デフォルトのタイマ割込みハンドラ
 */
void hw_timer_handler( void )
{
                                /*
                                 *  タイマ割込み要求のクリア
                                 */
    hw_timer_int_clear() ;
                                /*
                                 *  タイムティックの供給
                                 */
    isig_tim();
}

