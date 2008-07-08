/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003      by Advanced Data Controls, Corp
 *  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: integrator.h,v 1.2 2004/05/26 19:14:24 honda Exp $
 */

#ifndef _INTEGRATOR_H_
#define _INTEGRATOR_H_

#ifndef _MACRO_ONLY
#include <itron.h>
#include <sil.h>
#endif /* _MACRO_ONLY */

#include "armv4.h"
#include "sys_config.h"
#include "cpu_config.h"




/*
 * CMxx Base Address
 */
#define CM_BASE  0x10000000

/*
 * CM Registers
 */
#define CM_ID           (CM_BASE)
#define CM_PROC         (CM_BASE + 0004)
#define CM_OSC          (CM_BASE + 0x008)
#define CM_CTRL         (CM_BASE + 0x00c)
#define CM_STAT         (CM_BASE + 0x010)
#define CM_LOCK         (CM_BASE + 0x014)
#define CM_SDRAM        (CM_BASE + 0x020)
#define CM_IRQ_STAT     (CM_BASE + 0x040)
#define CM_IRQ_RSTAT    (CM_BASE + 0x044)
#define CM_IRQ_ENSET    (CM_BASE + 0x048)
#define CM_IRQ_ENCLR    (CM_BASE + 0x04c)
#define CM_SOFT_INTSET  (CM_BASE + 0x050)
#define CM_SOFT_INTCLR  (CM_BASE + 0x054)
#define CM_FIR_STAT     (CM_BASE + 0x060)
#define CM_FIR_RSTAT    (CM_BASE + 0x064)
#define CM_FIR_ENSET    (CM_BASE + 0x068)
#define CM_FIR_ENCLR    (CM_BASE + 0x06c)
#define CM_SPD          (CM_BASE + 0x100)


/*
 * Core module oscillator register : CM_OSC
 */
#define L_OD_MASK        (0x7 << 20)
#define L_OD_DIV10       (0x0 << 20)
#define L_OD_DIV2        (0x1 << 20)
#define L_OD_DIV8        (0x2 << 20)
#define L_OD_DIV4        (0x3 << 20)
#define L_OD_DIV5        (0x4 << 20)
#define L_OD_DIV7        (0x5 << 20)
#define L_OD_DIV9        (0x6 << 20)
#define L_OD_DIV6        (0x7 << 20)

#define L_VDW_MASK       (0xff << 12)
#define L_VDW(x)         ((x) << 12)

#define LCLK_MASK        (L_OD_MASK | L_VDW_MASK)
#define LCLK_20MHZ       (L_OD_DIV2 | L_VDW( 0x20))
#define LCLK_33MHZ       (L_OD_DIV2 | L_VDW( 0x3a))
#define LCLK_50MHZ       (L_OD_DIV2 | L_VDW( 0x5c))
#define LCLK_66MHZ       (L_OD_DIV2 | L_VDW( 0x7c))

#define C_OD_MASK        (0x7 << 8)
#define C_OD_DIV10       (0x0 << 8)
#define C_OD_DIV2        (0x1 << 8)
#define C_OD_DIV8        (0x2 << 8)
#define C_OD_DIV4        (0x3 << 8)
#define C_OD_DIV5        (0x4 << 8)
#define C_OD_DIV7        (0x5 << 8)
#define C_OD_DIV9        (0x6 << 8)
#define C_OD_DIV6        (0x7 << 8)

#define C_VDW_MASK       (0xff << 0)
#define C_VDW(x)         ((x) << 0)

#define CORECLK_MASK     (C_OD_MASK | C_VDW_MASK)
#define CORECLK_50MHZ    (C_OD_DIV2 | C_VDW( 0x2a))
#define CORECLK_66MHZ    (C_OD_DIV2 | C_VDW( 0x3a))
#define CORECLK_100MHZ   (C_OD_DIV2 | C_VDW( 0x5c))
#define CORECLK_133MHZ   (C_OD_DIV2 | C_VDW( 0x7d))
#define CORECLK_150MHZ   (C_OD_DIV2 | C_VDW( 0x8e))
#define CORECLK_160MHZ   (C_OD_DIV2 | C_VDW( 0x98))


/*
 * Core module control register : CM_CTRL
 */
#define CM_CTRL_RESET 0x08
#define CM_CTRL_REMAP 0x04
#define CM_CTRL_LED   0x01


