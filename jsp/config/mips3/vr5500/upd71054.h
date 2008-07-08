/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#ifndef _UPD71054_H_
#define _UPD71054_H_

#include <s_services.h>

/*
 *  タイマ uPD71054(NEC) 関連の定義
 */

/* uPD71054のレジスタのアドレス */
#define PCNT0		0x00
#define PCNT1		0x10
#define PCNT2		0x20
#define PCNTL		0x30

/* control word foemat */
#define BINARY			0u	/* BIT0 */
#define BCD			BIT0

#define MODE_0			0u	/* BIT1, 2, 3 */
#define MODE_1			BIT1
#define MODE_2			BIT2
#define MODE_3			(BIT2 | BIT1)
#define MODE_4			BIT3
#define MODE_5			(BIT3 | BIT1)

#define COUNT_LATCH_COMM	0u	/* BIT5, 4 */
#define LOWER_BYTE		BIT4
#define HIGH_BYTE		BIT5
#define LOW_HIGH_BYTE		(BIT5 | BIT4)

#define COUNTER_0		0u	/* BIT6, 7 */
#define COUNTER_1		BIT6
#define COUNTER_2		BIT7
#define MULT_LATCH_COMM		(BIT7 | BIT6)

/* multiple latch command format */
#define SEL_COUNTER_0		BIT1
#define SEL_COUNTER_1		BIT2
#define SEL_COUNTER_2		BIT3
#define DO_NOT_LATCH_STATUS	BIT4
#define LATCH_STATUS		0u	/* BIT4 */
#define DO_NOT_LATCH_COUNT	BIT5
#define LATCH_COUNT		0u	/* BIT5 */

/*
 *  タイマのレジスタへのアクセス関数
 */
#define upd71054_reb( addr )		sil_reb_mem( (VP)(TIMER_BASE_ADDR + addr) )
#define upd71054_wrb( addr, val )	sil_wrb_mem( (VP)(TIMER_BASE_ADDR + addr), val )

#endif /* _UPD71054_H_ */
