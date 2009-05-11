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
 *  @(#) $Id: hw_sysio.h,v 1.5 2003/12/24 07:24:40 honda Exp $
 */

#ifndef _HW_SYSIO_H_
#define _HW_SYSIO_H_


#include "integrator.h"

/*
 *  シリアルI/Oポートの初期化
 */
Inline void
hw_sysio_initialize()
{
    /*disable interrupt*/
    sil_wrw_mem((VP)UART0_CR,~UCR_UARTEN);
    /*baud rate 38400*/
    sil_wrw_mem((VP)UART0_LCRM,ULCRM_38400);
    sil_wrw_mem((VP)UART0_LCRL,ULCRL_38400);
    /*8bits, 1Stop bit, no parity*/
    sil_wrw_mem((VP)UART0_LCRH,ULCRH_WLEN_8BIT);
    /*Clear all registers*/
    sil_wrw_mem((VP)UART0_ICR,0xff);
    /*enable interrupt*/
    sil_wrw_mem((VP)UART0_CR,UCR_UARTEN | UCR_RIE);
}


/*
 *  シリアルI/Oポートの終了
 */
Inline void
hw_sysio_terminate()
{
    /*disable interrupt*/
    sil_wrw_mem((VP)UART0_CR,~UCR_UARTEN);
}


/*
 *  文字を送信できるか？
 */
Inline BOOL
hw_sysio_putready()
{
    return((sil_rew_mem((VP)UART0_FR) & UFR_BUSY)==0);
}


/*
 *  送信する文字の書き込み
 */
Inline void
hw_sysio_putchar(UB c)
{
    while(!hw_sysio_putready())
        ;
    sil_wrw_mem((VP)UART0_DR,c);
}

#endif /* _HW_SYSIO_H_ */
