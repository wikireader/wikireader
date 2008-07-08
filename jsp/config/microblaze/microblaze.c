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
 *  @(#) $Id: microblaze.c,v 1.8 2003/12/11 04:52:44 honda Exp $
 */

/*                                                                              * Microblaze 標準ペリフェラル用ドライバ
 */
#include <s_services.h>
#include <microblaze.h>


/*
 * UART Lite用 簡易ドライバ
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

Inline void
uart_ena_int(void){
  sil_wrw_mem((VP)UARTLITE_CTREG,
              sil_rew_mem((VP)UARTLITE_CTREG) | UARTLITE_ENABLE_INTR);
}

Inline void
uart_dis_int(void){
  sil_wrw_mem((VP)UARTLITE_CTREG,
              sil_rew_mem((VP)UARTLITE_CTREG) & ~UARTLITE_ENABLE_INTR);
}

/*
 * 文字を受信したか?                                                       
 */
Inline BOOL
uart_getready(SIOPCB *siopcb)
{
    return((sil_rew_mem((VP)UARTLITE_SRREG) &
            UARTLITE_RX_FIFO_VALID_DATA) != 0);

}

/*                                                                             
 * 文字を送信できるか?                                                         
 */
Inline BOOL
uart_putready(SIOPCB *siopcb)
{
  return((sil_rew_mem((VP)UARTLITE_SRREG) & UARTLITE_TX_FIFO_FULL) == 0);
}

/*                                                                             
 *  受信した文字の取り出し                                                     
 */
Inline UB
uart_getchar(SIOPCB *siopcb)
{
    return((UB)sil_rew_mem((VP)UARTLITE_RXREG));
}

/*                                                                            
 *  送信する文字の書き込み                                                    
 */
Inline void
uart_putchar(SIOPCB *siopcb, UB c)
{
  sil_wrw_mem((VP)UARTLITE_TXREG, c);
}

/*
 * 送信制御関数
 * UART Lite には送信割込みを個別に禁止許可する機能はない
 * そのため，送信終了時には必ず1回割込みが入る  
 */
/*                                                                             
 *  送信割込み許可                                                             
 */
Inline void
uart_enable_send(SIOPCB *siopcb)
{
  
}

/*                                                                             
 *  送信割込み禁止                                                             
 */
Inline void
uart_disable_send(SIOPCB *siopcb)
{

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

    /*
     *  送受信バッファクリア
     */
    sil_wrw_mem((VP)UARTLITE_CTREG, UARTLITE_RST_RX_FIFO
                | UARTLITE_RST_TX_FIFO | UARTLITE_ENABLE_INTR);
    
//    sil_wrw_mem((VP)UARTLITE_CTREG, UARTLITE_RST_RX_FIFO
//                | UARTLITE_RST_TX_FIFO );
    
    siopcb->exinf = exinf;
    siopcb->getready = siopcb->putready = FALSE;
    siopcb->openflag = TRUE;

    return(siopcb);
}


void
uart_cls_por(SIOPCB *siopcb){
  sil_wrw_mem((VP)UARTLITE_CTREG, ~UARTLITE_ENABLE_INTR);   /* 割込み禁止    */
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
    if (uart_getready(siopcb)) {
        return((INT)(UB) uart_getchar(siopcb));
    }
    return(-1);
}

/*
 *  OPB_UARTLITE は送信と受信割込みを個別に許可/禁止する機能は持たない
 */

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{

}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{

}


/*                                                                            
 *  シリアルI/Oポートに対する割込み処理                                       
 */
static void
uart_isr_siop(SIOPCB *siopcb)
{
    /*
     * エッジ割込みのため割込み処理前にACK
     */
    intc_ack_interrupt(1 << (UARTINTLVL - 1));
    
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

