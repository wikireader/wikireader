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
 *  Copyright (C) 2006 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: m3029.h,v 1.2 2006/08/03 04:14:02 honda Exp $
 */

#ifndef _M3029_H_
#define	_M3029_H_

/*
 *  割込み番号の定義
 */
#define	INT_BRK         0			/* BRK */
#define INT_INT3        4			/* INT3 */
#define INT_TB5	        5			/* タイマB5 */
#define INT_TB4         6			/* タイマB4 */
#define INT_TB3         7			/* タイマB3 */
#define INT_SIO4INT5    8			/* SI/O4/INT5 */
#define INT_SIO3INT4    9			/* SI/O3/INT4 */
#define INT_BUS         10			/* バス衝突検出 */
#define INT_DMA0        11			/* DMA0 */
#define INT_DMA1        12			/* DMA1 */
#define INT_KEY         13			/* キー入力割り込み */
#define INT_AD          14			/* Ａ-Ｄ変換器 */
#define INT_S2TNACK     15			/* UART2送信,NACK2 */
#define INT_S2RACK      16			/* UART2受信,ACK2 */
#define INT_S0T         17			/* UART0送信 */
#define INT_S0R         18			/* UART0受信 */
#define INT_S1T         19			/* UART1送信 */
#define INT_S1R         20			/* UART1受信 */
#define INT_TA0         21			/* タイマA0 */
#define INT_TA1         22			/* タイマA1 */
#define INT_TA2         23			/* タイマA2 */
#define INT_TA3         24			/* タイマA3 */
#define INT_TA4         25			/* タイマA4 */
#define INT_TB0         26			/* タイマB0 */
#define INT_TB1         27			/* タイマB1 */
#define INT_TB2         28			/* タイマB2 */
#define INT_INT0        29			/* INT0 */
#define INT_INT1        30			/* INT1 */
#define INT_INT2        31			/* INT2 */

/*
 *  M3029 SFRアドレス定義
 *
 *  割込みレジスタ
 */
#define	TADR_SFR_INT_BASE       0x040			/* 割込み関連のベースアドレス */
#define TADR_SFR_INT3IC_OFFSET  4				/* INT3 割込み制御レジスタ */
#define TADR_SFR_TB5IC_OFFSET   5				/* timerB5 割込み制御レジスタ */
#define TADR_SFR_TB4IC_OFFSET   6				/* timerB4 割込み制御レジスタ */
#define TADR_SFR_TB3IC_OFFSET   7				/* timerB3 割込み制御レジスタ */
#define TADR_SFR_SO4INT5_OFFSET 8				/* SI/O4/INT5 割込み制御レジスタ */
#define TADR_SFR_SO3INT4_OFFSET 9				/* SI/O3/INT4 割込み制御レジスタ */
#define TADR_SFR_BCNIC_OFFSET   10				/* バス衝突検出割込み制御レジスタ */
#define TADR_SFR_DMA0IC_OFFSET  11				/* DMA0割込み制御レジスタ */
#define TADR_SFR_DMA1IC_OFFSET  12				/* DMA1割込み制御レジスタ */
#define TADR_SFR_KUPIC_OFFSET   13				/* キー入力割込み制御レジスタ */
#define TADR_SFR_ADIC_OFFSET    14				/* A-D変換割込み制御レジスタ */
#define	TADR_SFR_S2TIC_OFFSET   15				/* uart2送信割込み制御レジスタ */
#define	TADR_SFR_S2RIC_OFFSET   16				/* uart2受信割込み制御レジスタ */
#define	TADR_SFR_S0TIC_OFFSET   17				/* uart0送信割込み制御レジスタ */
#define	TADR_SFR_S0RIC_OFFSET   18				/* uart0受信割込み制御レジスタ */
#define	TADR_SFR_S1TIC_OFFSET   19				/* uart1送信割込み制御レジスタ */
#define	TADR_SFR_S1RIC_OFFSET   20				/* uart1受信割込み制御レジスタ */
#define	TADR_SFR_TA0IC_OFFSET   21				/* timerA0 割込み制御レジスタ */
#define TADR_SFR_TA1IC_OFFSET   22				/* timerA1 割込み制御レジスタ */
#define TADR_SFR_TA2IC_OFFSET   23				/* timerA2 割込み制御レジスタ */
#define TADR_SFR_TA3IC_OFFSET   24				/* timerA3 割込み制御レジスタ */
#define TADR_SFR_TA4IC_OFFSET   25				/* timerA4 割込み制御レジスタ */
#define TADR_SFR_TB0IC_OFFSET   26				/* timerB0 割込み制御レジスタ */
#define TADR_SFR_TB1IC_OFFSET   27				/* timerB1 割込み制御レジスタ */
#define TADR_SFR_TB2IC_OFFSET   28				/* timerB2 割込み制御レジスタ */
#define TADR_SFR_INT0IC_OFFSET  29				/* INT0 割込み制御レジスタ */
#define TADR_SFR_INT1IC_OFFSET  30				/* INT1 割込み制御レジスタ */
#define TADR_SFR_INT12C_OFFSET  31				/* INT2 割込み制御レジスタ */

