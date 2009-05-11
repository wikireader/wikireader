/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                2003      by Advanced Data Controls, Corp
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
 *  @(#) $Id: hw_serial.h,v 1.3 2003/12/19 11:24:18 honda Exp $
 */

/*
 *  シリアルI/Oデバイス（SIO）ドライバ（Integrator用）
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <integrator.h>

/*
 *  シリアルI/Oの割込みハンドラのベクタ番号
 */
#define INHNO_SIO	IRQ_UART1_BIT
#define INHNO_SIO2      IRQ_UART0_BIT

/*
 *  SIOドライバの初期化ルーチン
 */
#define sio_initialize  uart_initialize

#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポートのオープン
 */
Inline SIOPCB *
sio_opn_por(ID siopid, VP_INT exinf)
{
    SIOPCB  *siopcb;
    BOOL    openflag;

    /*
     *  オープンしたポートがあるかを openflg に読んでおく．
     */
    openflag = uart_openflag();
    
    /*
     *  デバイス依存のオープン処理．
     */
    siopcb = uart_opn_por(siopid, exinf);

      
    /*
     * 割込み関連の設定
     */
    /* Enable Interrupt Rx */
    if(!openflag){
        sil_wrw_mem((VP)IRQ0_ENABLESET,IRQ_UART1);
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
     *  デバイス依存のクローズ処理．
     */
    uart_cls_por(siopcb);

    /*
     *  シリアルI/O割込みをマスクする．
     */
    if (!uart_openflag) {
        /* Clear Rx Tx Pending bit */
        sil_wrw_mem((VP)IRQ0_ENABLECLR,(IRQ_UART1));
    }
}


/*
 *  SIOの割込みハンドラ
 */
#define sio_handler  uart_isr

/*
 *  シリアルI/Oポートへの文字送信
 */
#define sio_snd_chr uart_snd_chr

/*
 *  シリアルI/Oポートからの文字受信
 */
#define sio_rcv_chr uart_rcv_chr

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
#define sio_ena_cbr uart_ena_cbr

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
#define sio_dis_cbr uart_dis_cbr

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
#define sio_ierdy_snd   uart_ierdy_snd

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
#define sio_ierdy_rcv   uart_ierdy_rcv

#endif /* _MACRO_ONLY */
#endif /* _HW_SERIAL_H_ */

