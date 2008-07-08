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
 *  @(#) $Id: serial.h,v 1.7 2003/06/04 01:46:02 hiro Exp $
 */

/*
 *	シリアルインタフェースドライバ
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

/*
 *  シリアルインタフェースドライバの用いるパケット
 */
typedef struct {
		UINT	reacnt;		/* 受信バッファ中の文字数 */
		UINT	wricnt;		/* 送信バッファ中の文字数 */
	} T_SERIAL_RPOR;

/*
 *  シリアルインタフェースドライバの初期化ルーチン
 */
extern void	serial_initialize(VP_INT exinf) throw();

/*
 *  シリアルインタフェースドライバのサービスコール
 */
extern ER	serial_opn_por(ID portid) throw();
extern ER	serial_cls_por(ID portid) throw();
extern ER_UINT	serial_rea_dat(ID portid, char *buf, UINT len) throw();
extern ER_UINT	serial_wri_dat(ID portid, char *buf, UINT len) throw();
extern ER	serial_ctl_por(ID portid, UINT ioctl) throw();
extern ER	serial_ref_por(ID portid, T_SERIAL_RPOR *pk_rpor) throw();

/*
 *  シリアルインタフェースドライバの動作制御用のための定数
 *
 *  以下の定数は，ビット毎に論理和をとって用いる．
 */
#define	IOCTL_NULL	0u		/* 指定なし */
#define	IOCTL_ECHO	0x0001u		/* 受信した文字をエコーバック */
#define	IOCTL_CRLF	0x0010u		/* LF を送信する前に CR を付加 */
#define	IOCTL_FCSND	0x0100u		/* 送信に対してフロー制御を行う */
#define	IOCTL_FCANY	0x0200u		/* どのような文字でも送信再開 */
#define	IOCTL_FCRCV	0x0400u		/* 受信に対してフロー制御を行う */

#endif /* _SERIAL_H_ */
