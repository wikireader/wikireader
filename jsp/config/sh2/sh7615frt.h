/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2002-2004 by Hokkaido Industrial Research Institute, JAPAN
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
 *  @(#) $Id: sh7615frt.h,v 1.5 2005/07/06 00:45:07 honda Exp $
 */

/*
 *	タイマドライバ
 *  FRTを使用
 */

#ifndef _SH7615FRT_H_
#define _SH7615FRT_H_

/*
 *   タイマへの入力クロックの分周比設定
 *   	f/8でカウント
 */
#define TCR_CKS         0x0

/*
 *   タイマに供給されるクロック周波数[kHz]
 *   	14.7456MHz/8 =1.8432MHz =1843.2kHz
 */
#define TIMER_CLOCK	1843


/*
 *  タイマ値の内部表現の型
 */
typedef UH CLOCK;

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 *
 */
#define	TO_CLOCK(nume, deno)	(TIMER_CLOCK * (nume) / (deno))
#define	TO_USEC(clock)		((clock) * 1000 / TIMER_CLOCK)
#define CLOCK_PER_TICK	((CLOCK) TO_CLOCK(TIC_NUME, TIC_DENO))

/*
 *  設定できる最大のタイマ周期（単位は内部表現）
 */
#define	MAX_CLOCK	((CLOCK) 0xffff)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE	100		/* 処理遅れの見積り値（単位は内部表現） */
#define	BEFORE_IREQ(clock) \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  コントロールレジスタのアドレス
 */

#define FRT_FTCSR	(VB *)0xfffffe11
#define FRT_TOCR	(VB *)0xfffffe17
#define FRT_TCR		(VB *)0xfffffe16
#define FRT_OCRH	(VB *)0xfffffe14
#define FRT_OCRL	(VB *)0xfffffe15
#define FRT_TIER	(VB *)0xfffffe10
#define FRT_FRCH	(VB *)0xfffffe12
#define FRT_FRCL	(VB *)0xfffffe13


#ifndef _MACRO_ONLY

/*
 *  タイマスタート
 */
Inline void
sh2_timer_start ()
{
	/* OCRA一致による割込発生許可 */
	sil_wrb_mem (FRT_TIER, sil_reb_mem(FRT_TIER) | 0x08);
}

/*
 *  タイマ一時停止
 */
Inline void
sh2_timer_stop ()
{
	/* OCRA一致による割込発生不許可 */
	sil_wrb_mem (FRT_TIER, sil_reb_mem(FRT_TIER) & ~0x08);
}
/*
 *  タイマ割込み要求のクリア
 *	TCRレジスタのIMFAビットは1回読み出した後に０を書き込む
 */
Inline void
sh2_timer_int_clear ()
{
	/* 割り込み要求をクリア     */
	/*  （GRAコンペアマッチフラグ） */
	sil_wrb_mem (FRT_FTCSR, sil_reb_mem (FRT_FTCSR) & 0xf7);
	/* OCRAコンペアマッチフラグのクリア */

}


/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
sh2_timer_initialize ()
{
	CLOCK cyc = TO_CLOCK (TIC_NUME, TIC_DENO);


	/*
	 *  タイマ関連の設定
	 */
	sh2_timer_stop ();			/* タイマ停止 */
	/* タイマ上限値のチェック */
	assert (cyc <= MAX_CLOCK);

	/* OCRA選択 */
	sil_wrb_mem (FRT_TOCR, 0x00);
	/*  分周比設定  clock 1/8   */
	sil_wrb_mem (FRT_TCR, 0x00);
	/* OCRA一致によるFRCのクリア */
	sil_wrb_mem (FRT_FTCSR, sil_reb_mem (FRT_FTCSR) | 0x01);
	/* コンペアマッチフラグのクリア */
	sil_wrb_mem (FRT_FTCSR, sil_reb_mem (FRT_FTCSR) & 0xf7);
	/*  OCRAレジスタ設定（カウンタ目標値）      */
	sil_wrb_mem (FRT_OCRH, (CLOCK_PER_TICK >> 8) & 0x00ff);
	sil_wrb_mem (FRT_OCRL, (CLOCK_PER_TICK) & 0x00ff);
	/* OCRA一致による割込発生許可 */
	sil_wrb_mem (FRT_TIER, 0x09);
	/* カウンタをクリア             */
	sil_wrb_mem (FRT_FRCH, 0);
	sil_wrb_mem (FRT_FRCL, 0);
}

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
Inline void
sh2_timer_terminate ()
{
	sh2_timer_stop ();			/* タイマを停止     */
	sh2_timer_int_clear ();		/* 割り込み要求をクリア */

	sil_wrb_mem (FRT_TIER, 0x01);	//割り込み不許可
	sil_wrb_mem (FRT_FRCH, 0);
	sil_wrb_mem (FRT_FRCL, 0);
	sil_wrb_mem (FRT_OCRH, 0);
	sil_wrb_mem (FRT_OCRL, 0);
}

/*
 *  タイマの現在値の読出し
 *　　　タイマ値を読み出す．
 */
Inline CLOCK
sh2_timer_get_current ()
{
	CLOCK	clk;
	CLOCK up, low;


	up = sil_reb_mem (FRT_FRCH);
	low = sil_reb_mem (FRT_FRCL);
	clk = (up << 8) | low;
	return (clk);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
sh2_timer_fetch_interrupt ()
{
	return (sil_reb_mem (FRT_FTCSR) & 0x08);
}

#endif /* _MACRO_ONLY */
#endif /* _SH7615FRT_H_ */
