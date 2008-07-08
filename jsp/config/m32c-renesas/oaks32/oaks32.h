/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: oaks32.h,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

#ifndef _OAKS32_H_
#define	_OAKS32_H_

/*
 *  割込み番号の定義
 */
#define	INT_BRK		0		/* BRK */
#define INT_AD1		7		/* Ａ-Ｄ変換器1 */
#define INT_DMA0	8		/* DMA0 */
#define INT_DMA1	9		/* DMA1 */
#define INT_DMA2	10		/* DMA2 */
#define INT_DMA3	11		/* DMA3 */
#define INT_TA0		12		/* タイマA0 */
#define INT_TA1		13		/* タイマA1 */
#define INT_TA2		14		/* タイマA2 */
#define INT_TA3		15		/* タイマA3 */
#define INT_TA4		16		/* タイマA4 */
#define INT_S0TNACK	17		/* UART0送信,NACK */
#define INT_S0RACK	18		/* UART0受信,ACK */
#define INT_S1TNACK	19		/* UART1送信,NACK */
#define INT_S1RACK	20		/* UART1受信,ACK */
#define INT_TB0		21		/* タイマB0 */
#define INT_TB1		22		/* タイマB1 */
#define INT_TB2		23		/* タイマB2 */
#define INT_TB3		24		/* タイマB3 */
#define INT_TB4		25		/* タイマB4 */
#define INT_INT5	26		/* INT5 */
#define INT_INT4	27		/* INT4 */
#define INT_INT3	28		/* INT3 */
#define INT_INT2	29		/* INT2 */
#define INT_INT1	30		/* INT1 */
#define INT_INT0	31		/* INT0 */
#define INT_TB5		32		/* タイマB5 */
#define INT_S2TNACK 	33		/* UART2送信,NACK */
#define INT_S2RACK 	34		/* UART2受信,ACK */
#define INT_S3TNACK 	35		/* UART3送信,NACK */
#define INT_S3RACK 	36		/* UART3受信,ACK */
#define INT_S4TNACK 	37		/* UART4送信,NACK */
#define INT_S4RACK 	38		/* UART4受信,ACK */

#define INT_AD0		42		/* Ａ-Ｄ変換器0 */
#define INT_KEY		43		/* キー入力割り込み */
#define INT_IIO0	44		/* インテリジェントI/O割込み0 */
#define INT_IIO1	45		/* インテリジェントI/O割込み1 */
#define INT_IIO2	46		/* インテリジェントI/O割込み2 */
#define INT_IIO3	47		/* インテリジェントI/O割込み3 */
#define INT_IIO4	48		/* インテリジェントI/O割込み4 */
#define INT_IIO5	49		/* インテリジェントI/O割込み5 */
#define INT_IIO6	50		/* インテリジェントI/O割込み6 */
#define INT_IIO7	51		/* インテリジェントI/O割込み7 */
#define INT_IIO8	52		/* インテリジェントI/O割込み8 */
#define INT_IIO9CAN0	53		/* インテリジェントI/O割込み9,CAN0 */
#define INT_IIO10CAN1	54		/* インテリジェントI/O割込み10,CAN1 */
#define INT_IIO11CAN2	57		/* インテリジェントI/O割込み11,CAN2 */

/*
 *  OAKS32 SFRアドレス定義
 *
 *  割込みレジスタ
 */
