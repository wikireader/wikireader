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
 *  @(#) $Id: hw_serial.h,v 1.9 2003/12/15 07:19:22 takayuki Exp $
 */



/*
 *  TOPPERS/JSP for Windows - シリアルエミュレータ
 */

#ifndef __HW_SERIAL_H
#define __HW_SERIAL_H

#include <s_services.h>

/*
 *  コールバックルーチンの識別番号
 *  OPB_UARTLITE は，送受信割込みが分かれていないため，意味はない．
 */
#define SIO_ERDY_SND    1u              /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u              /* 受信通知コールバック */

	/* デバイスを開く側の設定 */
#define SIO_MOD_MASK		0x80000000
#define SIO_MOD(x)			((x) & SIO_MOD_MASK)
#define SIO_MOD_SERVER		0x80000000
#define SIO_MOD_CLIENT		0x00000000

	/* デバイスの種別 */
#define SIO_TYP_MASK		0x000000ff
#define SIO_TYP(x)			((x) & SIO_TYP_MASK)
#define SIO_TYP_CONSOLE		0x00000001			/* ウィンドウコンソール  */
#define SIO_TYP_TTY			0x00000002			/* Windowsのコンソール */
#define SIO_TYP_SCRBUF		0x00000003			/* スクリーンバッファ    */
#define SIO_TYP_RAWSIO		0x00000004			/* シリアル              */

	/* デバイスの状態 */
#define SIO_STA_MASK		0x0fff0000
#define SIO_STA(x)			((x) & SIO_STA_MASK)
#define SIO_STA_OPEN		0x00010000
#define SIO_STA_INTRCV		0x00020000
#define SIO_STA_INTSND		0x00040000

#define INHNO_SERIAL	3

#define TNUM_PORT		1

#ifndef _MACRO_ONLY

typedef struct serial_port_descriptor
{
	int					Flag;
	HANDLE				Handle;
	volatile int		ReceiveBuffer;
	void *				versatile;
	VP_INT              exinf;
} SIOPCB;

extern SIOPCB siopcb_table[TNUM_PORT];

/*
 *  SIOドライバの初期化ルーチン
 */
extern void	sio_initialize(void);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB * sio_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void	sio_cls_por(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL	sio_snd_chr(SIOPCB *siopcb, INT chr);

/*
 *  シリアルI/Oポートへの文字受信
 */
extern INT	sio_rcv_chr(SIOPCB *siopcb);

/*
 *  SIOの割込みサービスルーチン
 */
extern void	sio_isr(void);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void sio_ena_cbr(SIOPCB * siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void sio_dis_cbr(SIOPCB * siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートへの文字送信完了コールバック
 */
extern void sio_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートへの文字受信完了コールバック
 */
extern void sio_ierdy_rcv(VP_INT exinf);

/*
 *  シリアルI/Oポートからの文字受信通知コールバック
 */
extern void	sio_ercv_chr(ID siopid, INT chr);

/*
 *  シリアルI/Oポート割込みハンドラ
 */
extern void	sio_handler(void);

#endif

#endif

