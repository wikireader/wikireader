/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2003-2004 by CHUO ELECTRIC WORKS Co.,LTD. JAPAN
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
 *  (株)中央製作所製μITRON搭載SH1CPUボード用
 *  ハードウェア資源の定義
 */

#ifndef _ZUNDA_SH1_H_
#define _ZUNDA_SH1_H_

/* ＳＣＩチャネル０ */
#if 0 /* sh1sci2.cに定義されている */
#define SCI_SMR0	((VH *)0x5fffec0)	/* シリアル･モード･レジスタ */
#define SCI_BRR0	((VH *)0x5fffec1)	/* ビットレート･レジスタ */
#define SCI_SCR0	((VH *)0x5fffec2)	/* シリアル･コントロール･レジスタ */
#define SCI_TDR0	((VH *)0x5fffec3)	/* トランスミット･データ･レジスタ */
#define SCI_SSR0	((VH *)0x5fffec4)	/* シリアル･ステータス･レジスタ */
#define SCI_RDR0	((VH *)0x5fffec5)	/* レシーブ･データ･レジスタ */

/* ＳＣＩチャネル１ */
#define SCI_SMR1	((VH *)0x5fffec8)	/* シリアル･モード･レジスタ */
#define SCI_BRR1	((VH *)0x5fffec9)	/* ビットレート･レジスタ */
#define SCI_SCR1	((VH *)0x5fffeca)	/* シリアル･コントロール･レジスタ */
#define SCI_TDR1	((VH *)0x5fffecb)	/* トランスミット･データ･レジスタ */
#define SCI_SSR1	((VH *)0x5fffecc)	/* シリアル･ステータス･レジスタ */
#define SCI_RDR1	((VH *)0x5fffecd)	/* レシーブ･データ･レジスタ */
#endif	/* sh1sci2.c */

#define PBCR1_TD1_RD1_MASK	~0xf0u		/*  TxD1,RxD1端子設定用マスク	*/
#define PBCR1_TD1 		0x80u		/*  TxD1端子設定用マクロ  	*/
#define PBCR1_RD1 		0x20u		/*  RxD1端子設定用マクロ  	*/

/* Ａ／Ｄ */
#define AD_DRAH		((VH *)0x5fffee0)	/* A/D データ･レジスタＡ H（ﾋﾞｯﾄ9〜2）*/
#define AD_DRAL		((VH *)0x5fffee1)	/* A/D データ･レジスタＡ L（ﾋﾞｯﾄ1〜0）*/
#define AD_DRBH		((VH *)0x5fffee2)	/* A/D データ･レジスタＢ H（ﾋﾞｯﾄ9〜2）*/
#define AD_DRBL		((VH *)0x5fffee3)	/* A/D データ･レジスタＢ L（ﾋﾞｯﾄ1〜0）*/
#define AD_DRCH		((VH *)0x5fffee4)	/* A/D データ･レジスタＣ H（ﾋﾞｯﾄ9〜2）*/
#define AD_DRCL		((VH *)0x5fffee5)	/* A/D データ･レジスタＣ L（ﾋﾞｯﾄ1〜0）*/
#define AD_DRDH		((VH *)0x5fffee6)	/* A/D データ･レジスタＤ H（ﾋﾞｯﾄ9〜2）*/
#define AD_DRDL		((VH *)0x5fffee7)	/* A/D データ･レジスタＤ L（ﾋﾞｯﾄ1〜0）*/
#define AD_CSR		((VH *)0x5fffee8)	/* A/D コントロール／ステータス･レジスタ */
#define AD_CR		((VH *)0x5fffee9)	/* A/D コントロール･レジスタ 		 */
#define	ADF			0x80		/* ADF				*/
#define	ADST			0x20		/* A/D start			*/

