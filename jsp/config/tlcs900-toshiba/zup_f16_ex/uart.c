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
 *  @(#) $Id: uart.c,v 1.1 2006/04/10 08:19:25 honda Exp $
 */

/*
 *  非同期通信 (UART)シリアルチャンネルデバイス（SIO）ドライバ
 *  （Zup-F16拡張ボード用）
 */
#include <s_services.h>
#include <sil.h>
#include <tmp91cy22.h>
#include <uart.h>

/*
 *  シリアルI/O制御レジスタへのオフセット定義
 */
#define		TOFFSET_SC0BUF			0x0000
#define		TOFFSET_SC0CR			0x0001
#define		TOFFSET_SC0MOD0			0x0002
#define		TOFFSET_BR0CR			0x0003
#define		TOFFSET_BR0ADD			0x0004
#define		TOFFSET_SC0MOD1			0x0005
 
/*
 *  シリアルI/Oポート状態フラグの定義
 */
#define		TBIT_STS_DEF			0x00
#define		TBIT_TXB_EMPTY			0x01
#define		TBIT_ENE_TXCBR			0x02
#define		TBIT_ENE_RXCBR			0x04
#define		TBIT_LOG_PORT			0x80

/*
 *  シリアルI/Oポート初期化ブロック
 *
 *  ID = 1 をuart1，ID = 2 をuart0に対応させている．
 */
static const SIOPINIB siopinib_table[TNUM_PORT] = {
	{ TADR_SFR_SC1BUF, TADR_SFR_INTES1, 0x08, (TBIT_SIOSMU8 | TBIT_SIORXE | TBIT_SIOSCBRG),
	  0x00, BRCR_19200, BRADD_19200, TBIT_RX1_CLR }	/* ID1用 UART1 19200bps	*/
/*	{ TADR_SFR_SC1BUF, TADR_SFR_INTES1, 0x08, (TBIT_SIOSMU8 | TBIT_SIORXE | TBIT_SIOSCBRG),
	  0x00, BRCR_38400, BRADD_38400, TBIT_RX1_CLR }*/	/* ID1用 UART1 38400bps	*/
/*	{ TADR_SFR_SC0BUF, TADR_SFR_INTES0, 0x01, (TBIT_SIOSMU8 | TBIT_SIORXE | TBIT_SIOSCBRG),
	  0x00, BRCR_19200, BRADD_19200, TBIT_RX0_CLR }*/	/* ID2用 UART0 19200bps	*/
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
static SIOPCB siopcb_table[TNUM_PORT];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  SIOドライバの初期化ルーチン
 */
void
uart_initialize()
{
	SIOPCB	*siopcb;
	UINT	i;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (siopcb = siopcb_table, i = 0; i < TNUM_PORT; siopcb++, i++) {
		siopcb->siopinib = &(siopinib_table[i]);
		siopcb->sts_flag = TBIT_STS_DEF;
		siopcb->rxb = -1;
	}
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
uart_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB		*siopcb;
	const SIOPINIB	*siopinib;
	int i;

	/* ポートIDから管理ブロックを取得	*/
	siopcb = get_siopcb(siopid);
	siopinib = siopcb->siopinib;

	/* 管理ブロックに情報設定	*/
	siopcb->sts_flag |= TBIT_TXB_EMPTY;
	siopcb->exinf = exinf;
	if( siopid == LOGTASK_PORTID ){	/* システムログ用のポートの場合	*/
		siopcb->sts_flag |= TBIT_LOG_PORT;
		if( log_io_busy == TRUE ){
			/* 送信中の場合は完了まで待つ	*/
			while( (sil_reb_mem((VP)(siopinib->hint)) & TBIT_INTC_H) == 0 );
		}
		log_io_busy = FALSE;	/* 初期化なので明示的に行っている   */
	}

	/* 受信停止	*/
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_SC0MOD0), 0x00 );
	/* ポート設定(TXDとして使用)	*/
	sil_wrb_mem((VP)(TADR_SFR_P9FC), siopinib->pcrfc_def );
	sil_wrb_mem((VP)(TADR_SFR_P9CR), siopinib->pcrfc_def );

	/* 動作モード設定	*/
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_SC0MOD0), siopinib->scmod0_def );
	sil_reb_mem((VP)(siopinib->cntrl+TOFFSET_SC0CR));	/* 読出しによりエラーフラグクリア	*/
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_SC0CR), siopinib->sccr_def );
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_BR0CR), siopinib->brcr_def);
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_BR0ADD), siopinib->bradd_def);
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_SC0MOD1), TBIT_SIOI2S);

	/* シリアル割込みの設定および要求フラグクリア	 */
	sil_wrb_mem((VP)TADR_SFR_INTCLR, (siopinib->int_clr) );		/* 受信要求クリア	*/
	sil_wrb_mem((VP)TADR_SFR_INTCLR, (VB)(siopinib->int_clr + 1) );	/* 送信要求クリア	*/
	sil_wrb_mem((VP)(siopinib->hint), (VB)((INT_LEVEL_UART << 4) | INT_LEVEL_UART) );

	/*
	 * ダミーデータ受信
	 */
	sil_reb_mem((VP)(siopinib->cntrl+TOFFSET_SC0BUF));
	sil_reb_mem((VP)(siopinib->cntrl+TOFFSET_SC0BUF));

	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
