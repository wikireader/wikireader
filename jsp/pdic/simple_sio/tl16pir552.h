/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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
 */

#ifndef _TL16PIR552_H_
#define _TL16PIR552_H_

#include <s_services.h>		/* デバイスドライバ用標準インクルードファイル */

/*
 *  シリアル／パラレル I/O TL16PIR552(TI) 関連の定義
 */
/* TL16PIR552のレジスタのアドレス */
#define RBR	0x00	/* Receiver Buffer (読み出し) */
#define THR	0x00	/* Transmitter Holding Register (書き出し) */
#define DLL	0x00	/* Divisor Latch (LSB) */
#define IER	0x10	/* Interrupt-Enable Register */
#define DLM	0x10	/* Divisor Latch (MSB) */
#define	IIR	0x20	/* Interrupt-Ident Register (読み出し) */
#define	FCR	0x20	/* FIFO Control Register (書き出し) */
#define	LCR	0x30	/* Line-Control Register */
#define	MCR	0x40	/* Modem-Control Register */
#define	LSR	0x50	/* Line-Status Register */
#define	MSR	0x60	/* Modem-Status Register */
#define	SCR	0x70	/* Scratch Register */

/* for LCR */
#define	WORD_LENGTH_8		(BIT1 | BIT0)
#define	STOP_BITS_1		0u		/* BIT2 */
#define	PARITY_NON		0u		/* BIT3, 4 */
/* BIT5,6 省略 */
#define	DIVISOR_LATCH_ACC	BIT7

/* for IER */
#define	DIS_INT			0u
#define	RECEIVE_DATA_AVAILABLE	BIT0
#define	TRANS_REG_EMPTY		BIT1
#define RECEIVE_LINE_STATUS	BIT2
#define MODEM_STATUS		BIT3

/* for MCR */
#define	DTR			BIT0
#define	RTS			BIT1
#define	ENABLE_EXT_INT		BIT3
#define	AUTOFLOW_CONTROL	BIT5

/* for FCR */
#define	FIFO_ENABLE		BIT0
#define	RECEIVE_FIFO_RESET	BIT1
#define	TRANS_FIFO_RESET	BIT2
#define RECEIVE_TRIG_1_BYTE	0u		/* BIT6, 7 */
#define RECEIVE_TRIG_4_BYTE	BIT6
#define	RECEIVE_TRIG_8_BYTE	BIT7
#define	RECEIVE_TRIG_14_BYTE	(BIT6 | BIT7)

/* for IIR */
#define	INT_MASK		0x0f
#define	INT_RECEIVE_DATA	BIT2
#define INT_CHAR_TIME_OUT	(BIT3 | BIT2)
#define	INT_TRANS_EMPTY		BIT1

/* for LSR */
#define	THRE			BIT5
#define TEMT			BIT6

/* ボーレート定義関係 */
#define	PRE_DIVISOR	4
#define	DIVISOR		XIN_CLOCK / (8 * DEVIDE_RATIO * PRE_DIVISOR)

#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block	SIOPCB;

#endif /* _MACRO_ONLY */

/*
 *  SIO用システムインタフェースレイヤー
 */
/* x : ポートのベースアドレス、y : レジスタオフセット、z : レジスタ値 */
#define tl16pir552_wrb( x, y, z )	sil_wrb_mem( (VP)(x + y), z )
#define tl16pir552_reb( x, y )		sil_reb_mem( (VP)(x + y) )

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND	1u		/* 送信可能コールバック */
#define SIO_ERDY_RCV	2u		/* 受信通知コールバック */

#ifndef _MACRO_ONLY

/*
 *  SIOドライバの初期化ルーチン
 */
extern void	tl16pir552_initialize(void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL	tl16pir552_openflag(void);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB	*tl16pir552_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void	tl16pir552_cls_por(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL	tl16pir552_snd_chr(SIOPCB *siopcb, char c);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT	tl16pir552_rcv_chr(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void	tl16pir552_ena_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void	tl16pir552_dis_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  SIOの割込みサービスルーチン
 */
extern void	tl16pir552_uart0_isr(void);
extern void	tl16pir552_uart1_isr(void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void	tl16pir552_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void	tl16pir552_ierdy_rcv(VP_INT exinf);

/*
 *  カーネル起動時用の初期化 (sys_putcで利用)
 */
extern void	tl16pir552_init(void);

/*
 *  シリアルI/Oポートへの文字送信（ポーリング）
 */
extern void	tl16pir552_putchar_pol(char c);

#endif /* _MACRO_ONLY */
#endif /* _TL16PIR552_H_ */
