/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005 by Freelines CO.,Ltd
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
 *  @(#) $Id: hw_serial.h,v 1.2 2007/01/05 02:10:17 honda Exp $
 */

/*
 * ターゲット依存シリアルI/Oモジュール（V850ES/SG2 内蔵非同期シリアルch0/ch1/ch2用）
 *   ボーレート設定はSERIAL_COMPAREVALUEを変更する。
 *   データフォーマットは8bit data, non-parity, 1stop-bit固定
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <s_services.h>

/*
 *  シリアルポートの初期化ブロック
 */

typedef struct sio_port_initialization_block {
} SIOPINIB;

/*
 *  シリアルポートの制御ブロック
 */
typedef struct sio_port_control_block {
	const SIOPINIB	*inib;		/* 初期化ブロック	*/
	VP_INT		exinf;		/* 拡張情報		*/
	BOOL		openflag;	/* オープン済みフラグ	*/
	int			port_id;	/* ポート番号(0〜) */
} SIOPCB;

extern SIOPCB siopcb_table[TNUM_PORT];

/*
 *  SIO ID から管理ブロックへの変換マクロ
 */

#define INDEX_SIO(sioid)	((UINT)((sioid) - 1))
#define get_siopcb(sioid)	(&(siopcb_table[INDEX_SIO(sioid)]))

/*
 *  コールバックルーチンの識別番号
 */

#define SIO_ERDY_SND		1u			/* 送信可能コールバック			*/
#define SIO_ERDY_RCV		2u			/* 受信通知コールバック			*/

/*
 *  シリアルポートのハードウェア依存情報の定義
 */

#define SERIAL_PORT				(CONSOLE_PORTID-1)		/* システムのポート番号 */
#define SERIAL_INT_PRIORITY		1	/* 割り込み優先度 */
#define SERIAL_CLKDIV			3	/* clk = fxx/8     */
#define SERIAL_COMPAREVALUE		11	/* 9600bps=130, 19200bps=65, 38400bps=33, 57600bps=22, 115200bps=11 */

/* シリアル関連レジスタ */
#define UAnCTL0(x)  (UA0CTL0 + ((x) * 0x10))
#define UAnCTL1(x)  (UA0CTL1 + ((x) * 0x10))
#define UAnCTL2(x)  (UA0CTL2 + ((x) * 0x10))
#define UAnOPT0(x)  (UA0OPT0 + ((x) * 0x10))
#define UAnSTR(x)   (UA0STR  + ((x) * 0x10))
#define UAnRX(x)    (UA0RX   + ((x) * 0x10))
#define UAnTX(x)    (UA0TX   + ((x) * 0x10))
#define UAnRIC(x)	(UA0RIC  + ((x) * 0x4))
#define UAnTIC(x)	(UA0TIC  + ((x) * 0x4))

/*
 *  シリアルI/Oの割込みハンドラのベクタ番号
 */
#define INHNO_SERIAL_IN		(0x31 + (SERIAL_PORT * 2))
#define INHNO_SERIAL_OUT	(0x32 + (SERIAL_PORT * 2))


/*
 *  低レベルポート情報管理ブロックの初期値
 */

#define NUM_PORT	1	/* サポートするシリアルポートの数 */

#define FLG_RECEIVED 1
#define FLG_TXRBUSY  2

extern unsigned int _serial_flag[TNUM_PORT];

/*
 *  シリアルI/Oポートの初期化
 */
Inline BOOL
hw_port_initialize(int port_id)
{
   	_serial_flag[port_id] = 0;
	if( (sil_reb_mem((VP)UAnCTL0(port_id)) & 0x80) == 0 )
	{
    	/* 非同期シリアル通信路の初期化 */

		sil_wrb_mem((VP)UAnCTL0(port_id), 0x80);	/* UART enable */
		/* ポートを開ける */
		if (port_id == 0) {
			sil_wrh_mem((VP)PFC3, sil_reh_mem((VP)PFC3) & ~0x0003);
			sil_wrh_mem((VP)PMC3, sil_reh_mem((VP)PMC3) | 0x0003);
		} else if (port_id == 1) {
			sil_wrh_mem((VP)PFCE9, sil_reh_mem((VP)PFCE9) | 0x0003);
			sil_wrh_mem((VP)PFC9,  sil_reh_mem((VP)PFC9)  & ~0x0003);
			sil_wrh_mem((VP)PMC9,  sil_reh_mem((VP)PMC9)  | 0x0003);
		} else {
			sil_wrh_mem((VP)PFC3, sil_reh_mem((VP)PFC3) & ~0x0300);
			sil_wrh_mem((VP)PMC3, sil_reh_mem((VP)PMC3) | 0x0300);
		}
		/* ボーレート発生器の初期化 */
		sil_wrb_mem((VP)UAnCTL1(port_id), SERIAL_CLKDIV);
		sil_wrb_mem((VP)UAnCTL2(port_id), SERIAL_COMPAREVALUE);

		__asm("nop; nop");

		sil_wrb_mem((VP)UAnCTL0(port_id), sil_reb_mem((VP)UAnCTL0(port_id)) | 0x12);	/* 8bit data, non-parity, 1-stopbit */

		__asm("nop; nop");

		sil_wrb_mem((VP)UAnCTL0(port_id), sil_reb_mem((VP)UAnCTL0(port_id)) | 0x60);	/* Tx, Rx enable */

		/* 割込み制御 */
		sil_wrb_mem((VP)UAnRIC(port_id), SERIAL_INT_PRIORITY);
		sil_wrb_mem((VP)UAnTIC(port_id), SERIAL_INT_PRIORITY);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートの終了
 */
Inline void
hw_port_terminate(int port_id)
{
}

/*
 *  シリアルポート割込みサービスルーチン（前方参照）
 */
extern void	serial_handler_in(int portid);
extern void	serial_handler_out(int portid);

/*
 *  文字を受信したか？
 */
Inline BOOL
hw_port_getready(int port_id)
{
	return (_serial_flag[port_id] & FLG_RECEIVED) != 0 ? TRUE : FALSE;
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
hw_port_putready(int port_id)
{
//	return (sil_reb_mem((VP)UAnSTR(port_id)) & 0x80) != 0 ? FALSE : TRUE;
	return (_serial_flag[port_id] & FLG_TXRBUSY) != 0 ? FALSE : TRUE;
}

Inline BOOL
hw_port_putready2(int port_id)
{
	return (sil_reb_mem((VP)UAnSTR(port_id)) & 0x80) != 0 ? FALSE : TRUE;
//	return (_serial_flag[port_id] & FLG_TXRBUSY) != 0 ? FALSE : TRUE;
}

/*
 *  受信した文字の取り出し
 */
Inline unsigned char
hw_port_getchar(int port_id)
{
	_serial_flag[port_id] &= ~FLG_RECEIVED;
	sil_wrb_mem((VP)UAnRIC(port_id), sil_reb_mem((VP)UAnRIC(port_id)) & 0x7f);
	return sil_reb_mem((VP)UAnRX(port_id));
}

/*
 *  送信する文字の書き込み
 */
Inline void
hw_port_putchar(int port_id, unsigned char c)
{
	sil_wrb_mem((VP)UAnTX(port_id), c);
	_serial_flag[port_id] |= FLG_TXRBUSY;
}

/*
 *  送信する文字の書き込み（送信可能になるまで待つ）
 */
Inline void
hw_port_wait_putchar(int port_id, unsigned char c)
{
	while( !hw_port_putready2(port_id) ) ;
	hw_port_putchar(port_id, c);
}

/*
 *  送信制御関数
 */
Inline void
hw_port_sendstart(int port_id)
{
}

Inline void
hw_port_sendstop(int port_id)
{
}

extern void	sio_ierdy_rcv(VP_INT exinf);
extern void	sio_ierdy_snd(VP_INT exinf);

Inline void
hw_port_handler_in(int port_id)
{
	_serial_flag[port_id] |= FLG_RECEIVED;
    sio_ierdy_rcv(siopcb_table[port_id].exinf);
}

Inline void
hw_port_handler_out(int port_id)
{
	_serial_flag[port_id] &= ~FLG_TXRBUSY;
	sio_ierdy_snd(siopcb_table[port_id].exinf);
}

/***********************************************************************************/

/*
 *  sio_initialize -- シリアルポートドライバの初期化
 */
Inline void
sio_initialize (void)
{
	int ix;

	/* SIO 管理ブロックの初期化 */
	for (ix = TNUM_PORT; ix -- > 0; ) {
		siopcb_table[ix].openflag = FALSE;
	}
}

/*
 *  sio_opn_por -- ポートのオープン
 */
Inline SIOPCB *
sio_opn_por(ID sioid, VP_INT exinf)
{
	SIOPCB	*pcb;

	pcb = get_siopcb(sioid);
	pcb->inib     = NULL;
	pcb->exinf    = exinf;
	pcb->openflag = TRUE;
	pcb->port_id  = sioid-1;
	hw_port_initialize(pcb->port_id);
	return pcb;
}

/*
 *  sio_snd_chr -- 文字送信
 */
Inline BOOL
sio_snd_chr(SIOPCB *pcb, INT chr)
{
	if (hw_port_putready(pcb->port_id)) {
		hw_port_putchar(pcb->port_id, (UB)chr);
		return TRUE;
		}
	else
		return FALSE;
}

/*
 *  sio_rcv_chr -- 文字受信
 */
Inline INT
sio_rcv_chr(SIOPCB *pcb)
{
	if (hw_port_getready(pcb->port_id))
		return hw_port_getchar(pcb->port_id);
	else
		return -1;
}

/*
 *  sio_ena_cbr -- シリアル I/O からのコールバックの許可
 */
Inline void
sio_ena_cbr(SIOPCB *pcb, UINT cbrtn)
{
}

/*
 *  sio_dis_cbr -- シリアル I/O からのコールバックの禁止
 */
Inline void
sio_dis_cbr(SIOPCB *pcb, UINT cbrtn)
{
}

/*
 *  sio_cls_por -- ポートのクローズ
 */
Inline void
sio_cls_por(SIOPCB *pcb)
{
	sio_dis_cbr(pcb, SIO_ERDY_SND);
	sio_dis_cbr(pcb, SIO_ERDY_RCV);
	pcb->openflag = FALSE;
}

Inline void
sio_in_handler(void)
{
	hw_port_handler_in(0);
}

Inline void
sio_out_handler(void)
{
	hw_port_handler_out(0);
}

#if TNUM_PORT > 1
Inline void
sio_in_handler2(void)
{
	hw_port_handler_in(1);
}

Inline void
sio_out_handler2(void)
{
	hw_port_handler_out(1);
}
#endif

#endif /* _HW_SERIAL_H_ */
