/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#include <h8s_tpu.h>

/*
 *   タイマドライバ（H8Sファミリー用）
 *     H8Sファミリーには、TPU(16bitタイマ) と 8bitタイマ の２種類が搭載
 *     されており、その搭載本数は、各プロセッサにより異なる。
 *     本実装では、TPU(16bitタイマ) を利用している。（特にチャンネル０）
 *     他のH8Sプロセッサにおいて、8bitタイマ を利用する場合は、変更を
 *     行う必要がある。
 */

/*
 *  タイマ割込みハンドラのベクタ番号
 */
#define	INHNO_TIMER	IRQ_TGI0A

#ifndef _MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef UH	CLOCK;

#endif	/* _MACRO_ONLY */

/*
 *  タイマ周期の単位を内部表現に変換
 *  	jsp/kernel/time_manage.cでも使用
 */
#define TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define TO_USEC(clock)		((clock) * 1000 / TIMER_CLOCK)

/*
 *  タイマ周期の単位を内部表現に変換
 */
#define CLOCK_PER_TICK	((CLOCK) TO_CLOCK(TIC_NUME, TIC_DENO))

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define MAX_CLOCK	((CLOCK) 0xffff)

/*
 *  性能評価用システム時刻を取り出す際の処理時間の見積り値
 *	（単位は内部表現）
 *	注意（本当は分周クロックに依存）
 */

#define GET_TOLERANCE	100u
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

#ifndef _MACRO_ONLY

/*
 *  タイマ割込み要求のクリア
 */
Inline void
hw_timer_int_clear(void)
{
	/* チャネル０の割り込み要求クリア (TGFA = 0) */
	h8s_andb( (VP) TSR0, ~TGFA );
}

/*
 *  タイマの起動処理
 *    タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
hw_timer_initialize(void)
{
	/* モジュールストップモード解除 */
	h8s_andh( (VP) MSTPCR, ~BIT13 );

	/* チャネル０の動作停止 */
	h8s_andb( (VP) TSTR, ~CST0 );

	/* TCR の設定
	   ・TGRAコンペアマッチでカウンタをクリア(CCLR = 1)
	   ・立ち上がりエッジでカウント(CKEG1,0 = 0)
	、 ・内部クロックφ/4でカウント(分周比設定；TPSC0 =1) */
	h8s_wrb_mem( (VP) TCR0, (CCLR0 | TPSC0) );

	/* TIER の設定
	   ・TGFAビットによる割込み要求を許可 */
	h8s_wrb_mem( (VP) TIER0, TGIEA );

	/* TIOR の設定
	   ・TGRAコンペアマッチによる端子出力禁止 */
	/* 設定すべきものは無い。 */

	/* TGR0A の設定
	   ・TGRAレジスタ設定（カウンタ目標値）*/
	h8s_wrh_mem( (VP) TGR0A, CLOCK_PER_TICK );

	h8s_wrh_mem( (VP) TCNT0, 0 );		/* カウンタをクリア */

	/*
	 *  割込み関連の設定
	 */
	icu_set_ilv( (VP) IPRF, IPR_UPR, TPU0_INT_LVL );
						/* 割込みレベル設定 */
	hw_timer_int_clear();			/* 割込み要求をクリア */

	/* チャネル０の動作開始 */
	h8s_orb( (VP) TSTR, CST0 );
}

/*
 *  タイマの停止処理
 */
Inline void
hw_timer_terminate(void)
{
	/* チャネル０の動作停止 */
	h8s_andb( (VP) TSTR, ~CST0 );

	hw_timer_int_clear();			/* 割込み要求をクリア */
}

/*
 *  タイマの現在値の読み出し
 *    割込み禁止区間中で呼び出すこと．
 */
Inline CLOCK
hw_timer_get_current(void)
{
	/* アップカウンタなので。 */
	return( (CLOCK) h8s_reh_mem( (VP) TCNT0 ) );
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
hw_timer_fetch_interrupt(void)
{
	return( h8s_reb_mem( (VP) TSR0 ) & TGFA );
}

#endif	/* _MACRO_ONLY */

#endif /* _HW_TIMER_H_ */
