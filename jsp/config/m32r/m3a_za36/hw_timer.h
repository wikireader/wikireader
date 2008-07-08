/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 * 32 (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
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
 *  @(#) $Id: hw_timer.h,v 1.2 2007/05/28 02:03:55 honda Exp $
 */

/*
 *  ターゲット依存タイマモジュール（M32R 内蔵タイマ(MFT0)用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <sys_config.h>

#define WAIT __asm("nop;nop;nop;nop");

/*
 *  タイマ割込みハンドラのベクタ番号
 */

#define	INHNO_TIMER 20 //INT_MFT0(16) + TIMER_PORT(4) 

#ifndef _MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef UW	CLOCK;

/*                                                                             
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換                               
 */
#define TO_CLOCK(nume, deno)  	(TIMER_CLKCNT * (nume) / (deno))
#define TO_USEC(clock)      	((TO_CLOCK(TIC_NUME, TIC_DENO) - (clock)) * 1000 / TIMER_CLKCNT)

/*
 *  性能評価用システム時刻を取り出す際の処理時間の見積り値（単位は内部表現）
 */
#define GET_TOLERANCE			100
#define BEFORE_IREQ(clock)    	((clock) <= GET_TOLERANCE)

/*
 *  タイマの初期化
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize()
{
	/* タイマ初期化 */
	*(volatile unsigned short *)(ICUCR(MFT,TIMER_PORT)+2) = 0x1000;
	*(volatile unsigned short *)(MFTMOD(TIMER_PORT)+2) = 0x8001;
	*(volatile unsigned short *)(MFTCUT(TIMER_PORT)+2) = TIMER_CLKCNT;
	*(volatile unsigned short *)(MFTCR+2) = 0x8080 >> TIMER_PORT;
}

/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear()
{	
	*(volatile unsigned char *)(ICUCR(MFT,TIMER_PORT)+2) = 0x10;	
}

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止する．
 */
Inline void
hw_timer_terminate()
{	
	*(volatile unsigned char *)(ICUCR(MFT,TIMER_PORT)+2) = 0x0;	
}

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
hw_timer_get_current(void)
{
	return *(volatile unsigned short *)(MFTCUT(TIMER_PORT)+2); 
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
	return *(volatile unsigned char *)(ICUCR(MFT,TIMER_PORT)+2) & 0x1;
}

#endif /* _MACRO_ONLY */

#endif /* _HW_TIMER_H_ */