#define	TADR_SFR_INT_BASE       0x60			/* 割込み関連のベースアドレス */
#define TADR_SFR_DM0IC_OFFSET  8				/* DMA0割込み制御レジスタ */
#define TADR_SFR_TB5IC_OFFSET   9				/* timerB5 割込み制御レジスタ */
#define TADR_SFR_DMA2IC_OFFSET  10				/* DMA2割込み制御レジスタ */
#define	TADR_SFR_S2RIC_OFFSET   11				/* uart2受信割込み制御レジスタ */
#define	TADR_SFR_TA0IC_OFFSET   12				/* timerA0 割込み制御レジスタ */
#define	TADR_SFR_S3RIC_OFFSET   13				/* uart3受信割込み制御レジスタ */
#define TADR_SFR_TA2IC_OFFSET   14				/* timerA2 割込み制御レジスタ */
#define	TADR_SFR_S4RIC_OFFSET   15				/* uart4受信割込み制御レジスタ */
#define TADR_SFR_TA4IC_OFFSET   16				/* timerA4 割込み制御レジスタ */
#define TADR_SFR_BCN0IC_OFFSET  17				/* UART0バス衝突検出割込み制御レジスタ */
#define TADR_SFR_BCN3IC_OFFSET  17				/* UART3バス衝突検出割込み制御レジスタ */
#define	TADR_SFR_S0RIC_OFFSET   18				/* uart0受信割込み制御レジスタ */
#define TADR_SFR_AD0IC_OFFSET   19				/* A-D0変換割込み制御レジスタ */
#define	TADR_SFR_S1RIC_OFFSET   20				/* uart1受信割込み制御レジスタ */
#define TADR_SFR_IIO0IC_OFFSET  21				/* インテリジェントI/O割込み制御レジスタ0 */
#define TADR_SFR_TB1IC_OFFSET   22				/* timerB1 割込み制御レジスタ */
#define TADR_SFR_IIO2IC_OFFSET  23				/* インテリジェントI/O割込み制御レジスタ2 */
#define TADR_SFR_TB3IC_OFFSET   24				/* timerB3 割込み制御レジスタ */
#define TADR_SFR_IIO4IC_OFFSET  25				/* インテリジェントI/O割込み制御レジスタ4 */
#define TADR_SFR_INT5IC_OFFSET  26				/* SI/O4/INT5 割込み制御レジスタ */
#define TADR_SFR_IIO6IC_OFFSET  27				/* インテリジェントI/O割込み制御レジスタ6 */
#define TADR_SFR_INT3IC_OFFSET  28				/* INT3 割込み制御レジスタ */
#define TADR_SFR_IIO8IC_OFFSET  29				/* インテリジェントI/O割込み制御レジスタ8 */
#define TADR_SFR_INT1IC_OFFSET  30				/* INT1 割込み制御レジスタ */
#define TADR_SFR_IIO10IC_OFFSET 31				/* インテリジェントI/O割込み制御レジスタ10 */
#define TADR_SFR_CAN1IC_OFFSET  31				/* CAN割込み1制御レジスタ */
#define TADR_SFR_IIO11IC_OFFSET 33				/* インテリジェントI/O割込み制御レジスタ11 */
#define TADR_SFR_CAN2IC_OFFSET  33				/* CAN割込み2制御レジスタ */
#define TADR_SFR_AD1IC_OFFSET   38				/* A-D1変換割込み制御レジスタ */
#define TADR_SFR_DM1IC_OFFSET   40				/* DMA1割込み制御レジスタ */
#define	TADR_SFR_S2TIC_OFFSET   41				/* uart2送信割込み制御レジスタ */
#define TADR_SFR_DM3IC_OFFSET   42				/* DMA3割込み制御レジスタ */
#define	TADR_SFR_S3TIC_OFFSET   43				/* uart3送信割込み制御レジスタ */
#define TADR_SFR_TA1IC_OFFSET   44				/* timerA1 割込み制御レジスタ */
#define	TADR_SFR_S4TIC_OFFSET   45				/* uart4送信割込み制御レジスタ */
#define TADR_SFR_TA3IC_OFFSET   46				/* timerA3 割込み制御レジスタ */
#define TADR_SFR_BCN2IC_OFFSET  47				/* UART2バス衝突検出割込み制御レジスタ */
#define	TADR_SFR_S0TIC_OFFSET   48				/* uart0送信割込み制御レジスタ */
#define TADR_SFR_BCN1IC_OFFSET  49				/* UART1バス衝突検出割込み制御レジスタ */
#define TADR_SFR_BCN4IC_OFFSET  49				/* UART4バス衝突検出割込み制御レジスタ */
#define	TADR_SFR_S1TIC_OFFSET   50				/* uart1送信割込み制御レジスタ */
#define TADR_SFR_KUPIC_OFFSET   51				/* キー入力割込み制御レジスタ */
#define TADR_SFR_TB0IC_OFFSET   52				/* timerB0 割込み制御レジスタ */
#define TADR_SFR_IIO1IC_OFFSET  53				/* インテリジェントI/O割込み制御レジスタ1 */
#define TADR_SFR_TB2IC_OFFSET   54				/* timerB2 割込み制御レジスタ */
#define TADR_SFR_IIO3IC_OFFSET  55				/* インテリジェントI/O割込み制御レジスタ3 */
#define TADR_SFR_TB4IC_OFFSET   56				/* timerB4 割込み制御レジスタ */
#define TADR_SFR_IIO5IC_OFFSET  57				/* インテリジェントI/O割込み制御レジスタ5 */
#define TADR_SFR_INT4IC_OFFSET  58				/* INT4 割込み制御レジスタ */
#define TADR_SFR_IIO7IC_OFFSET  59				/* インテリジェントI/O割込み制御レジスタ7 */
#define TADR_SFR_INT2IC_OFFSET  60				/* INT2 割込み制御レジスタ */
#define TADR_SFR_IIO9IC_OFFSET  61				/* インテリジェントI/O割込み制御レジスタ9 */
#define TADR_SFR_CAN0IC_OFFSET  61				/* CAN割込み0制御レジスタ */
#define TADR_SFR_INT0IC_OFFSET  62				/* INT0 割込み制御レジスタ */