/*
 *  DMAレジスタ
 */
#define TADR_SFR_SAR0           0x020			/* DMA0ソースポインタ */
#define TADR_SFR_DAR0           0x024			/* DMA0デスティネーションポインタ */
#define TADR_SFR_TCR0			0x028			/* DMA0転送カウンタ */
#define TADR_SFR_DM0CON         0x02c			/* DMA0制御レジスタ */
#define TADR_SFR_SAR1           0x030			/* DMA1ソースポインタ */
#define TADR_SFR_DAR1           0x034			/* DMA1デスティネーションポインタ */
#define TADR_SFR_TCR1			0x038			/* DMA1転送カウンタ */
#define TADR_SFR_DM1CON         0x03c			/* DMA1制御レジスタ */

/*
 *  タイマーとその他のデバイスレジスタ
 */
#define TADR_SFR_TA11           0x342			/* timerA1-1レジスタ */
#define TADR_SFR_TA21           0x344			/* timerA2-1レジスタ */
#define TADR_SFR_TA41           0x346			/* timerA4-1レジスタ */
#define TADR_SFR_INVC0          0x348			/* 三相PWM制御レジスタ０ */
#define TADR_SFR_INVC1          0x349			/* 三相PWM制御レジスタ１ */
#define TADR_SFR_IDB0           0x34a			/* 三相出力バッファレジスタ０ */
#define TADR_SFR_IDB1           0x34b			/* 三相出力バッファレジスタ１ */
#define TADR_SFR_DTT            0x34c			/* 短絡防止タイマ */
#define TADR_SFR_ICTB2          0x34d			/* timerB2割込み発生頻度設定カウンタ */

#define TADR_SFR_IFSR           0x35f			/* 割込み要因選択レジスタ */

#define TADR_SFR_S3TRR          0x360			/* SI/O3送受信レジスタ */
#define TADR_SFR_S3C            0x362			/* SI/O3制御レジスタ */
#define TADR_SFR_S3BRG          0x363			/* SI/O3転送速度レジスタ */
#define TADR_SFR_S4TRR          0x364			/* SI/O4送受信レジスタ */
#define TADR_SFR_S4C            0x366			/* SI/O4制御レジスタ */
#define TADR_SFR_S4BRG          0x367			/* SI/O4転送速度レジスタ */

#define	TADR_SFR_TABSR          0x380			/* カウント開始フラグ */
#define TADR_SFR_CPSRF          0x381			/* リセット プリスケーラ リセットフラグ */
#define TADR_SFR_ONSF           0x382			/* ワンショト スタートフラグ */
#define TADR_SFR_TRGSR          0x383			/* トリガーセレクト レジスタ */
#define	TADR_SFR_UDF            0x384			/* アップダウンフラグ */
#define	TADR_SFR_TA0            0x386			/* timerA0レジスタ */
#define TADR_SFR_TA1            0x388			/* timerA1レジスタ */
#define TADR_SFR_TA2            0x38a			/* timerA2レジスタ */
#define TADR_SFR_TA3            0x38c			/* timerA3レジスタ */
#define TADR_SFR_TA4            0x38e			/* timerA4レジスタ */
#define TADR_SFR_TB0            0x390			/* timerB0レジスタ */
#define TADR_SFR_TB1            0x392			/* timerB1レジスタ */
#define TADR_SFR_TB2            0x394			/* timerB2レジスタ */
#define	TADR_SFR_TA0MR          0x396			/* timerA0モードレジスタ */
#define	TADR_SFR_TA1MR          0x397			/* timerA1モードレジスタ */
#define	TADR_SFR_TA2MR          0x398			/* timerA2モードレジスタ */
#define	TADR_SFR_TA3MR          0x399			/* timerA3モードレジスタ */
#define	TADR_SFR_TA4MR          0x39a			/* timerA4モードレジスタ */
#define	TADR_SFR_TB0MR          0x39b			/* timerB0モードレジスタ */
#define	TADR_SFR_TB1MR			0x39c			/* timerB1モードレジスタ */
#define	TADR_SFR_TB2MR          0x39d			/* timerB2モードレジスタ */
#define	TADR_SFR_TB2SR          0x39e			/* timerB2スペシャルモードレジスタ */