#if 0 /* sh1itu.hに定義されている */
/* ＩＴＵ共通 */
#define ITU_TSTR	((VH *)0x5ffff00)	/* タイマ･スタート･レジスタ */
#define ITU_TSNC	((VH *)0x5ffff01)	/* タイマ･シンクロ･レジスタ */
#define ITU_TMDR	((VH *)0x5ffff02)	/* タイマ･モード･レジスタ 	*/
#define ITU_TFCR	((VH *)0x5ffff03)	/* タイマ･ファンクション･コントロール･レジスタ */
#define ITU_TOER	((VH *)0x5ffff31)	/* タイマ･アウトプット･マスタイネーブル･レジスタ */

/* ＩＴＵチャネル０ */
#define ITU_TCR0	((VH *)0x5ffff04)	/* タイマ･コントロール･レジスタ0 		*/
#define ITU_TIOR0	((VH *)0x5ffff05)	/* タイマＩ／Ｏコントロール･レジスタ0 	*/
#define ITU_TIER0	((VH *)0x5ffff06)	/* タイマ･インタラプト･イネーブル･レジスタ0 */
#define ITU_TSR0	((VH *)0x5ffff07)	/* タイマ･ステータス･レジスタ0 */
#define ITU_TCNT0	((VH *)0x5ffff08)	/* タイマ･カウンタ0 */
#define ITU_GRA0	((VH *)0x5ffff0a)	/* ジェネラル･レジスタＡ0 */
#define ITU_GRB0	((VH *)0x5ffff0c)	/* ジェネラル･レジスタＢ0 */
#endif /* sh1itu.h */

/* ＩＴＵチャネル１ */
#define ITU_TCR1	((VH *)0x5ffff0e)	/* タイマ･コントロール･レジスタ1 */
#define ITU_TIOR1	((VH *)0x5ffff0f)	/* タイマＩ／Ｏコントロール･レジスタ1 */
#define ITU_TIER1	((VH *)0x5ffff10)	/* タイマ･インタラプト･イネーブル･レジスタ1 */
#define ITU_TSR1	((VH *)0x5ffff11)	/* タイマ･ステータス･レジスタ1 */
#define ITU_TCNT1	((VH *)0x5ffff12)	/* タイマ･カウンタ1 */
#define ITU_GRA1	((VH *)0x5ffff14)	/* ジェネラル･レジスタＡ1 */
#define ITU_GRB1	((VH *)0x5ffff16)	/* ジェネラル･レジスタＢ1 */

/* ＩＴＵチャネル２ */
#define ITU_TCR2	((VH *)0x5ffff18)	/* タイマ･コントロール･レジスタ2  */
#define ITU_TIOR2	((VH *)0x5ffff19)	/* タイマＩ／Ｏコントロール･レジスタ2  */
#define ITU_TIER2	((VH *)0x5ffff1a)	/* タイマ･インタラプト･イネーブル･レジスタ2  */
#define ITU_TSR2	((VH *)0x5ffff1b)	/* タイマ･ステータス･レジスタ2  */
#define ITU_TCNT2	((VH *)0x5ffff1c)	/* タイマ･カウンタ2  */
#define ITU_GRA2	((VH *)0x5ffff1e)	/* ジェネラル･レジスタＡ2 */
#define ITU_GRB2	((VH *)0x5ffff20)	/* ジェネラル･レジスタＢ2 */

/* ＩＴＵチャネル３ */
#define ITU_TCR3	((VH *)0x5ffff22)	/* タイマ･コントロール･レジスタ3 */
#define ITU_TIOR3	((VH *)0x5ffff23)	/* タイマＩ／Ｏコントロール･レジスタ3 */
#define ITU_TIER3	((VH *)0x5ffff24)	/* タイマ･インタラプト･イネーブル･レジスタ3 */
#define ITU_TSR3	((VH *)0x5ffff25)	/* タイマ･ステータス･レジスタ3 */
#define ITU_TCNT3	((VH *)0x5ffff26)	/* タイマ･カウンタ3 */
#define ITU_GRA3	((VH *)0x5ffff28)	/* ジェネラル･レジスタＡ3 */
#define ITU_GRB3	((VH *)0x5ffff2a)	/* ジェネラル･レジスタＢ3 */
#define ITU_BRA3	((VH *)0x5ffff2c)	/* バッファ･レジスタＡ3 */
#define ITU_BRB3	((VH *)0x5ffff2e)	/* バッファ･レジスタＢ3 */

/* ＩＴＵチャネル４ */
#define ITU_TCR4	((VH *)0x5ffff32)	/* タイマ･コントロール･レジスタ4 */
#define ITU_TIOR4	((VH *)0x5ffff33)	/* タイマＩ／Ｏコントロール･レジスタ4 */
#define ITU_TIER4	((VH *)0x5ffff34)	/* タイマ･インタラプト･イネーブル･レジスタ4 */
#define ITU_TSR4	((VH *)0x5ffff35)	/* タイマ･ステータス･レジスタ4 */
#define ITU_TCNT4	((VH *)0x5ffff36)	/* タイマ･カウンタ4 */
#define ITU_GRA4	((VH *)0x5ffff38)	/* ジェネラル･レジスタＡ4 */
#define ITU_GRB4	((VH *)0x5ffff3a)	/* ジェネラル･レジスタＢ4 */
#define ITU_BRA4	((VH *)0x5ffff3c)	/* バッファ･レジスタＡ4 */
#define ITU_BRB4	((VH *)0x5ffff3e)	/* バッファ･レジスタＢ4 */

/* ＩＴＵ */
#if 0 /* sh1itu.hに定義されている */
#ifndef	TSR_IMFA
#define TSR_IMFA	0x0001			/* GRAｺﾝﾍﾟｱﾏｯﾁ/ｲﾝﾌﾟｯﾄｷｬﾌﾟﾁｬﾌﾗｸﾞA */
#endif
#endif /* sh1itu.h */

#ifndef	TSR_IMFB
#define TSR_IMFB	0x0002			/* GRBｺﾝﾍﾟｱﾏｯﾁ/ｲﾝﾌﾟｯﾄｷｬﾌﾟﾁｬﾌﾗｸﾞB */
#endif
#define TSR_OVF	 	0x0004				/* ｵｰﾊﾞｰﾌﾛｰﾌﾗｸﾞ */

/* ＤＭＡＣ共通 */
#define DMAC_DMAOR	((VH *)0x5ffff48)		/* DMA オペレーション･レジスタ */

/* ＤＭＡＣチャネル０ */
#define DMAC_SAR0	((VH *)0x5ffff40)	/* DMA ソース･アドレス･レジスタ0 */
#define DMAC_DAR0	((VH *)0x5ffff44)	/* DMA デスティネーション･アドレス･レジスタ0 */
#define DMAC_TCR0	((VH *)0x5ffff4a)	/* DMA トランスファ･カウント･レジスタ0 */
#define DMAC_CHCR0	((VH *)0x5ffff4e)		/* DMA チャネル･コントロール･レジスタ0 */

/* ＤＭＡＣチャネル１ */
#define DMAC_SAR1	((VH *)0x5ffff50)	/* DMA ソース･アドレス･レジスタ1 */
#define DMAC_DAR1	((VH *)0x5ffff54)	/* DMA デスティネーション･アドレス･レジスタ1 */
#define DMAC_TCR1	((VH *)0x5ffff5a)	/* DMA トランスファ･カウント･レジスタ1 */
#define DMAC_CHCR1	((VH *)0x5ffff5e)		/* DMA チャネル･コントロール･レジスタ1 */
/* ＤＭＡＣチャネル２ */

#define DMAC_SAR2	((VH *)0x5ffff60)	/* DMA ソース･アドレス･レジスタ2 */
#define DMAC_DAR2	((VH *)0x5ffff64)	/* DMA デスティネーション･アドレス･レジスタ2 */
#define DMAC_TCR2	((VH *)0x5ffff6a)	/* DMA トランスファ･カウント･レジスタ2 */
#define DMAC_CHCR2	((VH *)0x5ffff6e)		/* DMA チャネル･コントロール･レジスタ2 */

/* ＤＭＡＣチャネル３ */
#define DMAC_SAR3	((VH *)0x5ffff70)	/* DMA ソース･アドレス･レジスタ3 */
#define DMAC_DAR3	((VH *)0x5ffff74)	/* DMA デスティネーション･アドレス･レジスタ3 */
#define DMAC_TCR3	((VH *)0x5ffff7a)	/* DMA トランスファ･カウント･レジスタ3 */
#define DMAC_CHCR3	((VH *)0x5ffff7e)		/* DMA チャネル･コントロール･レジスタ3 */

/* 割込みコントローラ */
#define INTC_IPRA	((VH *)0x5ffff84)	/* 割込み優先レベル設定レジスタＡ */
#define INTC_IPRB	((VH *)0x5ffff86)	/* 割込み優先レベル設定レジスタＢ */
#define INTC_IPRC	((VH *)0x5ffff88)	/* 割込み優先レベル設定レジスタＣ */
#define INTC_IPRD	((VH *)0x5ffff8a)	/* 割込み優先レベル設定レジスタＤ */
#define INTC_IPRE	((VH *)0x5ffff8c)	/* 割込み優先レベル設定レジスタＥ */
#define INTC_ICR	((VH *)0x5ffff8e)	/* 割込みコントロールレジスタ */

/* ユーザブレーク･コントローラ */
#define UBC_BARH	((VH *)0x5ffff90)	/* ブレーク･アドレス･レジスタＨ */
#define UBC_BARL	((VH *)0x5ffff92)	/* ブレーク･アドレス･レジスタＬ */
#define UBC_BAMRH	((VH *)0x5ffff94)	/* ブレーク･アドレス･マスク･レジスタＨ */
#define UBC_BAMRL	((VH *)0x5ffff96)	/* ブレーク･アドレス･マスク･レジスタＬ */
#define UBC_BBR		((VH *)0x5ffff98)	/* ブレーク･バスサイクル･レジスタ */

/* バス･コントローラ */
#define BSC_BCR		((VH *)0x5ffffa0)	/* バス･コントロール･レジスタ */
#define BSC_WCR1	((VH *)0x5ffffa2)	/* ウェイト･ステート制御レジスタ1 */
#define BSC_WCR2	((VH *)0x5ffffa4)	/* ウェイト･ステート制御レジスタ2 */
#define BSC_WCR3	((VH *)0x5ffffa6)	/* ウェイト･ステート制御レジスタ3 */
#define BSC_DCR		((VH *)0x5ffffa8)	/* DRAM エリア･コントロール･レジスタ */
#define BSC_PCR		((VH *)0x5ffffaa)	/* DRAM パリティ･コントロール･レジスタ */
#define BSC_RCR		((VH *)0x5ffffac)	/* リフレｯシュ･コントロール･レジスタ */
#define BSC_RTCSR	((VH *)0x5ffffae)	/* リフレｯシｭ･タイマ･コントロｰル/ステｰタｽ･レジｽﾀ */
#define BSC_RTCNT	((VH *)0x5ffffb0)	/* リフレｯシュ･タイマ･カウンタ */
#define BSC_RTCOR	((VH *)0x5ffffb2)	/* リフレｯシュ･タイムコンスタント･レジスタ */

/* ＷＤＴ */
#define WDT_TCSR	((VH *)0x5ffffb8)	/* タイマ･コントロール／ステータス･レジスタ */
#define WDT_TCNT	((VH *)0x5ffffb8)	/* タイマ･カウンタ（ﾗｲﾄ）*/
#define WDT_TCNT_R	((VH *)0x5ffffb9)	/* タイマ･カウンタ（ﾘｰﾄﾞ）*/
#define WDT_RSTCSR	((VH *)0x5ffffba)	/* リセｯト･コントロｰル/ステｰタス･レジスタ（ﾗｲﾄ）*/
#define WDT_RSTCSR_R	((VH *)0x5ffffbb)		/* リセｯト･コントロｰル/ステｰタス･レジスタ（ﾘｰﾄﾞ）*/

