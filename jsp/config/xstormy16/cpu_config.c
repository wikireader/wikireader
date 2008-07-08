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
 *  プロセッサ依存モジュール（Xstormy16用）
 */

#include "jsp_kernel.h"
#include <s_services.h>
#include <hw_timer.h>
#include <hw_serial.h>
#include <cpu_sfrs.h>

/*
 *  タイムティックの周期
 */
UINT _kernel_tic_nume = 1u ;

/*
 *  プロセッサ依存の初期化
 */
void
cpu_initialize(void)
{
                                /*
                                 *  例外の許可
                                 */
    __UNLOCK_EXCP() ;               /* unlock writing EXCPL/EXCPH */
    __SFR_BITSET( __EXCPL, 0 ) ;    /* enable invalid instruction exception */
    __SFR_BITSET( __EXCPL, 2 ) ;    /* enable odd/word access exception */
    __SFR_BITSET( __EXCPL, 4 ) ;    /* enable outrange access exception */
    __LOCK_EXCP() ;                 /* lock writing EXCPL/EXCPH */
                                /*
                                 *  カーネルタイマーの起動
                                 */
    hw_timer_initialize() ;
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate(void)
{
                                /*
                                 *  カーネルタイマーの停止
                                 */
    hw_timer_terminate() ;
                                /*
                                 *  例外の禁止
                                 */
    __UNLOCK_EXCP() ;               /* unlock writing EXCPL/EXCPH */
    __SFR_BITCLR( __EXCPL, 0 ) ;    /* disable invalid instruction exception */
    __SFR_BITCLR( __EXCPL, 2 ) ;    /* disable odd/word access exception */
    __SFR_BITCLR( __EXCPL, 4 ) ;    /* disable outrange access exception */
    __LOCK_EXCP() ;                 /* lock writing EXCPL/EXCPH */
}

/*
 *  割り込みハンドラの登録
 */
void _kernel_define_inh(INHNO inhno, FP inthdr)
{
    extern FP _kernel_ih_table[TNUM_INTERRUPT] ;
    if ( inhno < TNUM_INTERRUPT ) {
        _kernel_ih_table[inhno] = inthdr ;
    }
}

/*
 *  例外ハンドラの登録
 */
void _kernel_define_exc(EXCNO excno, FP exchdr)
{
    extern FP _kernel_eh_table[TNUM_EXCEPTION] ;
    if ( excno < TNUM_EXCEPTION ) {
        _kernel_eh_table[excno] = exchdr ;
    }
}

/*
 *  ターゲットシステムの文字出力
 */
void sys_putc(char c)
{
    sio_asnd_chr( (INT)c ) ;
}

/*
 *  微少時間待ち
 */
void sil_dly_nse( UINT dlytim )
{
    UINT n ;
    for ( n = 3 ; n < dlytim ; n++ ) __asm__ volatile ( "nop" ) ;
}

