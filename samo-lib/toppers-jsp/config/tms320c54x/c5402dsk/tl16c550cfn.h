/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: tl16c550cfn.h,v 1.3 2003/12/04 08:25:44 honda Exp $
 */

/*
 * TL16C550CFN用 簡易SIOドライバ
 */
#ifndef _TL16C550CFN_H_
#define _TL16C550CFN_H_

#include <t_config.h>

/***********************************************
 *  I/O  メモリ・マップド・レジスタ            *
***********************************************/
/*
 *  complex Programmable Logic Device (CPLD)   *
 *  I/O メモリ・マップド・レジスタ                 *
 */

/*  レジスタ　アドレス　*/
#ifndef _MACRO_ONLY
volatile ioport UH	port0;
volatile ioport UH	port1;
volatile ioport UH	port2;
volatile ioport UH	port3;
volatile ioport UH	port4;
volatile ioport UH	port5;
volatile ioport UH	port6;
#endif /* _MACRO_ONLY */

/*  CPLD　レジスタ */
#define CPLD_CTRL1	port0
#define CPLD_STAT	port1
#define CPLD_DMCTRL	port2
#define CPLD_DBIO	port3
#define CPLD_CTRL2	port4
#define CPLD_SEM0	port5
#define CPLD_SEM1	port6

/*
 *  UART I/O メモリ・マップド・レジスタ *
 */
/*  レジスタ アドレス */
#ifndef _MACRO_ONLY
volatile ioport UH	port4000;
volatile ioport UH	port4001;
volatile ioport UH	port4002;
volatile ioport UH	port4003;
volatile ioport UH	port4004;
volatile ioport UH	port4005;
volatile ioport UH	port4006;
volatile ioport UH	port4007;
#endif /* _MACRO_ONLY */

/*  UART レジスタ  */
#define UART_RBR	port4000
#define UART_THR	port4000
#define UART_IER	port4001
#define UART_IIR	port4002
#define UART_FCR	port4002
#define UART_LCR	port4003
#define UART_MCR	port4004
#define UART_LSR	port4005
#define UART_MSR	port4006
#define UART_SCR	port4007
/*
 *  以下のレジスタにアクセスする前に、
 *  DLAB=1 (b7 of LCR) をセットする必要がある
 */
#define UART_DLL	port4000
#define UART_DLM	port4001

/*
 *  ビットフィールドの定義
 */
#define FCR_FIFO	0x01
#define FCR_RFRST	0x02
#define FCR_TFRST	0x04
#define FIFO_TRIG	0x00

#define LCR_DLAB	0x80

/* 受信送信割り込みの定義 */
#define IER_ERBI	0x01  /* 受信割り込み */
#define IER_RIE		IER_ERBI 
#define IER_ETBEI	0x02  /* 送信割り込み */
#define IER_TIE		IER_ETBEI

#define LSR_DR		0x01
#define LSR_THRE	0x20
#define LSR_TEMT	0x40

#define IIR_RXINT	0x04
#define IIR_TXINT	0x02

/*
 *  UARTのボーレートの設定
 */
/*
 * BAUD RATE 2 : 115200 bps
 * BAUD RATE 4 : 57600  bps
 */
#define UART_BAUD_RATE    2



#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_initialization_block {
	VP	dummy;
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロック
 */
typedef struct sio_port_control_block {
	const SIOPINIB	*siopinib;  /* シリアルI/Oポート初期化ブロック */
	VP_INT	exinf;			/* 拡張情報 */
	BOOL	openflag;		/* オープン済みフラグ */
	BOOL	getready;		/* 文字を受信した状態 */
	BOOL	putready;		/* 文字を受信できる状態 */
} SIOPCB;

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND    1u              /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u              /* 受信通知コールバック */

/*
 * カーネル起動時用の初期化(sys_putcを使用するため)
 */
extern void	uart_init(void);

/*
 *  SIOドライバの初期化ルーチン
 */
extern void	uart_initialize(void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL	uart_openflag(void);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB	*uart_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void	uart_cls_por(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL	uart_snd_chr(SIOPCB *siopcb, char chr);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT	uart_rcv_chr(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void	uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void	uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  SIOの割込みサービスルーチン
 */
extern void	uart_isr(void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void	uart_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void	uart_ierdy_rcv(VP_INT exinf);

/* ポーリングでシリアルに送信 */
Inline char
uart_putc(char c)
{
	while ((UART_LSR & LSR_THRE) == 0);      /*  送信レジスタが空かどうか  */
	UART_THR = c;
	return(c);
}

#endif /* _MACRO_ONLY */

#endif /* _TL16C550CFN_H_ */
