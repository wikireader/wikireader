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
 *  @(#) $Id: hw_timer.h,v 1.5 2003/12/24 07:24:40 honda Exp $
 */

/*
 *	ターゲット依存タイマモジュール（Linux用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#ifndef _MACRO_ONLY
#include <sys/time.h>
#endif /* _MACRO_ONLY */
/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define	INHNO_TIMER	SIGALRM

/*
 *
 *
 */
#define TIMER_CLOCK             1
#define TO_CLOCK(nume, deno)    (TIMER_CLOCK * (nume) / (deno))



#ifndef _MACRO_ONLY
/*
 *  タイマ値の内部表現の型
 */
typedef UW	CLOCK;

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize()
{
    struct itimerval val;

    val.it_interval.tv_sec = TO_CLOCK(TIC_NUME, TIC_DENO) / 1000;
    
    val.it_interval.tv_usec = (TO_CLOCK(TIC_NUME, TIC_DENO) % 1000) * 1000;
    val.it_value = val.it_interval;
    setitimer(ITIMER_REAL, &val, 0);
}


/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{
}

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
Inline void
hw_timer_terminate()
{
    struct itimerval val;

    val.it_interval.tv_sec = 0;
    val.it_interval.tv_usec = 0;
    val.it_value = val.it_interval;
    setitimer(ITIMER_REAL, &val, 0);
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */


