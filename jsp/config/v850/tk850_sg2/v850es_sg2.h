/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2005 by Freelines CO.,Ltd 
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
 *  @(#) $Id: v850es_sg2.h,v 1.4 2007/03/23 08:20:08 honda Exp $
 */

#ifndef	_V850ES_SG2_H_
#define	_V850ES_SG2_H_
/*
 *	プロセッサに依存する定義（V850用）
 */
#ifndef _MACRO_ONLY
typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned long   dword;
typedef volatile byte   IOREG;
typedef volatile word   HIOREG;
typedef volatile dword  LIOREG;
#endif /* _MACRO_ONLY */

/*
 * 内蔵RAM
 */
#define	IRAM_TOP	0x03FF7000		/* 内蔵RAMの先頭アドレス */
#define IRAM_SIZE	0x8000			/* 内蔵RAMの大きさ 32Kbyte(uPD70F3281Y) */
/*
 * 内蔵FlashROM
 */
#define	IROM_TOP	0x0000000		/* 内蔵FlashROMの先頭アドレス */
#define IROM_SIZE	0x60000			/* 内蔵FlashROMの大きさ 384Kbyte(uPD70F3281Y) */

/* 
 * V850ES/SG2のIOレジスタ定義
 */
#define	PDL			0xFFFFF004		/* ポートDLレジスタ */
#define	PDLL		0xFFFFF004		/* ポートDLレジスタL */
#define	PDLH		0xFFFFF005		/* ポートDLレジスタH */
#define	PDH			0xFFFFF006		/* ポートDHレジスタ */
#define	PCT			0xFFFFF00A		/* ポートCTレジスタ */
#define	PCM			0xFFFFF00C		/* ポートCMレジスタ */
#define	PMDL		0xFFFFF024		/* ポートDLモード・レジスタ */
#define	PMDLL		0xFFFFF024		/* ポートDLモード・レジスタL */
#define	PMDLH		0xFFFFF025		/* ポートDLモード・レジスタH */
#define	PMDH		0xFFFFF026		/* ポートDHモード・レジスタ */
#define	PMCT		0xFFFFF02A		/* ポートCTモード・レジスタ */
#define	PMCM		0xFFFFF02C		/* ポートCMモード・レジスタ */
#define	PMCDL		0xFFFFF044		/* ポートDLモード・コントロール・レジスタ */
#define	PMCDLL		0xFFFFF044		/* ポートDLモード・コントロール・レジスタL */
#define	PMCDLH		0xFFFFF045		/* ポートDLモード・コントロール・レジスタH */
#define	PMCDH		0xFFFFF046		/* ポートDHモード・コントロール・レジスタ */
#define	PMCCT		0xFFFFF04A		/* ポートCTモード・コントロール・レジスタ */
#define	PMCCM		0xFFFFF04C		/* ポートCMモード・コントロール・レジスタ */
#define	BPC			0xFFFFF064		/* 周辺I/O領域セレクト制御レジスタ */
#define	BSC			0xFFFFF066		/* バス・サイズ・コンフィギュレーション・レジスタ・バス */
#define	VSWC		0xFFFFF06E		/* システム・ウエイト・コントロール・レジスタ */
#define	DSA0L		0xFFFFF080		/* DMAソース・アドレス・レジスタ0L */
#define	DSA0H		0xFFFFF082		/* DMAソース・アドレス・レジスタ0H */
#define	DDA0L		0xFFFFF084		/* DMAデスティネーション・アドレス・レジスタ0L */
#define	DDA0H		0xFFFFF086		/* DMAデスティネーション・アドレス・レジスタ0H */
#define	DSA1L		0xFFFFF088		/* DMAソース・アドレス・レジスタ1L */
#define	DSA1H		0xFFFFF08A		/* DMAソース・アドレス・レジスタ1H */
#define	DDA1L		0xFFFFF08C		/* DMAデスティネーション・アドレス・レジスタ1L */
#define	DDA1H		0xFFFFF08E		/* DMAデスティネーション・アドレス・レジスタ1H */
#define	DSA2L		0xFFFFF090		/* DMAソース・アドレス・レジスタ2L */
#define	DSA2H		0xFFFFF092		/* DMAソース・アドレス・レジスタ2H */
#define	DDA2L		0xFFFFF094		/* DMAデスティネーション・アドレス・レジスタ2L */
#define	DDA2H		0xFFFFF096		/* DMAデスティネーション・アドレス・レジスタ2H */
#define	DSA3L		0xFFFFF098		/* DMAソース・アドレス・レジスタ3L */
#define	DSA3H		0xFFFFF09A		/* DMAソース・アドレス・レジスタ3H */
#define	DDA3L		0xFFFFF09C		/* DMAデスティネーション・アドレス・レジスタ3L */
#define	DDA3H		0xFFFFF09E		/* DMAデスティネーション・アドレス・レジスタ3H */
#define	DBC0		0xFFFFF0C0		/* DMA転送カウント・レジスタ0 */
#define	DBC1		0xFFFFF0C2		/* DMA転送カウント・レジスタ1 */
#define	DBC2		0xFFFFF0C4		/* DMA転送カウント・レジスタ2 */
#define	DBC3		0xFFFFF0C6		/* DMA転送カウント・レジスタ3 */
#define	DADC0		0xFFFFF0D0		/* DMAアドレシング・コントロール・レジスタ0 */
#define	DADC1		0xFFFFF0D2		/* DMAアドレシング・コントロール・レジスタ1 */
#define	DADC2		0xFFFFF0D4		/* DMAアドレシング・コントロール・レジスタ2 */
#define	DADC3		0xFFFFF0D6		/* DMAアドレシング・コントロール・レジスタ3 */
#define	DCHC0		0xFFFFF0E0		/* DMAチャネル・コントロール・レジスタ0 */
#define	DCHC1		0xFFFFF0E2		/* DMAチャネル・コントロール・レジスタ1 */
#define	DCHC2		0xFFFFF0E4		/* DMAチャネル・コントロール・レジスタ2 */
#define	DCHC3		0xFFFFF0E6		/* DMAチャネル・コントロール・レジスタ3 */
#define	IMR0		0xFFFFF100		/* 割り込みマスク・レジスタ0 */
#define	IMR0L		0xFFFFF100		/* 割り込みマスク・レジスタ0L */
#define	IMR0H		0xFFFFF101		/* 割り込みマスク・レジスタ0H */
#define	IMR1		0xFFFFF102		/* 割り込みマスク・レジスタ1 */
#define	IMR1L		0xFFFFF102		/* 割り込みマスク・レジスタ1L */
#define	IMR1H		0xFFFFF103		/* 割り込みマスク・レジスタ1H */
#define	IMR2		0xFFFFF104		/* 割り込みマスク・レジスタ2 */
#define	IMR2L		0xFFFFF104		/* 割り込みマスク・レジスタ2L */
#define	IMR2H		0xFFFFF105		/* 割り込みマスク・レジスタ2H */
#define	IMR3		0xFFFFF106		/* 割り込みマスク・レジスタ3 */
#define	IMR3L		0xFFFFF106		/* 割り込みマスク・レジスタ3L */
#define	IMR3H		0xFFFFF107		/* 割り込みマスク・レジスタ3H */
#define	LVIIC		0xFFFFF110		/* 割り込み制御レジスタ */
#define	PIC0		0xFFFFF112		/* 割り込み制御レジスタ */
#define	PIC1		0xFFFFF114		/* 割り込み制御レジスタ */
#define	PIC2		0xFFFFF116		/* 割り込み制御レジスタ */
#define	PIC3		0xFFFFF118		/* 割り込み制御レジスタ */
#define	PIC4		0xFFFFF11A		/* 割り込み制御レジスタ */
#define	PIC5		0xFFFFF11C		/* 割り込み制御レジスタ */
#define	PIC6		0xFFFFF11E		/* 割り込み制御レジスタ */
#define	PIC7		0xFFFFF120		/* 割り込み制御レジスタ */
#define	TQ0OVIC		0xFFFFF122		/* 割り込み制御レジスタ */
#define	TQ0CCIC0	0xFFFFF124		/* 割り込み制御レジスタ */
#define	TQ0CCIC1	0xFFFFF126		/* 割り込み制御レジスタ */
#define	TQ0CCIC2	0xFFFFF128		/* 割り込み制御レジスタ */
#define	TQ0CCIC3	0xFFFFF12A		/* 割り込み制御レジスタ */
#define	TP0OVIC		0xFFFFF12C		/* 割り込み制御レジスタ */
#define	TP0CCIC0	0xFFFFF12E		/* 割り込み制御レジスタ */
#define	TP0CCIC1	0xFFFFF130		/* 割り込み制御レジスタ */
#define	TP1OVIC		0xFFFFF132		/* 割り込み制御レジスタ */
#define	TP1CCIC0	0xFFFFF134		/* 割り込み制御レジスタ */
#define	TP1CCIC1	0xFFFFF136		/* 割り込み制御レジスタ */
#define	TP2OVIC		0xFFFFF138		/* 割り込み制御レジスタ */
#define	TP2CCIC0	0xFFFFF13A		/* 割り込み制御レジスタ */
#define	TP2CCIC1	0xFFFFF13C		/* 割り込み制御レジスタ */
#define	TP3OVIC		0xFFFFF13E		/* 割り込み制御レジスタ */
#define	TP3CCIC0	0xFFFFF140		/* 割り込み制御レジスタ */
#define	TP3CCIC1	0xFFFFF142		/* 割り込み制御レジスタ */
#define	TP4OVIC		0xFFFFF144		/* 割り込み制御レジスタ */
#define	TP4CCIC0	0xFFFFF146		/* 割り込み制御レジスタ */
#define	TP4CCIC1	0xFFFFF148		/* 割り込み制御レジスタ */
#define	TP5OVIC		0xFFFFF14A		/* 割り込み制御レジスタ */
#define	TP5CCIC0	0xFFFFF14C		/* 割り込み制御レジスタ */
#define	TP5CCIC1	0xFFFFF14E		/* 割り込み制御レジスタ */
#define	TM0EQIC0	0xFFFFF150		/* 割り込み制御レジスタ */
#define	CB0RIC		0xFFFFF152		/* 割り込み制御レジスタ */
#define	IICIC1		0xFFFFF152		/* 割り込み制御レジスタ */
#define	CB0TIC		0xFFFFF154		/* 割り込み制御レジスタ */
#define	CB1RIC		0xFFFFF156		/* 割り込み制御レジスタ */
#define	CB1TIC		0xFFFFF158		/* 割り込み制御レジスタ */
#define	CB2RIC		0xFFFFF15A		/* 割り込み制御レジスタ */
#define	CB2TIC		0xFFFFF15C		/* 割り込み制御レジスタ */
#define	CB3RIC		0xFFFFF15E		/* 割り込み制御レジスタ */
#define	CB3TIC		0xFFFFF160		/* 割り込み制御レジスタ */
#define	UA0RIC		0xFFFFF162		/* 割り込み制御レジスタ */
#define	CB4RIC		0xFFFFF162		/* 割り込み制御レジスタ */
#define	UA0TIC		0xFFFFF164		/* 割り込み制御レジスタ */
#define	CB4TIC		0xFFFFF164		/* 割り込み制御レジスタ */
#define	UA1RIC		0xFFFFF166		/* 割り込み制御レジスタ */
#define	IICIC2		0xFFFFF166		/* 割り込み制御レジスタ */
#define	UA1TIC		0xFFFFF168		/* 割り込み制御レジスタ */
#define	UA2RIC		0xFFFFF16A		/* 割り込み制御レジスタ */
#define	IICIC0		0xFFFFF16A		/* 割り込み制御レジスタ */
#define	UA2TIC		0xFFFFF16C		/* 割り込み制御レジスタ */
#define	ADIC		0xFFFFF16E		/* 割り込み制御レジスタ */
#define	DMAIC0		0xFFFFF170		/* 割り込み制御レジスタ */
#define	DMAIC1		0xFFFFF172		/* 割り込み制御レジスタ */
#define	DMAIC2		0xFFFFF174		/* 割り込み制御レジスタ */
#define	DMAIC3		0xFFFFF176		/* 割り込み制御レジスタ */
#define	KRIC		0xFFFFF178		/* 割り込み制御レジスタ */
#define	WTIIC		0xFFFFF17A		/* 割り込み制御レジスタ */
#define	WTIC		0xFFFFF17C		/* 割り込み制御レジスタ */
#define	ERRIC0		0xFFFFF17E		/* 割り込み制御レジスタ */
#define	ERRIC		0xFFFFF17E		/* 割り込み制御レジスタ */
#define	WUPIC0		0xFFFFF180		/* 割り込み制御レジスタ */
#define	STAIC		0xFFFFF180		/* 割り込み制御レジスタ */
#define	RECIC0		0xFFFFF182		/* 割り込み制御レジスタ */
#define	IEIC1		0xFFFFF182		/* 割り込み制御レジスタ */
#define	TRXIC0		0xFFFFF184		/* 割り込み制御レジスタ */
#define	IEIC2		0xFFFFF184		/* 割り込み制御レジスタ */
#define	ISPR		0xFFFFF1FA		/* インサービス・プライオリティ・レジスタ */
#define	PRCMD		0xFFFFF1FC		/* コマンド・レジスタ */
#define	PSC			0xFFFFF1FE		/* パワー・セーブ・コントロール・レジスタ */
#define	ADA0M0		0xFFFFF200		/* A/Dコンバータ・モード・レジスタ0 */
#define	ADA0M1		0xFFFFF201		/* A/Dコンバータ・モード・レジスタ1 */
#define	ADA0S		0xFFFFF202		/* A/Dコンバータ・チャネル指定レジスタ */
#define	ADA0M2		0xFFFFF203		/* A/Dコンバータ・モード・レジスタ2 */
#define	ADA0PFM		0xFFFFF204		/* パワー・フェイル比較モード・レジスタ */
#define	ADA0PFT		0xFFFFF205		/* パワー・フェイル比較しきい値レジスタ */
#define	ADA0CR0		0xFFFFF210		/* A/D変換結果レジスタ0 */
#define	ADA0CR0H	0xFFFFF211		/* A/D変換結果レジスタ0H */
#define	ADA0CR1		0xFFFFF212		/* A/D変換結果レジスタ1 */
#define	ADA0CR1H	0xFFFFF213		/* A/D変換結果レジスタ1H */
#define	ADA0CR2		0xFFFFF214		/* A/D変換結果レジスタ2 */
#define	ADA0CR2H	0xFFFFF215		/* A/D変換結果レジスタ2H */
#define	ADA0CR3		0xFFFFF216		/* A/D変換結果レジスタ3 */
#define	ADA0CR3H	0xFFFFF217		/* A/D変換結果レジスタ3H */
#define	ADA0CR4		0xFFFFF218		/* A/D変換結果レジスタ4 */
#define	ADA0CR4H	0xFFFFF219		/* A/D変換結果レジスタ4H */
#define	ADA0CR5		0xFFFFF21A		/* A/D変換結果レジスタ5 */
#define	ADA0CR5H	0xFFFFF21B		/* A/D変換結果レジスタ5H */
#define	ADA0CR6		0xFFFFF21C		/* A/D変換結果レジスタ6 */
#define	ADA0CR6H	0xFFFFF21D		/* A/D変換結果レジスタ6H */
#define	ADA0CR7		0xFFFFF21E		/* A/D変換結果レジスタ7 */
#define	ADA0CR7H	0xFFFFF21F		/* A/D変換結果レジスタ7H */
#define	ADA0CR8		0xFFFFF220		/* A/D変換結果レジスタ8 */
#define	ADA0CR8H	0xFFFFF221		/* A/D変換結果レジスタ8H */
#define	ADA0CR9		0xFFFFF222		/* A/D変換結果レジスタ9 */
#define	ADA0CR9H	0xFFFFF223		/* A/D変換結果レジスタ9H */
#define	ADA0CR10	0xFFFFF224		/* A/D変換結果レジスタ10 */
#define	ADA0CR10H	0xFFFFF225		/* A/D変換結果レジスタ10H */
#define	ADA0CR11	0xFFFFF226		/* A/D変換結果レジスタ11 */
#define	ADA0CR11H	0xFFFFF227		/* A/D変換結果レジスタ11H */
#define	DA0CS0		0xFFFFF280		/* D/Aコンバータ変換値設定レジスタ0 */
#define	DA0CS1		0xFFFFF281		/* D/Aコンバータ変換値設定レジスタ1 */
#define	DA0M		0xFFFFF282		/* D/Aコンバータ・モード・レジスタ */
#define	KRM			0xFFFFF300		/* キー・リターン・モード・レジスタ */
#define	SELCNT0		0xFFFFF308		/* セレクタ動作制御レジスタ */
#define	CRCIN		0xFFFFF310		/* CRCインプット・レジスタ */
#define	CRCD		0xFFFFF312		/* CRCデータ・レジスタ */
#define	NFC			0xFFFFF318		/* ノイズ除去制御レジスタ */
#define	PRSM1		0xFFFFF320		/* BRG1プリスケーラ・モード・レジスタ */
#define	PRSCM1		0xFFFFF321		/* BRG1プリスケーラ・コンペア・レジスタ */
#define	PRSM2		0xFFFFF324		/* BRG2プリスケーラ・モード・レジスタ */
#define	PRSCM2		0xFFFFF325		/* BRG2プリスケーラ・コンペア・レジスタ */
#define	PRSM3		0xFFFFF328		/* BRG3プリスケーラ・モード・レジスタ */
#define	PRSCM3		0xFFFFF329		/* BRG3プリスケーラ・コンペア・レジスタ */
#define	OCKS0		0xFFFFF340		/* IIC分周クロック選択レジスタ0 */
#define	OCKS1		0xFFFFF344		/* IIC分周クロック選択レジスタ1 */
#define	OCKS2		0xFFFFF348		/* IEBusクロック選択レジスタ */
#define	BCR			0xFFFFF360		/* IEBusコントロール・レジスタ */
#define	PSR			0xFFFFF361		/* IEBusパワー・セーブ・レジスタ */
#define	SSR			0xFFFFF362		/* IEBusスレーブ・ステータス・レジスタ */
#define	USR			0xFFFFF363		/* IEBusユニット・ステータス・レジスタ */
#define	ISR			0xFFFFF364		/* IEBusインタラプト・ステータス・レジスタ */
#define	ESR			0xFFFFF365		/* IEBusエラー・ステータス・レジスタ */
#define	UAR			0xFFFFF366		/* IEBusユニット・アドレス・レジスタ */
#define	SAR			0xFFFFF368		/* IEBusスレーブ・アドレス・レジスタ */
#define	PAR			0xFFFFF36A		/* IEBusパートナ・アドレス・レジスタ */
#define	RSA			0xFFFFF36C		/* IEBus受信スレーブ・アドレス・レジスタ */
#define	CDR			0xFFFFF36E		/* IEBusコントロール・データ・レジスタ */
#define	DLR			0xFFFFF36F		/* IEBus電文長レジスタ */
#define	DR			0xFFFFF370		/* IEBusデータ・レジスタ */
#define	FSR			0xFFFFF371		/* IEBusフィールド・ステータス・レジスタ */
#define	SCR			0xFFFFF372		/* IEBusサクセス・カウント・レジスタ */
#define	CCR			0xFFFFF373		/* IEBusコミュニケーション・カウント・レジスタ */
#define	P0			0xFFFFF400		/* ポート0レジスタ */
#define	P1			0xFFFFF402		/* ポート1レジスタ */
#define	P3			0xFFFFF406		/* ポート3レジスタ */
#define	P3L			0xFFFFF406		/* ポート3レジスタL */
#define	P3H			0xFFFFF407		/* ポート3レジスタH */
#define	P4			0xFFFFF408		/* ポート4レジスタ */
#define	P5			0xFFFFF40A		/* ポート5レジスタ */
#define	P7L			0xFFFFF40E		/* ポート7レジスタL */
#define	P7H			0xFFFFF40F		/* ポート7レジスタH */
#define	P9			0xFFFFF412		/* ポート9レジスタ */
#define	P9L			0xFFFFF412		/* ポート9レジスタL */
#define	P9H			0xFFFFF413		/* ポート9レジスタH */
#define	PM0			0xFFFFF420		/* ポート0モード・レジスタ */
#define	PM1			0xFFFFF422		/* ポート1モード・レジスタ */
#define	PM3			0xFFFFF426		/* ポート3モード・レジスタ */
#define	PM3L		0xFFFFF426		/* ポート3モード・レジスタL */
#define	PM3H		0xFFFFF427		/* ポート3モード・レジスタH */
#define	PM4			0xFFFFF428		/* ポート4モード・レジスタ */
#define	PM5			0xFFFFF42A		/* ポート5モード・レジスタ */
#define	PM7L		0xFFFFF42E		/* ポート7モード・レジスタL */
#define	PM7H		0xFFFFF42F		/* ポート7モード・レジスタH */
#define	PM9			0xFFFFF432		/* ポート9モード・レジスタ */
#define	PM9L		0xFFFFF432		/* ポート9モード・レジスタL */
#define	PM9H		0xFFFFF433		/* ポート9モード・レジスタH */
#define	PMC0		0xFFFFF440		/* ポート0モード・コントロール・レジスタ */
#define	PMC3		0xFFFFF446		/* ポート3モード・コントロール・レジスタ */
#define	PMC3L		0xFFFFF446		/* ポート3モード・コントロール・レジスタL */
#define	PMC3H		0xFFFFF447		/* ポート3モード・コントロール・レジスタH */
#define	PMC4		0xFFFFF448		/* ポート4モード・コントロール・レジスタ */
#define	PMC5		0xFFFFF44A		/* ポート5モード・コントロール・レジスタ */
#define	PMC9		0xFFFFF452		/* ポート9モード・コントロール・レジスタ */
#define	PMC9L		0xFFFFF452		/* ポート9モード・コントロール・レジスタL */
#define	PMC9H		0xFFFFF453		/* ポート9モード・コントロール・レジスタH */
#define	PFC0		0xFFFFF460		/* ポート0ファンクション・コントロール・レジスタ */
#define	PFC3		0xFFFFF466		/* ポート3ファンクション・コントロール・レジスタ */
#define	PFC3L		0xFFFFF466		/* ポート3ファンクション・コントロール・レジスタL */
#define	PFC3H		0xFFFFF467		/* ポート3ファンクション・コントロール・レジスタH */
#define	PFC4		0xFFFFF468		/* ポート4ファンクション・コントロール・レジスタ */
#define	PFC5		0xFFFFF46A		/* ポート5ファンクション・コントロール・レジスタ */
#define	PFC9		0xFFFFF472		/* ポート9ファンクション・コントロール・レジスタ */
#define	PFC9L		0xFFFFF472		/* ポート9ファンクション・コントロール・レジスタL */
#define	PFC9H		0xFFFFF473		/* ポート9ファンクション・コントロール・レジスタH */
#define	DWC0		0xFFFFF484		/* データ・ウエイト・コントロール・レジスタ0 */
#define	AWC			0xFFFFF488		/* アドレス・ウエイト・コントロール・レジスタ */
#define	BCC			0xFFFFF48A		/* バス・サイクル・コントロール・レジスタ */
#define	TQ0CTL0		0xFFFFF540		/* TMQ0制御レジスタ0 */
#define	TQ0CTL1		0xFFFFF541		/* TMQ0制御レジスタ1 */
#define	TQ0IOC0		0xFFFFF542		/* TMQ0I/O制御レジスタ0 */
#define	TQ0IOC1		0xFFFFF543		/* TMQ0I/O制御レジスタ1 */
#define	TQ0IOC2		0xFFFFF544		/* TMQ0I/O制御レジスタ2 */
#define	TQ0OPT0		0xFFFFF545		/* TMQ0オプション・レジスタ */
#define	TQ0CCR0		0xFFFFF546		/* TMQ0キャプチャ／コンペア・レジスタ0 */
#define	TQ0CCR1		0xFFFFF548		/* TMQ0キャプチャ／コンペア・レジスタ1 */
#define	TQ0CCR2		0xFFFFF54A		/* TMQ0キャプチャ／コンペア・レジスタ2 */
#define	TQ0CCR3		0xFFFFF54C		/* TMQ0キャプチャ／コンペア・レジスタ3 */
#define	TQ0CNT		0xFFFFF54E		/* TMQ0カウンタ・リード・バッファ・レジスタ */
#define	TP0CTL0		0xFFFFF590		/* TMP0制御レジスタ0 */
#define	TP0CTL1		0xFFFFF591		/* TMP0制御レジスタ1 */
#define	TP0IOC0		0xFFFFF592		/* TMP0I/O制御レジスタ0 */
#define	TP0IOC1		0xFFFFF593		/* TMP0I/O制御レジスタ1 */
#define	TP0IOC2		0xFFFFF594		/* TMP0I/O制御レジスタ2 */
#define	TP0OPT0		0xFFFFF595		/* TMP0オプション・レジスタ */
#define	TP0CCR0		0xFFFFF596		/* TMP0キャプチャ／コンペア・レジスタ0 */
#define	TP0CCR1		0xFFFFF598		/* TMP0キャプチャ／コンペア・レジスタ1 */
#define	TP0CNT		0xFFFFF59A		/* TMP0カウンタ・リード・バッファ・レジスタ */
#define	TP1CTL0		0xFFFFF5A0		/* TMP1制御レジスタ0 */
#define	TP1CTL1		0xFFFFF5A1		/* TMP1制御レジスタ1 */
#define	TP1IOC0		0xFFFFF5A2		/* TMP1I/O制御レジスタ0 */
#define	TP1IOC1		0xFFFFF5A3		/* TMP1I/O制御レジスタ1 */
#define	TP1IOC2		0xFFFFF5A4		/* TMP1I/O制御レジスタ2 */
#define	TP1OPT0		0xFFFFF5A5		/* TMP1オプション・レジスタ */
#define	TP1CCR0		0xFFFFF5A6		/* TMP1キャプチャ／コンペア・レジスタ0 */
#define	TP1CCR1		0xFFFFF5A8		/* TMP1キャプチャ／コンペア・レジスタ1 */
#define	TP1CNT		0xFFFFF5AA		/* TMP1カウンタ・リード・バッファ・レジスタ */
#define	TP2CTL0		0xFFFFF5B0		/* TMP2制御レジスタ0 */
#define	TP2CTL1		0xFFFFF5B1		/* TMP2制御レジスタ1 */
#define	TP2IOC0		0xFFFFF5B2		/* TMP2I/O制御レジスタ0 */
#define	TP2IOC1		0xFFFFF5B3		/* TMP2I/O制御レジスタ1 */
#define	TP2IOC2		0xFFFFF5B4		/* TMP2I/O制御レジスタ2 */
#define	TP2OPT0		0xFFFFF5B5		/* TMP2オプション・レジスタ */
#define	TP2CCR0		0xFFFFF5B6		/* TMP2キャプチャ／コンペア・レジスタ0 */
#define	TP2CCR1		0xFFFFF5B8		/* TMP2キャプチャ／コンペア・レジスタ1 */
#define	TP2CNT		0xFFFFF5BA		/* TMP2カウンタ・リード・バッファ・レジスタ */
#define	TP3CTL0		0xFFFFF5C0		/* TMP3制御レジスタ0 */
#define	TP3CTL1		0xFFFFF5C1		/* TMP3制御レジスタ1 */
#define	TP3IOC0		0xFFFFF5C2		/* TMP3I/O制御レジスタ0 */
#define	TP3IOC1		0xFFFFF5C3		/* TMP3I/O制御レジスタ1 */
#define	TP3IOC2		0xFFFFF5C4		/* TMP3I/O制御レジスタ2 */
#define	TP3OPT0		0xFFFFF5C5		/* TMP3オプション・レジスタ */
#define	TP3CCR0		0xFFFFF5C6		/* TMP3キャプチャ／コンペア・レジスタ0 */
#define	TP3CCR1		0xFFFFF5C8		/* TMP3キャプチャ／コンペア・レジスタ1 */
#define	TP3CNT		0xFFFFF5CA		/* TMP3カウンタ・リード・バッファ・レジスタ */
#define	TP4CTL0		0xFFFFF5D0		/* TMP4制御レジスタ0 */
#define	TP4CTL1		0xFFFFF5D1		/* TMP4制御レジスタ1 */
#define	TP4IOC0		0xFFFFF5D2		/* TMP4I/O制御レジスタ0 */
#define	TP4IOC1		0xFFFFF5D3		/* TMP4I/O制御レジスタ1 */
#define	TP4IOC2		0xFFFFF5D4		/* TMP4I/O制御レジスタ2 */
#define	TP4OPT0		0xFFFFF5D5		/* TMP4オプション・レジスタ */
#define	TP4CCR0		0xFFFFF5D6		/* TMP4キャプチャ／コンペア・レジスタ0 */
#define	TP4CCR1		0xFFFFF5D8		/* TMP4キャプチャ／コンペア・レジスタ1 */
#define	TP4CNT		0xFFFFF5DA		/* TMP4カウンタ・リード・バッファ・レジスタ */
#define	TP5CTL0		0xFFFFF5E0		/* TMP5制御レジスタ0 */
#define	TP5CTL1		0xFFFFF5E1		/* TMP5制御レジスタ1 */
#define	TP5IOC0		0xFFFFF5E2		/* TMP5I/O制御レジスタ0 */
#define	TP5IOC1		0xFFFFF5E3		/* TMP5I/O制御レジスタ1 */
#define	TP5IOC2		0xFFFFF5E4		/* TMP5I/O制御レジスタ2 */
#define	TP5OPT0		0xFFFFF5E5		/* TMP5オプション・レジスタ */
#define	TP5CCR0		0xFFFFF5E6		/* TMP5キャプチャ／コンペア・レジスタ0 */
#define	TP5CCR1		0xFFFFF5E8		/* TMP5キャプチャ／コンペア・レジスタ1 */
#define	TP5CNT		0xFFFFF5EA		/* TMP5カウンタ・リード・バッファ・レジスタ */
#define	WTM			0xFFFFF680		/* 時計タイマ動作モード・レジスタ */
#define	TM0CTL0		0xFFFFF690		/* TMM0制御レジスタ0 */
#define	TM0CMP0		0xFFFFF694		/* TMM0コンペア・レジスタ0 */
#define	OSTS		0xFFFFF6C0		/* 発振安定時間選択レジスタ */
#define	PLLS		0xFFFFF6C1		/* PLLロックアップ時間指定レジスタ */
#define	WDTM2		0xFFFFF6D0		/* ウォッチドッグ・タイマ・モード・レジスタ2 */
#define	WDTE		0xFFFFF6D1		/* ウォッチドッグ・タイマ・イネーブル・レジスタ */
#define	RTBL0		0xFFFFF6E0		/* リアルタイム出力バッファ・レジスタ0Ｌ */
#define	RTBH0		0xFFFFF6E2		/* リアルタイム出力バッファ・レジスタ0H */
#define	RTPM0		0xFFFFF6E4		/* リアルタイム出力ポート・モード・レジスタ0 */
#define	RTPC0		0xFFFFF6E5		/* リアルタイム出力ポート・コントロール・レジスタ0 */
#define	PFCE3L		0xFFFFF706		/* ポート3ファンクション・コントロール拡張レジスタL */
#define	PFCE5		0xFFFFF70A		/* ポート5ファンクション・コントロール拡張レジスタ */
#define	PFCE9		0xFFFFF712		/* ポート9ファンクション・コントロール拡張レジスタ */
#define	PFCE9L		0xFFFFF712		/* ポート9ファンクション・コントロール拡張レジスタL */
#define	PFCE9H		0xFFFFF713		/* ポート9ファンクション・コントロール拡張レジスタH */
#define	SYS			0xFFFFF802		/* システム・ステータス・レジスタ */
#define	RCM			0xFFFFF80C		/* リングOSCモード・レジスタ */
#define	DTFR0		0xFFFFF810		/* DMAトリガ要因レジスタ0 */
#define	DTFR1		0xFFFFF812		/* DMAトリガ要因レジスタ1 */
#define	DTFR2		0xFFFFF814		/* DMAトリガ要因レジスタ2 */
#define	DTFR3		0xFFFFF816		/* DMAトリガ要因レジスタ3 */
#define	PSMR		0xFFFFF820		/* パワー・セーブ・モード・レジスタ */
#define	CKC			0xFFFFF822		/* クロック・コントロール・レジスタ */
#define	LOCKR		0xFFFFF824		/* ロック・レジスタ */
#define	PCC			0xFFFFF828		/* プロセッサ・クロック・コントロール・レジスタ */
#define	PLLCTL		0xFFFFF82C		/* PLLコントロール・レジスタ */
#define	CCLS		0xFFFFF82E		/* CPU動作クロック・ステータス・レジスタ */
#define	CORAD0		0xFFFFF840		/* コレクション・アドレス・レジスタ0 */
#define	CORAD0L		0xFFFFF840		/* コレクション・アドレス・レジスタ0L */
#define	CORAD0H		0xFFFFF842		/* コレクション・アドレス・レジスタ0H */
#define	CORAD1		0xFFFFF844		/* コレクション・アドレス・レジスタ1 */
#define	CORAD1L		0xFFFFF844		/* コレクション・アドレス・レジスタ1L */
#define	CORAD1H		0xFFFFF846		/* コレクション・アドレス・レジスタ1H */
#define	CORAD2		0xFFFFF848		/* コレクション・アドレス・レジスタ2 */
#define	CORAD2L		0xFFFFF848		/* コレクション・アドレス・レジスタ2L */
#define	CORAD2H		0xFFFFF84A		/* コレクション・アドレス・レジスタ2H */
#define	CORAD3		0xFFFFF84C		/* コレクション・アドレス・レジスタ3 */
#define	CORAD3L		0xFFFFF84C		/* コレクション・アドレス・レジスタ3L */
#define	CORAD3H		0xFFFFF84E		/* コレクション・アドレス・レジスタ3H */
#define	CLM			0xFFFFF870		/* クロック・モニタ・モード・レジスタ */
#define	CORCN		0xFFFFF880		/* コレクション・コントロール・レジスタ */
#define	RESF		0xFFFFF888		/* リセット要因フラグ・レジスタ */
#define	LVIM		0xFFFFF890		/* 低電圧検出レジスタ */
#define	LVIS		0xFFFFF891		/* 低電圧検出レベル選択レジスタ */
#define	RAMS		0xFFFFF892		/* 内蔵RAMデータ・ステータス・レジスタ */
#define	PRSM0		0xFFFFF8B0		/* プリスケーラ・モード・レジスタ */
#define	PRSCM0		0xFFFFF8B1		/* プリスケーラ・コンペア・レジスタ */
#define	OCDM		0xFFFFF9FC		/* オンチップ・ディバグ・モード・レジスタ */
#define	PEMU1		0xFFFFF9FE		/* 周辺エミュレーション・レジスタ1 */
#define	UA0CTL0		0xFFFFFA00		/* UARTA0制御レジスタ0 */
#define	UA0CTL1		0xFFFFFA01		/* UARTA0制御レジスタ1 */
#define	UA0CTL2		0xFFFFFA02		/* UARTA0制御レジスタ2 */
#define	UA0OPT0		0xFFFFFA03		/* UARTA0オプション制御レジスタ0 */
#define	UA0STR		0xFFFFFA04		/* UARTA0状態レジスタ */
#define	UA0RX		0xFFFFFA06		/* UARTA0受信データ・レジスタ */
#define	UA0TX		0xFFFFFA07		/* UARTA0送信データ・レジスタ */
#define	UA1CTL0		0xFFFFFA10		/* UARTA1制御レジスタ0 */
#define	UA1CTL1		0xFFFFFA11		/* UARTA1制御レジスタ1 */
#define	UA1CTL2		0xFFFFFA12		/* UARTA1制御レジスタ2 */
#define	UA1OPT0		0xFFFFFA13		/* UARTA1オプション制御レジスタ0 */
#define	UA1STR		0xFFFFFA14		/* UARTA1状態レジスタ */
#define	UA1RX		0xFFFFFA16		/* UARTA1受信データ・レジスタ */
#define	UA1TX		0xFFFFFA17		/* UARTA1送信データ・レジスタ */
#define	UA2CTL0		0xFFFFFA20		/* UARTA2制御レジスタ0 */
#define	UA2CTL1		0xFFFFFA21		/* UARTA2制御レジスタ1 */
#define	UA2CTL2		0xFFFFFA22		/* UARTA2制御レジスタ2 */
#define	UA2OPT0		0xFFFFFA23		/* UARTA2オプション制御レジスタ0 */
#define	UA2STR		0xFFFFFA24		/* UARTA2状態レジスタ */
#define	UA2RX		0xFFFFFA26		/* UARTA2受信データ・レジスタ */
#define	UA2TX		0xFFFFFA27		/* UARTA2送信データ・レジスタ */
#define	INTF0		0xFFFFFC00		/* 外部割り込み立ち下がりエッジ指定レジスタ0 */
#define	INTF3		0xFFFFFC06		/* 外部割り込み立ち下がりエッジ指定レジスタ3 */
#define	INTF9H		0xFFFFFC13		/* 外部割り込み立ち下がりエッジ指定レジスタ9H */
#define	INTR0		0xFFFFFC20		/* 外部割り込み立ち上がりエッジ指定レジスタ0 */
#define	INTR3		0xFFFFFC26		/* 外部割り込み立ち上がりエッジ指定レジスタ3 */
#define	INTR9H		0xFFFFFC33		/* 外部割り込み立ち上がりエッジ指定レジスタ9H */
#define	PF0			0xFFFFFC60		/* ポート0ファンクション制御レジスタ */
#define	PF3			0xFFFFFC66		/* ポート3ファンクション制御レジスタ */
#define	PF3L		0xFFFFFC66		/* ポート3ファンクション制御レジスタL */
#define	PF3H		0xFFFFFC67		/* ポート3ファンクション制御レジスタH */
#define	PF4			0xFFFFFC68		/* ポート4ファンクション制御レジスタ */
#define	PF5			0xFFFFFC6A		/* ポート5ファンクション制御レジスタ */
#define	PF9			0xFFFFFC72		/* ポート9ファンクション制御レジスタ */
#define	PF9L		0xFFFFFC72		/* ポート9ファンクション制御レジスタL */
#define	PF9H		0xFFFFFC73		/* ポート9ファンクション制御レジスタH */
#define	CB0CTL0		0xFFFFFD00		/* CSIB0制御レジスタ0 */
#define	CB0CTL1		0xFFFFFD01		/* CSIB0制御レジスタ1 */
#define	CB0CTL2		0xFFFFFD02		/* CSIB0制御レジスタ2 */
#define	CB0STR		0xFFFFFD03		/* CSIB0状態レジスタ */
#define	CB0RX		0xFFFFFD04		/* CSIB0受信データ・レジスタ */
#define	CB0RXL		0xFFFFFD04		/* CSIB0受信データ・レジスタL */
#define	CB0TX		0xFFFFFD06		/* CSIB0送信データ・レジスタ */
#define	CB0TXL		0xFFFFFD06		/* CSIB0送信データ・レジスタL */
#define	CB1CTL0		0xFFFFFD10		/* CSIB1制御レジスタ0 */
#define	CB1CTL1		0xFFFFFD11		/* CSIB1制御レジスタ1 */
#define	CB1CTL2		0xFFFFFD12		/* CSIB1制御レジスタ2 */
#define	CB1STR		0xFFFFFD13		/* CSIB1状態レジスタ */
#define	CB1RX		0xFFFFFD14		/* CSIB1受信データ・レジスタ */
#define	CB1RXL		0xFFFFFD14		/* CSIB1受信データ・レジスタL */
#define	CB1TX		0xFFFFFD16		/* CSIB1送信データ・レジスタ */
#define	CB1TXL		0xFFFFFD16		/* CSIB1送信データ・レジスタL */
#define	CB2CTL0		0xFFFFFD20		/* CSIB2制御レジスタ0 */
#define	CB2CTL1		0xFFFFFD21		/* CSIB2制御レジスタ1 */
#define	CB2CTL2		0xFFFFFD22		/* CSIB2制御レジスタ2 */
#define	CB2STR		0xFFFFFD23		/* CSIB2状態レジスタ */
#define	CB2RX		0xFFFFFD24		/* CSIB2受信データ・レジスタ */
#define	CB2RXL		0xFFFFFD24		/* CSIB2受信データ・レジスタL */
#define	CB2TX		0xFFFFFD26		/* CSIB2送信データ・レジスタ */
#define	CB2TXL		0xFFFFFD26		/* CSIB2送信データ・レジスタL */
#define	CB3CTL0		0xFFFFFD30		/* CSIB3制御レジスタ0 */
#define	CB3CTL1		0xFFFFFD31		/* CSIB3制御レジスタ1 */
#define	CB3CTL2		0xFFFFFD32		/* CSIB3制御レジスタ2 */
#define	CB3STR		0xFFFFFD33		/* CSIB3状態レジスタ */
#define	CB3RX		0xFFFFFD34		/* CSIB3受信データ・レジスタ */
#define	CB3RXL		0xFFFFFD34		/* CSIB3受信データ・レジスタL */
#define	CB3TX		0xFFFFFD36		/* CSIB3送信データ・レジスタ */
#define	CB3TXL		0xFFFFFD36		/* CSIB3送信データ・レジスタL */
#define	CB4CTL0		0xFFFFFD40		/* CSIB4制御レジスタ0 */
#define	CB4CTL1		0xFFFFFD41		/* CSIB4制御レジスタ1 */
#define	CB4CTL2		0xFFFFFD42		/* CSIB4制御レジスタ2 */
#define	CB4STR		0xFFFFFD43		/* CSIB4状態レジスタ */
#define	CB4RX		0xFFFFFD44		/* CSIB4受信データ・レジスタ */
#define	CB4RXL		0xFFFFFD44		/* CSIB4受信データ・レジスタL */
#define	CB4TX		0xFFFFFD46		/* CSIB4送信データ・レジスタ */
#define	CB4TXL		0xFFFFFD46		/* CSIB4送信データ・レジスタL */
#define	IIC0		0xFFFFFD80		/* IICシフト・レジスタ0 */
#define	IICC0		0xFFFFFD82		/* IICコントロール・レジスタ0 */
#define	SVA0		0xFFFFFD83		/* スレーブ・アドレス・レジスタ0 */
#define	IICCL0		0xFFFFFD84		/* IICクロック選択レジスタ0 */
#define	IICX0		0xFFFFFD85		/* IIC機能拡張レジスタ0 */
#define	IICS0		0xFFFFFD86		/* IIC状態レジスタ0 */
#define	IICF0		0xFFFFFD8A		/* IICフラグ・レジスタ0 */
#define	IIC1		0xFFFFFD90		/* IICシフト・レジスタ1 */
#define	IICC1		0xFFFFFD92		/* IICコントロール・レジスタ1 */
#define	SVA1		0xFFFFFD93		/* IICスレーブ・アドレス・レジスタ1 */
#define	IICCL1		0xFFFFFD94		/* IICクロック選択レジスタ1 */
#define	IICX1		0xFFFFFD95		/* IIC機能拡張レジスタ1 */
#define	IICS1		0xFFFFFD96		/* IIC状態レジスタ1 */
#define	IICF1		0xFFFFFD9A		/* IICフラグ・レジスタ1 */
#define	IIC2		0xFFFFFDA0		/* IICシフト・レジスタ2 */
#define	IICC2		0xFFFFFDA2		/* IICコントロール・レジスタ2 */
#define	SVA2		0xFFFFFDA3		/* IICスレーブ・アドレス・レジスタ2 */
#define	IICCL2		0xFFFFFDA4		/* IICクロック選択レジスタ2 */
#define	IICX2		0xFFFFFDA5		/* IIC機能拡張レジスタ2 */
#define	IICS2		0xFFFFFDA6		/* IIC状態レジスタ2 */
#define	IICF2		0xFFFFFDAA		/* IICフラグ・レジスタ2 */
#define	EXIMC		0xFFFFFFBE		/* 外部バス・インタフェース・モード・コントロール・レジスタ */

#endif	/* _V850ES_SG2_H_ */