/*
 *  シリアルレジスタ
 */
#define	TADR_SFR_UART0_BASE     0x3a0			/* uart0レジスタベースアドレス */
#define	TADR_SFR_UART1_BASE     0x3a8			/* uart1レジスタベースアドレス */
#define	TADR_SFR_UART2_BASE     0x378			/* uart2レジスタベースアドレス */
#define	TADR_SFR_U2SMR          0x377			/* uart2特殊モードレジスタ */
#define	TADR_SFR_U2SMR2         0x376			/* uart2特殊モードレジスタ２ */
#define	TADR_SFR_U2SMR3         0x375			/* uart2特殊モードレジスタ３ */

#define	TADR_SFR_UMR_OFFSET     0				/* uart送受信モードレジスタ */
#define	TADR_SFR_UBRG_OFFSET    1				/* uartボーレートジェネレータ */
#define	TADR_SFR_UTB_OFFSET     2				/* uart送信バッファレジスタ */
#define	TADR_SFR_UC0_OFFSET     4				/* uart送受信制御レジスタ#0 */
#define	TADR_SFR_UC1_OFFSET     5				/* uart送受信制御レジスタ#1 */
#define	TADR_SFR_URB_OFFSET     6				/* uart受信バッファレジスタ */
#define	TADR_SFR_UC2_OFFSET     16				/* uart送受信制御レジスタ#2 */

/*
 *  A-D/D-Aレジスタ
 */
#define TADR_SFR_AD0            0x3c0			/* A-Dレジスタ０ */
#define TADR_SFR_AD1            0x3c2			/* A-Dレジスタ１ */
#define TADR_SFR_AD2            0x3c4			/* A-Dレジスタ２ */
#define TADR_SFR_AD3            0x3c6			/* A-Dレジスタ３ */
#define TADR_SFR_AD4            0x3c8			/* A-Dレジスタ４ */
#define TADR_SFR_AD5            0x3ca			/* A-Dレジスタ５ */
#define TADR_SFR_AD6            0x3cc			/* A-Dレジスタ６ */
#define TADR_SFR_AD7            0x3ce			/* A-Dレジスタ７ */
#define TADR_SFR_ADCON2         0x3d4			/* A-D制御レジスタ２ */
#define TADR_SFR_ADCON0         0x3d6			/* A-D制御レジスタ０ */
#define TADR_SFR_ADCON1         0x3d7			/* A-D制御レジスタ１ */
#define TADR_SFR_DA0            0x3d8			/* D-Aレジスタ０ */
#define TADR_SFR_DA1            0x3da			/* D-Aレジスタ１ */
#define TADR_SFR_DACON          0x3dc			/* D-A制御レジスタ */

/*
 *  汎用ポートレジスタ
 */