/*
 * Core modeule lock register : CM_LOCK
 */
#define CM_LOCK_LOCKED   0x0100
#define LOCKVAL_LOCK     0x0000
#define LOCKVAL_UNLOCK   0xa05f


/*
 * Integrator AP Peripheral Base Address 
 * 
 */
#define IRQ_BASE_REG      0x14000000
#define TIMER_BASE_REG    0x13000000
#define UART0_BASE_REG    0x16000000
#define UART1_BASE_REG    0x17000000


/*
 * Interrupt Control Registers
 */
#define IRQ0_STATUS     (IRQ_BASE_REG)
#define IRQ0_RAWSTAT    (IRQ_BASE_REG + 0x04)
#define IRQ0_ENABLESET  (IRQ_BASE_REG + 0x08)
#define IRQ0_ENABLECLR  (IRQ_BASE_REG + 0x0c)

#define IRQ_EXTINT_BIT      0x15
#define IRQ_PCILBINT_BIT    0x14
#define IRQ_ENUMINT_BIT     0x13
#define IRQ_DEGINT_BIT      0x12
#define IRQ_LINT_BIT        0x11
#define IRQ_PCIINT3_BIT     0x10
#define IRQ_PCIINT2_BIT     0x0f
#define IRQ_PCIINT1_BIT     0x0e
#define IRQ_PCIINT0_BIT     0x0d
#define IRQ_EXPINT3_BIT     0x0c
#define IRQ_EXPINT2_BIT     0x0b
#define IRQ_EXPINT1_BIT     0x0a
#define IRQ_EXPINT0_BIT     0x09
#define IRQ_RTCINT_BIT      0x08
#define IRQ_TM2_BIT         0x07
#define IRQ_TM1_BIT         0x06
#define IRQ_TM0_BIT         0x05
#define IRQ_MOUSEINT_BIT    0x04
#define IRQ_KBDINT_BIT      0x03
#define IRQ_UART1_BIT       0x02
#define IRQ_UART0_BIT       0x01
#define IRQ_SOFTINT_BIT     0x00

#define IRQ_EXTINT     (1 << IRQ_EXTINT_BIT)
#define IRQ_PCILBIN    (1 << IRQ_PCILBIN_BIT)
#define IRQ_ENUMINT    (1 << IRQ_ENUMINT_BIT)
#define IRQ_DEGINT     (1 << IRQ_DEGINT_BIT)
#define IRQ_LINT       (1 << IRQ_LINT_BIT)
#define IRQ_PCIINT3    (1 << IRQ_PCIINT3_BIT)
#define IRQ_PCIINT2    (1 << IRQ_PCIINT2_BIT)
#define IRQ_PCIINT1    (1 << IRQ_PCIINT1_BIT)
#define IRQ_PCIINT0    (1 << IRQ_PCIINT0_BIT)
#define IRQ_EXPINT3    (1 << IRQ_EXPINT3_BIT)
#define IRQ_EXPINT2    (1 << IRQ_EXPINT2_BIT)
#define IRQ_EXPINT1    (1 << IRQ_EXPINT1_BIT)
#define IRQ_EXPINT0    (1 << IRQ_EXPINT0_BIT)
#define IRQ_RTCINT     (1 << IRQ_RTCINT_BIT)
#define IRQ_TM2        (1 << IRQ_TM2_BIT)
#define IRQ_TM1        (1 << IRQ_TM1_BIT)
#define IRQ_TM0        (1 << IRQ_TM0_BIT)
#define IRQ_MOUSEINT   (1 << IRQ_MOUSEINT_BIT)
#define IRQ_KBDINT     (1 << IRQ_KBDINT_BIT)
#define IRQ_UART1      (1 << IRQ_UART1_BIT)
#define IRQ_UART0      (1 << IRQ_UART0_BIT)
#define IRQ_SOFTINT    (1 << IRQ_SOFTINT_BIT)


/*
 * Interrupt Mask
 * Timer2は最高優先度をもつ ??
 */

#define MASK_IRQ_TM2         0x3FFFFF
#define MASK_IRQ_TM1         0x3FFF7F
#define MASK_IRQ_TM0         0x3FFF3F
#define MASK_IRQ_MOUSEINT    0x3FFF1F
#define MASK_IRQ_KBDINT      0x3FFF0F
#define MASK_IRQ_UART1       0x3FFF07
#define MASK_IRQ_UART0       0x3FFF03
#define MASK_IRQ_SOFTINT     0x3FFF01