uart_cls_por(SIOPCB *siopcb)
{
	const SIOPINIB	*siopinib;

	siopinib = siopcb->siopinib;

	/* 受信停止	*/
	sil_wrb_mem((VP)(siopinib->cntrl+TOFFSET_SC0MOD0), 0x00 );

	/* システムログ用ポートの処理	*/
	if( (siopcb->sts_flag & TBIT_LOG_PORT) == TBIT_LOG_PORT ){
		if( log_io_busy == TRUE ){
			/* 送信中の場合は完了まで待つ	*/
			while( (sil_reb_mem((VP)(TADR_SFR_INTES0)) & TBIT_INTC_H) == 0 );
		}
		log_io_busy = FALSE;
	}

	/* シリアル割込みの禁止および要求フラグクリア	 */
	sil_wrb_mem((VP)(siopinib->hint), 0x00 );
	sil_wrb_mem((VP)TADR_SFR_INTCLR, (siopinib->int_clr) );		/* 受信要求クリア	*/
	sil_wrb_mem((VP)TADR_SFR_INTCLR, (VB)(siopinib->int_clr + 1) );	/* 送信要求クリア	*/
	/* ポート設定(ポートとして使用)	*/
	sil_wrb_mem((VP)(TADR_SFR_P9FC), 0x00 );
	sil_wrb_mem((VP)(TADR_SFR_P9CR), 0x00 );

	siopcb->sts_flag = TBIT_STS_DEF;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
uart_snd_chr(SIOPCB *siopcb, char c)
{
	if( (siopcb->sts_flag & TBIT_TXB_EMPTY) == TBIT_TXB_EMPTY ){
		siopcb->sts_flag &= (UB)~TBIT_TXB_EMPTY;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl+TOFFSET_SC0BUF), c);

	/* システムログ用ポートの処理	*/
		if( (siopcb->sts_flag & TBIT_LOG_PORT) == TBIT_LOG_PORT ){
			log_io_busy = TRUE;	/* 送信中	*/
		}
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
uart_rcv_chr(SIOPCB *siopcb)
{
	return(siopcb->rxb);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case UART_ERDY_SND:
		siopcb->sts_flag |= TBIT_ENE_TXCBR;
		break;
	case UART_ERDY_RCV:
		siopcb->sts_flag |= TBIT_ENE_RXCBR;
		break;
	default:
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case UART_ERDY_SND:
		siopcb->sts_flag &= (UB)~TBIT_ENE_TXCBR;
		break;
	case UART_ERDY_RCV:
		siopcb->sts_flag &= (UB)~TBIT_ENE_RXCBR;
		break;
	default:
		break;
	}
}

/*
 *  ポートID=1に指定されているSIOチャンネル(uart1)からの受信割込み
 */
void serial_in_handler1()
{
	SIOPCB *siopcb = &siopcb_table[0];

	/* エラーの場合処理しない   */
	if( ( sil_reb_mem((VP)(siopcb->siopinib->cntrl+TOFFSET_SC0CR)) & 0x1c ) == 0 ){

		/* データ受信	*/
		siopcb->rxb = (INT)sil_reb_mem((VP)(siopcb->siopinib->cntrl+TOFFSET_SC0BUF));

		/* コールバックが許可されている場合	*/
		if( (siopcb->sts_flag & TBIT_ENE_RXCBR) == TBIT_ENE_RXCBR ){
			/*
			 *  受信通知コールバックルーチンを呼び出す．
			 */
			uart_ierdy_rcv(siopcb->exinf);
		}
	}
}

/*
 *  ポートID=1に指定されているSIOチャンネル(uart1)からの送信割込み
 */
void serial_out_handler1()
{
	SIOPCB *siopcb = &siopcb_table[0];

	/* データ送信完了フラグON	*/
	siopcb->sts_flag |= TBIT_TXB_EMPTY;

	/* システムログ用ポートの処理	*/
	if( (siopcb->sts_flag & TBIT_LOG_PORT) == TBIT_LOG_PORT ){
		log_io_busy = FALSE;	/* 送信完了	*/
	}

	/* コールバックが許可されている場合	*/
	if( (siopcb->sts_flag & TBIT_ENE_TXCBR) == TBIT_ENE_TXCBR ){
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		uart_ierdy_snd(siopcb->exinf);
	}
}

#if TNUM_PORT >= 2
/*
 *  ポートID=2に指定されているSIOチャンネル(uart0)からの受信割込み
 */
void serial_in_handler2()
{
	SIOPCB *siopcb = &siopcb_table[1];

	/* エラーの場合処理しない   */
	if( ( sil_reb_mem((VP)(siopcb->siopinib->cntrl+TOFFSET_SC0CR)) & 0x1c ) == 0 ){

		/* データ受信	*/
		siopcb->rxb = (INT)sil_reb_mem((VP)(siopcb->siopinib->cntrl+TOFFSET_SC0BUF));

		/* コールバックが許可されている場合	*/
		if( (siopcb->sts_flag & TBIT_ENE_RXCBR) == TBIT_ENE_RXCBR ){
			/*
			 *  受信通知コールバックルーチンを呼び出す．
			 */
			uart_ierdy_rcv(siopcb->exinf);
		}
	}
}

/*
 *  ポートID=2に指定されているSIOチャンネル(uart0)からの送信割込み
 */
void serial_out_handler2()
{
	SIOPCB *siopcb = &siopcb_table[1];

	/* データ送信完了フラグON	*/
	siopcb->sts_flag |= TBIT_TXB_EMPTY;

	/* システムログ用ポートの処理	*/
	if( (siopcb->sts_flag & TBIT_LOG_PORT) == TBIT_LOG_PORT ){
		log_io_busy = FALSE;	/* 送信完了	*/
	}

	/* コールバックが許可されている場合	*/
	if( (siopcb->sts_flag & TBIT_ENE_TXCBR) == TBIT_ENE_TXCBR ){
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		uart_ierdy_snd(siopcb->exinf);
	}
}

#endif

