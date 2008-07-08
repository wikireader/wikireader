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
 *  @(#) $Id: hw_serial.h,v 1.2 2007/05/28 02:03:55 honda Exp $
 */

/*
 *  シリアルI/Oデバイス（SIO）ドライバ（M32102内蔵非同期シリアル用）
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <sil.h>
#include <sys_config.h>

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND    1u              /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u              /* 受信通知コールバック */

/*
 *  シリアルI/Oの割込みハンドラのベクタ番号
 */
#define INHNO_SERIAL_IN		(48 + ((CONSOLE_PORTID-1)*2))
#define INHNO_SERIAL_OUT	(49 + ((CONSOLE_PORTID-1)*2))

#define SIO_TYP(x)			( (x) & 0x000000ff )
#define SIO_TYP_M32RUART	0x00000001

#define SIO_STS(x)			( (x) & 0xffff0000 )
#define SIO_STS_SENDING		0x80000000

typedef struct
{
	int		flags;
	int		port;
	VP_INT	exinf;
} SIOPCB;

//extern SIOPCB siopcb_table[TNUM_PORT];

extern void sio_handler_in(void);
extern void sio_handler_out(void);

/*
 *  シリアルポートの初期化
 */
extern void sio_initialize(void);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB * sio_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void sio_cls_por(SIOPCB *siopcb);

/*
 *  一文字送信
 */
Inline BOOL
sio_snd_chr(SIOPCB * siopcb, INT chr)
{
	if((siopcb->flags & SIO_STS_SENDING) != 0)
		return FALSE;

	switch(SIO_TYP(siopcb->flags))
	{
	case SIO_TYP_M32RUART:	
		if((sil_reb_mem((void *)(SIOSTS(siopcb->port)+3)) & 0x3) != 0)
		{
			siopcb->flags |= SIO_STS_SENDING;
			sil_wrb_mem((void *)(SIOTXB(siopcb->port)+3), (char)chr);
		} else
			return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


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

#endif /* _HW_SERIAL_H_ */
