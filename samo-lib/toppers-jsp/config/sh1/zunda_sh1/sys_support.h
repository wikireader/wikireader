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
#ifndef _SYS_SUPPORT_H_
#define _SYS_SUPPORT_H_

/*
 *  ポートＡ・拡張Ｉ／Ｏへ開放端子の定義
 *  すべてプルアップあり
 */
/*
 *  PA15 選択する機能を 1 にセットしてください
 */
#define	PA15_PORT_IN	1	/* 汎用入力ポート		PI15	*/
#define	PA15_PORT_OUT	0	/* 汎用出力ポート		PO15	*/
#define	PA15_IRQ3	0	/* 割り込み要求入力		IRQ3	*/
#define	PA15_DREQ1	0	/* ＤＭＡ転送要求入力		DREQ1	*/

/*
 *  PA14 選択する機能を 1 にセットしてください
 */
#define	PA14_PORT_IN	1	/* 汎用入力ポート		PI14	*/
#define	PA14_PORT_OUT	0	/* 汎用出力ポート		PO14	*/
#define	PA14_IRQ2	0	/* 割り込み要求入力 		IRQ2	*/
#define	PA14_DACK1	0	/* ＤＭＡ転送要求受付出力	DACK1	*/

/*
 *  PA13 選択する機能を 1 にセットしてください
 */
#define	PA13_PORT_IN	1	/* 汎用入力ポート		PI13	*/
#define	PA13_PORT_OUT	0	/* 汎用出力ポート		PO13	*/
#define	PA13_IRQ1	0	/* 割り込み要求入力 		IRQ1	*/
#define	PA13_TCLKB	0	/* ＩＴＵタイマクロック入力	TCLKB	*/
#define	PA13_DREQ0	0	/* ＤＭＡ転送要求入力		DREQ0	*/

/*
 *  PA12 選択する機能を 1 にセットしてください
 */
#define	PA12_PORT_IN	1	/* 汎用入力ポート		PI12	*/
#define	PA12_PORT_OUT	0	/* 汎用出力ポート		PO12	*/
#define	PA12_IRQ0	0	/* 割り込み要求入力 		IRQ0	*/
#define	PA13_TCLKA	0	/* ＩＴＵタイマクロック入力	TCLKA	*/
#define	PA13_DACK0	0	/* ＤＭＡ転送要求受付出力	DACK0	*/

/*
 *  PA03 選択する機能を 1 にセットしてください
 */
#define	PA03_PORT_IN	1	/* 汎用入力ポート		PI03	*/
#define	PA03_PORT_OUT	0	/* 汎用出力ポート		PO03	*/
#define	PA03_CS7	0	/* チップセレクト７出力		CS7	*/
#define	PA03_WAIT	0	/* ウエイトステート入力		WAIT	*/

/*
 *  PA01 選択する機能を 1 にセットしてください
 */
#define	PA01_PORT_IN	1	/* 汎用入力ポート		PI01	*/
#define	PA01_PORT_OUT	0	/* 汎用出力ポート		PO01	*/
#define	PA01_CS5	0	/* チップセレクト５出力		CS5	*/
#define	PA01_RAS	0	/* ロウアドレスストローブ出力	RAS	*/

/*####################################################
 *
 *  以下編集しないこと
 *
 *####################################################*/
/*
 *  ＰＡ、ＰＢ出力バッファ初期値
 *  下のピンファンクション定義を参照してください
 */
#define	VAL_PA_DR 	(1<<10 | 1<<9 | 1<<8)	/* RTC-4553-CS=1,RTC-4553-WR=1,LEDCTL1=1(off) */
#define	VAL_PB_DR 	(1<<11 | 1<<9 | 1<<6)	/* TXD1=1,TXD0=1,LEDCTL2=1(off) */

/*
 *  PA 固定機能（PA15,14,13,12,03,01を除く）
 *
 *  PNO  I/O PAIOR  PACR1/2
 *  PA15 IN   [0]    [00] − 拡張Ｉ／Ｏに開放
 *  PA14 IN   [0]    [00] − 拡張Ｉ／Ｏに開放
 *  PA13 IN   [0]    [00] − 拡張Ｉ／Ｏに開放
 *  PA12 IN   [0]    [00] − 拡張Ｉ／Ｏに開放
 *  PA11 IN   [0]    [00] − RTC-4553-SOUT
 *  PA10 OUT  [1]    [00] − RTC-4553-CS[active-low]
 *  PA09 OUT  [1]    [00] − RTC-4553-WR[active-low]
 *  PA08 OUT  [1]    [10] − LEDCTL1 LED1制御[active-low]
 *  PA07 IN   [0]    [10] − 未使用
 *  PA06 −   [0]    [11] − RD に割り当て−
 *  PA05 OUT  [1]    [10] − WD-PULSE ウォッチドックリセットパルス↑↓＿にて
 *  PA04 −   [0]    [11] − WR に割り当て
 *  PA03 −   [0]    [00] − 拡張Ｉ／Ｏに開放
 *  PA02 −   [0]    [01] − CS6 に割り当て（ＬＡＮコントローラ用）
 *  PA01 −   [0]    [00] − 拡張Ｉ／Ｏに開放
 *  PA00 −   [0]    [01] − CS4 に割り当て（ROMICE 用）
 */
