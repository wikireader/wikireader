/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
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
 *  @(#) $Id: hw_serial.h,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

/*
 *	ターゲット依存シリアルI/Oモジュール（OAKS16-MINI内蔵非同期シリアル用）
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <s_services.h>
#include <sil.h>
#include "oaks16mini.h"

/*
 * メモリ不足のため、簡易型シリアルドライバーを実装します。
 */

/*
 * シリアルコントロールレジスタの設定値 
 */
#define	MR_DEF		0x05	/* 内部クロック、非同期、8ビット、パリティなし、スリープなし */
#define	C0_DEF		0x10	/* RTS/CTS未使用, カウントソースf1 */
#define	C1R_DEF		0x05	/* シリアル送受信可能 */
#define	C1S_DEF		0x00	/* シリアル送受信不可 */
#define	BRG1_DEF	64		/* 19200bps = 20000000/{(UxBRG+1)*16 */
#define	RB_LEVEL	6		/* 受信割込みレベル */

/*
 *  シリアルI/Oの割込みハンドラのベクタ番号
 */
#define	INHNO_SERIAL_IN1	INT_S0R
#define	INHNO_SERIAL_OUT1	INT_S0T
#define INHNO_SERIAL_IN2	INT_S1R
#define INHNO_SERIAL_OUT2	INT_S1T

#endif /* _HW_SERIAL_H_ */