/* 低消費電力 */
#define SYS_SBYCR	((VH *)0x5ffffbc)	/* スタンバイ･コントロール･レジスタ */

/* ポートＡ〜Ｃ */
#define PA_DR		((VH *)0x5ffffc0)	/* ポートＡ データ･レジスタ */
#define PB_DR		((VH *)0x5ffffc2)	/* ポートＢ データ･レジスタ */
#define PC_DR		((VH *)0x5ffffd0)	/* ポートＣ データ･レジスタ */

#define PB_RTS0		(1<<2)  		/* RTS0 入力端子（PB2）*/
#define PB_CTS0		(1<<3)  		/* CTS0 出力端子（PB3）*/
#define PB_TXE1		(1<<5)  		/* TXE1 出力端子（PB5）*/
#define PB_RXD0		(1<<8)  		/* RxD0 入力端子（PB8）*/
#define PB_TXD0		(1<<9)  		/* TxD0 出力端子（PB9）*/
#define PB_RXD1		(1<<10) 		/* RxD1 入力端子（PB10）*/
#define PB_TXD1		(1<<11)			/* TxD1 出力端子（PB11）*/

/* ピン･ファンクション･コントローラ */
#define PFC_PAIOR	((VH *)0x5ffffc4)	/* ポートＡ ＩＯレジスタ */
#define PFC_PBIOR	((VH *)0x5ffffc6)	/* ポートＢ ＩＯレジスタ */
#define PFC_PACR1	((VH *)0x5ffffc8)	/* ポートＡ コントロール･レジスタ1 */
#define PFC_PACR2	((VH *)0x5ffffca)	/* ポートＡ コントロール･レジスタ2 */
#define PFC_PBCR1	((VH *)0x5ffffcc)	/* ポートＢ コントロール･レジスタ1 */
#define PFC_PBCR2	((VH *)0x5ffffce)	/* ポートＢ コントロール･レジスタ2 */
#define PFC_CASCR	((VH *)0x5ffffee)	/* カラムアドレス･ストロｰブ･ピン･コンﾄﾛｰﾙ･レジｽﾀ */

/* ＴＰＣ */
#define TPC_TPMR	((VH *)0x5fffff0)	/* TPC 出力モード･レジスタ */
#define TPC_TPCR	((VH *)0x5fffff1)	/* TPC 出力コントロール･レジスタ */
#define TPC_NDERB	((VH *)0x5fffff2)	/* ネクスト･データ･イネーブル･レジスタＢ */
#define TPC_NDERA	((VH *)0x5fffff3)	/* ネクスト･データ･イネーブル･レジスタＡ */
#define TPC_NDRB	((VH *)0x5fffff4)	/* ネクスト･データ･レジスタＢ */
#define TPC_NDRA	((VH *)0x5fffff5)	/* ネクスト･データ･レジスタＡ */
#define TPC_NDRB_2	((VH *)0x5fffff6)	/* ネクスト･データ･レジスタＢ（出力ﾄﾘｶﾞ異なる時）*/
#define TPC_NDRA_2	((VH *)0x5fffff7)	/* ネクスト･データ･レジスタＡ（出力ﾄﾘｶﾞ異なる時）*/