#define MASK_IRQ_EXTINT      0x3FFF00
#define MASK_IRQ_PCILBINT    0x1FFF00
#define MASK_IRQ_ENUMINT     0x0FFF00
#define MASK_IRQ_DEGINT      0x07FF00
#define MASK_IRQ_LINT        0x03FF00
#define MASK_IRQ_PCIINT3     0x01FF00
#define MASK_IRQ_PCIINT2     0x00FF00
#define MASK_IRQ_PCIINT1     0x007F00
#define MASK_IRQ_PCIINT0     0x003F00
#define MASK_IRQ_EXPINT3     0x001F00
#define MASK_IRQ_EXPINT2     0x000F00
#define MASK_IRQ_EXPINT1     0x000700
#define MASK_IRQ_EXPINT0     0x000300
#define MASK_IRQ_RTCINT      0x000100

#define INT_DISABLE_ALL      0x3FFFFF



/*
 * UART0 Control Registers
 */
#define UART0_DR    (UART0_BASE_REG)
#define UART0_RSR   (UART0_BASE_REG + 0x04)
#define UART0_ECR   (UART0_BASE_REG + 0x04)
#define UART0_LCRH  (UART0_BASE_REG + 0x08)
#define UART0_LCRM  (UART0_BASE_REG + 0x0C)
#define UART0_LCRL  (UART0_BASE_REG + 0x10)
#define UART0_CR    (UART0_BASE_REG + 0x14)
#define UART0_FR    (UART0_BASE_REG + 0x18)
#define UART0_IIR   (UART0_BASE_REG + 0x1C)
#define UART0_ICR   (UART0_BASE_REG + 0x1C)


/*
 * UART1 Control Registers
 */
#define UART1_DR    (UART1_BASE_REG)
#define UART1_RSR   (UART1_BASE_REG + 0x04)
#define UART1_ECR   (UART1_BASE_REG + 0x04)
#define UART1_LCRH  (UART1_BASE_REG + 0x08)
#define UART1_LCRM  (UART1_BASE_REG + 0x0C)
#define UART1_LCRL  (UART1_BASE_REG + 0x10)
#define UART1_CR    (UART1_BASE_REG + 0x14)
#define UART1_FR    (UART1_BASE_REG + 0x18)
#define UART1_IIR   (UART1_BASE_REG + 0x1C)
#define UART1_ICR   (UART1_BASE_REG + 0x1C)

/*
 * Receive Error Constants : UARTx_RSR
 */
#define URSR_OVERRUN  0x8
#define URSR_BREAK    0x4
#define URSR_PARITY   0x2
#define URSR_FRAMING  0x1

/*
 * Line Control Constants: UARTx_LCRH
 */
/*[6:5] Word Length*/
#define ULCRH_WLEN_8BIT   0x60
#define ULCRH_WLEN_7BIT   0x40
#define ULCRH_WLEN_6BIT   0x20
#define ULCRH_WLEN_5BIT   0x00
/*[4] FIFO*/
#define ULCRH_FEN     0x10
/*[3] Stop Bit*/
#define ULCRH_STP2    0x08
/*[2] Even Parity Select*/
#define ULCRH_EPS     0x04
/*[1] Parity Enable*/
#define ULCRH_PEN     0x02
/*[0] Send Break*/
#define ULCRH_BRK     0x01


/*
 * Line Control Constants: UARTx_LCRM
 */
/*[7:0] Baud Rate*/
#define ULCRM_460800    0x00
#define ULCRL_460800    0x01

#define ULCRM_230400    0x00
#define ULCRL_230400    0x03

#define ULCRM_115200    0x00
#define ULCRL_115200    0x07

#define ULCRM_76800     0x00
#define ULCRL_76800     0x0B

#define ULCRM_57600     0x00
#define ULCRL_57600     0x0F

#define ULCRM_38400     0x0
#define ULCRL_38400     0x17

#define ULCRM_19200     0x00
#define ULCRL_19200     0x2F

#define ULCRM_14400     0x00
#define ULCRL_14400     0x3F

#define ULCRM_9600      0x0
#define ULCRL_9600      0x5F

#define ULCRM_2400      0x00
#define ULCRL_2400      0x7F

#define ULCRM_1200      0x00
#define ULCRL_1200      0xFF


