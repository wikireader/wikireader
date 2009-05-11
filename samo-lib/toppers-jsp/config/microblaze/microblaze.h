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
 *  @(#) $Id: microblaze.h,v 1.6 2004/02/05 09:16:25 honda Exp $
 */

#ifndef _MICROBLAZE_H_
#define _MICROBLAZE_H_



#ifndef _MACRO_ONLY
#include <itron.h>
#include <sil.h>
#endif /* _MACRO_ONLY */


/*
 *  MSRのビット
 */
#define MSR_CC  0x80000000
#define MSR_DCE 0x80
#define MSR_DZ  0x40
#define MSR_ICE 0x20
#define MSR_FSL 0x10
#define MSR_BIP 0x08
#define MSR_C   0x04
#define MSR_IE  0x02
#define MSR_BE  0x01


/*
 * INTC Registers
 */
#define INTC_INT_STATUS_REG    0x00
#define INTC_INT_PENDING_REG   0x04
#define INTC_INT_ENABLE_REG    0x08
#define INTC_INT_ACK_REG       0x0c
#define INTC_SET_INT_ENABLE    0x10
#define INTC_CLEAR_INT_ENABLE  0x14
#define INTC_INT_VECTOR_REG    0x18
#define INTC_MASTER_ENABLE_REG 0x1c

/* 
 * Masks for the INTC Registers
 */
#define INTC_HIE_MASK 0x2
#define INTC_ME_MASK  0x1



#define INTC_ISR (INTC_BASE + INTC_INT_STATUS_REG)
#define INTC_IPR (INTC_BASE + INTC_INT_PENDING_REG)
#define INTC_IER (INTC_BASE + INTC_INT_ENABLE_REG)
#define INTC_IAR (INTC_BASE + INTC_INT_ACK_REG)
#define INTC_SIE (INTC_BASE + INTC_SET_INT_ENABLE)
#define INTC_CIE (INTC_BASE + INTC_CLEAR_INT_ENABLE)
#define INTC_IVR (INTC_BASE + INTC_INT_VECTOR_REG)
#define INTC_MER (INTC_BASE + INTC_MASTER_ENABLE_REG)

#ifndef _MACRO_ONLY
/*
 *  INTC操作関数
 */
Inline void
intc_start(void){
  sil_wrw_mem((VP)INTC_MER, INTC_HIE_MASK | INTC_ME_MASK);
}

Inline void
intc_disable_allinterrupt(){
  sil_wrw_mem((VP)INTC_MER, 0);
}

Inline void
intc_enable_interrupt(UW mask)
{
  sil_wrw_mem((VP)INTC_SIE, mask);
}

Inline void
intc_disable_interrupt(UW mask)
{
  sil_wrw_mem((VP)INTC_CIE, mask);
}

Inline void
intc_ack_interrupt(UW mask)
{
  sil_wrw_mem((VP)INTC_IAR, mask);
}

#endif /* _MACRO_ONLY */



/*
 * タイマドライバ用のマクロ定義
 */


/*
 * Masks for the control/status register
 */
#define TIMER_ENABLE_ALL    0x400
#define TIMER_PWM           0x200
#define TIMER_INTERRUPT     0x100
#define TIMER_ENABLE        0x080
#define TIMER_ENABLE_INTR   0x040
#define TIMER_RESET         0x020
#define TIMER_RELOAD        0x010
#define TIMER_EXT_CAPTURE   0x008
#define TIMER_EXT_COMPARE   0x004
#define TIMER_DOWN_COUNT    0x002
#define TIMER_CAPTURE_MODE  0x001

#define TIMER_CONTROL_STATUS_0  0x0
#define TIMER_COMPARE_CAPTURE_0 0x4
#define TIMER_COUNTER_0         0x8
#define TIMER_CONTROL_STATUS_1  0x10
#define TIMER_COMPARE_CAPTURE_1 0x14
#define TIMER_COUNTER_1         0x18


#define TIMER_TCSR0 (TIMER_BASE + TIMER_CONTROL_STATUS_0)
#define TIMER_TCCR0 (TIMER_BASE + TIMER_COMPARE_CAPTURE_0)
#define TIMER_TCR0  (TIMER_BASE + TIMER_COUNTER_0)
#define TIMER_TCSR1 (TIMER_BASE + TIMER_CONTROL_STATUS_1)
#define TIMER_TCCR1 (TIMER_BASE + TIMER_COMPARE_CAPTURE_1)
#define TIMER_TCR1  (TIMER_BASE + TIMER_COUNTER_1)


/*
 *  UARTドライバ用のマクロ定義
 */

/*
 * Error condition masks 
 */
#define UARTLITE_PAR_ERROR     0x80
#define UARTLITE_FRAME_ERROR   0x40
#define UARTLITE_OVERRUN_ERROR 0x20


/*
 * Other status bit masks
 */
#define UARTLITE_INTR_ENABLED       0x10
#define UARTLITE_TX_FIFO_FULL       0x08
#define UARTLITE_TX_FIFO_EMPTY      0x04
#define UARTLITE_RX_FIFO_FULL       0x02
#define UARTLITE_RX_FIFO_VALID_DATA 0x01

/*
 * Control bit masks 
 */
#define UARTLITE_ENABLE_INTR  0x10
#define UARTLITE_RST_RX_FIFO  0x02
#define UARTLITE_RST_TX_FIFO  0x01

/*
 * UARTLITE Registers
 */
#define UARTLITE_RECEIVE_REG  0x0
#define UARTLITE_TRANSMIT_REG 0x4
#define UARTLITE_STATUS_REG   0x8
#define UARTLITE_CONTROL_REG  0xc


#define UARTLITE_RXREG (UART_BASE + UARTLITE_RECEIVE_REG)
#define UARTLITE_TXREG (UART_BASE + UARTLITE_TRANSMIT_REG)
#define UARTLITE_SRREG (UART_BASE + UARTLITE_STATUS_REG)
#define UARTLITE_CTREG (UART_BASE + UARTLITE_CONTROL_REG)



#ifndef _MACRO_ONLY
/*                                                                          
 * 内蔵UART用 簡易SIOドライバ                                                 
 */

/*                                                                            
 *  シリアルI/Oポート初期化ブロック                                         
 */
typedef struct sio_port_initialization_block {

} SIOPINIB;

/*                                                                            
 *  シリアルI/Oポート管理ブロック                                            
 */
typedef struct sio_port_control_block {
    const SIOPINIB  *siopinib;  /* シリアルI/Oポート初期化ブロック */
    VP_INT          exinf;      /* 拡張情報 */
    BOOL    openflag;           /* オープン済みフラグ */
    BOOL    sendflag;           /* 送信割込みイネーブルフラグ */
    BOOL    getready;           /* 文字を受信した状態 */
    BOOL    putready;           /* 文字を送信できる状態 */
} SIOPCB;

extern SIOPCB   siopcb_table[];

#define uart_openflag   (siopcb_table[0].openflag)

Inline void
uart_putc(unsigned char c){
  while(sil_rew_mem((VP)UARTLITE_SRREG) & UARTLITE_TX_FIFO_FULL);
  sil_wrw_mem((VP)UARTLITE_TXREG, c);
}

Inline unsigned char
uart_getc(void){
  while(!(sil_rew_mem((VP)UARTLITE_SRREG) & UARTLITE_RX_FIFO_VALID_DATA));
  return (char)(sil_rew_mem((VP)UARTLITE_RXREG));
}

/*
 *  コールバックルーチンの識別番号
 *  OPB_UARTLITE は，送受信割込みが分かれていないため，意味はない．
 */
#define SIO_ERDY_SND    1u              /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u              /* 受信通知コールバック */


/*                                                                           
 *  SIOドライバの初期化ルーチン                                                
 */
extern void uart_initialize(void);


/*                                                                           
 *  シリアルI/Oポートのオープン                                              
 */
extern SIOPCB   *uart_opn_por(ID siopid, VP_INT exinf);

/*                                                                         
 *  シリアルI/Oポートのクローズ                                             
 */
extern void uart_cls_por(SIOPCB *siopcb);

/*                                                                            
 *  シリアルI/Oポートへの文字送信                                            
 */
extern BOOL uart_snd_chr(SIOPCB *siopcb, INT chr);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT  uart_rcv_chr(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void uart_ena_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void uart_dis_cbr(SIOPCB *siopcb, UINT cbrtn);


/*                                                                            
 *  SIOの割込みサービスルーチン                                              
 */
extern void uart_isr(void);


/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void uart_ierdy_snd(VP_INT exinf);


/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void uart_ierdy_rcv(VP_INT exinf);




#endif  /* _MACRO_ONLY */

#endif /* _MICROBLAZE_H_*/

