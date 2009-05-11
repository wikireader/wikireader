/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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
 */

/*
 *	タイマドライバ（uPD71054用）
 */

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <upd71054.h>

/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define	INHNO_TIMER	INTNO_TIMER0

#ifndef _MACRO_ONLY
/*
 *  タイマ値の内部表現の型
 */
typedef UH		CLOCK;

#endif /* _MACRO_ONLY */

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 *  TIMER_CLOCKは、ボード関係のヘッダファイルに記載している。
 */
/* 1ミリ秒周期(isig_tim を呼び出す周期;1kHz)を発生させるためのカウント数 */
#define	TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
/* clock を usec 単位に変換するマクロ */
#define	TO_USEC(clock)		((clock) * 1000u / TIMER_CLOCK)

/*
 *  タイマ周期の単位を内部表現に変換
 *  (利用するタイマがダウンカウンタなので、このマクロがあると便利)
 */
#define CLOCK_PER_TICK		((CLOCK) TO_CLOCK(TIC_NUME, TIC_DENO))

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define MAX_CLOCK		((CLOCK) 0xffff)	/* ２バイト */

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE		100u	/* 処理遅れの見積り値(単位は内部表現) */
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  タイマ停止までの時間（nsec単位）
 *  値に根拠はない．
 */
#define	TIMER_STOP_DELAY	200u

#ifndef _MACRO_ONLY

/*
 *  タイマ割込み要求のクリア
 */
Inline void hw_timer_int_clear() {

	icu_wrb( (VP) ICU_INTR, TIMER0 );
}


/*
 *  タイマの起動処理
 *  ・タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void hw_timer_initialize() {

	upd71054_wrb( (VP) PCNTL, (COUNTER_0 | LOW_HIGH_BYTE | MODE_2 | BINARY) );

	/*
	 *  タイマ周期を設定し，タイマの動作を開始する．
	 */
	assert( CLOCK_PER_TICK <= MAX_CLOCK );

	upd71054_wrb( (VP) PCNT0, (UB) LO8(CLOCK_PER_TICK) );
	upd71054_wrb( (VP) PCNT0, (UB) HI8(CLOCK_PER_TICK) );

	/*
	 *  タイマ割込みの割込みレベルを設定し，要求をクリアした後，
	 *  マスクを解除する．
	 */
	all_set_ilv( INHNO_TIMER, &((IPM) IPM_TIMER0) );/* 割込みレベルの設定 */

	hw_timer_int_clear();				/* 割込み要求をクリア */

	icu_orb( (VP) ICU_INT0M, TIMER0 );		/* 割込みマスク解除 */
}

/*
 *  タイマの停止処理
 *  タイマの動作を停止させる．
 */
Inline void hw_timer_terminate() {

	/*  タイマの動作を停止する  */
	upd71054_wrb( (VP) PCNTL, (COUNTER_0 | LOW_HIGH_BYTE | MODE_2 | BINARY) );

	upd71054_wrb( (VP) PCNT0, 0 );
	upd71054_wrb( (VP) PCNT0, 0 );

	/*
	 *  タイマ割込みをマスクし，要求をクリアする．
	 */
	icu_andb( (VP) ICU_INT0M, ~TIMER0 );		/* 割込みをマスク */

	hw_timer_int_clear();				/* 割込み要求をクリア */
}

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK hw_timer_get_current() {

	UH 	count;
	UB	hi, lo;

	/* カウンタ値の読み出し */
	upd71054_wrb( (VP) PCNTL, (COUNTER_0 | COUNT_LATCH_COMM) );

	lo = upd71054_reb( (VP) PCNT0 );
	hi = upd71054_reb( (VP) PCNT0 );
	count = JOIN8( hi, lo );

	return (CLOCK_PER_TICK - count);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL hw_timer_fetch_interrupt() {

	return( ( icu_reb( (VP) ICU_INTR ) & TIMER0 ) != 0);
}

#endif /* _MACRO_ONLY */
#endif /* _HW_TIMER_H_ */
