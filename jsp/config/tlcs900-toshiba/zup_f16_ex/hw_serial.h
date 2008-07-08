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
 *  @(#) $Id: hw_serial.h,v 1.1 2006/04/10 08:19:25 honda Exp $
 */

/*
 *	ターゲット依存シリアルI/Oモジュール（Zup-F16拡張ボード用）
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <s_services.h>
#include <sil.h>
#include <tmp91cy22.h>
#include <uart.h>

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND		UART_ERDY_SND			/* 送信可能コールバック			*/
#define SIO_ERDY_RCV		UART_ERDY_RCV			/* 受信通知コールバック			*/

#ifndef _MACRO_ONLY

/*
 *  SIOドライバの初期化ルーチン
 */
#define	sio_initialize	uart_initialize

/*
 *  シリアルI/Oポートのオープン
 */
#define	sio_opn_por		uart_opn_por

/*
 *  シリアルI/Oポートのクローズ
 */
#define	sio_cls_por		uart_cls_por

/*
 *  シリアルI/Oポートへの文字送信
 */
#define	sio_snd_chr		uart_snd_chr

/*
 *  シリアルI/Oポートからの文字受信
 */
#define	sio_rcv_chr		uart_rcv_chr

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
#define	sio_ena_cbr		uart_ena_cbr

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
#define	sio_dis_cbr		uart_dis_cbr

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
#define	sio_ierdy_snd	uart_ierdy_snd

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
#define	sio_ierdy_rcv	uart_ierdy_rcv

#endif /* _MACRO_ONLY */
#endif /* _HW_SERIAL_H_ */