#define	PAIOR_BASE	0x0720
#define	PACR1_BASE	0x0002
#define	PACR2_BASE	0xbb11

#if PA15_PORT_IN == 1
#define	PA15MD			0	/* 汎用入力ポート		PI15	*/
#define	PA15IOR			0
#endif
#if PA15_PORT_OUT == 1
#ifdef PA15MD
#error : 'PA15MD' redefined.
#endif
#define	PA15MD			0	/* 汎用出力ポート		PO15	*/
#define	PA15IOR			1
#endif
#if PA15_IRQ3 == 1
#ifdef PA15MD
#error : 'PA15MD' redefined.
#endif
#define	PA15MD			1	/* 割り込み要求入力		IRQ3	*/
#define	PA15IOR			0
#endif
#if PA15_DREQ1 == 1
#ifdef PA15MD
#error : 'PA15MD' redefined.
#endif
#define	PA15MD			3	/* ＤＭＡ転送要求入力		DREQ1	*/
#define	PA15IOR			0
#endif
#ifndef	PA15MD
#error : 'PA15MD' is not set up.
#endif

#if PA14_PORT_IN == 1
#define	PA14MD			0	/* 汎用入力ポート		PI14	*/
#define	PA14IOR			0
#endif
#if PA14_PORT_OUT == 1
#ifdef PA14MD
#error : 'PA14MD' redefined.
#endif
#define	PA14MD			0	/* 汎用出力ポート		PO14	*/
#define	PA14IOR			1
#endif
#if PA14_IRQ2 == 1
#ifdef PA14MD
#error : 'PA14MD' redefined.
#endif
#define	PA14MD			1	/* 割り込み要求入力		IRQ2	*/
#define	PA14IOR			0
#endif
#if PA14_DACK1 == 1
#ifdef PA14MD
#error : 'PA14MD' redefined.
#endif
#define	PA14MD			3	/* ＤＭＡ転送要求受付出力	DACK1	*/
#define	PA14IOR			0
#endif
#ifndef PA14MD
#error : 'PA14MD' is not set up.
#endif

#if PA13_PORT_IN == 1
#define	PA13MD			0	/* 汎用入力ポート		PI13	*/
#define	PA13IOR			0
#endif
#if PA13_PORT_OUT == 1
#ifdef PA13MD
#error : 'PA13MD' redefined.
#endif
#define	PA13MD			0	/* 汎用出力ポート		PO13	*/
#define	PA13IOR			1
#endif
#if PA13_IRQ1 == 1
#ifdef PA13MD
#error : 'PA13MD' redefined.
#endif
#define	PA13MD			1	/* 割り込み要求入力		IRQ1	*/
#define	PA13IOR			0
#endif
#if PA13_TCLKB == 1
#ifdef PA13MD
#error : 'PA13MD' redefined.
#endif
#define	PA13MD			2	/* ＩＴＵタイマクロック入力	TCLKB	*/
#define	PA13IOR			0
#endif
#if PA13_DREQ0 == 1
#ifdef PA13MD
#error : 'PA13MD' redefined.
#endif
#define	PA13MD			3	/* ＤＭＡ転送要求入力		DREQ0	*/
#define	PA13IOR			0
#endif
#ifndef PA13MD
#error : 'PA13MD' is not set up.
#endif

#if PA12_PORT_IN == 1
#define	PA12MD			0	/* 汎用入力ポート		PI12	*/
#define	PA12IOR			0
#endif
#if PA12_PORT_OUT == 1
#ifdef PA12MD
#error : 'PA12MD' redefined.
#endif
#define	PA12MD			0	/* 汎用出力ポート		PO12	*/
#define	PA12IOR			1
#endif
#if PA12_IRQ0 == 1
#ifdef PA12MD
#error : 'PA12MD' redefined.
#endif
#define	PA12MD			1	/* 割り込み要求入力		IRQ0	*/
#define	PA12IOR			0
#endif
#if PA13_TCLKA == 1
#ifdef PA12MD
#error : 'PA12MD' redefined.
#endif
#define	PA12MD			2	/* ＩＴＵタイマクロック入力	TCLKA	*/
#define	PA12IOR			0
#endif
#if PA13_DACK0 == 1
#ifdef PA12MD
#error : 'PA12MD' redefined.
#endif
#define	PA12MD			3	/* ＤＭＡ転送要求受付出力	DACK0	*/
#define	PA12IOR			0
#endif
#ifndef PA12MD
#error : 'PA12MD' is not set up.
#endif

