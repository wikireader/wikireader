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
 *  @(#) $Id: messages.h,v 1.5 2003/12/24 07:40:42 takayuki Exp $
 */

#ifndef __MESSAGES_H
#define __MESSAGES_H

#define MSG_TYP_DEVICE 0x40
#define MSG_TYP_REQUEST 0x80

	//大体共通のメッセージ
#define MSG_ACK 0
#define MSG_NAK 1

	//マネージャからデバイスへのメッセージ
#define MSG_DEVREP_INITIALIZE 0x82
#define MSG_DEVREP_KERNELSTART 0x83
#define MSG_DEVREP_READ 0x84
#define MSG_DEVREP_WRITE 0x85
#define MSG_DEVREP_FINALIZE 0x06
#define MSG_DEVREP_KERNELEXIT 0x07

	//デバイスからマネージャへのメッセージ
#define MSG_DEVREQ_ADD 0xc2
#define MSG_DEVREQ_DELETE 0xc3
#define MSG_DEVREQ_READ 0xc4
#define MSG_DEVREQ_WRITE 0xc5
#define MSG_DEVREQ_INTERRUPT 0xc6
#define MSG_DEVREQ_MAPPING 0xc7
#define MSG_DEVREQ_UNMAPPING 0xc8

	//マネージャからカーネルへ
#define MSG_KERREP_INTERRUPT 0x86

#define MSG_KERREQ_KEEPALIVE -1

	//カーネルからマネージャへ
#define MSG_KERREQ_KERNELSTART 0x82
#define MSG_KERREQ_KERNELEXIT 0x83
#define MSG_KERREQ_READ 0x84
#define MSG_KERREQ_WRITE 0x85

#endif
