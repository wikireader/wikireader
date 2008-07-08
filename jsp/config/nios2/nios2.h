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
 *  @(#) $Id: nios2.h,v 1.5 2005/03/11 07:37:57 honda Exp $
 */

#ifndef _NIOSII_H_
#define _NIOSII_H_

#ifndef _MACRO_ONLY
#include <itron.h>
#endif /* _MACRO_ONLY */

/*
 *  割込みビット
 */
#define STATUS_U   0x02
#define STATUS_PIE 0x01

/*
 * 割込みの数
 */
#define MAX_INT_NUM 32

/*
 * 例外の個数
 */
#define MAX_EXC_NUM 32


/*
 *  Timer
 *  Full-featuredサポート 
 */

/*
 * オフセット値
 */
#define TIM_STATUS_OFFSET  0x00
#define TIM_CONTROL_OFFSET 0x04
#define TIM_PERIODL_OFFSET 0x08
#define TIM_PERIODH_OFFSET 0x0C
#define TIM_SNAPL_OFFSET 0x10
#define TIM_SNAPH_OFFSET 0x14

#define TIM_STATUS_RUN 0x02
#define TIM_STATUS_TO  0x01

#define TIM_CONTROL_STOP  0x08
#define TIM_CONTROL_START 0x04
#define TIM_CONTROL_COUNT 0x02
#define TIM_CONTROL_ITO   0x01

/*
 *
 */
#define TIM_STATUS  (TIM_BASE + TIM_STATUS_OFFSET)
#define TIM_CONTROL (TIM_BASE + TIM_CONTROL_OFFSET)
#define TIM_PERIODL (TIM_BASE + TIM_PERIODL_OFFSET)
#define TIM_PERIODH (TIM_BASE + TIM_PERIODH_OFFSET)
#define TIM_SNAPL   (TIM_BASE + TIM_SNAPL_OFFSET)
#define TIM_SNAPH   (TIM_BASE + TIM_SNAPH_OFFSET)


/*
 * UART
 */
#define UART_RXDATA_OFFSET  0x00
#define UART_TXDATA_OFFSET  0x04
#define UART_STATUS_OFFSET  0x08
#define UART_CONTROL_OFFSET 0x0C
#define UART_DIVISOR_OFFSET 0x10
#define UART_ENDOFPACKET_OFFSET 0x1C

#define UART_STATUS_EOP   0x1000
#define UART_STATUS_CTS   0x0800
#define UART_STATUS_DCTS  0x0400
#define UART_STATUS_E     0x0100
#define UART_STATUS_RRDY  0x0080
#define UART_STATUS_TRDY  0x0040
#define UART_STATUS_TMT   0x0020
#define UART_STATUS_TOE   0x0010
#define UART_STATUS_ROE   0x0008
#define UART_STATUS_BRK   0x0004
#define UART_STATUS_FE    0x0002
#define UART_STATUS_PE    0x0001

#define UART_CONTROL_IEOP  0x1000
#define UART_CONTROL_RTS   0x0800
#define UART_CONTROL_IDCTS 0x0400
#define UART_CONTROL_TRDK  0x0200
#define UART_CONTROL_IE    0x0100
#define UART_CONTROL_IRRDY 0x0080
#define UART_CONTROL_ITRD  0x0040
#define UART_CONTROL_ITMT  0x0020
#define UART_CONTROL_ITOE  0x0010
#define UART_CONTROL_IROE  0x0008
#define UART_CONTROL_IBRK  0x0004
#define UART_CONTROL_IFE   0x0002
#define UART_CONTROL_IPE   0x0001

#define UART_RXDATA  (UART_BASE + UART_RXDATA_OFFSET)
#define UART_TXDATA  (UART_BASE + UART_TXDATA_OFFSET)
#define UART_STATUS  (UART_BASE + UART_STATUS_OFFSET)
#define UART_CONTROL (UART_BASE + UART_CONTROL_OFFSET)
#define UART_DIVISOR (UART_BASE + UART_DIVISOR_OFFSET)
#define UART_ENDOFPACKET (UART_BASE + UART_ENDOFPACKET_OFFSET)


/*
 * JTAG UART関連
 */ 
#define JTAG_UART_DATA_OFFSET    0x00
#define JTAG_UART_CONTROL_OFFSET 0x04

#define JTAG_UART_DATA_RVALID 0x8000

#define JTAG_UART_CONTROL_RIE 0x01
#define JTAG_UART_CONTROL_WIE 0x02
#define JTAG_UART_CONTROL_RIP 0x04
#define JTAG_UART_CONTROL_WIP 0x08
#define JTAG_UART_CONTROL_WSAPCE 0x0ffff0000

#define JTAG_UART_DATA    (UART_BASE + JTAG_UART_DATA_OFFSET)
#define JTAG_UART_CONTROL (UART_BASE + JTAG_UART_CONTROL_OFFSET)


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
#ifndef USE_JTAG_UART	
	while(!(sil_rew_mem((VP)UART_STATUS) & UART_STATUS_TRDY));
	sil_wrw_mem((VP)UART_TXDATA, c);
#else
	while(!((sil_rew_mem((VP)JTAG_UART_CONTROL) & JTAG_UART_CONTROL_WSAPCE) > 0));
	sil_wrw_mem((VP)JTAG_UART_DATA, c);	
#endif /* USE_JTAG_UART	*/	
}

Inline unsigned char
uart_getc(void){
#ifndef USE_JTAG_UART	
	while(!(sil_rew_mem((VP)UART_STATUS) & UART_STATUS_RRDY));
	return (char)(sil_rew_mem((VP)UART_RXDATA));
#else
	int tmp;
	do{
		tmp = sil_rew_mem((VP)JTAG_UART_DATA);
	}while((tmp &JTAG_UART_DATA_RVALID) == 0);

	return (char)tmp;
#endif /* USE_JTAG_UART	*/			
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

#endif /* _NIOSII_H_ */

