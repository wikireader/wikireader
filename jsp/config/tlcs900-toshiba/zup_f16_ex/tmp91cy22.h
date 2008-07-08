/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2006 by Witz Corporation, JAPAN
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
 *  @(#) $Id: tmp91cy22.h,v 1.1 2006/04/10 08:19:25 honda Exp $
 */

/*
 *  特殊機能レジスタ関連の定義（TMP91CY22用）
 */

#ifndef _TMP91CY22_H_
#define _TMP91CY22_H_

/*
 *  割込み番号の定義
 */
#define		INT_NO_RST_SW0		1	/* リセット/SWI 0 命令				*/
#define		INT_NO_SW1			2	/* SWI 1 命令						*/
#define		INT_NO_UNDEF_SW2	3	/* 未定義命令実行違反/SWI 2 命令	*/
#define		INT_NO_SW3			4	/* SWI 3 命令						*/
#define		INT_NO_SW4			5	/* SWI 4 命令						*/
#define		INT_NO_SW5			6	/* SWI 5 命令						*/
#define		INT_NO_SW6			7	/* SWI 6 命令						*/
#define		INT_NO_SW7			8	/* SWI 7 命令						*/
#define		INT_NO_NMI			9	/* NMI								*/
#define		INT_NO_WD			10	/* ウォッチドッグタイマ				*/
#define		INT_NO_0			11	/* INT0								*/
#define		INT_NO_1			12	/* INT1								*/
#define		INT_NO_2			13	/* INT2								*/
#define		INT_NO_3			14	/* INT3								*/
#define		INT_NO_4			15	/* INT4								*/
#define		INT_NO_5			16	/* INT5								*/
#define		INT_NO_6			17	/* INT6								*/
#define		INT_NO_7			18	/* INT7								*/
#define		INT_NO_8			19	/* INT8								*/
#define		INT_NO_TA0			20	/* 8ビットタイマ0					*/
#define		INT_NO_TA1			21	/* 8ビットタイマ1					*/
#define		INT_NO_TA2			22	/* 8ビットタイマ2					*/
#define		INT_NO_TA3			23	/* 8ビットタイマ3					*/
#define		INT_NO_TA4			24	/* 8ビットタイマ4					*/
#define		INT_NO_TA5			25	/* 8ビットタイマ5					*/
#define		INT_NO_TA6			26	/* 8ビットタイマ6					*/
#define		INT_NO_TA7			27	/* 8ビットタイマ7					*/
#define		INT_NO_TB00			28	/* 16ビットタイマ00					*/
#define		INT_NO_TB01			29	/* 16ビットタイマ01					*/
#define		INT_NO_TB10			30	/* 16ビットタイマ10					*/
#define		INT_NO_TB11			31	/* 16ビットタイマ11					*/
#define		INT_NO_TBOF0		32	/* 16ビットタイマ0オーバーフロー	*/
#define		INT_NO_TBOF1		33	/* 16ビットタイマ1オーバーフロー	*/
#define		INT_NO_RX0			34	/* シリアル受信0					*/
#define		INT_NO_TX0			35	/* シリアル送信0					*/
#define		INT_NO_RX1			36	/* シリアル受信1					*/
#define		INT_NO_TX1			37	/* シリアル送信1					*/
#define		INT_NO_SBI			38	/* シリアルバスインタフェース		*/
#define		INT_NO_RTC			39	/* 時計用タイマ						*/
#define		INT_NO_AD			40	/* AD変換終了						*/
#define		INT_NO_TC0			41	/* マイクロDMA終了0					*/
#define		INT_NO_TC1			42	/* マイクロDMA終了1					*/
#define		INT_NO_TC2			43	/* マイクロDMA終了2					*/
#define		INT_NO_TC3			44	/* マイクロDMA終了3					*/

/*
 *  SFRアドレス定義
 */
/*
 *  ポートレジスタ
 */
#define		TADR_SFR_P0				0x0000
#define		TADR_SFR_P1				0x0001
#define		TADR_SFR_P0CR			0x0002
#define		TADR_SFR_P1CR			0x0004
#define		TADR_SFR_P1FC			0x0005
#define		TADR_SFR_P2				0x0006
#define		TADR_SFR_P3				0x0007
#define		TADR_SFR_P2CR			0x0008
#define		TADR_SFR_P2FC			0x0009
#define		TADR_SFR_P3CR			0x000a
#define		TADR_SFR_P3FC			0x000b
#define		TADR_SFR_P4				0x000c
#define		TADR_SFR_P5				0x000d
#define		TADR_SFR_P4CR			0x000e
#define		TADR_SFR_P4FC			0x000f
#define		TADR_SFR_P6				0x0012
#define		TADR_SFR_P7				0x0013
#define		TADR_SFR_P6CR			0x0014
#define		TADR_SFR_P6FC			0x0015
#define		TADR_SFR_P7CR			0x0016
#define		TADR_SFR_P7FC			0x0017
#define		TADR_SFR_P8				0x0018
#define		TADR_SFR_P9				0x0019
#define		TADR_SFR_P8CR			0x001a
#define		TADR_SFR_P8FC			0x001b
#define		TADR_SFR_P9CR			0x001c
#define		TADR_SFR_P9FC			0x001d
#define		TADR_SFR_PA				0x001e
#define		TADR_SFR_PACR			0x0020
#define		TADR_SFR_PAFC			0x0021
#define		TADR_SFR_ODE			0x002f

/*
 *  割込みレジスタ
 */
#define		TADR_SFR_DMA0V			0x0080
#define		TADR_SFR_DMA1V			0x0081
#define		TADR_SFR_DMA2V			0x0082
#define		TADR_SFR_DMA3V			0x0083
#define		TADR_SFR_INTCLR			0x0088
#define		TADR_SFR_DMAR			0x0089
#define		TADR_SFR_DMAB			0x008a
#define		TADR_SFR_IIMC			0x008c
#define		TADR_SFR_INTE0AD		0x0090
#define		TADR_SFR_INTE12			0x0091
#define		TADR_SFR_INTE34			0x0092
#define		TADR_SFR_INTE56			0x0093
#define		TADR_SFR_INTE78			0x0094
#define		TADR_SFR_INTETA01		0x0095
#define		TADR_SFR_INTETA23		0x0096
#define		TADR_SFR_INTETA45		0x0097
#define		TADR_SFR_INTETA67		0x0098
#define		TADR_SFR_INTETB0		0x0099
#define		TADR_SFR_INTETB1		0x009a
#define		TADR_SFR_INTETB01V		0x009b
#define		TADR_SFR_INTES0			0x009c
#define		TADR_SFR_INTES1			0x009d
#define		TADR_SFR_INTSBIRTC		0x009e
#define		TADR_SFR_INTETC01		0x00a0
#define		TADR_SFR_INTETC23		0x00a1

/*
 *  チップセレクト/ウェイトコントローラレジスタ
 */
#define		TADR_SFR_B0CS			0x00c0
#define		TADR_SFR_B1CS			0x00c1
#define		TADR_SFR_B2CS			0x00c2
#define		TADR_SFR_B3CS			0x00c3
#define		TADR_SFR_BEXCS			0x00c7
#define		TADR_SFR_MSAR0			0x00c8
#define		TADR_SFR_MAMR0			0x00c9
#define		TADR_SFR_MSAR1			0x00ca
#define		TADR_SFR_MAMR1			0x00cb
#define		TADR_SFR_MSAR2			0x00cc
#define		TADR_SFR_MAMR2			0x00cd
#define		TADR_SFR_MSAR3			0x00ce
#define		TADR_SFR_MAMR3			0x00cf

/*
 *  クロックギア/クロック逓倍回路レジスタ
 */
#define		TADR_SFR_SYSCR0			0x00e0
#define		TADR_SFR_SYSCR1			0x00e1
#define		TADR_SFR_SYSCR2			0x00e2
#define		TADR_SFR_EMCCR0			0x00e3
#define		TADR_SFR_EMCCR1			0x00e4
#define		TADR_SFR_DFMCR0			0x00e8
#define		TADR_SFR_DFMCR1			0x00e9

/*
 *  8bitタイマAレジスタ
 */
#define		TADR_SFR_TA01RUN		0x0100
#define		TADR_SFR_TA0REG			0x0102
#define		TADR_SFR_TA1REG			0x0103
#define		TADR_SFR_TA01MOD		0x0104
#define		TADR_SFR_TA1FFCR		0x0105
#define		TADR_SFR_TA23RUN		0x0108
#define		TADR_SFR_TA2REG			0x010a
#define		TADR_SFR_TA3REG			0x010b
#define		TADR_SFR_TA23MOD		0x010c
#define		TADR_SFR_TA3FFCR		0x010d
#define		TADR_SFR_TA45RUN		0x0110
#define		TADR_SFR_TA4REG			0x0112
#define		TADR_SFR_TA5REG			0x0113
#define		TADR_SFR_TA45MOD		0x0114
#define		TADR_SFR_TA5FFCR		0x0115
#define		TADR_SFR_TA67RUN		0x0118
#define		TADR_SFR_TA6REG			0x011a
#define		TADR_SFR_TA7REG			0x011b
#define		TADR_SFR_TA67MOD		0x011c
#define		TADR_SFR_TA7FFCR		0x011d

/*
 *  16bitタイマBレジスタ
 */
#define		TADR_SFR_TB0RUN			0x0180
#define		TADR_SFR_TB0MOD			0x0182
#define		TADR_SFR_TB0FFCR		0x0183
#define		TADR_SFR_TB0RG0L		0x0188
#define		TADR_SFR_TB0RG0H		0x0189
#define		TADR_SFR_TB0RG1L		0x018a
#define		TADR_SFR_TB0RG1H		0x018b
#define		TADR_SFR_TB0CP0L		0x018c
#define		TADR_SFR_TB0CP0H		0x018d
#define		TADR_SFR_TB0CP1L		0x018e
#define		TADR_SFR_TB0CP1H		0x018f
#define		TADR_SFR_TB1RUN			0x0190
#define		TADR_SFR_TB1MOD			0x0192
#define		TADR_SFR_TB1FFCR		0x0193
#define		TADR_SFR_TB1RG0L		0x0198
#define		TADR_SFR_TB1RG0H		0x0199
#define		TADR_SFR_TB1RG1L		0x019a
#define		TADR_SFR_TB1RG1H		0x019b
#define		TADR_SFR_TB1CP0L		0x019c
#define		TADR_SFR_TB1CP0H		0x019d
#define		TADR_SFR_TB1CP1L		0x019e
#define		TADR_SFR_TB1CP1H		0x019f

/*
 *  シリアルI/O(UART/I2Cバス/SIO)レジスタ
 */
#define		TADR_SFR_SC0BUF			0x0200
#define		TADR_SFR_SC0CR			0x0201
#define		TADR_SFR_SC0MOD0		0x0202
#define		TADR_SFR_BR0CR			0x0203
#define		TADR_SFR_BR0ADD			0x0204
#define		TADR_SFR_SC0MOD1		0x0205
#define		TADR_SFR_SIRCR			0x0207
#define		TADR_SFR_SC1BUF			0x0208
#define		TADR_SFR_SC1CR			0x0209
#define		TADR_SFR_SC1MOD0		0x020a
#define		TADR_SFR_BR1CR			0x020b
#define		TADR_SFR_BR1ADD			0x020c
#define		TADR_SFR_SC1MOD1		0x020d
#define		TADR_SFR_SBI0CR1		0x0240
#define		TADR_SFR_SBI0DBR		0x0241
#define		TADR_SFR_I2C0AR			0x0242
#define		TADR_SFR_SBI0CR2		0x0243
#define		TADR_SFR_SBI0BR0		0x0244
#define		TADR_SFR_SBI0BR1		0x0245

/*
 *  10ビットADCレジスタ
 */
#define		TADR_SFR_ADREG04L		0x02a0
#define		TADR_SFR_ADREG04H		0x02a1
#define		TADR_SFR_ADREG15L		0x02a2
#define		TADR_SFR_ADREG15H		0x02a3
#define		TADR_SFR_ADREG26L		0x02a4
#define		TADR_SFR_ADREG26H		0x02a5
#define		TADR_SFR_ADREG37L		0x02a6
#define		TADR_SFR_ADREG37H		0x02a7
#define		TADR_SFR_ADMOD0			0x02b0
#define		TADR_SFR_ADMOD1			0x02b1

/*
 *  ウォッチドッグタイマレジスタ
 */
#define		TADR_SFR_WDMOD			0x0300
#define		TADR_SFR_WDCR			0x0301

/*
 *  時計用タイマレジスタ
 */
#define		TADR_SFR_RTCCR			0x0310

/*
 *  SFRbitマスク定義
 */
/*
 *  割込みレジスタ
 */
/* 優先順位設定レジスタ	*/
#define		TBIT_INTM_L			0x07
#define		TBIT_INTC_L			0x08
#define		TBIT_INTM_H			0x70
#define		TBIT_INTC_H			0x80
/* 割込みクリア制御レジスタ	*/
/* クリアベクタ(マイクロDMA起動ベクタ)	*/
#define		TBIT_INT0_CLR		0x0a	/* INT0								*/
#define		TBIT_INT1_CLR		0x0b	/* INT1								*/
#define		TBIT_INT2_CLR		0x0c	/* INT2								*/
#define		TBIT_INT3_CLR		0x0d	/* INT3								*/
#define		TBIT_INT4_CLR		0x0e	/* INT4								*/
#define		TBIT_INT5_CLR		0x0f	/* INT5								*/
#define		TBIT_INT6_CLR		0x10	/* INT6								*/
#define		TBIT_INT7_CLR		0x11	/* INT7								*/
#define		TBIT_INT8_CLR		0x12	/* INT8								*/
#define		TBIT_TA0_CLR		0x13	/* 8ビットタイマ0					*/
#define		TBIT_TA1_CLR		0x14	/* 8ビットタイマ1					*/
#define		TBIT_TA2_CLR		0x15	/* 8ビットタイマ2					*/
#define		TBIT_TA3_CLR		0x16	/* 8ビットタイマ3					*/
#define		TBIT_TA4_CLR		0x17	/* 8ビットタイマ4					*/
#define		TBIT_TA5_CLR		0x18	/* 8ビットタイマ5					*/
#define		TBIT_TA6_CLR		0x19	/* 8ビットタイマ6					*/
#define		TBIT_TA7_CLR		0x1a	/* 8ビットタイマ7					*/
#define		TBIT_TB00_CLR		0x1b	/* 16ビットタイマ00					*/
#define		TBIT_TB01_CLR		0x1c	/* 16ビットタイマ01					*/
#define		TBIT_TB10_CLR		0x1d	/* 16ビットタイマ10					*/
#define		TBIT_TB11_CLR		0x1e	/* 16ビットタイマ11					*/
#define		TBIT_TBOF0_CLR		0x1f	/* 16ビットタイマ0オーバーフロー	*/
#define		TBIT_TBOF1_CLR		0x20	/* 16ビットタイマ1オーバーフロー	*/
#define		TBIT_RX0_CLR		0x21	/* シリアル受信0					*/
#define		TBIT_TX0_CLR		0x22	/* シリアル送信0					*/
#define		TBIT_RX1_CLR		0x23	/* シリアル受信1					*/
#define		TBIT_TX1_CLR		0x24	/* シリアル送信1					*/
#define		TBIT_SBI_CLR		0x25	/* シリアルバスインタフェース		*/
#define		TBIT_RTC_CLR		0x26	/* 時計用タイマ						*/
#define		TBIT_AD_CLR			0x27	/* AD変換終了						*/

/*
 *  8bitタイマレジスタ
 */
/* RUNレジスタ	*/
#define		TBIT_TA0RUN			0x01
#define		TBIT_TA1RUN			0x02
#define		TBIT_TA01PRUN		0x04
#define		TBIT_I2TA01			0x08
#define		TBIT_TA0RDE			0x80
/* モードレジスタ	*/
#define		TBIT_TA0CLK			0x03
#define		TBIT_TA0CLK0		0x00
#define		TBIT_TA0CLK1		0x01
#define		TBIT_TA0CLK4		0x02
#define		TBIT_TA0CLK16		0x03
#define		TBIT_TA1CLK			0x0c
#define		TBIT_TA1CLK0		0x00
#define		TBIT_TA1CLK1		0x04
#define		TBIT_TA1CLK16		0x08
#define		TBIT_TA1CLK256		0x0c
#define		TBIT_PWM			0x30
#define		TBIT_PWM26			0x10
#define		TBIT_PWM27			0x20
#define		TBIT_PWM28			0x30
#define		TBIT_TA01M			0xc0
#define		TBIT_TA01M8			0x00
#define		TBIT_TA01M16		0x40
#define		TBIT_TA01MPPG		0x80
#define		TBIT_TA01MPWM		0xc0

/*
 *  シリアルI/Oレジスタ
 */
/* モードレジスタ0	*/
#define		TBIT_SIOSC			0x03
#define		TBIT_SIOSCT			0x00
#define		TBIT_SIOSCBRG		0x01
#define		TBIT_SIOSCFSYS		0x02
#define		TBIT_SIOSCSCLK		0x03
#define		TBIT_SIOSM			0x0c
#define		TBIT_SIOSMIO		0x00
#define		TBIT_SIOSMU7		0x04
#define		TBIT_SIOSMU8		0x08
#define		TBIT_SIOSMU9		0x0c
#define		TBIT_SIOWU			0x10
#define		TBIT_SIORXE			0x20
#define		TBIT_SIOCTSE		0x40
#define		TBIT_SIOTB8			0x80
/* 制御レジスタ	*/
#define		TBIT_SIOIOC			0x01
#define		TBIT_SIOSCLKS		0x02
#define		TBIT_SIOFERR		0x04
#define		TBIT_SIOPERR		0x08
#define		TBIT_SIOOERR		0x10
#define		TBIT_SIOPE			0x20
#define		TBIT_SIOEVEN		0x40
#define		TBIT_SIORB8			0x80
/* ボーレートコントローラ	*/
#define		TBIT_SIOBRS			0x0f
#define		TBIT_SIOBRCK		0x30
#define		TBIT_SIOBRCK0		0x00
#define		TBIT_SIOBRCK2		0x10
#define		TBIT_SIOBRCK8		0x20
#define		TBIT_SIOBRCK32		0x30
#define		TBIT_SIOBRADD		0x40
#define		TBIT_SIOBRK			0x0f
/* モードレジスタ1	*/
#define		TBIT_SIOFDPX		0x40
#define		TBIT_SIOI2S			0x80

#endif	/* _TMP91CY22_H_	*/

