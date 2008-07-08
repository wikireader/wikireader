/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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

/*
 *	シリアルI/Oデバイス（SIO）ドライバ（H8Sファミリー用）
 */
#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <h8s_sci.h>

/*
 *  SIOの割込みハンドラのベクタ番号
 */
/* ポート1 */
#ifndef OMIT_SCI0
#define INHNO_SERIAL1_ERROR	IRQ_ERI0
#define INHNO_SERIAL1_IN	IRQ_RXI0
#define INHNO_SERIAL1_OUT	IRQ_TXI0
#endif /* OMIT_SCI0 */

#if TNUM_PORT >= 2
/* ポート2 */
#define INHNO_SERIAL2_ERROR	IRQ_ERI1
#define INHNO_SERIAL2_IN	IRQ_RXI1
#define INHNO_SERIAL2_OUT	IRQ_TXI1
#endif /* TNUM_PORT */

#if TNUM_PORT >= 3
/* ポート3 */
#define INHNO_SERIAL3_ERROR	IRQ_ERI2
#define INHNO_SERIAL3_IN	IRQ_RXI2
#define INHNO_SERIAL3_OUT	IRQ_TXI2
#endif /* TNUM_PORT */


/*
 *  モジュールストップモードコントロールレジスタのビット定義
 */
#ifndef OMIT_SCI0

#if TNUM_SIOP == 1
#define MSTPCR_SCI	MSTPCR_SCI0
#endif /* TNUM_SIOP == 1 */

#if TNUM_SIOP == 2
#define MSTPCR_SCI	(MSTPCR_SCI0 | MSTPCR_SCI1)
#endif /* TNUM_SIOP == 2 */

#if TNUM_SIOP == 3
#define MSTPCR_SCI	(MSTPCR_SCI0 | MSTPCR_SCI1 | MSTPCR_SCI2)
#endif /* TNUM_SIOP == 3 */

#else /* OMIT_SCI0 */

#if TNUM_SIOP == 1
#error Invalid serial port ID.
#endif /* TNUM_SIOP == 1 */

#if TNUM_SIOP == 2
#define MSTPCR_SCI	MSTPCR_SCI1
#endif /* TNUM_SIOP == 2 */

#if TNUM_SIOP == 3
#define MSTPCR_SCI	(MSTPCR_SCI1 | MSTPCR_SCI2)
#endif /* TNUM_SIOP == 3 */

#endif /* OMIT_SCI0 */

/*
 *  SIOドライバの初期化ルーチン
 */
#define	sio_initialize		h8s_sci_initialize

/*
 *  カーネル起動時用の初期化 (sys_putcで利用)
 */
#define sio_init		h8s_sci_init

#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポートのオープン
 */
Inline SIOPCB *
sio_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB	*siopcb;
	BOOL	openflag;

	/*
	 *  オープンしたポートがあるかを openflag に読んでおく。
	 */
	openflag = h8s_sci_openflag();

	/*
	 * モジュールストップモード解除
	 */
	h8s_andh_reg( (VP) MSTPCR, ~MSTPCR_SCI );

	/*
	 *  デバイス依存のオープン処理
	 */
	siopcb = h8s_sci_opn_por(siopid, exinf);

	/*
	 *  シリアルI/O割込みの割込みレベルを設定する。
	 */
	if (!openflag) {
		/* 割込みレベル設定 */
#ifndef OMIT_SCI0
		icu_set_ilv( IPRJ, IPR_LOW, SCI0_INT_LVL );
#endif 	/*  OMIT_SCI0  */

#if TNUM_SIOP >= 2
		icu_set_ilv( IPRK, IPR_UPR, SCI1_INT_LVL );
#endif /* TNUM_SIOP >= 2 */

#if TNUM_SIOP >= 3
		icu_set_ilv( IPRK, IPR_LOW, SCI0_INT_LVL );
#endif /* TNUM_SIOP >= 3 */

	}
	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
Inline void
sio_cls_por(SIOPCB *siopcb)
{
	/*
	 *  デバイス依存のクローズ処理
	 */
	h8s_sci_cls_por(siopcb);

	/*
	 *  モジュールストップモードセット
	 */
	h8s_orh_reg( (VP) MSTPCR,   MSTPCR_SCI);

	/*
	 *  シリアルI/O割込みをマスクする。
	 */
	if (!h8s_sci_openflag()) {
		/* 割込みマスク処理 */
		icu_set_ilv( IPRJ, IPR_LOW, 0 );
#if TNUM_SIOP >= 2
		icu_set_ilv( IPRK, IPR_UPR, 0 );
#endif /* TNUM_SIOP >= 2 */

#if TNUM_SIOP >= 3
		icu_set_ilv( IPRK, IPR_LOW, 0 );
#endif /* TNUM_SIOP >= 3 */

	}
}

#endif /* _MACRO_ONLY */

/*
 *  SIOの割込みハンドラ
 *    sio1_handler_in    : 受信割込みハンドラ
 *    sio1_handler_out   : 送信割込みハンドラ
 *    sio1_handler_error : 受信エラー割込みハンドラ
 */
/* ポート1 */
#define	sio1_handler_in		h8s_sci0_isr_in
#define	sio1_handler_out	h8s_sci0_isr_out
#define	sio1_handler_error	h8s_sci0_isr_error

#if TNUM_SIOP >= 2
/* ポート2 */
#define	sio2_handler_in		h8s_sci1_isr_in
#define	sio2_handler_out	h8s_sci1_isr_out
#define	sio2_handler_error	h8s_sci1_isr_error
#endif /* TNUM_SIOP >= 2 */

#if TNUM_SIOP >= 3
/* ポート3 */
#define	sio3_handler_in		h8s_sci2_isr_in
#define	sio3_handler_out	h8s_sci2_isr_out
#define	sio3_handler_error	h8s_sci2_isr_error
#endif /* TNUM_SIOP >= 3 */

/*
 *  シリアルI/Oポートへの文字送信
 */
#define	sio_snd_chr		h8s_sci_snd_chr

/*
 *  シリアルI/Oポートへの文字送信（ポーリング）
 */
#define	sio_snd_chr_pol		h8s_sci_putchar_pol

/*
 *  シリアルI/Oポートからの文字受信
 */
#define	sio_rcv_chr		h8s_sci_rcv_chr

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
#define	sio_ena_cbr		h8s_sci_ena_cbr

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
#define	sio_dis_cbr		h8s_sci_dis_cbr

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
#define	sio_ierdy_snd		h8s_sci_ierdy_snd

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
#define	sio_ierdy_rcv		h8s_sci_ierdy_rcv

#endif /* _HW_SERIAL_H_ */