/*
 *  タイマーレジスタ
 */
#define TADR_SFR_TBSR           0x300			/* timerB3,4,5カウント開始フラグ */
#define TADR_SFR_TA11           0x302			/* timerA1-1レジスタ */
#define TADR_SFR_TA21           0x304			/* timerA2-1レジスタ */
#define TADR_SFR_TA41           0x306			/* timerA4-1レジスタ */
#define TADR_SFR_INVC0          0x308			/* 三相PWM制御レジスタ０ */
#define TADR_SFR_INVC1          0x309			/* 三相PWM制御レジスタ１ */
#define TADR_SFR_IDB0           0x30a			/* 三相出力バッファレジスタ０ */
#define TADR_SFR_IDB1           0x30b			/* 三相出力バッファレジスタ１ */
#define TADR_SFR_DTT            0x30c			/* 短絡防止タイマ */
#define TADR_SFR_ICTB2          0x30d			/* timerB2割込み発生頻度設定カウンタ */
#define TADR_SFR_TB3            0x310			/* timerB3レジスタ */
#define TADR_SFR_TB4            0x312			/* timerB4レジスタ */
#define TADR_SFR_TB5            0x314			/* timerB5レジスタ */
#define	TADR_SFR_TB3MR          0x31b			/* timerB3モードレジスタ */
#define	TADR_SFR_TB4MR          0x31c			/* timerB4モードレジスタ */
#define	TADR_SFR_TB5MR          0x31d			/* timerB5モードレジスタ */
#define TADR_SFR_IFSR           0x31f			/* 割込み要因選択レジスタ */

