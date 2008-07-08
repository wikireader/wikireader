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
 */

#ifndef _VR4131_DSIU_H_
#define _VR4131_DSIU_H_

#include <s_services.h>		/* デバイスドライバ用標準インクルードファイル */

/*
 *  VR4131内蔵デバッグシリアルインタフェースユニット(DSIU)関連の定義
 *  (簡易NS16550)
 */

/* DSIUレジスタのアドレス */
#define DSIURB		0x0f000820	/* 受信バッファレジスタ(リード時) */
#define DSIUTH		0x0f000820	/* 送信保持レジスタ(ライト時) */
#define DSIUDLL		0x0f000820	/* 分周比下位レジスタ */
#define DSIUIE		0x0f000821	/* 割込み許可レジスタ */
#define DSIUDLM		0x0f000821	/* 分周比上位レジスタ */
#define DSIUIID		0x0f000822	/* 割込み表示レジスタ(リード時) */
#define DSIUFC		0x0f000822	/* FIFO 制御レジスタ(ライト時) */
#define DSIULC		0x0f000823	/* ライン制御レジスタ */
#define DSIUMC		0x0f000824	/* モデム制御レジスタ */
#define DSIULS		0x0f000825	/* ライン状態レジスタ */
#define DSIUMS		0x0f000826	/* モデム状態レジスタ */
#define DSIUSC		0x0f000827	/* スクラッチレジスタ */

/* for DSIULC */
#define	WORD_LENGTH_8		(BIT1 | BIT0)
#define	STOP_BITS_1		0u		/* BIT2 */
#define	PARITY_NON		0u		/* BIT3, 4 */
/* BIT5,6 省略 */
#define	DIVISOR_LATCH_ACC	BIT7

/* for DSIUIE */
#define	DIS_INT			0u
#define	RECEIVE_DATA_AVAILABLE	BIT0
#define	TRANS_REG_EMPTY		BIT1
#define RECEIVE_LINE_STATUS	BIT2
#define MODEM_STATUS		BIT3

/* for DSIUMC */
#define	DTR			BIT0
#define	RTS			BIT1

/* for DSIUFC */
#define	FIFO_ENABLE		BIT0
#define	RECEIVE_FIFO_RESET	BIT1
#define	TRANS_FIFO_RESET	BIT2
#define RECEIVE_TRIG_1_BYTE	0u	/* BIT6, 7 */
#define RECEIVE_TRIG_4_BYTE	BIT6
#define	RECEIVE_TRIG_8_BYTE	BIT7
#define	RECEIVE_TRIG_14_BYTE	(BIT6 | BIT7)

/* for DSIUIID */
#define	INT_MASK		0x0eu
#define	INT_RECEIVE_DATA	BIT2
#define INT_CHAR_TIME_OUT	(BIT3 | BIT2)
#define	INT_TRANS_EMPTY		BIT1

/* for DSIULS */
#define	THRE			BIT5
#define TEMT			BIT6

/* ボーレート定義関係 */
#define	DIVISOR			XIN_CLOCK / (16u * DEVIDE_RATIO)

#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_control_block	SIOPCB;

#endif /* _MACRO_ONLY */

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND	1u		/* 送信可能コールバック */
#define SIO_ERDY_RCV	2u		/* 受信通知コールバック */

#ifndef _MACRO_ONLY

/*
 *  SIOドライバの初期化ルーチン
 */
extern void	vr4131_dsiu_initialize(void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL	vr4131_dsiu_openflag(void);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB	*vr4131_dsiu_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void	vr4131_dsiu_cls_por(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL	vr4131_dsiu_snd_chr(SIOPCB *siopcb, char c);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT	vr4131_dsiu_rcv_chr(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void	vr4131_dsiu_ena_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void	vr4131_dsiu_dis_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  SIOの割込みサービスルーチン
 */
extern void	vr4131_dsiu_dsiu_isr(void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void	vr4131_dsiu_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void	vr4131_dsiu_ierdy_rcv(VP_INT exinf);

/*
 *  カーネル起動時用の初期化 (sys_putcで利用)
 */
extern void	vr4131_dsiu_init(void);

/*
 *  シリアルI/Oポートへの文字送信（ポーリング）
 */
extern void	vr4131_dsiu_putchar_pol(char c);

#endif /* _MACRO_ONLY */

#endif /* _VR4131_DSIU_H_ */
