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
 *  @(#) $Id: fdc37c935a.c,v 1.3 2005/11/12 15:00:43 honda Exp $
 */

/*
 * スーパI/O FDC37C935A 用ドライバ
 */

#include <s_services.h>
#include <fdc37c935a.h>

/*
 *  初期化
 */
void
smsc_init(void)
{
    /*
     * Enter Config mode 
     */
    sil_wrh_mem((VP)SMSC_CONFIG_PORT, (0x55 << 8));
    sil_wrh_mem((VP)SMSC_CONFIG_PORT, (0x55 << 8));

    /*
     * Init SCI0
     */
    /* Power on */
    smsc_config_write(0x22, (smsc_config_read(0x22) | 0x10));
    /* Select SCI0 */
    smsc_config_write(0x07, 0x04);
    /* Enable SCI0 */
    smsc_config_write(0x30, 0x01);
    /* Set SCI0 Base Address */
    smsc_config_write(0x60, (SMSC_SCI0_BASE_ADDR & 0xff00) >> 8);
    smsc_config_write(0x61, (SMSC_SCI0_BASE_ADDR & 0xff));    
    /* IRQ4 */
    smsc_config_write(0x70, 0x04);
    

    /*
     *  Exit Config mode
     */
    sil_wrh_mem((VP)SMSC_CONFIG_PORT, (0xAA << 8));
}


/*
 *  スーパーI/O(FDC37C935A)用 簡易SIOドライバ
 */

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_initialization_block {

} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロック
 */
struct sio_port_control_block {
    const SIOPINIB  *siopinib;  /* シリアルI/Oポート初期化ブロック */
    VP_INT          exinf;      /* 拡張情報 */
    BOOL    openflag;           /* オープン済みフラグ */
    BOOL    sendflag;           /* 送信割込みイネーブルフラグ */
    BOOL    getready;           /* 文字を受信した状態 */
    BOOL    putready;           /* 文字を送信できる状態 */
};

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


/*
 * 文字を受信したか?
 */ 
Inline BOOL
smsc_sci0_getready(SIOPCB *siopcb)
{
    return(((sil_reh_mem((VP)SMSC_SCI0_LSR) >> 8) & 0x01) != 0);
}

/*
 * 文字を送信できるか?
 */
Inline BOOL
smsc_sci0_putready(SIOPCB *siopcb)
{
	return(((sil_reh_mem((VP)SMSC_SCI0_LSR) >> 8) & 0x60) != 0);
}

/*
 *  受信した文字の取り出し
 */
Inline char
smsc_sci0_getchar(SIOPCB *siopcb)
{
    return (sil_reh_mem((VP)SMSC_SCI0_RBR) >> 8);
}

/*
 *  送信する文字の書き込み
 */
Inline void
smsc_sci0_putchar(SIOPCB *siopcb, char c)
{
    sil_wrh_mem((VP)SMSC_SCI0_THR, c << 8);
}

/*
 *  送信割込み許可
 */
Inline void
smsc_sci0_enable_send(SIOPCB *siopcb)
{

    sil_wrh_mem((VP)SMSC_SCI0_IER,
                (((sil_reh_mem((VP)SMSC_SCI0_IER) >> 8) | 0x02) << 8));   
}

/*
 *  送信割込み禁止
 */
Inline void
smsc_sci0_disable_send(SIOPCB *siopcb)
{
    sil_wrh_mem((VP)SMSC_SCI0_IER,
                (((sil_reh_mem((VP)SMSC_SCI0_IER) >> 8) & ~0x02) << 8));   
}

/*
 *  受信割込み許可
 */
Inline void
smsc_sci0_enable_rcv(SIOPCB *siopcb)
{

    sil_wrh_mem((VP)SMSC_SCI0_IER,
                (((sil_reh_mem((VP)SMSC_SCI0_IER) >> 8) | 0x01) << 8));   
}

/*
 *  受信割込み禁止
 */
Inline void
smsc_sci0_disable_rcv(SIOPCB *siopcb)
{
    sil_wrh_mem((VP)SMSC_SCI0_IER,
                (((sil_reh_mem((VP)SMSC_SCI0_IER) >> 8) & ~0x01) << 8));   
}





/*
 *  SIOドライバの初期化ルーチン
 *  1ポートしかないため，あまり意味はない
 */
void
smsc_sci0_initialize()
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

/*
 *  割込み以外の初期化
 */
void
fdc37c935a_init(void)
{
    volatile UH dummy;
    
    /* BIT7 = 1 */
    sil_wrh_mem((VP)SMSC_SCI0_LCR, 0x83 << 8);

    /* Set BPS */
    sil_wrh_mem((VP)SMSC_SCI0_DLL, ((SMSC_SCI0_BPS & 0x00ff) << 8));   
    sil_wrh_mem((VP)SMSC_SCI0_DLM, ((SMSC_SCI0_BPS >> 8) << 8));

    /* BIT7 = 0 Divisor Latch BIT6 = 0 No Break : BIT3 = 0 NoParity
       : BIT2 = 0 1Stopbit : BIT1,0 = {1.1} 8bitData */    
    sil_wrh_mem((VP)SMSC_SCI0_LCR, 0x03 << 8);

    /* Do not use FIFO */
    sil_wrh_mem((VP)SMSC_SCI0_FCR, 0x0000);

    /* Clear Status */
    dummy = sil_reh_mem((VP)SMSC_SCI0_RBR);
    dummy = sil_reh_mem((VP)SMSC_SCI0_LSR);    
}

/*
 * オープンしているポートがあるか
 */
BOOL
smsc_sci0_openflag(void)
{
    return(siopcb_table[0].openflag);
}

/*
 * シリアルI/Oポートのオープン
 */
SIOPCB *
smsc_sci0_opn_por(ID siopid, VP_INT exinf)
{
    SIOPCB      *siopcb;
    const SIOPINIB  *siopinib;


    siopcb = get_siopcb(siopid);
    siopinib = siopcb->siopinib;

    fdc37c935a_init();
        
    /* Enable Receive Data Interrupt */
    sil_wrh_mem((VP)SMSC_SCI0_IER, (0x01 << 8));
    sil_wrh_mem((VP)SMSC_SCI0_MCR, 0x08 << 8);

    /*
     * MS7729RSE01では，一度送信割込みを発生させておかないと割込み
     * レベル16の割込みが発生してしまい正しく動作しない．
     */
    /* 送信割り込み要求を許可 */
    sil_wrh_mem((VP)SMSC_SCI0_IER,
                (((sil_reh_mem((VP)SMSC_SCI0_IER) >> 8) | 0x02) << 8));   
    /* 送信割り込み要求を禁止 */        
    sil_wrh_mem((VP)SMSC_SCI0_IER,
                (((sil_reh_mem((VP)SMSC_SCI0_IER) >> 8) & ~0x02) << 8));   

    siopcb->exinf = exinf;
    siopcb->getready = siopcb->putready = FALSE;
    siopcb->openflag = TRUE;

    return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
smsc_sci0_cls_por(SIOPCB *siopcb)
{
    sil_wrh_mem((VP)SMSC_SCI0_IER, 0x00);      /* 割込みの禁止 */
    siopcb->openflag = FALSE;
}

/*
 * シリアルI/Oポートへのポーリングでの出力
 */
void
fdc37c935a_pol_putc(char c)
{
    while(((sil_reh_mem((VP)SMSC_SCI0_LSR) >> 8) & 0x60) == 0)
        ;

    sil_wrh_mem((VP)SMSC_SCI0_THR, c << 8);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
smsc_sci0_snd_chr(SIOPCB *siopcb, char c)
{
    if (smsc_sci0_putready(siopcb)){
        smsc_sci0_putchar(siopcb, c);
        return(TRUE);
    }
    return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
smsc_sci0_rcv_chr(SIOPCB *siopcb)
{
    if (smsc_sci0_getready(siopcb)) {
        return((INT)(UB) smsc_sci0_getchar(siopcb));
    }
    return(-1);
}


/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
smsc_sci0_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{

    switch (cbrtn) {
        case SIO_ERDY_SND:
            smsc_sci0_enable_send(siopcb);
            break;
        case SIO_ERDY_RCV:
            smsc_sci0_enable_rcv(siopcb);
            break;
    }
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
smsc_sci0_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
    switch (cbrtn) {
        case SIO_ERDY_SND:
            smsc_sci0_disable_send(siopcb);
            break;
        case SIO_ERDY_RCV:
            smsc_sci0_disable_rcv(siopcb);
            break;
    }
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
static void
smsc_sci0_isr_siop(SIOPCB *siopcb)
{
    if (smsc_sci0_getready(siopcb)) {
        /*
         *  受信通知コールバックルーチンを呼び出す．
         */
        smsc_sci0_ierdy_rcv(siopcb->exinf);
    }
    if (smsc_sci0_putready(siopcb)) {
        /*
         *  送信可能コールバックルーチンを呼び出す．
         */
        smsc_sci0_ierdy_snd(siopcb->exinf);
    }
}

/*
 *  SIOの割込みサービスルーチン
 */
void
smsc_sci0_isr()
{
    smsc_sci0_isr_siop(&(siopcb_table[0]));
}