#define	TADR_SFR_TABSR          0x340			/* カウント開始フラグ */
#define TADR_SFR_CPSRF          0x341			/* リセット プリスケーラ リセットフラグ */
#define TADR_SFR_ONSF           0x342			/* ワンショト スタートフラグ */
#define TADR_SFR_TRGSR          0x343			/* トリガーセレクト レジスタ */
#define	TADR_SFR_UDF            0x344			/* アップダウンフラグ */
#define	TADR_SFR_TA0            0x346			/* timerA0レジスタ */
#define TADR_SFR_TA1            0x348			/* timerA1レジスタ */
#define TADR_SFR_TA2            0x34a			/* timerA2レジスタ */
#define TADR_SFR_TA3            0x34c			/* timerA3レジスタ */
#define TADR_SFR_TA4            0x34e			/* timerA4レジスタ */
#define TADR_SFR_TB0            0x350			/* timerB0レジスタ */
#define TADR_SFR_TB1            0x352			/* timerB1レジスタ */
#define TADR_SFR_TB2            0x354			/* timerB2レジスタ */
#define	TADR_SFR_TA0MR          0x356			/* timerA0モードレジスタ */
#define	TADR_SFR_TA1MR          0x357			/* timerA1モードレジスタ */
#define	TADR_SFR_TA2MR          0x358			/* timerA2モードレジスタ */
#define	TADR_SFR_TA3MR          0x359			/* timerA3モードレジスタ */
#define	TADR_SFR_TA4MR          0x35a			/* timerA4モードレジスタ */
#define	TADR_SFR_TB0MR          0x35b			/* timerB0モードレジスタ */
#define	TADR_SFR_TB1MR		0x35c			/* timerB1モードレジスタ */
#define	TADR_SFR_TB2MR          0x35d			/* timerB2モードレジスタ */
#define	TADR_SFR_TB2SC          0x35e			/* timerB2スペシャルモードレジスタ */
#define TADR_SFR_TCSPR          0x35f			/* カウントソースプリスケーラレジスタ */

/*
 *  シリアルレジスタ
 */
#define	TADR_SFR_UART0_BASE     0x360			/* uart0レジスタベースアドレス */
#define	TADR_SFR_UART1_BASE     0x2e0			/* uart1レジスタベースアドレス */
#define	TADR_SFR_UART2_BASE     0x330			/* uart2レジスタベースアドレス */
#define	TADR_SFR_UART3_BASE     0x320			/* uart3レジスタベースアドレス */
#define	TADR_SFR_UART4_BASE     0x2f0			/* uart4レジスタベースアドレス */

#define	TADR_SFR_USMR_OFFSET    7			/* 特殊モードレジスタ */
#define	TADR_SFR_USMR2_OFFSET   6			/* 特殊モードレジスタ2 */
#define	TADR_SFR_USMR3_OFFSET   5			/* 特殊モードレジスタ3 */
#define	TADR_SFR_USMR4_OFFSET   4			/* 特殊モードレジスタ4 */

#define	TADR_SFR_UMR_OFFSET     8			/* uart送受信モードレジスタ */
#define	TADR_SFR_UBRG_OFFSET    9			/* uartボーレートジェネレータ */
#define	TADR_SFR_UTB_OFFSET     10			/* uart送信バッファレジスタ */
#define	TADR_SFR_UC0_OFFSET     12			/* uart送受信制御レジスタ#0 */
#define	TADR_SFR_UC1_OFFSET     13			/* uart送受信制御レジスタ#1 */
#define	TADR_SFR_URB_OFFSET     14			/* uart受信バッファレジスタ */

/*
 *  A-D/D-Aレジスタ
 */
#define TADR_SFR_AD00            0x380			/* A-Dレジスタ０ */
#define TADR_SFR_AD01            0x382			/* A-Dレジスタ１ */
#define TADR_SFR_AD02            0x384			/* A-Dレジスタ２ */
#define TADR_SFR_AD03            0x386			/* A-Dレジスタ３ */
#define TADR_SFR_AD04            0x388			/* A-Dレジスタ４ */
#define TADR_SFR_AD05            0x38a			/* A-Dレジスタ５ */
#define TADR_SFR_AD06            0x38c			/* A-Dレジスタ６ */
#define TADR_SFR_AD07            0x38e			/* A-Dレジスタ７ */
#define TADR_SFR_AD0CON2         0x394			/* A-D制御レジスタ２ */
#define TADR_SFR_AD0CON0         0x396			/* A-D制御レジスタ０ */
#define TADR_SFR_AD0CON1         0x397			/* A-D制御レジスタ１ */

