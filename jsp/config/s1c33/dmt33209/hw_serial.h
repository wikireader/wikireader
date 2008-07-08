/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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

/*
 * ターゲット依存シリアルI/Oモジュール(DMT33209)
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include "s_services.h"

/*
 *  シリアルI/Oの割込みハンドラのベクタ番号
 */
#define INHNO_SERIAL_IN		S1C33_INHNO_SERIAL0RX
#define INHNO_SERIAL_OUT	S1C33_INHNO_SERIAL0TX
#define INHNO_SERIAL_ERR	S1C33_INHNO_SERIAL0ERR

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND		1	/* 送信可能コールバック	*/
#define SIO_ERDY_RCV		2	/* 受信通知コールバック	*/

#define INDEX_SIO(sioid)	((UINT)((sioid) - 1))

/*
 *  SIO割り込み要因フラグ ビット位置
 */
#define SIO_INT_ERR		0x01
#define	SIO_INT_RCV		0x02
#define	SIO_INT_SND		0x04

/*
 * エラーフラグ ビット位置
 */
#define SIO_ERR_FRAME		0x10
#define SIO_ERR_PARITY		0x08
#define SIO_ERR_OVERRUN		0x04

#ifndef _MACRO_ONLY
/*
 *	シリアルポートのハードウェア依存情報の定義
 */
typedef struct {
	UB	ch_no;		/* SIOチャネル番号		*/
	UB	err_vector;	/* 通信エラー割り込みベクタ番号	*/
	UB	rx_vector;	/* 受信割り込みベクタ番号	*/
	UB	tx_vector;	/* 送信完了割り込みベクタ番号	*/
} SIOPINIB;

/*
 *  シリアルポートの制御ブロック
 */
typedef struct {
	const SIOPINIB	*inib;		/* 初期化ブロック	*/
	VP_INT		exinf;		/* 拡張情報		*/
	BOOL		openflag;	/* オープン済みフラグ	*/
	BOOL		tx_cbrflag;	/* 送信コールバック実行フラグ*/
	BOOL		rx_cbrflag;	/* 受信コールバック実行フラグ*/
} SIOPCB;

extern SIOPCB siopcb_table[TNUM_PORT];

/*
 *  外部関数の参照
 */
extern void	sio_initialize (void);
extern void	sio_ena_cbr(SIOPCB *siopcb, UINT cbrtn);
extern void	sio_dis_cbr(SIOPCB *siopcb, UINT cbrtn);
extern void	serial_handler_in(ID portid);
extern void	serial_handler_out(ID portid);
extern void	sio_ierdy_snd(VP_INT exinf);
extern void	sio_ierdy_rcv(VP_INT exinf);
extern ER	ena_int(INTNO);
extern ER	dis_int(INTNO);

/*
 *  シリアルポートの初期化
 */
Inline SIOPCB
*sio_opn_por(ID siopid, VP_INT exinf)
{
	B	temp;

	/*
	 *  割り込みの禁止
	 */
	if(siopid == 1){
		dis_int(S1C33_INHNO_SERIAL0RX);
		dis_int(S1C33_INHNO_SERIAL0TX);
#ifdef SIO_ERR_HANDLER
		dis_int(S1C33_INHNO_SERIAL0ERR);
#endif
	} else {
		dis_int(S1C33_INHNO_SERIAL1RX);
		dis_int(S1C33_INHNO_SERIAL1TX);
#ifdef SIO_ERR_HANDLER
		dis_int(S1C33_INHNO_SERIAL1ERR);
#endif
	}

	/*
	 *  シリアルI/Fの設定
	 *
	 *  8bit調歩同期  パリティなし ストップビット1bit  外部クロック
	 */
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bControl = 0x07;

	((s1c33Port_t *)S1C33_PORT_BASE)->stPPort[INDEX_SIO(siopid)].bFuncSwitch = 0x07;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bIrDA = 0x00;

	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bControl = 0xc7;

	/*
	 *  割込み要因をクリア
	 */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntFactor[6] |= (0x07 << INDEX_SIO(siopid));

	/*
	 *  受信バッファのクリア
	 */
	temp = ((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bRxd;

	/*
	 *  制御情報の設定
	 */
	siopcb_table[INDEX_SIO(siopid)].openflag = TRUE;
	if(exinf != NULL){
		siopcb_table[INDEX_SIO(siopid)].exinf = exinf;
	}

	/*
	 *  割り込みの許可
	 */
	if(siopid == 1){
		ena_int(S1C33_INHNO_SERIAL0RX);
		ena_int(S1C33_INHNO_SERIAL0TX);
#ifdef SIO_ERR_HANDLER
		ena_int(S1C33_INHNO_SERIAL0ERR);
#endif
	} else {
		ena_int(S1C33_INHNO_SERIAL1RX);
		ena_int(S1C33_INHNO_SERIAL1TX);
#ifdef SIO_ERR_HANDLER
		ena_int(S1C33_INHNO_SERIAL1ERR);
#endif
	}

	return (&(siopcb_table[INDEX_SIO(siopid)]));
}

/*
 *  通信の停止
 */
Inline void
sio_cls_por(SIOPCB *siopcb)
{
	UB	ch_no;

	ch_no = siopcb->inib->ch_no;
	/*
	 *  転送完了を待つ
	 */
	while(((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bStatus & 0x20){
		;
	}

	/*
	 *  通信を禁止する
	 */
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bControl = 0x07;

	return;
}

/*
 *  送信機能の状態を取得
 */
Inline BOOL
sio_get_txready(UB ch_no)
{
	UB	bSioSts;

	bSioSts = ((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bStatus;

	return(!(bSioSts & 0x20));
}

/*
 *  受信機能の状態を取得
 */
Inline BOOL
sio_get_rxready(UB ch_no)
{
	UB	bSioSts;

	bSioSts = ((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bStatus;

	return(bSioSts & 0x01);
}

/*
 *  受信した1文字を読み込む
 */
Inline INT
sio_rcv_chr(SIOPCB *siopcb)
{
	BOOL	blRxSts;
	UB	ch_no;


	ch_no = siopcb->inib->ch_no;
	blRxSts  = sio_get_rxready(ch_no);

	if(blRxSts){
		return (((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bRxd);
	}

	return (-1);
}

/*
 *  送信する1文字を書き込む
 */
Inline BOOL
sio_snd_chr(SIOPCB *siopcb, char c)
{
	BOOL	blTxSts;
	UB	ch_no;

	ch_no = siopcb->inib->ch_no;
	blTxSts = sio_get_txready(ch_no);
	if(blTxSts){
		((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bTxd = c;
	}

	return(blTxSts);
}

/*
 *  指定されたSIOチャネルの割り込み要因をクリアする
 */
Inline void
sio_clr_factor(UB bFlag)
{
	((volatile s1c33Intc_t *) S1C33_INTC_BASE)->bIntFactor[6] |= bFlag;
}

/*  指定されたSIOチャネルのエラーフラグをクリアする
 *
 */
Inline void
sio_clr_errflag(UB ch_no)
{
	((volatile s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bStatus
		&= ~(SIO_ERR_OVERRUN | SIO_ERR_PARITY | SIO_ERR_FRAME);
}

#endif /* _MACRO_ONLY   */
#endif /* _HW_SERIAL_H_ */
