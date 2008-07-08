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
 *  @(#) $Id: v850es_kj1.h,v 1.9 2007/03/23 08:20:08 honda Exp $
 */

#ifndef	_V850ES_KJ1_H_
#define	_V850ES_KJ1_H_
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
#define	IRAM_TOP	0x03FFB000		/* 内蔵RAMの先頭アドレス */
#define IRAM_SIZE	0x4000			/* 内蔵RAMの大きさ 16Kbyte(uPD70F3318Y) */
/*
 * 内蔵FlashROM
 */
#define	IROM_TOP	0x0000000		/* 内蔵FlashROMの先頭アドレス */
#define IROM_SIZE	0x40000			/* 内蔵FlashROMの大きさ 256Kbyte(uPD70F3318Y) */

/* 
 * V850ES/KJ1のIOレジスタ定義
 */
#define	PDL			0xFFFFF004		/* ポートDLレジスタ(0x0000) */
#define	PDLL		0xFFFFF004		/* ポートDLレジスタL(0x00) */
#define	PDLH		0xFFFFF005		/* ポートDLレジスタ(0x00) */
#define	PDH			0xFFFFF006		/* ポートDHレジスタ(0x00) */
#define	PCS			0xFFFFF008		/* ポートCSレジスタ(0x00) */
#define	PCT			0xFFFFF00A		/* ポートCTレジスタ(0x00) */
#define	PCM			0xFFFFF00C		/* ポートCMレジスタ(0x00) */
#define	PCD			0xFFFFF00E		/* ポートCDレジスタ(0x00) */
#define	PMDL		0xFFFFF024		/* ポートDLモード・レジスタ(0xFFFF) */
#define	PMDLL		0xFFFFF024		/* ポートDLモード・レジスタ(0xFF) */
#define	PMDLH		0xFFFFF025		/* ポートDLモード・レジスタ(0xFF) */
#define	PMDH		0xFFFFF026		/* ポートDHモード・レジスタ(0xFF) */
#define	PMCS		0xFFFFF028		/* ポートCSモード・レジスタ(0xFF) */
#define	PMCT		0xFFFFF02A		/* ポートCTモード・レジスタ(0xFF) */
#define	PMCM		0xFFFFF02C		/* ポートCMモード・レジスタ(0xFF) */
#define	PMCD		0xFFFFF02E		/* ポートCDモード・レジスタ(0xFF) */
#define	PMCDL		0xFFFFF044		/* ポートDLモード・コントロール・レジスタ(0x0000) */
#define	PMCDLL		0xFFFFF044		/* ポートDLモード・コントロール・レジスタ(0x00) */
#define	PMCDLH		0xFFFFF045		/* ポートDLモード・コントロール・レジスタ(0x00) */
#define	PMCDH		0xFFFFF046		/* ポートDHモード・コントロール・レジスタ(0x00) */
#define	PMCCS		0xFFFFF048		/* ポートCSモード・コントロール・レジスタ(0x00) */
#define	PMCCT		0xFFFFF04A		/* ポートCTモード・コントロール・レジスタ(0x00) */
#define	PMCCM		0xFFFFF04C		/* ポートCMモード・コントロール・レジスタ(0x00) */
#define	BSC			0xFFFFF066		/* バス・サイズ・コンフィギュレーション・レジスタ(0x5555) */
#define	VSWC		0xFFFFF06E		/* システム・ウエイト・コントロール・レジスタ(0x77) */
#define	IMR0		0xFFFFF100		/* 割り込みマスクレジスタ(0xFFFF) */
#define	IMR0L		0xFFFFF100		/* 割り込みマスク・レジスタ0(0xFF) */
#define	IMR0H		0xFFFFF101		/* 割り込みマスク・レジスタ0(0xFF) */
#define	IMR1		0xFFFFF102		/* 割り込みマスク・レジスタ(0xFFFF) */
#define	IMR1L		0xFFFFF102		/* 割り込みマスク・レジスタ1(0xFF) */
#define	IMR1H		0xFFFFF103		/* 割り込みマスク・レジスタ1(0xFF) */
#define	IMR2		0xFFFFF104		/* 割り込みマスク・レジスタ(0xFFFF) */
#define	IMR2L		0xFFFFF104		/* 割り込みマスク・レジスタ2(0xFF) */
#define	IMR2H		0xFFFFF105		/* 割り込みマスク・レジスタ2(0xFF) */
#define	IMR3		0xFFFFF106		/* 割り込みマスク・レジスタ(0xFFFF) */
#define	IMR3L		0xFFFFF106		/* 割り込みマスク・レジスタ3(0xFF) */
#define	WDT1IC		0xFFFFF110		/* 割り込み制御レジスタ(0x47) */
#define	PIC0		0xFFFFF112		/* 割り込み制御レジスタ(0x47) */
#define	PIC1		0xFFFFF114		/* 割り込み制御レジスタ(0x47) */
#define	PIC2		0xFFFFF116		/* 割り込み制御レジスタ(0x47) */
#define	PIC3		0xFFFFF118		/* 割り込み制御レジスタ(0x47) */
#define	PIC4		0xFFFFF11A		/* 割り込み制御レジスタ(0x47) */
#define	PIC5		0xFFFFF11C		/* 割り込み制御レジスタ(0x47) */
#define	PIC6		0xFFFFF11E		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC00		0xFFFFF120		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC01		0xFFFFF122		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC10		0xFFFFF124		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC11		0xFFFFF126		/* 割り込み制御レジスタ(0x47) */
#define	TM5IC0		0xFFFFF128		/* 割り込み制御レジスタ(0x47) */
#define	TM5IC1		0xFFFFF12A		/* 割り込み制御レジスタ(0x47) */
#define	CSI0IC0		0xFFFFF12C		/* 割り込み制御レジスタ(0x47) */
#define	CSI0IC1		0xFFFFF12E		/* 割り込み制御レジスタ(0x47) */
#define	SREIC0		0xFFFFF130		/* 割り込み制御レジスタ(0x47) */
#define	SRIC0		0xFFFFF132		/* 割り込み制御レジスタ(0x47) */
#define	STIC0		0xFFFFF134		/* 割り込み制御レジスタ(0x47) */
#define	SREIC1		0xFFFFF136		/* 割り込み制御レジスタ(0x47) */
#define	SRIC1		0xFFFFF138		/* 割り込み制御レジスタ(0x47) */
#define	STIC1		0xFFFFF13A		/* 割り込み制御レジスタ(0x47) */
#define	TMHIC0		0xFFFFF13C		/* 割り込み制御レジスタ(0x47) */
#define	TMHIC1		0xFFFFF13E		/* 割り込み制御レジスタ(0x47) */
#define	CSIAIC0		0xFFFFF140		/* 割り込み制御レジスタ(0x47) */
#define	IICIC0		0xFFFFF142		/* 割り込み制御レジスタ(0x47) */
#define	ADIC		0xFFFFF144		/* 割り込み制御レジスタ(0x47) */
#define	KRIC		0xFFFFF146		/* 割り込み制御レジスタ(0x47) */
#define	WTIIC		0xFFFFF148		/* 割り込み制御レジスタ(0x47) */
#define	WTIC		0xFFFFF14A		/* 割り込み制御レジスタ(0x47) */
#define	BRGIC		0xFFFFF14C		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC20		0xFFFFF14E		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC21		0xFFFFF150		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC30		0xFFFFF152		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC31		0xFFFFF154		/* 割り込み制御レジスタ(0x47) */
#define	CSIAIC1		0xFFFFF156		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC40		0xFFFFF158		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC41		0xFFFFF15A		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC50		0xFFFFF15C		/* 割り込み制御レジスタ(0x47) */
#define	TM0IC51		0xFFFFF15E		/* 割り込み制御レジスタ(0x47) */
#define	CSI0IC2		0xFFFFF160		/* 割り込み制御レジスタ(0x47) */
#define	SREIC2		0xFFFFF162		/* 割り込み制御レジスタ(0x47) */
#define	SRIC2		0xFFFFF164		/* 割り込み制御レジスタ(0x47) */
#define	STIC2		0xFFFFF166		/* 割り込み制御レジスタ(0x47) */
#define	IICIC1		0xFFFFF168		/* 割り込み制御レジスタ(0x47) */
#define	TP0OVIC		0xFFFFF174		/* 割り込み制御レジスタ(0x47) */
#define	TP0CCIC0	0xFFFFF176		/* 割り込み制御レジスタ(0x47) */
#define	TP0CCIC1	0xFFFFF178		/* 割り込み制御レジスタ(0x47) */
#define	ISPR		0xFFFFF1FA		/* インサービス・プライオリティ・レジスタ(0x00) */
#define	PRCMD		0xFFFFF1FC		/* コマンド・レジスタ(不定) */
#define	PSC			0xFFFFF1FE		/* パワー・セーブ・コントロール・レジスタ(0x00) */
#define	ADM			0xFFFFF200		/* A/Dコンバータ・モード・レジスタ(0x00) */
#define	ADS			0xFFFFF201		/* アナログ入力チャネル指定レジスタ(0x00) */
#define	PFM			0xFFFFF202		/* パワー・フェイル比較モード・レジスタ(0x00) */
#define	PFT			0xFFFFF203		/* パワー・フェイル比較しきい値レジスタ(0x00) */
#define	ADCR		0xFFFFF204		/* A/D変換結果レジスタ(不定) */
#define	ADCRH		0xFFFFF205		/* A/D変換結果レジスタ(不定) */
#define	DACS0		0xFFFFF280		/* D/A変換値設定レジスタ(0x00) */
#define	DACS1		0xFFFFF282		/* D/A変換値設定レジスタ(0x00) */
#define	DAM			0xFFFFF284		/* D/Aコンバータ・モード・レジスタ(0x00) */
#define	KRM			0xFFFFF300		/* キー・リターン・モード・レジスタ(0x00) */
#define	P0			0xFFFFF400		/* ポート0レジスタ(0x00) */
#define	P1			0xFFFFF402		/* ポート1レジスタ(0x00) */
#define	P3			0xFFFFF406		/* ポート3レジスタ(0x0000) */
#define	P3L			0xFFFFF406		/* ポート3レジスタ(0x00) */
#define	P3H			0xFFFFF407		/* ポート3レジスタ(0x00) */
#define	P4			0xFFFFF408		/* ポート4レジスタ(0x00) */
#define	P5			0xFFFFF40A		/* ポート5レジスタ(0x00) */
#define	P6			0xFFFFF40C		/* ポート6レジスタ(0x0000) */
#define	P6L			0xFFFFF40C		/* ポート6レジスタ(0x00) */
#define	P6H			0xFFFFF40D		/* ポート6レジスタ(0x00) */
#define	P7			0xFFFFF40E		/* ポート7レジスタ(不定) */
#define	P7L			0xFFFFF40E		/* ポート7レジスタ(不定) */
#define	P7H			0xFFFFF40F		/* ポート7レジスタ(不定) */
#define	P8			0xFFFFF410		/* ポート8レジスタ(0x00) */
#define	P9			0xFFFFF412		/* ポート9レジスタ(0x0000) */
#define	P9L			0xFFFFF412		/* ポート9レジスタ(0x00) */
#define	P9H			0xFFFFF413		/* ポート9レジスタ(0x00) */
#define	PM0			0xFFFFF420		/* ポート0モード・レジスタ(0xFF) */
#define	PM1			0xFFFFF422		/* ポート1モード・レジスタ(0xFF) */
#define	PM3			0xFFFFF426		/* ポート3モード・レジスタ(0xFFFF) */
#define	PM3L		0xFFFFF426		/* ポート3モード・レジスタ(0xFF) */
#define	PM3H		0xFFFFF427		/* ポート3モード・レジスタ(0xFF) */
#define	PM4			0xFFFFF428		/* ポート4モード・レジスタ(0xFF) */
#define	PM5			0xFFFFF42A		/* ポート5モード・レジスタ(0xFF) */
#define	PM6			0xFFFFF42C		/* ポート6モード・レジスタ(0xFFFF) */
#define	PM6L		0xFFFFF42C		/* ポート6モード・レジスタ(0xFF) */
#define	PM6H		0xFFFFF42D		/* ポート6モード・レジスタ(0xFF) */
#define	PM8			0xFFFFF430		/* ポート8モード・レジスタ(0xFF) */
#define	PM9			0xFFFFF432		/* ポート9モード・レジスタ(0xFFFF) */
#define	PM9L		0xFFFFF432		/* ポート9モード・レジスタ(0xFF) */
#define	PM9H		0xFFFFF433		/* ポート9モード・レジスタ(0xFF) */
#define	PMC0		0xFFFFF440		/* ポート0モード・コントロール・レジスタ(0x00) */
#define	PMC3		0xFFFFF446		/* ポート3モード・コントロール・レジスタ(0x0000) */
#define	PMC3L		0xFFFFF446		/* ポート3モード・コントロール・レジスタ(0x00) */
#define	PMC3H		0xFFFFF447		/* ポート3モード・コントロール・レジスタ(0x00) */
#define	PMC4		0xFFFFF448		/* ポート4モード・コントロール・レジスタ(0x00) */
#define	PMC5		0xFFFFF44A		/* ポート5モード・コントロール・レジスタ(0x00) */
#define	PMC6		0xFFFFF44C		/* ポート6モード・コントロール・レジスタ(0x0000) */
#define	PMC6L		0xFFFFF44C		/* ポート6モード・コントロール・レジスタ(0x00) */
#define	PMC6H		0xFFFFF44D		/* ポート6モード・コントロール・レジスタ(0x00) */
#define	PMC8		0xFFFFF450		/* ポート8モード・コントロール・レジスタ(0x00) */
#define	PMC9		0xFFFFF452		/* ポート9モード・コントロール・レジスタ(0x0000) */
#define	PMC9L		0xFFFFF452		/* ポート9モード・コントロール・レジスタ(0x00) */
#define	PMC9H		0xFFFFF453		/* ポート9モード・コントロール・レジスタ(0x00) */
#define	PFC3		0xFFFFF466		/* ポート3ファンクション・コントロール・レジスタ(0x00) */
#define	PFC5		0xFFFFF46A		/* ポート5ファンクション・コントロール・レジスタ(0x00) */
#define	PFC6H		0xFFFFF46D		/* ポート6ファンクション・コントロール・レジスタ(0x00) */
#define	PFC8		0xFFFFF470		/* ポート8ファンクション・コントロール・レジスタ(0x00) */
#define	PFC9		0xFFFFF472		/* ポート9ファンクション・コントロール・レジスタ(0x0000) */
#define	PFC9L		0xFFFFF472		/* ポート9ファンクション・コントロール・レジスタ(0x00) */
#define	PFC9H		0xFFFFF473		/* ポート9ファンクション・コントロール・レジスタ(0x00) */
#define	DWC0		0xFFFFF484		/* データ・ウエイト制御レジスタ(0x7777) */
#define	AWC			0xFFFFF488		/* アドレス・ウエイト制御レジスタ(0xFFFF) */
#define	BCC			0xFFFFF48A		/* バス・サイクル制御レジスタ(0xAAAA) */
#define	TMHMD0		0xFFFFF580		/* 8ビット・タイマHモード・レジスタ(0x00) */
#define	TMCYC0		0xFFFFF581		/* 8ビット・タイマHキャリア・コントロール・レジスタ(0x00) */
#define	CMP00		0xFFFFF582		/* 8ビット・タイマHコンペア・レジスタ0(0x00) */
#define	CMP01		0xFFFFF583		/* 8ビット・タイマHコンペア・レジスタ0(0x00) */
#define	TMHMD1		0xFFFFF590		/* 8ビット・タイマHモード・レジスタ(0x00) */
#define	TMCYC1		0xFFFFF591		/* 8ビット・タイマHキャリア・コントロール・レジスタ(0x00) */
#define	CMP10		0xFFFFF592		/* 8ビット・タイマHコンペア・レジスタ1(0x00) */
#define	CMP11		0xFFFFF593		/* 8ビット・タイマHコンペア・レジスタ1(0x00) */
#define	TP0CTL0		0xFFFFF5A0		/* TMP0制御レジスタ(0x00) */
#define	TP0CTL1		0xFFFFF5A1		/* TMP0制御レジスタ(0x00) */
#define	TP0IOC0		0xFFFFF5A2		/* TMP0I/O制御レジスタ(0x00) */
#define	TP0IOC1		0xFFFFF5A3		/* TMP0I/O制御レジスタ(0x00) */
#define	TP0IOC2		0xFFFFF5A4		/* TMP0I/O制御レジスタ(0x00) */
#define	TP0OPT0		0xFFFFF5A5		/* TMP0オプション・レジスタ(0x00) */
#define	TP0CCR0		0xFFFFF5A6		/* TMP0キャプチャ／コンペア・レジスタ(0x0000) */
#define	TP0CCR1		0xFFFFF5A8		/* TMP0キャプチャ／コンペア・レジスタ(0x0000) */
#define	TP0CNT		0xFFFFF5AA		/* TMP0カウンタ・リード・バッファ・レジスタ(0x0000) */
#define	TM5			0xFFFFF5C0		/* 16ビット・タイマ・カウンタ(0x0000) */
#define	TM50		0xFFFFF5C0		/* 8ビット・タイマ・カウンタ5(0x00) */
#define	TM51		0xFFFFF5C1		/* 8ビット・タイマ・カウンタ5(0x00) */
#define	CR5			0xFFFFF5C2		/* 16ビット・タイマ・コンペア・レジスタ(0x0000) */
#define	CR50		0xFFFFF5C2		/* 8ビット・タイマ・コンペア・レジスタ5(0x00) */
#define	CR51		0xFFFFF5C3		/* 8ビット・タイマ・コンペア・レジスタ5(0x00) */
#define	TCL5		0xFFFFF5C4		/* タイマ・クロック選択レジスタ(0x0000) */
#define	TCL50		0xFFFFF5C4		/* タイマ・クロック選択レジスタ5(0x00) */
#define	TCL51		0xFFFFF5C5		/* タイマ・クロック選択レジスタ5(0x00) */
#define	TMC5		0xFFFFF5C6		/* 16ビット・タイマ・モード・コントロール・レジスタ(0x0000) */
#define	TMC50		0xFFFFF5C6		/* 8ビット・タイマ・モード・コントロール・レジスタ5(0x00) */
#define	TMC51		0xFFFFF5C7		/* 8ビット・タイマ・モード・コントロール・レジスタ5(0x00) */
#define	TM00		0xFFFFF600		/* 16ビット・タイマ・カウンタ0(0x0000) */
#define	CR000		0xFFFFF602		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ00(0x0000) */
#define	CR001		0xFFFFF604		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ00(0x0000) */
#define	TMC00		0xFFFFF606		/* 16ビット・タイマ・モード・コントロール・レジスタ0(0x00) */
#define	PRM00		0xFFFFF607		/* プリスケーラ・モード・レジスタ0(0x00) */
#define	CRC00		0xFFFFF608		/* キャプチャ／コンペア・コントロール・レジスタ0(0x00) */
#define	TOC00		0xFFFFF609		/* 16ビット・タイマ出力コントロール・レジスタ0(0x00) */
#define	TM01		0xFFFFF610		/* 16ビット・タイマ・カウンタ0(0x0000) */
#define	CR010		0xFFFFF612		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ01(0x0000) */
#define	CR011		0xFFFFF614		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ01(0x0000) */
#define	TMC01		0xFFFFF616		/* 16ビット・タイマ・モード・コントロール・レジスタ0(0x00) */
#define	PRM01		0xFFFFF617		/* プリスケーラ・モード・レジスタ0(0x00) */
#define	CRC01		0xFFFFF618		/* キャプチャ／コンペア・コントロール・レジスタ0(0x00) */
#define	TOC01		0xFFFFF619		/* 16ビット・タイマ出力コントロール・レジスタ0(0x00) */
#define	TM02		0xFFFFF620		/* 16ビット・タイマ・カウンタ0(0x0000) */
#define	CR020		0xFFFFF622		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ02(0x0000) */
#define	CR021		0xFFFFF624		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ02(0x0000) */
#define	TMC02		0xFFFFF626		/* 16ビット・タイマ・モード・コントロール・レジスタ0(0x00) */
#define	PRM02		0xFFFFF627		/* プリスケーラ・モード・レジスタ0(0x00) */
#define	CRC02		0xFFFFF628		/* キャプチャ／コンペア・コントロール・レジスタ0(0x00) */
#define	TOC02		0xFFFFF629		/* 16ビット・タイマ出力コントロール・レジスタ0(0x00) */
#define	TM03		0xFFFFF630		/* 16ビット・タイマ・カウンタ0(0x0000) */
#define	CR030		0xFFFFF632		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ03(0x0000) */
#define	CR031		0xFFFFF634		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ03(0x0000) */
#define	TMC03		0xFFFFF636		/* 16ビット・タイマ・モード・コントロール・レジスタ0(0x00) */
#define	PRM03		0xFFFFF637		/* プリスケーラ・モード・レジスタ0(0x00) */
#define	CRC03		0xFFFFF638		/* キャプチャ／コンペア・コントロール・レジスタ0(0x00) */
#define	TOC03		0xFFFFF639		/* 16ビット・タイマ出力コントロール・レジスタ0(0x00) */
#define	TM04		0xFFFFF640		/* 16ビット・タイマ・カウンタ0(0x0000) */
#define	CR040		0xFFFFF642		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ04(0x0000) */
#define	CR041		0xFFFFF644		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ04(0x0000) */
#define	TMC04		0xFFFFF646		/* 16ビット・タイマ・モード・コントロール・レジスタ0(0x00) */
#define	PRM04		0xFFFFF647		/* プリスケーラ・モード・レジスタ0(0x00) */
#define	CRC04		0xFFFFF648		/* キャプチャ／コンペア・コントロール・レジスタ0(0x00) */
#define	TOC04		0xFFFFF649		/* 16ビット・タイマ出力コントロール・レジスタ0(0x00) */
#define	TM05		0xFFFFF650		/* 16ビット・タイマ・カウンタ0(0x0000) */
#define	CR050		0xFFFFF652		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ05(0x0000) */
#define	CR051		0xFFFFF654		/* 16ビット・タイマ・キャプチャ／コンペア・レジスタ05(0x0000) */
#define	TMC05		0xFFFFF656		/* 16ビット・タイマ・モード・コントロール・レジスタ0(0x00) */
#define	PRM05		0xFFFFF657		/* プリスケーラ・モード・レジスタ0(0x00) */
#define	CRC05		0xFFFFF658		/* キャプチャ／コンペア・コントロール・レジスタ0(0x00) */
#define	TOC05		0xFFFFF659		/* 16ビット・タイマ出力コントロール・レジスタ0(0x00) */
#define	WTM			0xFFFFF680		/* 時計タイマ動作モード・レジスタ(0x00) */
#define	OSTS		0xFFFFF6C0		/* 発振安定時間選択レジスタ(0x01) */
#define	WDCS		0xFFFFF6C1		/* ウォッチドッグ・タイマ・クロック選択レジスタ(0x00) */
#define	WDTM1		0xFFFFF6C2		/* ウォッチドッグ・タイマ・モード・レジスタ(0x00) */
#define	WDTM2		0xFFFFF6D0		/* ウォッチドッグ・タイマ・モード・レジスタ(0x67) */
#define	WDTE		0xFFFFF6D1		/* ウォッチドッグ・タイマ・イネーブル・レジスタ(0x9A) */
#define	RTBL0		0xFFFFF6E0		/* リアルタイム出力バッファ・レジスタL(0x00) */
#define	RTBH0		0xFFFFF6E2		/* リアルタイム出力バッファ・レジスタH(0x00) */
#define	RTPM0		0xFFFFF6E4		/* リアルタイム出力ポート・モード・レジスタ(0x00) */
#define	RTPC0		0xFFFFF6E5		/* リアルタイム出力ポート・コントロール・レジスタ(0x00) */
#define	RTBL1		0xFFFFF6F0		/* リアルタイム出力バッファ・レジスタL(0x00) */
#define	RTBH1		0xFFFFF6F2		/* リアルタイム出力バッファ・レジスタH(0x00) */
#define	RTPM1		0xFFFFF6F4		/* リアルタイム出力ポート・モード・レジスタ(0x00) */
#define	RTPC1		0xFFFFF6F5		/* リアルタイム出力ポート・コントロール・レジスタ(0x00) */
#define	PFCE3		0xFFFFF706		/* ポート3ファンクション・コントロール拡張レジスタ(0x00) */
#define	SYS			0xFFFFF802		/* システム・ステータス・レジスタ(0x00) */
#define	PLLCTL		0xFFFFF806		/* PLLコントロール・レジスタ(0x01) */
#define	PSMR		0xFFFFF820		/* パワー・セーブ・モード・レジスタ(0x00) */
#define	PCC			0xFFFFF828		/* プロセッサ・クロック・コントロール・レジスタ(0x03) */
#define	CORAD0		0xFFFFF840		/* コレクション・アドレス・レジスタ(0x00000000) */
#define	CORAD0L		0xFFFFF840		/* コレクション・アドレス・レジスタ0(0x0000) */
#define	CORAD0H		0xFFFFF842		/* コレクション・アドレス・レジスタ0(0x0000) */
#define	CORAD1		0xFFFFF844		/* コレクション・アドレス・レジスタ(0x00000000) */
#define	CORAD1L		0xFFFFF844		/* コレクション・アドレス・レジスタ1(0x0000) */
#define	CORAD1H		0xFFFFF846		/* コレクション・アドレス・レジスタ1(0x0000) */
#define	CORAD2		0xFFFFF848		/* コレクション・アドレス・レジスタ(0x00000000) */
#define	CORAD2L		0xFFFFF848		/* コレクション・アドレス・レジスタ2(0x0000) */
#define	CORAD2H		0xFFFFF84A		/* コレクション・アドレス・レジスタ2(0x0000) */
#define	CORAD3		0xFFFFF84C		/* コレクション・アドレス・レジスタ(0x00000000) */
#define	CORAD3L		0xFFFFF84C		/* コレクション・アドレス・レジスタ3(0x0000) */
#define	CORAD3H		0xFFFFF84E		/* コレクション・アドレス・レジスタ3(0x0000) */
#define	CORCN		0xFFFFF880		/* コレクション・コントロール・レジスタ(0x00) */
#define	PRSM		0xFFFFF8B0		/* インターバル・タイマBRGモード・レジスタ(0x00) */
#define	PRSCM		0xFFFFF8B1		/* インターバル・タイマBRGコンペア・レジスタ(0x00) */
#define	OCDM		0xFFFFF9FC		/* オンチップ・ディバグ・モード・レジスタ(0x01) */
#define	ASIM0		0xFFFFFA00		/* アシンクロナス・シリアル・インタフェース・モード・レジスタ(0x01) */
#define	RXB0		0xFFFFFA02		/* 受信バッファ・レジスタ(0xFF) */
#define	ASIS0		0xFFFFFA03		/* アシンクロナス・シリアル・インタフェース・ステータス・レジスタ(0x00) */
#define	TXB0		0xFFFFFA04		/* 送信バッファ・レジスタ(0xFF) */
#define	ASIF0		0xFFFFFA05		/* アシンクロナス・シリアル・インタフェース送信ステータス・レジスタ(0x00) */
#define	CKSR0		0xFFFFFA06		/* クロック選択レジスタ(0x00) */
#define	BRGC0		0xFFFFFA07		/* ボー・レート・ジェネレータ・コントロール・レジスタ(0xFF) */
#define	ASIM1		0xFFFFFA10		/* アシンクロナス・シリアル・インタフェース・モード・レジスタ(0x01) */
#define	RXB1		0xFFFFFA12		/* 受信バッファ・レジスタ(0xFF) */
#define	ASIS1		0xFFFFFA13		/* アシンクロナス・シリアル・インタフェース・ステータス・レジスタ(0x00) */
#define	TXB1		0xFFFFFA14		/* 送信バッファ・レジスタ(0xFF) */
#define	ASIF1		0xFFFFFA15		/* アシンクロナス・シリアル・インタフェース送信ステータス・レジスタ(0x00) */
#define	CKSR1		0xFFFFFA16		/* クロック選択レジスタ(0x00) */
#define	BRGC1		0xFFFFFA17		/* ボー・レート・ジェネレータ・コントロール・レジスタ(0xFF) */
#define	ASIM2		0xFFFFFA20		/* アシンクロナス・シリアル・インタフェース・モード・レジスタ(0x01) */
#define	RXB2		0xFFFFFA22		/* 受信バッファ・レジスタ(0xFF) */
#define	ASIS2		0xFFFFFA23		/* アシンクロナス・シリアル・インタフェース・ステータス・レジスタ(0x00) */
#define	TXB2		0xFFFFFA24		/* 送信バッファ・レジスタ(0xFF) */
#define	ASIF2		0xFFFFFA25		/* アシンクロナス・シリアル・インタフェース送信ステータス・レジスタ(0x00) */
#define	CKSR2		0xFFFFFA26		/* クロック選択レジスタ(0x00) */
#define	BRGC2		0xFFFFFA27		/* ボー・レート・ジェネレータ・コントロール・レジスタ(0xFF) */
#define	P0NFC		0xFFFFFB00		/* TIP00ノイズ除去制御レジスタ(0x00) */
#define	P1NFC		0xFFFFFB04		/* TIP01ノイズ除去制御レジスタ(0x00) */
#define	INTF0		0xFFFFFC00		/* 外部割り込み立ち下がりエッジ指定レジスタ(0x00) */
#define	INTF9H		0xFFFFFC13		/* 外部割り込み立ち下がりエッジ指定レジスタ9(0x00) */
#define	INTR0		0xFFFFFC20		/* 外部割り込み立ち上がりエッジ指定レジスタ(0x00) */
#define	INTR9H		0xFFFFFC33		/* 外部割り込み立ち上がりエッジ指定レジスタ9(0x00) */
#define	PU0			0xFFFFFC40		/* プルアップ抵抗オプション・レジスタ(0x00) */
#define	PU1			0xFFFFFC42		/* プルアップ抵抗オプション・レジスタ(0x00) */
#define	PU3			0xFFFFFC46		/* プルアップ抵抗オプション・レジスタ(0x00) */
#define	PU4			0xFFFFFC48		/* プルアップ抵抗オプション・レジスタ(0x00) */
#define	PU5			0xFFFFFC4A		/* プルアップ抵抗オプション・レジスタ(0x00) */
#define	PU6			0xFFFFFC4C		/* プルアップ抵抗オプション・レジスタ(0x0000) */
#define	PU6L		0xFFFFFC4C		/* プルアップ抵抗オプション・レジスタ6(0x00) */
#define	PU6H		0xFFFFFC4D		/* プルアップ抵抗オプション・レジスタ6(0x00) */
#define	PU8			0xFFFFFC50		/* プルアップ抵抗オプション・レジスタ(0x00) */
#define	PU9			0xFFFFFC52		/* プルアップ抵抗オプション・レジスタ(0x0000) */
#define	PU9L		0xFFFFFC52		/* プルアップ抵抗オプション・レジスタ9(0x00) */
#define	PU9H		0xFFFFFC53		/* プルアップ抵抗オプション・レジスタ9(0x00) */
#define	PF3H		0xFFFFFC67		/* ポート3ファンクション・レジスタ(0x00) */
#define	PF4			0xFFFFFC68		/* ポート4ファンクション・レジスタ(0x00) */
#define	PF5			0xFFFFFC6A		/* ポート5ファンクション・レジスタ(0x00) */
#define	PF6			0xFFFFFC6C		/* ポート6ファンクション・レジスタ(0x0000) */
#define	PF6L		0xFFFFFC6C		/* ポート6ファンクション・レジスタ(0x00) */
#define	PF6H		0xFFFFFC6D		/* ポート6ファンクション・レジスタ(0x00) */
#define	PF8			0xFFFFFC70		/* ポート8ファンクション・レジスタ(0x00) */
#define	PF9H		0xFFFFFC73		/* ポート9ファンクション・レジスタ(0x00) */
#define	CSIM00		0xFFFFFD00		/* クロック同期式シリアル・インタフェース・モード・レジスタ0(0x00) */
#define	CSIC0		0xFFFFFD01		/* クロック同期式シリアル・インタフェース・クロック選択レジスタ(0x00) */
#define	SIRB0		0xFFFFFD02		/* クロック同期式シリアル・インタフェース受信バッファ・レジスタ(0x0000) */
#define	SIRB0L		0xFFFFFD02		/* クロック同期式シリアル・インタフェース受信バッファ・レジスタ0(0x00) */
#define	SOTB0		0xFFFFFD04		/* クロック同期式シリアル・インタフェース送信バッファ・レジスタ(0x0000) */
#define	SOTB0L		0xFFFFFD04		/* クロック同期式シリアル・インタフェース送信バッファ・レジスタ0(0x00) */
#define	SIRBE0		0xFFFFFD06		/* クロック同期式シリアル・インタフェース・リード専用受信バッファ・レジスタ(0x0000) */
#define	SIRBE0L		0xFFFFFD06		/* クロック同期式シリアル・インタフェース・リード専用受信バッファ・レジスタ0(0x00) */
#define	SOTBF0		0xFFFFFD08		/* クロック同期式シリアル・インタフェース初段送信バッファ・レジスタ(0x0000) */
#define	SOTBF0L		0xFFFFFD08		/* クロック同期式シリアル・インタフェース初段送信バッファ・レジスタ0(0x00) */
#define	SIO00		0xFFFFFD0A		/* シリアルI/Oシフト・レジスタ(0x00) */
#define	SIO00L		0xFFFFFD0A		/* シリアルI/Oシフト・レジスタ0(0x0000) */
#define	CSIM01		0xFFFFFD10		/* クロック同期式シリアル・インタフェース・モード・レジスタ0(0x00) */
#define	CSIC1		0xFFFFFD11		/* クロック同期式シリアル・インタフェース・クロック選択レジスタ(0x00) */
#define	SIRB1		0xFFFFFD12		/* クロック同期式シリアル・インタフェース受信バッファ・レジスタ(0x0000) */
#define	SIRB1L		0xFFFFFD12		/* クロック同期式シリアル・インタフェース受信バッファ・レジスタ1(0x00) */
#define	SOTB1		0xFFFFFD14		/* クロック同期式シリアル・インタフェース送信バッファ・レジスタ(0x0000) */
#define	SOTB1L		0xFFFFFD14		/* クロック同期式シリアル・インタフェース送信バッファ・レジスタ1(0x00) */
#define	SIRBE1		0xFFFFFD16		/* クロック同期式シリアル・インタフェース・リード専用受信バッファ・レジスタ(0x0000) */
#define	SIRBE1L		0xFFFFFD16		/* クロック同期式シリアル・インタフェース・リード専用受信バッファ・レジスタ1(0x00) */
#define	SOTBF1		0xFFFFFD18		/* クロック同期式シリアル・インタフェース初段送信バッファ・レジスタ(0x0000) */
#define	SOTBF1L		0xFFFFFD18		/* クロック同期式シリアル・インタフェース初段送信バッファ・レジスタ1(0x00) */
#define	SIO01		0xFFFFFD1A		/* シリアルI/Oシフト・レジスタ(0x00) */
#define	SIO01L		0xFFFFFD1A		/* シリアルI/Oシフト・レジスタ1(0x0000) */
#define	CSIM02		0xFFFFFD20		/* クロック同期式シリアル・インタフェース・モード・レジスタ0(0x00) */
#define	CSIC2		0xFFFFFD21		/* クロック同期式シリアル・インタフェース・クロック選択レジスタ(0x00) */
#define	SIRB2		0xFFFFFD22		/* クロック同期式シリアル・インタフェース受信バッファ・レジスタ(0x0000) */
#define	SIRB2L		0xFFFFFD22		/* クロック同期式シリアル・インタフェース受信バッファ・レジスタ2(0x00) */
#define	SOTB2		0xFFFFFD24		/* クロック同期式シリアル・インタフェース送信バッファ・レジスタ(0x0000) */
#define	SOTB2L		0xFFFFFD24		/* クロック同期式シリアル・インタフェース送信バッファ・レジスタ2(0x00) */
#define	SIRBE2		0xFFFFFD26		/* クロック同期式シリアル・インタフェース・リード専用受信バッファ・レジスタ(0x0000) */
#define	SIRBE2L		0xFFFFFD26		/* クロック同期式シリアル・インタフェース・リード専用受信バッファ・レジスタ2(0x00) */
#define	SOTBF2		0xFFFFFD28		/* クロック同期式シリアル・インタフェース初段送信バッファ・レジスタ(0x0000) */
#define	SOTBF2L		0xFFFFFD28		/* クロック同期式シリアル・インタフェース初段送信バッファ・レジスタ2(0x00) */
#define	SIO02		0xFFFFFD2A		/* シリアルI/Oシフト・レジスタ(0x00) */
#define	SIO02L		0xFFFFFD2A		/* シリアルI/Oシフト・レジスタ2(0x0000) */
#define	CSIMA0		0xFFFFFD40		/* シリアル動作モード指定レジスタ(0x00) */
#define	CSIS0		0xFFFFFD41		/* シリアル・ステータス・レジスタ(0x00) */
#define	CSIT0		0xFFFFFD42		/* シリアル・トリガ・レジスタ(0x00) */
#define	BRGCA0		0xFFFFFD43		/* 分周値選択レジスタ(0x03) */
#define	ADTP0		0xFFFFFD44		/* 自動データ転送アドレス・ポイント指定レジスタ(0x00) */
#define	ADTI0		0xFFFFFD45		/* 自動データ転送間隔指定レジスタ(0x00) */
#define	SIOA0		0xFFFFFD46		/* シリアルI/Oシフト・レジスタA(0x00) */
#define	ADTC0		0xFFFFFD47		/* 自動データ転送アドレス・カウント・レジスタ(0x00) */
#define	CSIMA1		0xFFFFFD50		/* シリアル動作モード指定レジスタ(0x00) */
#define	CSIS1		0xFFFFFD51		/* シリアル・ステータス・レジスタ(0x00) */
#define	CSIT1		0xFFFFFD52		/* シリアル・トリガ・レジスタ(0x00) */
#define	BRGCA1		0xFFFFFD53		/* 分周値選択レジスタ(0x03) */
#define	ADTP1		0xFFFFFD54		/* 自動データ転送アドレス・ポイント指定レジスタ(0x00) */
#define	ADTI1		0xFFFFFD55		/* 自動データ転送間隔指定レジスタ(0x00) */
#define	SIOA1		0xFFFFFD56		/* シリアルI/Oシフト・レジスタA(0x00) */
#define	ADTC1		0xFFFFFD57		/* 自動データ転送アドレス・カウント・レジスタ(0x00) */
#define	IIC0		0xFFFFFD80		/* IICシフト・レジスタ(0x00) */
#define	IICC0		0xFFFFFD82		/* IICコントロール・レジスタ(0x00) */
#define	SVA0		0xFFFFFD83		/* スレーブ・アドレス・レジスタ(0x00) */
#define	IICCL0		0xFFFFFD84		/* IICクロック選択レジスタ(0x00) */
#define	IICX0		0xFFFFFD85		/* IIC機能拡張レジスタ(0x00) */
#define	IICS0		0xFFFFFD86		/* IIC状態レジスタ(0x00) */
#define	IICF0		0xFFFFFD8A		/* IICフラグ・レジスタ(0x00) */
#define	IIC1		0xFFFFFD90		/* IICシフト・レジスタ(0x00) */
#define	IICC01		0xFFFFFD92		/* IICコントロール・レジスタ(0x00) */
#define	SVA01		0xFFFFFD93		/* スレーブ・アドレス・レジスタ(0x00) */
#define	IICCL01		0xFFFFFD94		/* IICクロック選択レジスタ(0x00) */
#define	IICX1		0xFFFFFD95		/* IIC機能拡張レジスタ(0x00) */
#define	IICS01		0xFFFFFD96		/* IIC状態レジスタ(0x00) */
#define	IICF1		0xFFFFFD9A		/* IICフラグ・レジスタ(0x00) */
#define	CSIA0B0		0xFFFFFE00		/* CSIA0バッファRAM0(不定) */
#define	CSIA0B0L	0xFFFFFE00		/* CSIA0バッファRAM0L(不定) */
#define	CSIA0B0H	0xFFFFFE01		/* CSIA0バッファRAM0H(不定) */
#define	CSIA0B1		0xFFFFFE02		/* CSIA0バッファRAM1(不定) */
#define	CSIA0B1L	0xFFFFFE02		/* CSIA0バッファRAM1L(不定) */
#define	CSIA0B1H	0xFFFFFE03		/* CSIA0バッファRAM1H(不定) */
#define	CSIA0B2		0xFFFFFE04		/* CSIA0バッファRAM2(不定) */
#define	CSIA0B2L	0xFFFFFE04		/* CSIA0バッファRAM2L(不定) */
#define	CSIA0B2H	0xFFFFFE05		/* CSIA0バッファRAM2H(不定) */
#define	CSIA0B3		0xFFFFFE06		/* CSIA0バッファRAM3(不定) */
#define	CSIA0B3L	0xFFFFFE06		/* CSIA0バッファRAM3L(不定) */
#define	CSIA0B3H	0xFFFFFE07		/* CSIA0バッファRAM3H(不定) */
#define	CSIA0B4		0xFFFFFE08		/* CSIA0バッファRAM4(不定) */
#define	CSIA0B4L	0xFFFFFE08		/* CSIA0バッファRAM4L(不定) */
#define	CSIA0B4H	0xFFFFFE09		/* CSIA0バッファRAM4H(不定) */
#define	CSIA0B5		0xFFFFFE0A		/* CSIA0バッファRAM5(不定) */
#define	CSIA0B5L	0xFFFFFE0A		/* CSIA0バッファRAM5L(不定) */
#define	CSIA0B5H	0xFFFFFE0B		/* CSIA0バッファRAM5H(不定) */
#define	CSIA0B6		0xFFFFFE0C		/* CSIA0バッファRAM6(不定) */
#define	CSIA0B6L	0xFFFFFE0C		/* CSIA0バッファRAM6L(不定) */
#define	CSIA0B6H	0xFFFFFE0D		/* CSIA0バッファRAM6H(不定) */
#define	CSIA0B7		0xFFFFFE0E		/* CSIA0バッファRAM7(不定) */
#define	CSIA0B7L	0xFFFFFE0E		/* CSIA0バッファRAM7L(不定) */
#define	CSIA0B7H	0xFFFFFE0F		/* CSIA0バッファRAM7H(不定) */
#define	CSIA0B8		0xFFFFFE10		/* CSIA0バッファRAM8(不定) */
#define	CSIA0B8L	0xFFFFFE10		/* CSIA0バッファRAM8L(不定) */
#define	CSIA0B8H	0xFFFFFE11		/* CSIA0バッファRAM8H(不定) */
#define	CSIA0B9		0xFFFFFE12		/* CSIA0バッファRAM9(不定) */
#define	CSIA0B9L	0xFFFFFE12		/* CSIA0バッファRAM9L(不定) */
#define	CSIA0B9H	0xFFFFFE13		/* CSIA0バッファRAM9H(不定) */
#define	CSIA0BA		0xFFFFFE14		/* CSIA0バッファRAMA(不定) */
#define	CSIA0BAL	0xFFFFFE14		/* CSIA0バッファRAMAL(不定) */
#define	CSIA0BAH	0xFFFFFE15		/* CSIA0バッファRAMAH(不定) */
#define	CSIA0BB		0xFFFFFE16		/* CSIA0バッファRAMB(不定) */
#define	CSIA0BBL	0xFFFFFE16		/* CSIA0バッファRAMBL(不定) */
#define	CSIA0BBH	0xFFFFFE17		/* CSIA0バッファRAMBH(不定) */
#define	CSIA0BC		0xFFFFFE18		/* CSIA0バッファRAMC(不定) */
#define	CSIA0BCL	0xFFFFFE18		/* CSIA0バッファRAMCL(不定) */
#define	CSIA0BCH	0xFFFFFE19		/* CSIA0バッファRAMCH(不定) */
#define	CSIA0BD		0xFFFFFE1A		/* CSIA0バッファRAMD(不定) */
#define	CSIA0BDL	0xFFFFFE1A		/* CSIA0バッファRAMDL(不定) */
#define	CSIA0BDH	0xFFFFFE1B		/* CSIA0バッファRAMDH(不定) */
#define	CSIA0BE		0xFFFFFE1C		/* CSIA0バッファRAME(不定) */
#define	CSIA0BEL	0xFFFFFE1C		/* CSIA0バッファRAMEL(不定) */
#define	CSIA0BEH	0xFFFFFE1D		/* CSIA0バッファRAMEH(不定) */
#define	CSIA0BF		0xFFFFFE1E		/* CSIA0バッファRAMF(不定) */
#define	CSIA0BFL	0xFFFFFE1E		/* CSIA0バッファRAMFL(不定) */
#define	CSIA0BFH	0xFFFFFE1F		/* CSIA0バッファRAMFH(不定) */
#define	CSIA1B0		0xFFFFFE20		/* CSIA1バッファRAM0(不定) */
#define	CSIA1B0L	0xFFFFFE20		/* CSIA1バッファRAM0L(不定) */
#define	CSIA1B0H	0xFFFFFE21		/* CSIA1バッファRAM0H(不定) */
#define	CSIA1B1		0xFFFFFE22		/* CSIA1バッファRAM1(不定) */
#define	CSIA1B1L	0xFFFFFE22		/* CSIA1バッファRAM1L(不定) */
#define	CSIA1B1H	0xFFFFFE23		/* CSIA1バッファRAM1H(不定) */
#define	CSIA1B2		0xFFFFFE24		/* CSIA1バッファRAM2(不定) */
#define	CSIA1B2L	0xFFFFFE24		/* CSIA1バッファRAM2L(不定) */
#define	CSIA1B2H	0xFFFFFE25		/* CSIA1バッファRAM2H(不定) */
#define	CSIA1B3		0xFFFFFE26		/* CSIA1バッファRAM3(不定) */
#define	CSIA1B3L	0xFFFFFE26		/* CSIA1バッファRAM3L(不定) */
#define	CSIA1B3H	0xFFFFFE27		/* CSIA1バッファRAM3H(不定) */
#define	CSIA1B4		0xFFFFFE28		/* CSIA1バッファRAM4(不定) */
#define	CSIA1B4L	0xFFFFFE28		/* CSIA1バッファRAM4L(不定) */
#define	CSIA1B4H	0xFFFFFE29		/* CSIA1バッファRAM4H(不定) */
#define	CSIA1B5		0xFFFFFE2A		/* CSIA1バッファRAM5(不定) */
#define	CSIA1B5L	0xFFFFFE2A		/* CSIA1バッファRAM5L(不定) */
#define	CSIA1B5H	0xFFFFFE2B		/* CSIA1バッファRAM5H(不定) */
#define	CSIA1B6		0xFFFFFE2C		/* CSIA1バッファRAM6(不定) */
#define	CSIA1B6L	0xFFFFFE2C		/* CSIA1バッファRAM6L(不定) */
#define	CSIA1B6H	0xFFFFFE2D		/* CSIA1バッファRAM6H(不定) */
#define	CSIA1B7		0xFFFFFE2E		/* CSIA1バッファRAM7(不定) */
#define	CSIA1B7L	0xFFFFFE2E		/* CSIA1バッファRAM7L(不定) */
#define	CSIA1B7H	0xFFFFFE2F		/* CSIA1バッファRAM7H(不定) */
#define	CSIA1B8		0xFFFFFE30		/* CSIA1バッファRAM8(不定) */
#define	CSIA1B8L	0xFFFFFE30		/* CSIA1バッファRAM8L(不定) */
#define	CSIA1B8H	0xFFFFFE31		/* CSIA1バッファRAM8H(不定) */
#define	CSIA1B9		0xFFFFFE32		/* CSIA1バッファRAM9(不定) */
#define	CSIA1B9L	0xFFFFFE32		/* CSIA1バッファRAM9L(不定) */
#define	CSIA1B9H	0xFFFFFE33		/* CSIA1バッファRAM9H(不定) */
#define	CSIA1BA		0xFFFFFE34		/* CSIA1バッファRAMA(不定) */
#define	CSIA1BAL	0xFFFFFE34		/* CSIA1バッファRAMAL(不定) */
#define	CSIA1BAH	0xFFFFFE35		/* CSIA1バッファRAMAH(不定) */
#define	CSIA1BB		0xFFFFFE36		/* CSIA1バッファRAMB(不定) */
#define	CSIA1BBL	0xFFFFFE36		/* CSIA1バッファRAMBL(不定) */
#define	CSIA1BBH	0xFFFFFE37		/* CSIA1バッファRAMBH(不定) */
#define	CSIA1BC		0xFFFFFE38		/* CSIA1バッファRAMC(不定) */
#define	CSIA1BCL	0xFFFFFE38		/* CSIA1バッファRAMCL(不定) */
#define	CSIA1BCH	0xFFFFFE39		/* CSIA1バッファRAMCH(不定) */
#define	CSIA1BD		0xFFFFFE3A		/* CSIA1バッファRAMD(不定) */
#define	CSIA1BDL	0xFFFFFE3A		/* CSIA1バッファRAMDL(不定) */
#define	CSIA1BDH	0xFFFFFE3B		/* CSIA1バッファRAMDH(不定) */
#define	CSIA1BE		0xFFFFFE3C		/* CSIA1バッファRAME(不定) */
#define	CSIA1BEL	0xFFFFFE3C		/* CSIA1バッファRAMEL(不定) */
#define	CSIA1BEH	0xFFFFFE3D		/* CSIA1バッファRAMEH(不定) */
#define	CSIA1BF		0xFFFFFE3E		/* CSIA1バッファRAMF(不定) */
#define	CSIA1BFL	0xFFFFFE3E		/* CSIA1バッファRAMFL(不定) */
#define	CSIA1BFH	0xFFFFFE3F		/* CSIA1バッファRAMFH(不定) */
#define	EXIMC		0xFFFFFFBE		/* 外部バス・インタフェース・モード・コントロール・レジスタ(0x00) */

#endif	/* _V850ES_KJ1_H_ */
