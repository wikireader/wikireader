/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 * 
 *  @(#) $Id: sh1itu.h,v 1.8 2005/11/14 08:00:44 honda Exp $
 */

/*
 *	タイマドライバ
 *　　　　　SH1内蔵のインテグレーテッド・タイマ・パルス・ユニットITUの
 *　　　　　チャネル0を使用
 */

#ifndef _SH1ITU_H_
#define _SH1ITU_H_

/*
 *  クロック周波数依存の設定
 *  
 *  　　クロック周波数はコンパイルオプションで与えられる
 */

/*
 *   タイマへの入力クロックの分周比設定
 *   	f/8でカウント
 */
#define TCR_TPSC	0x3u

/*
 *   タイマに供給されるクロック周波数[kHz]
 *   	20MHz/8 =2.5MHz =2500kHz
 */
#ifdef CONFIG_20MHZ
#define TIMER_CLOCK	2500
#endif

/*
 *   	19.6608MHz/8 =2.4576MHz =2457.6kHz
 */
#ifdef CONFIG_19MHZ
#define TIMER_CLOCK	2458
#endif

/*
 *   	16MHz/8 =2MHz =2000kHz
 */
#ifdef CONFIG_16MHZ
#define TIMER_CLOCK	2000
#endif



/*
 *  タイマ値の内部表現の型
 */
typedef UH	CLOCK;

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
#define	MAX_CLOCK	((CLOCK) 0xffffu)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define	GET_TOLERANCE	100	/* 処理遅れの見積り値（単位は内部表現）*/
#define	BEFORE_IREQ(clock)  \
		((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)


/*
 *  コントロールレジスタのアドレス
 */

    	/*  共通  */
#define ITU_TSTR (VB *)0x5ffff00 /*  タイマスタートレジスタ （1バイト）*/
#define ITU_TSNC (VB *)0x5ffff01 /*  タイマシンクロレジスタ （1バイト）*/
#define ITU_TMDR (VB *)0x5ffff02 /*  タイマモードレジスタ （1バイト）  */
	/*  タイマファンクションコントロールレジスタ （1バイト） */
#define ITU_TFCR (VB *)0x5ffff03 
	/*  タイマアウトプットコントロールレジスタ （1バイト）   */
#define ITU_TOCR (VB *)0x5ffff31

	/*  ITU0  */
#define ITU_TCR0  (VB *)0x5ffff04 /*  タイマコントロールレジスタ0 （1バイト）*/
		/*  タイマI/Oコントロールレジスタ0 （1バイト）*/
#define ITU_TIOR0 (VB *)0x5ffff05 
		/*  タイマインタラプトイネーブルレジスタ0 （1バイト）*/
#define ITU_TIER0 (VB *)0x5ffff06 
#define ITU_TSR0  (VB *)0x5ffff07 /*  タイマステータスレジスタ0 （1バイト）*/
#define ITU_TCNT0 (VH *)0x5ffff08 /*  タイマカウンタ0 （2バイト）*/
#define ITU_GRA0  (VH *)0x5ffff0a /*  ジェネラルレジスタA0 （2バイト）*/
#define ITU_GRB0  (VH *)0x5ffff0b /*  ジェネラルレジスタB0 （2バイト）*/


/*
 *  コントロールレジスタの設定値
 */
#define TSR_IMFA	0x01u	/*  GRAコンペアマッチフラグ  		*/
#define TSTR_STR0	0x01u	/*  タイマスタートビット  		*/
#define TCR_CCLR_GRA	0x20u	/*  GRAコンペアマッチでカウンタをクリア */
#define TIER_RESERVE	0x78u	/*  TIERレジスタの予約ビット  		*/
#define TIER_IMIEA	0x01u	/*  GRAコンペアマッチによる割込み要求を許可 */
#define TIOR_RESERVE	0x08u	/*  TIORレジスタの予約ビット  		*/
#define TIOR_IOA	0x00u	/*  GRAコンペアマッチによる端子出力禁止 */


#ifndef _MACRO_ONLY

/*
 *  タイマスタート
 */
Inline void
sh1_itu_start(void)
{
	sh1_orb_reg(ITU_TSTR, TSTR_STR0);
}

/*
 *  タイマ一時停止
 */
Inline void
sh1_itu_stop(void)
{
	sh1_anb_reg(ITU_TSTR, (VB)~TSTR_STR0);
}

/*
 *  タイマ割込み要求のクリア
 *	TCRレジスタのIMFAビットは1回読み出した後に０を書き込む
 */
Inline void
sh1_itu_int_clear(void)
{
	/* 割り込み要求をクリア 	*/
	/*  （GRAコンペアマッチフラグ） */
	sh1_anb_reg(ITU_TSR0, (VB)~TSR_IMFA);
}


/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
Inline void
sh1_itu_initialize(void)
{
	CLOCK	cyc = TO_CLOCK(TIC_NUME, TIC_DENO);


	/*
	 *  タイマ関連の設定
	 */
	sh1_itu_stop();		/* タイマ停止 */
			     		/* タイマ上限値のチェック */
	assert(cyc <= MAX_CLOCK);
			/*  GRAコンペアマッチでカウンタをクリア */
			/*  分周比設定  			*/
	sil_wrb_mem(ITU_TCR0, TCR_CCLR_GRA | TCR_TPSC);
			/*  GRAコンペアマッチによる割込み要求を許可 */
	sil_wrb_mem(ITU_TIER0, TIER_RESERVE | TIER_IMIEA);
			/*  GRAコンペアマッチによる端子出力禁止 */
	sil_wrb_mem(ITU_TIOR0, TIOR_RESERVE | TIOR_IOA);
			/*  GRAレジスタ設定（カウンタ目標値）  	*/
	sil_wrh_mem(ITU_GRA0, CLOCK_PER_TICK);
	sil_wrh_mem(ITU_TCNT0, 0);	/* カウンタをクリア 	*/

}

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
Inline void
sh1_itu_terminate(void)
{
	sh1_itu_stop();			/* タイマを停止 	*/
	sh1_itu_int_clear();		/* 割り込み要求をクリア */

					/* タイマ割込みを禁止   */
			/*  （OVIE,IMIEB,IMIEAビットをクリア）  */
	sil_wrb_mem(ITU_TIER0, TIER_RESERVE);
}

/*
 *  タイマの現在値の読出し
 *　　　タイマの動作を一時的に停止し，タイマ値を読み出す．
 */
Inline CLOCK
sh1_itu_get_current(void)
{
	CLOCK	clk;
	

	sh1_itu_stop();			/*  タイマ停止  */
	
	/*  本来は待ち時間を入れるべき  */
	
	clk = sil_reh_mem(ITU_TCNT0);
	sh1_itu_start();			/*  タイマスタート  */
	
	return(clk);
}

/*
 *  タイマ割込み要求のチェック
 */
Inline BOOL
sh1_itu_fetch_interrupt(void)
{
	VB tsr0 = sil_reb_mem(ITU_TSR0);
	return(tsr0 & TSR_IMFA);
}

#endif /* _MACRO_ONLY */
#endif /* _SH1ITU_H_ */
