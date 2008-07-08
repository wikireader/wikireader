/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: hw_serial.c,v 1.2 2007/05/28 02:03:55 honda Exp $
 */

/*
 *	シリアルI/Oデバイス（SIO）ドライバ（M32102内蔵非同期シリアル用）
 */

#include <sil.h>
#include <hw_serial.h>

#include <cpu_rename.h>
#include <sys_rename.h>

SIOPCB siopcb_table[TNUM_PORT];

extern FP InterruptHandlerEntry[];

/* 
 *  作業用関数 
 */
#define ID_PORT(x)		((x) + 1)
#define INDEX_PORT(x)	((x) - 1)
#define GET_SIOPCB(x)	(&siopcb_table[INDEX_PORT(x)])

/*
 *  シリアルポートの初期化
 */
void
sio_initialize(void)
{
	int i;

	for(i=0;i<TNUM_PORT;i++)
	{
		siopcb_table[i].flags = SIO_TYP_M32RUART;
		siopcb_table[i].port  = i;
		siopcb_table[i].exinf = 0;

		InterruptHandlerEntry[INT_SIO0RCV + (siopcb_table[i].port * 2) - 1] = sio_handler_in;
		InterruptHandlerEntry[INT_SIO0XMT + (siopcb_table[i].port * 2) - 1] = sio_handler_out;
	}
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
sio_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB * siopcb = GET_SIOPCB(siopid);

	siopcb->exinf = exinf;

	switch(SIO_TYP(siopcb->flags))
	{
	case SIO_TYP_M32RUART:
		/* UART初期化 */
		sil_wrb_mem((void *)(SIOCR(siopcb->port)+3), 0);

		/* ポートを開く(UART0) */
		sil_wrb_mem((void *)PDATA(5), 0);
		sil_wrb_mem((void *)PDIR(5),  0x80);

		/* UART, Non-parity, 1 stop-bit */
		sil_wrb_mem((void *)(SIOMOD0(siopcb->port)+3), 0);

		/* 8bit, internal clock */
		sil_wrh_mem((void *)(SIOMOD1(siopcb->port)+2), 0x0800);

		/* m3a_za36 10MHzで57600bps */
		sil_wrh_mem((void *)(SIOBAUR(siopcb->port)+2),SERIAL_CLKDIV);
		sil_wrb_mem((void *)(SIORBAUR(siopcb->port)+3), SERIAL_CLKCMP);

		/* 送受信割込み発生許可 */
		sil_wrh_mem((void *)(SIOTRCR(siopcb->port)+2), 0x0006);

		/* ステータスクリア, 送受信動作開始 */
		sil_wrb_mem((void *)(SIOCR(siopcb->port)+2), 0x3);
		sil_wrb_mem((void *)(SIOCR(siopcb->port)+3), 0x3);

		/* 送受信割込み要求受付許可 */
		sil_wrh_mem((void *)(ICUCR(SIO,siopcb->port)+2), 0x1000); /* 受信割込み要求許可 */
		sil_wrh_mem((void *)(ICUCR(SIO,siopcb->port)+6), 0x1000); /* 送信割込み要求許可 */

		/* TxD,RxDピン有効(UART0) */
		sil_wrh_mem((void *)PMOD(5), 0x5500);

		break;
	}
	return siopcb;
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
sio_cls_por(SIOPCB *siopcb)
{
	switch(SIO_TYP(siopcb->flags))
	{
	case SIO_TYP_M32RUART:
		/* 送受信動作を禁止する */
		sil_wrb_mem((void *)(SIOCR(siopcb->port)+3), 0);
		break;
	}
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT sio_rcv_chr(SIOPCB * siopcb)
{
	switch(SIO_TYP(siopcb->flags))
	{
	case SIO_TYP_M32RUART:
		return sil_reb_mem((void *)(SIORXB(siopcb->port)+3));
	}
	return -1;
}


/*
 *  文字を受信したか？
 */

inline BOOL
hw_port_getready(SIOPCB *p)
{
	switch(SIO_TYP(p->flags))
	{
	case SIO_TYP_M32RUART:
		return (sil_reb_mem((void *)(SIOSTS(p->port)+3)) & 0x4) != 0 ? TRUE : FALSE;
	}
	return FALSE;
}

/*
 *  シリアルI/Oポートからのコールバック許可
 */
void sio_ena_cbr(SIOPCB * siopcb, UINT cbrtn)
{}

/*
 *  シリアルI/Oポートからのコールバック禁止
 */
void sio_dis_cbr(SIOPCB * siopcb, UINT cbrtn)
{}

/*
 * シリアルポートの受信割込みハンドラ
 */
void sio_handler_in(void)
{
	int port;

	for(port = 0; port < TNUM_PORT; ++ port){
		if( hw_port_getready(&siopcb_table[port]) == TRUE )
			{
				sio_ierdy_rcv(GET_SIOPCB(ID_PORT(port))->exinf);
			}	
	}
}

/*
 * シリアルポートの送信割込みハンドラ
 */
void sio_handler_out(void)
{
	int port;
	
	for(port = 0; port < TNUM_PORT; ++ port) {
		if((siopcb_table[port].flags & SIO_STS_SENDING) != 0) {		
			//送信ステータス初期化
			sil_wrb_mem((void *)(SIOCR(port)+2), 0x1);
			siopcb_table[port].flags &= ~SIO_STS_SENDING;

			sio_ierdy_snd(GET_SIOPCB(ID_PORT(port))->exinf);
		}
	}
}		