/* 割込みベクタ番号 */
#define INT_NMI		11		/* NMI */
#define INT_UB		12		/* ユーザブレーク */
#define INT_IRQ0	64		/* 外部端子 IRQ0 */
#define INT_IRQ1	65		/* 外部端子 IRQ1 */
#define INT_IRQ2	66		/* 外部端子 IRQ2 */
#define INT_IRQ3	67		/* 外部端子 IRQ3 */
#define INT_IRQ4	68		/* 外部端子 IRQ4 */
#define INT_IRQ5	69		/* 外部端子 IRQ5 */
#define INT_IRQ6	70		/* 外部端子 IRQ6 */
#define INT_IRQ7	71		/* 外部端子 IRQ7 */
#define INT_DEI0	72		/* DMAC チャネル0 */
#define INT_DEI1	74		/* DMAC チャネル1 */
#define INT_DEI2	76		/* DMAC チャネル2 */
#define INT_DEI3	78		/* DMAC チャネル3 */
#define INT_IMIA0	80		/* ITU チｬネル0 コンペアマｯチ/インプｯﾄキｬプチｬA0 */
#define INT_IMIB0	81		/* ITU チｬネル0 コンペアマｯチ/インプｯﾄキｬプチｬB0 */
#define INT_OVI0	82		/* ITU チｬネル0 オーバフロー0 */
#define INT_IMIA1	84		/* ITU チｬネル1 コンペアマｯチ/インプｯﾄキｬプチｬA1 */
#define INT_IMIB1	85		/* ITU チｬネル1 コンペアマｯチ/インプｯﾄキｬプチｬB1 */
#define INT_OVI1	86		/* ITU チｬネル1 オーバフロー1 */
#define INT_IMIA2	88		/* ITU チｬネル2 コンペアマｯチ/インプｯﾄキｬプチｬA2 */
#define INT_IMIB2	89		/* ITU チｬネル2 コンペアマｯチ/インプｯﾄキｬプチｬB2 */
#define INT_OVI2	90		/* ITU チｬネル2 オーバフロー2 */
#define INT_IMIA3	92		/* ITU チｬネル3 コンペアマｯチ/インプｯﾄキｬプチｬA3 */
#define INT_IMIB3	93		/* ITU チｬネル3 コンペアマｯチ/インプｯﾄキｬプチｬB3 */
#define INT_OVI3	94		/* ITU チｬネル3 オーバフロー3 */
#define INT_IMIA4	96		/* ITU チｬネル4 コンペアマｯチ/インプｯﾄキｬプチｬA4 */
#define INT_IMIB4	97		/* ITU チｬネル4 コンペアマｯチ/インプｯﾄキｬプチｬB4 */
#define INT_OVI4	98		/* ITU チｬネル4 オーバフロー4 */
#define INT_ERI0	100		/* SCI チャネル0 受信エラー */
#define INT_RXI0	101	 	/* SCI チャネル0 受信完了 */
#define INT_TXI0	102		/* SCI チャネル0 送信データ･エンプティ */
#define INT_TEI0	103		/* SCI チャネル0 送信終了 */
#define INT_ERI1	104		/* SCI チャネル1 受信エラー */
#define INT_RXI1	105		/* SCI チャネル1 受信完了 */
#define INT_TXI1	106		/* SCI チャネル1 送信データ･エンプティ */
#define INT_TEI1	107		/* SCI チャネル1 送信終了 */
#define INT_PEI		108		/* パリティ */
#define INT_ADI		109		/* A/D エンド */
#define INT_ITI		112		/* ウォッチドック･タイマ（インターバル･タイマ）*/
#define INT_CMI		113		/* リフレッシュコ･ントローラ･コンペアマッチ */

/*
 *  GDB STUB呼出しルーチン
 */
#ifndef _MACRO_ONLY
#ifdef GDB_STUB

#define zunda_sh1_exit	gdb_stub_exit
#define zunda_sh1_putc	gdb_stub_putc

Inline void
gdb_stub_exit(void)
{
	Asm("trapa #0xff"::);		/*  未完成  */
}

/*
 *  gdb stubによる出力
 */
Inline int
gdb_stub_putc(int c)
{
	Asm("mov   #0x00,r0
	     mov   %0,r4
	     trapa #0x21"
               : /* no output */
               : "r"(c)
               : "r0","r4");
	return(c);
}

/*
 *  ROM化の場合
 */
#else /* GDB_STUB */

Inline void
zunda_sh1_exit(void)
{
	while(1);
}

extern void	sh1sci_putc_pol(char c);	/*  sh1sci2.c  */

Inline void 
zunda_sh1_putc(char c)
{
	sh1sci_putc_pol(c);
}

#endif /* GDB_STUB */
#endif /* _MACRO_ONLY */
#endif /* _ZUNDA_SH1_H_ */
