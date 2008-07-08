/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 *  @(#) $Id: pit.h,v 1.1 2004/09/03 16:39:56 honda Exp $
 */

/*
 *	タイマドライバのサンプル
 */

#ifndef _PIT_H_
#define _PIT_H_

/*  ボード毎にSILを用意する  */
//#include <sil_mpc860.h>


/*
 *  タイマ値の内部表現の型
 */
typedef UH	CLOCK;

/*
 *	タイマに供給されるクロック周波数[kHz]
 */

#define TIMER_CLOCK	12500

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 *　　　TIMER_CLOCKはtb6102s.hで定義している
 */
#define	TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)		((clock) * 1000 / TIMER_CLOCK)
#define CLOCK_PER_TICK	((CLOCK) (TO_CLOCK(TIC_NUME, TIC_DENO) - 1))

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffffu)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE	100	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  コントロールレジスタの定義
 */


#ifndef _MACRO_ONLY

/*
 *  タイマスタート
 */
Inline void
pit_start()
{
}

/*
 *  タイマ一時停止
 */
Inline void
pit_stop()
{
}

/*
 *  タイマ割込み要求のクリア
 *          PISCRレジスタのPSビットに1を書き込む
 */
Inline void
pit_int_clear()
{
}


/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
pit_initialize()
{
}

/*
 *  タイマの停止処理
 *
 *  　タイマの動作を停止させる．
 */
Inline void
pit_terminate()
{
}

/*
 *  タイマの現在値の読出し
 *　　　割込み禁止区間中で呼び出すこと
 */
Inline CLOCK
pit_get_current()
{
	return ((CLOCK)0);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
pit_fetch_interrupt()
{
	return (TRUE);
}

#endif /* _MACRO_ONLY */
#endif /* _PIT_H_ */
/*  end of file  */
