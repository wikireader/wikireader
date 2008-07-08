/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: hw_serial.h,v 1.2 2006/08/03 04:14:02 honda Exp $
 */

/*
 *	シリアルI/Oデバイス（SIO）ドライバ（M3029用）
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <s_services.h>
#ifndef _MACRO_ONLY
#include <sfruart.h>
#endif /* _MACRO_ONLY */

/*
 *  シリアルI/Oの割込みハンドラのベクタ番号
 */
#define	INHNO_SERIAL_IN1	INT_S0R
#define	INHNO_SERIAL_OUT1	INT_S0T
#define INHNO_SERIAL_IN2	INT_S1R
#define INHNO_SERIAL_OUT2	INT_S1T

#ifndef _MACRO_ONLY

/*
 *  SIOドライバの初期化ルーチン
 */
#define	sio_initialize	sfruart_initialize

/*
 *  シリアルI/Oポートのオープン
 */
#define	sio_opn_por		sfruart_opn_por

/*
 *  シリアルI/Oポートのクローズ
 */
#define	sio_cls_por		sfruart_cls_por

/*
 *  シリアルI/Oポートへの文字送信
 */
#define	sio_snd_chr		sfruart_snd_chr

/*
 *  シリアルI/Oポートからの文字受信
 */
#define	sio_rcv_chr		sfruart_rcv_chr

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
#define	sio_ena_cbr		sfruart_ena_cbr

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
#define	sio_dis_cbr		sfruart_dis_cbr

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
#define	sio_ierdy_snd	sfruart_ierdy_snd

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
#define	sio_ierdy_rcv	sfruart_ierdy_rcv

#endif /* _MACRO_ONLY */
#endif /* _HW_SERIAL_H_ */