#if PA03_PORT_IN == 1
#define	PA03MD			0	/* 汎用入力ポート		PI03	*/
#define	PA03IOR			0
#endif
#if PA03_PORT_OUT == 1
#ifdef PA03MD
#error : 'PA03MD' redefined.
#endif
#define	PA03MD			0	/* 汎用出力ポート		PO03	*/
#define	PA03IOR			1
#endif
#if PA03_CS7 == 1
#ifdef PA03MD
#error : 'PA03MD' redefined.
#endif
#define	PA03MD			1	/* チップセレクト７出力		CS7	*/
#define	PA03IOR			0
#endif
#if PA03_WAIT == 1
#ifdef PA03MD
#error : 'PA03MD' redefined.
#endif
#define	PA03MD			2	/* ウエイトステート入力		WAIT	*/
#define	PA03IOR			0
#endif
#ifndef PA03MD
#error : 'PA03MD' is not set up.
#endif

#if PA01_PORT_IN == 1
#define	PA01MD			0	/* 汎用入力ポート		PI01	*/
#define	PA01IOR			0
#endif
#if PA01_PORT_OUT == 1
#ifdef PA01MD
#error : 'PA01MD' redefined.
#endif
#define	PA01MD			0	/* 汎用出力ポート		PO01	*/
#define	PA01IOR			1
#endif
#if PA01_CS5 == 1
#ifdef PA01MD
#error : 'PA01MD' redefined.
#endif
#define	PA01MD			1	/* チップセレクト５出力		CS5	*/
#define	PA01IOR			0
#endif
#if PA01_RAS == 1
#ifdef PA01MD
#error : 'PA01MD' redefined.
#endif
#define	PA01MD			2	/* ロウアドレスストローブ出力	RAS	*/
#define	PA01IOR			0
#endif
#ifndef PA01MD
#error : 'PA01MD' is not set up.
#endif

/*
 *  ポートＡピンファンクション定義
 */
#define VAL_PFC_PAIOR   (PAIOR_BASE | PA15IOR<<15 | PA14IOR<<14 | PA13IOR<<13 | PA12IOR<<12 | PA03IOR<<3 | PA01IOR<<1)
#define VAL_PFC_PACR1   (PACR1_BASE | PA15MD<<14  | PA14MD<<12  | PA13MD<<10  | PA12MD<<8)
#define VAL_PFC_PACR2   (PACR2_BASE | PA03MD<<6   | PA01MD<<2)

/*
 *  ポートＢピンファンクション定義
 */
#define VAL_PFC_PBIOR   0x107b
#define VAL_PFC_PBCR1   0x14aa
#define VAL_PFC_PBCR2   0x0000
 /*
 *  PNO  I/O PBIOR  PBCR1/2
 *  PB15 IN   [0]    [00] − 未使用 IRQ7 に割り当て（ＲＴＣ用）
 *  PB14 −   [0]    [01] − IRQ6 に割り当て（ＵＳＢコントローラ用）
 *  PB13 −   [0]    [01] − IRQ5 に割り当て（ＬＡＮコントローラ用）
 *  PB12 OUT  [1]    [00] − LEDCTL3 LED3 制御[active-low]
 *  PB11 −   [0]    [10] − TDX1 に割り当て（RS485 データ出力）
 *  PB10 −   [0]    [10] − RDX1 に割り当て（RS485 データ入力）
 *  PB09 −   [0]    [10] − TDX0 に割り当て（RS-232C データ出力）
 *  PB08 −   [0]    [10] − RDX0 に割り当て（RS-232C データ入力）
 *  PB07 IN   [0]    [00] − シリアルEEPROM-DO
 *  PB06 OUT  [1]    [00] − LEDCTL2 LED2 制御[active-low]
 *  PB05 OUT  [1]    [00] − TXE0 RS485 送信イネーブル出力送信停止
 *  PB04 OUT  [1]    [00] − シリアルEEPROM-CS[active-high]
 *  PB03 OUT  [1]    [00] − CTS0 RS232C のCTS 信号（未実装）
 *  PB02 IN   [0]    [00] − RTS0 RS232C のRTS 信号（未実装）
 *  PB01 OUT  [1]    [00] − シリアルEEPROM-SK、RTC-4553-SCK
 *  PB00 OUT  [1]    [00] − シリアルEEPROM-DI、RTC-4553-SIN
 *
 *  PBIOR=0x107b
 *  PBCR1=0x14aa
 *  PBCR2=0x0000
 *
 */

/*
 *  カラムアドレスストローブピンコントロール定義
 */
#define VAL_PFC_CASCR   0x5fff

#ifndef _MACRO_ONLY
/*
 * 関数宣言
 */
void	software_init_hook(void);
void	hardware_init_hook(void);

#endif /* _MACRO_ONLY */
#endif	/* _SYS_SUPPORT_H_ */


