/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: h8.h,v 1.11 2007/03/23 07:22:15 honda Exp $
 */

#ifndef _H8_H_
#define _H8_H_

/*
 *  H8 用定義
 */

/*
 *  数値データ文字列化用マクロ
 */

#define TO_STRING(arg)	#arg
#define _TO_STRING(arg)	TO_STRING(arg)

/* Condition Code Register (CCR) */

#define H8CCR_I_BIT		7
#define H8CCR_UI_BIT		6
#define H8CCR_H_BIT		5
#define H8CCR_U_BIT		4
#define H8CCR_N_BIT		3
#define H8CCR_Z_BIT		2
#define H8CCR_V_BIT		1
#define H8CCR_C_BIT		0

#define H8CCR_I			(1<<H8CCR_I_BIT)
#define H8CCR_UI		(1<<H8CCR_UI_BIT)
#define H8CCR_H			(1<<H8CCR_H_BIT)
#define H8CCR_U			(1<<H8CCR_U_BIT)
#define H8CCR_N			(1<<H8CCR_N_BIT)
#define H8CCR_Z			(1<<H8CCR_Z_BIT)
#define H8CCR_V			(1<<H8CCR_V_BIT)
#define H8CCR_C			(1<<H8CCR_C_BIT)

/* CCR の IビットとUIビットを割り込みマスクビットとして使用する。*/

#define H8INT_MASK_ALL		(H8CCR_I | H8CCR_UI)

#define H8INT_DIS_ALL		H8INT_MASK_ALL
#define H8INT_ENA_ALL		(0xff & ~(H8INT_MASK_ALL))

#define str_H8INT_DIS_ALL	_TO_STRING(H8INT_DIS_ALL)
#define str_H8INT_ENA_ALL	_TO_STRING(H8INT_ENA_ALL)

/*
 *  レベル０　すべての割込みを受け付ける
 */
#define IPM_LEVEL0      0

/*
 *  レベル１　NMIおよびプライオリティレベル１の割込みのみを受け付ける
 */
#define IPM_LEVEL1      H8CCR_I

/*
 *  レベル２　NMI以外の割込みを受け付けない
 */
#define IPM_LEVEL2      (H8CCR_I | H8CCR_UI)


#endif /* _H8_H_ */