#define TADR_SFR_AD10            0x1c0			/* A-Dレジスタ０ */
#define TADR_SFR_AD11            0x1c2			/* A-Dレジスタ１ */
#define TADR_SFR_AD12            0x1c4			/* A-Dレジスタ２ */
#define TADR_SFR_AD13            0x1c6			/* A-Dレジスタ３ */
#define TADR_SFR_AD14            0x1c8			/* A-Dレジスタ４ */
#define TADR_SFR_AD15            0x1ca			/* A-Dレジスタ５ */
#define TADR_SFR_AD16            0x1cc			/* A-Dレジスタ６ */
#define TADR_SFR_AD17            0x1ce			/* A-Dレジスタ７ */
#define TADR_SFR_AD1CON2         0x1d4			/* A-D制御レジスタ２ */
#define TADR_SFR_AD1CON0         0x1d6			/* A-D制御レジスタ０ */
#define TADR_SFR_AD1CON1         0x1d7			/* A-D制御レジスタ１ */

#define TADR_SFR_DA0            0x398			/* D-Aレジスタ０ */
#define TADR_SFR_DA1            0x39a			/* D-Aレジスタ１ */
#define TADR_SFR_DACON          0x39c			/* D-A制御レジスタ */

/*
 *  汎用ポートレジスタ(一部、144ピン版のみ)
 */
#define TADR_SFR_P0             0x3e0           /* ポートP0レジスタ */
#define TADR_SFR_P1             0x3e1           /* ポートP1レジスタ */
#define TADR_SFR_PD0            0x3e2		/* ポートP0方向レジスタ */
#define TADR_SFR_PD1            0x3e3		/* ポートP1方向レジスタ */
#define TADR_SFR_P2             0x3e4		/* ポートP2レジスタ */
#define TADR_SFR_P3             0x3e5		/* ポートP3レジスタ */
#define	TADR_SFR_PD2            0x3e6		/* ポートP2方向レジスタ */
#define	TADR_SFR_PD3            0x3e7		/* ポートP3方向レジスタ */
#define TADR_SFR_P4             0x3e8		/* ポートP4レジスタ */
#define	TADR_SFR_P5             0x3e9		/* ポートP5レジスタ */
#define	TADR_SFR_PD4            0x3ea		/* ポートP4方向レジスタ */
#define	TADR_SFR_PD5            0x3eb		/* ポートP5方向レジスタ */
#define TADR_SFR_P6             0x3c0		/* ポートP6レジスタ */
#define	TADR_SFR_P7             0x3c1		/* ポートP7レジスタ */
#define	TADR_SFR_PD6            0x3c2		/* ポートP6方向レジスタ */
#define	TADR_SFR_PD7            0x3c3		/* ポートP7方向レジスタ */
#define	TADR_SFR_P8             0x3c4		/* ポートP8レジスタ */
#define TADR_SFR_P9             0x3c5		/* ポートP9レジスタ */
#define	TADR_SFR_PD8            0x3c6		/* ポートP8方向レジスタ */
#define TADR_SFR_PD9            0x3c7		/* ポートP9方向レジスタ */
#define TADR_SFR_P10            0x3c8		/* ポートP10レジスタ */
#define TADR_SFR_P11            0x3c9		/* ポートP11レジスタ(144ピン版のみ) */
#define TADR_SFR_PD10           0x3ca		/* ポートP10方向レジスタ */
#define TADR_SFR_PD11           0x3cb		/* ポートP11方向レジスタ(144ピン版のみ) */
#define TADR_SFR_P12            0x3cc		/* ポートP12レジスタ(144ピン版のみ) */
#define TADR_SFR_P13            0x3cd		/* ポートP13レジスタ(144ピン版のみ) */
#define TADR_SFR_PD12           0x3ce		/* ポートP12方向レジスタ(144ピン版のみ) */
#define TADR_SFR_PD13           0x3cf		/* ポートP13方向レジスタ(144ピン版のみ) */
#define TADR_SFR_P14            0x3d0		/* ポートP14レジスタ(144ピン版のみ) */
#define TADR_SFR_P15            0x3d1		/* ポートP15レジスタ(144ピン版のみ) */
#define TADR_SFR_PD14           0x3d2		/* ポートP14方向レジスタ(144ピン版のみ) */
#define TADR_SFR_PD15           0x3d3		/* ポートP15方向レジスタ(144ピン版のみ) */