/*
 * Control Register Constants : UARTx_CR
 */
/*[7] Loop Back Enable*/
#define UCR_LBE     0x80
/*[6] Receive Timeout Interrupt Enable*/
#define UCR_RTIE    0x40
/*[5] Transmit Interrupt Enable*/
#define UCR_TIE     0x20
/*[4] Receive Interrupt Enable*/
#define UCR_RIE     0x10
/*[3] Modem Status Interrupt Enable*/
#define UCR_MSIE    0x08
/*[0] UART Enable*/
#define UCR_UARTEN  0x01

/*
 * Flag Register Constants : UARTx_FR
 */
/*[7] Transmit FIFO Empty*/
#define UFR_TXFE    0x80
/*[6] Receive FIFO Full*/
#define UFR_RXFF    0x40
/*[5] Transmit FIFO Full*/
#define UFR_TXFF    0x20
/*[4] Receive FIFO Empty*/
#define UFR_RXFE    0x10
/*[3] UART Busy*/
#define UFR_BUSY    0x08
/*[2] Data Carrier Detect*/
#define UFR_DCD     0x04
/*[1] Data Set Ready*/
#define UFR_DSR     0x02
/*[0] Clear To Send*/
#define UFR_CTS     0x01

/*
 * Interrupt Identifier Constantse: UARTx_IIR
 */
/*[3] Receive Timeout Interrupt Status*/
#define IIR_TRIS    0x08
/*[2] Transmit Interrupt Status*/
#define IIR_TIS     0x04
/*[1] Receive Interrupt Status*/
#define IIR_RIS     0x02
/*[0] Modem Interrupt Status*/
#define IIR_MIS     0x01


/*
 * Timer 1 Control Registers
 */
#define TM1_LOAD  (TIMER_BASE_REG + 0x100)
#define TM1_VAL   (TIMER_BASE_REG + 0x104)
#define TM1_CTRL  (TIMER_BASE_REG + 0x108)
#define TM1_CLR   (TIMER_BASE_REG + 0x10c)

/*
 * Timer 0 Control Registers
 */
#define TM2_LOAD   (TIMER_BASE_REG + 0x200)
#define TM2_VAL    (TIMER_BASE_REG + 0x204)
#define TM2_CTRL   (TIMER_BASE_REG + 0x208)
#define TM2_CLR    (TIMER_BASE_REG + 0x20C)

/*
 * Timer Constants : TMx_CTRL and TMx_CLR
 */
#define TCTRL_STOP 0x0 
/*
 * start bit (7th bit) enabled
 * periodic bit (6th bit) enabled
 */
#define TCTRL_START 0x80
#define TCLR_CLEAR  0x01

#define TCTRL_PERIODIC 0x40

#define TCTRL_D016 0x04
#define TCTRL_D256 0x08

/*disable timer1 interrupt*/
#define TCLR_DISABLE 0x20

#ifndef _MACRO_ONLY

/*
 * 内蔵UART用 簡易SIOドライバ
 */
/*
 * カーネル起動時用の初期化(sys_putcを使用するため)
 */
extern void init_uart(void);


/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_initialization_block 
{
    VP uart_data;
    VP uart_control;
    VP linectrl_lo;
    VP linectrl_mid;
    VP linectrl_hi;
    VP int_identifier;
    VP flag_register;
    VW irq_bit;
}
SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block 
{
    const SIOPINIB  *siopinib;  /* シリアルI/Oポート初期化ブロック */
    VP_INT          exinf;      /* 拡張情報 */
    BOOL    openflag;       /* オープン済みフラグ */
    BOOL    sendflag;       /* 送信割込みイネーブルフラグ */
    BOOL    getready;       /* 文字を受信した状態 */
    BOOL    putready;       /* 文字を送信できる状態 */

}SIOPCB;


/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND    1u      /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u      /* 受信通知コールバック */

/*
 *  オンチップのUARTからのポーリング出力
 */
extern void uart_putc(char c);

/*
 *  SIOドライバの初期化ルーチン
 */
extern void uart_initialize(void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL uart_openflag(void);

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
extern BOOL uart_snd_chr(SIOPCB *siopcb, char c);

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
extern void uart_in_isr(void);
extern void uart_out_isr(void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void uart_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void uart_ierdy_rcv(VP_INT exinf);



#endif /* _MACRO_ONLY */
#endif /*  _INTEGRATOR_H_ */