#define TADR_SFR_P0             0x3e0           /* ポートP0レジスタ */
#define TADR_SFR_P1             0x3e1           /* ポートP1レジスタ */
#define TADR_SFR_PD0            0x3e2			/* ポートP0方向レジスタ */
#define TADR_SFR_PD1            0x3e3			/* ポートP1方向レジスタ */
#define TADR_SFR_P2             0x3e4			/* ポートP2レジスタ */
#define TADR_SFR_P3             0x3e5			/* ポートP3レジスタ */
#define	TADR_SFR_PD2            0x3e6			/* ポートP2方向レジスタ */
#define	TADR_SFR_PD3            0x3e7			/* ポートP3方向レジスタ */
#define TADR_SFR_P4             0x3e8			/* ポートP4レジスタ */
#define	TADR_SFR_P5             0x3e9			/* ポートP5レジスタ */
#define	TADR_SFR_PD4            0x3ea			/* ポートP4方向レジスタ */
#define	TADR_SFR_PD5            0x3eb			/* ポートP5方向レジスタ */
#define TADR_SFR_P6             0x3ec			/* ポートP6レジスタ */
#define	TADR_SFR_P7             0x3ed			/* ポートP7レジスタ */
#define	TADR_SFR_PD6            0x3ee			/* ポートP6方向レジスタ */
#define	TADR_SFR_PD7            0x3ef			/* ポートP7方向レジスタ */
#define	TADR_SFR_P8             0x3f0			/* ポートP8レジスタ */
#define TADR_SFR_P9             0x3f1			/* ポートP9レジスタ */
#define	TADR_SFR_PD8            0x3f2			/* ポートP8方向レジスタ */
#define TADR_SFR_PD9            0x3f3			/* ポートP9方向レジスタ */
#define TADR_SFR_P10            0x3f4			/* ポートP10レジスタ */
#define TADR_SFR_PD10           0x3f6			/* ポートP10方向レジスタ */
#define	TADR_SFR_PUR0           0x3fc			/* プルアップ制御レジスタ０ */
#define	TADR_SFR_PUR1           0x3fd			/* プルアップ制御レジスタ１ */
#define	TADR_SFR_PUR2           0x3fe			/* プルアップ制御レジスタ２ */
#define	TADR_SFR_PCR            0x3ff			/* ポート制御レジスタ */



/*
 * 割込み制御レジスタのビット定義
 */
#define	TBIT_IC_IR	0x08

/*
 *  送受信制御レジスタ1のビット定義 (UART0,UART1)
 */
#define	TBIT_UiC1_RI	0x08	/* 受信完了フラグ */
#define	TBIT_UiC1_RE	0x04	/* 受信許可ビット */
#define	TBIT_UiC1_TI	0x02	/* 送信バッファ空フラグ */
#define	TBIT_UiC1_TE	0x01	/* 送信許可ビット */

#define TBIT_UiC0_TXEPT	0x08	/* 送信レジスタ空フラグ */


/*
 *  タイマー関連のレジスタのビット定義
 */

/*
 *  カウント開始フラグ (TABSR)
 */
#define TBIT_TABSR_TA0S	0x01
#define TBIT_TABSR_TA1S	0x02
#define TBIT_TABSR_TA2S	0x04
#define TBIT_TABSR_TA3S	0x08
#define TBIT_TABSR_TA4S	0x10

/*
 *  アップダウンフラグ(UDF)
 */
#define TBIT_UDF_TA0UD	0x01
#define TBIT_UDF_TA1UD	0x02
#define TBIT_UDF_TA2UD	0x04
#define TBIT_UDF_TA3UD	0x08
#define TBIT_UDF_TA4UD	0x10


/*
 *  プロテクトレジスタ
 */
#define TADR_SFR_PRCR       0x00A

#define TVAL_PACR_PRCR      0x04   /* PACR書き込み有効 */


/*
 * 端子割り当て制御レジスタと設定値  
 */
#define TADR_SFR_PACR       0x25D
#define TVAL_80_PACR        0x03   /* 80ピン*/
#define TVAL_64_PACR        0x02   /* 64ピン*/


#ifndef _MACRO_ONLY

#include "cpu_defs.h"

/*
 *  ターゲットシステムの文字出力
 */

Inline void
m3029_putc(char c)
{
#if CONSOLE_PORTID == 1
	volatile char * p = (volatile char *)TADR_SFR_UART0_BASE;
#else	/* CONSOLE_PORTID == 2 */
	volatile char * p = (volatile char *)TADR_SFR_UART1_BASE;
#endif
	
	while((p[TADR_SFR_UC1_OFFSET] & TBIT_UiC1_TI) == 0);
	p[TADR_SFR_UTB_OFFSET] = c;
}

#endif /* _MACRO_ONLY */
#endif /* _M3029_H_ */

