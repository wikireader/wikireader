/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2005 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN    
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
 *  @(#) $Id: nios2.c,v 1.6 2005/07/17 14:24:04 honda Exp $
 */

/*
 * Nios2 標準ペリフェラル用ドライバ
 */
#include <s_services.h>
#include <nios2.h>


/*
 * UART 用 簡易ドライバ
 */

/*                                                                       
 * シリアルI/Oポート初期化ブロック                                         
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {{}};

/*                                                                      
 *  シリアルI/Oポート管理ブロックのエリア                                
 */
SIOPCB  siopcb_table[TNUM_SIOP];

/*                                                                          
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ                  
 */
#define INDEX_SIOP(siopid)  ((UINT)((siopid) - 1))
#define get_siopcb(siopid)  (&(siopcb_table[INDEX_SIOP(siopid)]))

#ifdef USE_JTAG_UART
/*
 *  JTAG_UART はステータスとデータを同時に取得するため，ステータスを
 *  チェックするとデータを読み込んでしまう．そのため，uart_getready()
 *  を実行した後のデータを入れる．  
 */ 
UB jtag_uart_buf = -1;
#endif /* USE_JTAG_UART */

/*
 * 文字を受信したか?                                                       
 */
Inline BOOL
uart_getready(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART
	return((sil_rew_mem((VP)UART_STATUS) &
			UART_STATUS_RRDY) != 0);
#else
	UW tmp = sil_rew_mem((VP)JTAG_UART_DATA);
	jtag_uart_buf = (UB)tmp;	
	return ((tmp & JTAG_UART_DATA_RVALID) != 0);
#endif /* USE_JTAG_UART	*/	
}

/*                                                                             
 * 文字を送信できるか?                                                         
 */
Inline BOOL
uart_putready(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART		
	return((sil_rew_mem((VP)UART_STATUS) & UART_STATUS_TRDY) != 0);
#else
	return((sil_rew_mem(
		(VP)JTAG_UART_CONTROL) & JTAG_UART_CONTROL_WSAPCE) > 0);
#endif /* USE_JTAG_UART	*/
}

/*                                                                             
 *  受信した文字の取り出し                                                     
 */
Inline UB
uart_getchar(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART
	return((UB)(sil_rew_mem((VP)UART_RXDATA)));
#else
	return((UB)(sil_rew_mem((VP)JTAG_UART_DATA)));
#endif /* USE_JTAG_UART	*/
}

/*                                                                            
 *  送信する文字の書き込み                                                    
 */
Inline void
uart_putchar(SIOPCB *siopcb, UB c)
{
#ifndef USE_JTAG_UART
	sil_wrw_mem((VP)UART_TXDATA, c);
#else
	sil_wrw_mem((VP)JTAG_UART_DATA,c);
#endif /* USE_JTAG_UART	*/
}

/*
 * 送信制御関数
 */
/*                                                                             
 *  送信割込み許可                                                             
 */
Inline void
uart_enable_send(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART
	sil_wrw_mem((VP)UART_CONTROL,
				sil_rew_mem((VP)UART_CONTROL)|UART_CONTROL_ITRD);
#else
	sil_wrw_mem((VP)JTAG_UART_CONTROL,
				sil_rew_mem((VP)JTAG_UART_CONTROL)|JTAG_UART_CONTROL_WIE);
#endif /* USE_JTAG_UART	*/	
}

/*                                                                             
 *  送信割込み禁止                                                             
 */
Inline void
uart_disable_send(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART
	sil_wrw_mem((VP)UART_CONTROL,
				sil_rew_mem((VP)UART_CONTROL)&~UART_CONTROL_ITRD);
#else
	sil_wrw_mem((VP)JTAG_UART_CONTROL,
				sil_rew_mem((VP)JTAG_UART_CONTROL)&~JTAG_UART_CONTROL_WIE);
#endif /* USE_JTAG_UART	*/
}

/*
 * 受信制御関数
 */
/*                                                                             
 *  受信割込み許可                                                             
 */
Inline void
uart_enable_rcv(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART
	sil_wrw_mem((VP)UART_CONTROL,
				sil_rew_mem((VP)UART_CONTROL)|UART_STATUS_RRDY);
#else
	sil_wrw_mem((VP)JTAG_UART_CONTROL,
				sil_rew_mem((VP)JTAG_UART_CONTROL)|JTAG_UART_CONTROL_RIE);
#endif /* USE_JTAG_UART	*/
}

/*                                                                             
 *  受信割込み禁止                                                             
 */
Inline void
uart_disable_rcv(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART	
	sil_wrw_mem((VP)UART_CONTROL,
				sil_rew_mem((VP)UART_CONTROL)&~UART_STATUS_RRDY);
#else
	sil_wrw_mem((VP)JTAG_UART_CONTROL,
				sil_rew_mem((VP)JTAG_UART_CONTROL)&~JTAG_UART_CONTROL_RIE);
#endif /* USE_JTAG_UART	*/	
}


/*                                                                             
 *  SIOドライバの初期化ルーチン                                                
 *  1ポートしかないため，あまり意味はない                                      
 */
void
uart_initialize()
{
    SIOPCB  *siopcb;
    UINT    i;

    /*                                                                         
     *  シリアルI/Oポート管理ブロックの初期化                                  
     */
    for (siopcb = siopcb_table, i = 0; i < TNUM_SIOP; siopcb++, i++) {
        siopcb->siopinib = &(siopinib_table[i]);
        siopcb->openflag = FALSE;
        siopcb->sendflag = FALSE;
    }
}


SIOPCB   *
uart_opn_por(ID siopid, VP_INT exinf){

    SIOPCB      *siopcb;
    const SIOPINIB  *siopinib;

    siopcb = get_siopcb(siopid);
    siopinib = siopcb->siopinib;

#ifndef USE_JTAG_UART		
//    sil_wrw_mem(UART_DIVISOR, UART_DIVISOR_VAL);
    sil_wrw_mem((VP)UART_STATUS, 0x00);
    
    /*
     *  受信割り込み許可
     */
    sil_wrw_mem((VP)UART_CONTROL, UART_STATUS_RRDY);
#else
	sil_wrw_mem((VP)JTAG_UART_CONTROL,JTAG_UART_CONTROL_RIE);
#endif /* USE_JTAG_UART	*/
	
    siopcb->exinf = exinf;
    siopcb->getready = siopcb->putready = FALSE;
    siopcb->openflag = TRUE;

    return(siopcb);
}


void
uart_cls_por(SIOPCB *siopcb){
#ifndef USE_JTAG_UART	
    /* 割込み禁止 */
    sil_wrw_mem((VP)UART_CONTROL, 0x00);
#else
	sil_wrw_mem((VP)JTAG_UART_CONTROL,0x00);
#endif /* USE_JTAG_UART	*/		
    siopcb->openflag = FALSE;
    siopcb->sendflag = FALSE;
}



/*                                                                             
 *  シリアルI/Oポートへの文字送信                                            
 */
BOOL
uart_snd_chr(SIOPCB *siopcb, INT chr)
{
    if(uart_putready(siopcb)){
        uart_putchar(siopcb, (UB) chr);
        return(TRUE);
    }
    return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
uart_rcv_chr(SIOPCB *siopcb)
{
#ifndef USE_JTAG_UART
	if (uart_getready(siopcb)) {
		return((INT)(UB) uart_getchar(siopcb));
	}
	return(-1);
#else
	UB tmp;
	
	if (jtag_uart_buf != -1) {
		tmp = jtag_uart_buf;
		jtag_uart_buf = -1;
		return tmp;
	}
	return(-1);
#endif /* USE_JTAG_UART	*/	
}


/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
    switch (cbrtn) {
      case SIO_ERDY_SND:
        uart_enable_send(siopcb);
        break;
      case SIO_ERDY_RCV:
        uart_enable_rcv(siopcb);
        break;
    }

}


/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
    switch (cbrtn) {
      case SIO_ERDY_SND:
        uart_disable_send(siopcb);
        break;
      case SIO_ERDY_RCV:
        uart_disable_rcv(siopcb);
        break;
    }
}


/*                                                                            
 *  シリアルI/Oポートに対する割込み処理                                       
 */
static void
uart_isr_siop(SIOPCB *siopcb)
{
    if (uart_getready(siopcb)) {
        /*
         *  受信通知コールバックルーチンを呼び出す．
         */
        uart_ierdy_rcv(siopcb->exinf);
    }

    if (uart_putready(siopcb)) {
        /*
         *  送信可能コールバックルーチンを呼び出す．
         */
        uart_ierdy_snd(siopcb->exinf);
    }
}


/*                                                                             
 *  SIOの割込みサービスルーチン                                                
 */
void
uart_isr()
{
    if (siopcb_table[0].openflag) {
        uart_isr_siop(&(siopcb_table[0]));
    }
}

