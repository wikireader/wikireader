/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: oaks16mini.h,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

#ifndef _OAKS16MINI_H_
#define	_OAKS16MINI_H_

/*
 *  割込み番号の定義
 */
#define	INT_BRK         0			/* BRK */
#define INT_DMA0        11			/* DMA0 */
#define INT_DMA1        12			/* DMA1 */
#define INT_KEY         13			/* キー入力割り込み */
#define INT_AD          14			/* Ａ-Ｄ変換器 */
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

/*
 *  OAKS16 MINI SFRアドレス定義
 *
 *  割込みレジスタ
 */
#define	TADR_SFR_INT_BASE       0x040			/* 割込み関連のベースアドレス */
#define TADR_SFR_DMA0IC_OFFSET  11				/* DMA0割込み制御レジスタ */
#define TADR_SFR_DMA1IC_OFFSET  12				/* DMA1割込み制御レジスタ */
#define TADR_SFR_KUPIC_OFFSET   13				/* キー入力割込み制御レジスタ */
#define TADR_SFR_ADIC_OFFSET    14				/* A-D変換割込み制御レジスタ */
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
 *  タイマーレジスタ
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

#define	TADR_SFR_UMR_OFFSET     0				/* uart送受信モードレジスタ */
#define	TADR_SFR_UBRG_OFFSET    1				/* uartボーレートジェネレータ */
#define	TADR_SFR_UTB_OFFSET     2				/* uart送信バッファレジスタ */
#define	TADR_SFR_UC0_OFFSET     4				/* uart送受信制御レジスタ#0 */
#define	TADR_SFR_UC1_OFFSET     5				/* uart送受信制御レジスタ#1 */
#define	TADR_SFR_URB_OFFSET     6				/* uart受信バッファレジスタ */
#define	TADR_SFR_UC2_OFFSET     16				/* uart送受信制御レジスタ#2 */

/*
 *  汎用ポートレジスタ
 */
#define TADR_SFR_P1             0x3e1           /* ポートP1レジスタ */
#define TADR_SFR_PD1            0x3e3			/* ポートP1方向レジスタ */
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
#define	P7_LED1					0x10			/* LED1ビット定義 */
#define	P7_LED2					0x20			/* LED2ビット定義 */

/*
 *  ポート８設定
 */
#define	P8_SW1					0x01			/* SW1ビット定義 */
#define	P8_SW2					0x02			/* SW2ビット定義 */

#ifndef _MACRO_ONLY

#include "cpu_defs.h"

/*
 *  ターゲットシステムの文字出力
 */

Inline void
oaks16_putc(char c)
{
	volatile char * p = (volatile char *)TADR_SFR_UART1_BASE;

	while((p[TADR_SFR_UC1_OFFSET] & TBIT_UiC1_TI) == 0);
	p[TADR_SFR_UTB_OFFSET] = c;
}

#endif /* _MACRO_ONLY */
#endif /* _OAKS16MINI_H_ */

