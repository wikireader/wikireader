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
 *  ターゲット依存シリアルI/Oモジュール
 */

#include "jsp_kernel.h"
#include "hw_serial.h"

/*
 *  シリアルポートの初期化ブロック
 */

const SIOPINIB siopinib_table[TNUM_PORT] = {

	{
		0,
		S1C33_INHNO_SERIAL0ERR,
		S1C33_INHNO_SERIAL0RX,
		S1C33_INHNO_SERIAL0TX,
	},
#if TNUM_PORT == 2
	{
		1,
		S1C33_INHN1_SERIAL0ERR,
		S1C33_INHN1_SERIAL0RX,
		S1C33_INHN1_SERIAL0TX,
	},
#endif	/* of #if TNUM_PORT == 2 */
};


/*
 *  シリアルポートの制御ブロック
 */

SIOPCB siopcb_table[TNUM_PORT];

/*
 *  SIO ID から管理ブロックへの変換マクロ
 */
#define get_siopinib(sioid)	(&(siopinib_table[INDEX_SIO(sioid)]))
#define get_siopcb(sioid)	(&(siopcb_table[INDEX_SIO(sioid)]))

/*
 *  受信割り込みサービスルーチン.
 */

void
sio_in_service(ID sioid)
{
	SIOPCB	*pcb;

	sio_clr_errflag(INDEX_SIO(sioid));
	pcb = get_siopcb(sioid);
	if (pcb->rx_cbrflag){
		/* 受信可能コールバックルーチンを呼出す。*/
		sio_ierdy_rcv(pcb->exinf);
	}
}

/*
 *  送信完了割り込みサービスルーチン
 */

void
sio_out_service(ID sioid)
{
	SIOPCB	*pcb;

	pcb = get_siopcb(sioid);
	if (pcb->tx_cbrflag){
		/* 送信可能コールバックルーチンを呼出す。*/
		sio_ierdy_snd(pcb->exinf);
	}
}

#ifdef SIO_ERR_HANDLER

/*
 *  受信エラー割り込みサービスルーチン
 */

void
sio_err_service(ID sioid)
{
	SIOPCB	*pcb;
	UB	status;

	/* エラーフラグをクリア */
	sio_clr_errflag(INDEX_SIO(sioid));

	/* ポートを再度初期化する */
	sio_opn_por(sioid, (VP_INT)NULL);
}

#endif	/* of #ifdef SIO_ERR_HANDLER */

/*
 *  シリアルポート制御テーブル初期化
 */

void
sio_initialize (void)
{
	int iLoop;

	/*
	 *  SIO 制御ブロックの初期化
	 */
	for (iLoop = 0 ; iLoop < TNUM_PORT ; iLoop++ ) {
		siopcb_table[iLoop].inib     = &siopinib_table[iLoop];
		siopcb_table[iLoop].exinf    = (VP_INT)NULL;
		siopcb_table[iLoop].openflag = FALSE;
		siopcb_table[iLoop].tx_cbrflag  = FALSE;
		siopcb_table[iLoop].rx_cbrflag  = FALSE;
	}
}

/*
 *  シリアルポートの初期化
 */
SIOPCB
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
	 *  8bit調歩同期  パリティなし ストップビット1bit  内部クロック 57,600bps
	 */
	((s1c33PEPort_t *)S1C33_PORT_BASE)->stFuncSelect[0].bCfp03 = 0x05;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bIrDA    = 0x10;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bControl = 0x00;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bStatus  = 0x00;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bBRTRun  = 0x00;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bBRTRdl  = 51;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bBRTRdm  = 0x00;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bBRTRun  = 0x01;
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bControl = 0x03;

	/*
	 *  割込み要因をクリア
	 */
	(*(s1c33Intc_t *) S1C33_INTC_BASE).bIntFactor[6] = (0x07 << INDEX_SIO(siopid));

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
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[INDEX_SIO(siopid)].bControl |= 0xc0;

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
 *  コールバックを許可する
 */
void
sio_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	UB	ch_no, sio_ctl;

	ch_no  = siopcb->inib->ch_no;
	sio_ctl = ((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bControl;
	switch (cbrtn){
	case	SIO_ERDY_SND:
		siopcb->tx_cbrflag = TRUE;
		break;
	case	SIO_ERDY_RCV:
		siopcb->rx_cbrflag = TRUE;
		break;
	}

	return;
}

/*
 *  コールバックを禁止する
 */
void
sio_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	UB	ch_no, sio_ctl;

	ch_no  = siopcb->inib->ch_no;
	sio_ctl = ((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bControl;
	switch (cbrtn){
	case	SIO_ERDY_SND:
		siopcb->tx_cbrflag = FALSE;
		break;
	case	SIO_ERDY_RCV:
		siopcb->rx_cbrflag = FALSE;
		break;
	}

	/*
	 *  送信機能有効ビットを設定する
	 */
	((s1c33Serial_t *)S1C33_SERIAL_BASE)->stChannel01[ch_no].bControl = sio_ctl;

	return;
}

/*
 *  SIO 割り込み
 */

void
sio_in_handler (void)
{
	sio_clr_factor(SIO_INT_RCV);
	sio_in_service(1);
}

void
sio_out_handler (void)
{
	sio_clr_factor(SIO_INT_SND);
	sio_out_service(1);
}

#ifdef SIO_ERR_HANDLER

void
sio_err_handler (void)
{
	sio_clr_factor(SIO_INT_ERR);
	sio_err_service(1);
	}

#endif	/* of #ifdef SIO_ERR_HANDLER */

#if TNUM_PORT >= 2

void
sio_in2_handler (void)
{
	sio_clr_factor(SIO_INT_RCV << 3);
	sio_in_handler(2);
	}

void
sio_out2_handler (void)
{
	sio_clr_factor(SIO_INT_SND << 3);
	sio_out_handler(2);
	}

#ifdef SIO_ERR_HANDLER

void
sio_err2_handler (void)
{
	sio_clr_factor(SIO_INT_ERR << 3)
	sio_err_handler(2);
	}

#endif	/* of #ifdef SIO_ERR_HANDLER */

#endif	/* of #if TNUM_PORT >= 2 */