#define	TADR_SFR_PUR0           0x3f0		/* プルアップ制御レジスタ０ */
#define	TADR_SFR_PUR1           0x3f1		/* プルアップ制御レジスタ１ */
#define	TADR_SFR_PUR2           0x3da		/* プルアップ制御レジスタ２ */
#define	TADR_SFR_PUR3           0x3db		/* プルアップ制御レジスタ３ */
#define	TADR_SFR_PUR4           0x3dc		/* プルアップ制御レジスタ４(144ピン版のみ) */

#define	TADR_SFR_PCR            0x3ff		/* ポート制御レジスタ */

#define TADR_SFR_PS0            0x3b0		/* 機能選択レジスタA0 */
#define TADR_SFR_PS1            0x3b1		/* 機能選択レジスタA1 */
#define TADR_SFR_PS2            0x3b4		/* 機能選択レジスタA2 */
#define TADR_SFR_PS3            0x3b5		/* 機能選択レジスタA3 */
#define TADR_SFR_PS5            0x3b9		/* 機能選択レジスタA5(144ピン版のみ) */
#define TADR_SFR_PS6            0x3bc		/* 機能選択レジスタA6(144ピン版のみ) */
#define TADR_SFR_PS7            0x3bd		/* 機能選択レジスタA7(144ピン版のみ) */
#define TADR_SFR_PS8            0x3a0		/* 機能選択レジスタA8(144ピン版のみ) */
#define TADR_SFR_PS9            0x3a1		/* 機能選択レジスタA9(144ピン版のみ) */

#define TADR_SFR_PSL0           0x3b2		/* 機能選択レジスタB0 */
#define TADR_SFR_PSL1           0x3b3		/* 機能選択レジスタB1 */
#define TADR_SFR_PSL2           0x3b6		/* 機能選択レジスタB2 */
#define TADR_SFR_PSL3           0x3b7		/* 機能選択レジスタB3 */

#define TADR_SFR_PSC            0x3af		/* 機能選択レジスタC */


/*
 *  送受信制御レジスタ1 (UART0,UART1)
 */
#define	TBIT_UiC1_RI	0x08	/* 受信完了フラグ */
#define	TBIT_UiC1_RE	0x04	/* 受信許可ビット */
#define	TBIT_UiC1_TI	0x02	/* 送信バッファ空フラグ */
#define	TBIT_UiC1_TE	0x01	/* 送信許可ビット */

#define TBIT_UiC0_TXEPT	0x08	/* 送信レジスタ空フラグ */

/*
 *  割込み制御レジスタ
 */
#define	TBIT_TAiIC_IR	0x08

/*
 *  カウント開始フラグ (TABSR)
 */
#define TBIT_TABSR_TA0S	0x01

/*
 *  アップダウンフラグ(UDF)
 */
#define TBIT_UDF_TA0UD	0x01


/*
 *  ポート７設定
 */
//#define	P7_LED1					0x10			/* LED1ビット定義 */
//#define	P7_LED2					0x20			/* LED2ビット定義 */

#ifndef _MACRO_ONLY

#include "cpu_defs.h"

/*
 *  ターゲットシステムの文字出力
 */

Inline void
oaks32_putc(char c)
{
	char enabled;
	volatile char * p = (volatile char *)TADR_SFR_UART0_BASE;

	/* 送信許可されていなければ許可する */
	enabled = p[TADR_SFR_UC1_OFFSET] & TBIT_UiC1_TE;
	if(!enabled) {
		p[TADR_SFR_UC1_OFFSET] |= TBIT_UiC1_TE;
	}

	/* 送信バッファが空になるまで待つ(有限時間内の条件成立を仮定) */
	while((p[TADR_SFR_UC1_OFFSET] & TBIT_UiC1_TI) == 0);

	/* 1文字送信 */
	p[TADR_SFR_UTB_OFFSET] = c;

	if(!enabled) {
		p[TADR_SFR_UC1_OFFSET] &= ~TBIT_UiC1_TE;
	}
}

#endif /* _MACRO_ONLY */
#endif /* _OAKS32_H_ */

