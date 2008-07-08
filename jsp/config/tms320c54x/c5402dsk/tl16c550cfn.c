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
 *  @(#) $Id: tl16c550cfn.c,v 1.4 2003/12/19 10:03:31 honda Exp $
 */

/*
 * TL16c550cfn 用ドライバ
 */

#include <s_services.h>
#include "tl16c550cfn.h"

/*
 * カーネル起動時用の初期化(sys_putcを使用するため)
 */
void
uart_init(void)
{
	/*  FIFOを有効にし、受信送信FIFOをクリア、
	 *  トリガレベル＝１(受信送信共通）
	 *  8データ、1ストップビット、no parity 
	 */ 
	UART_FCR = (FCR_FIFO | FCR_RFRST | FCR_TFRST | FIFO_TRIG);
	UART_LCR = 0x03;
	 /*  ボーレートレジスタにアクセス */
	UART_LCR |= LCR_DLAB;
	UART_DLL = UART_BAUD_RATE;
	UART_DLM = UART_BAUD_RATE >> 8;
	/*  BAUD RATE設定終了  */
	UART_LCR &= ~LCR_DLAB;
	UART_SCR = UART_LSR;
}

/*
 *  シリアルI/Oポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {0};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  文字を受信したか？
 */
Inline BOOL
uart_getready(SIOPCB *siopcb)
{
	return((UART_LSR & LSR_DR) != 0);
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
uart_putready(SIOPCB *siopcb)
{
	return((UART_LSR & LSR_THRE) != 0);
}

/*
 *  受信した文字の取出し
 */
Inline char
uart_getchar(SIOPCB *siopcb)
{
	return((char) UART_RBR);
}

/*
 *  送信する文字の書込み
 */
Inline void
uart_putchar(SIOPCB *siopcb, char c)
{
	UART_THR = (UB) c;
}

/*
 *  送信割込み許可
 */
Inline void
uart_enable_send(SIOPCB *siopcb)
{
	UART_IER |= IER_TIE;
}

/*
 *  送信割込み禁止
 */
Inline void
uart_disable_send(SIOPCB *siopcb)
{
	UART_IER &= ~IER_TIE;
}

/*
 *  受信割込み許可
 */
Inline void
uart_enable_rcv(SIOPCB *siopcb)
{
	UART_IER = IER_RIE;
}

/*
 *  受信割込み禁止
 */
Inline void
uart_disable_rcv(SIOPCB *siopcb)
{
	UART_IER &= ~IER_RIE;
}

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
	for (siopcb = siopcb_table, i = 0; i < TNUM_SIOP; siopcb++, i++) {
		siopcb->siopinib = &(siopinib_table[i]);
		siopcb->openflag = FALSE;
	}
}

/*
 * オープンしているポートがあるか
 */
BOOL
uart_openflag(void)
{
	return(siopcb_table[0].openflag);
}



/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
uart_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB	*siopcb;

	siopcb = get_siopcb(siopid);
	
	/*  FIFOを有効にし、受信送信FIFOをクリア、
	 *  トリガレベル＝１(受信送信共通）
	 *  8データ、1ストップビット、no parity 
	 */
	UART_FCR = (FCR_FIFO | FCR_RFRST | FCR_TFRST | FIFO_TRIG);
	UART_LCR = 0x03;
	/*  ボーレートレジスタにアクセス */
	UART_LCR |= LCR_DLAB;
	UART_DLL = UART_BAUD_RATE;
	UART_DLM = UART_BAUD_RATE >> 8;
	/*  BAUD RATE設定終了  */
	UART_LCR &= ~LCR_DLAB;
	/*  エラーフラグをクリア  */
	UART_SCR = UART_LSR;
	UART_IER = IER_RIE;       /*  受信割り込み許可  */
	
	siopcb->exinf = exinf;
	siopcb->openflag = TRUE;
	siopcb->getready = siopcb->putready = FALSE;

	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
uart_cls_por(SIOPCB *siopcb)
{
	/* 送信受信割り込み要求禁止 */
	UART_IER = 0;
	
	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
uart_snd_chr(SIOPCB *siopcb, char chr)
{
	if (uart_putready(siopcb)) {
		uart_putchar(siopcb, chr);
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
	if (uart_getready(siopcb)) {
		return((INT)(UB) uart_getchar(siopcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	    case SIO_ERDY_SND:
		uart_enable_send(siopcb);
		break;
	    case SIO_ERDY_RCV:
		uart_enable_rcv(siopcb);
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
	    case SIO_ERDY_SND:
		uart_disable_send(siopcb);
		break;
	    case SIO_ERDY_RCV:
		uart_disable_rcv(siopcb);
		break;
	}
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
static void
uart_isr_siop(SIOPCB *siopcb)
{
	if (uart_getready(siopcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		uart_ierdy_rcv(siopcb->exinf);
	}
	if (uart_putready(siopcb)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		uart_ierdy_snd(siopcb->exinf);
	}
}

/*
 *  SIOの割込みサービスルーチン
 */
void
uart_isr()
{
	if (siopcb_table[0].openflag) {
		uart_isr_siop(&(siopcb_table[0]));
	}
}
