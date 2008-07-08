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
 *  @(#) $Id: st16c2550.c,v 1.4 2005/11/12 15:00:43 honda Exp $
 */

/*
 *     ST16C2550 用 簡易SIOドライバ
 */
#include <s_services.h>
#include <st16c2550.h>
 
/*
 * 各レジスタのオフセット
 */
#define ST16C_RHR   0x00   /* H  LCR bit7=0 Read  */
#define ST16C_THR   0x00   /* H  LCR bit7=0 Write */
#define ST16C_DLL   0x00   /* H  LCR bit7=1       */
#define ST16C_IER   0x02   /* H  LCR bit7=0       */
#define ST16C_DLM   0x02   /* H  LCR bit7=1       */
#define ST16C_ISR   0x04   /* H  Read  */
#define ST16C_FCR   0x04   /* H  Write */
#define ST16C_LCR   0x06   /* H  */
#define ST16C_MCR   0x08   /* H  */
#define ST16C_LSR   0x0a   /* H  Read */
#define ST16C_MSR   0x0c   /* H  Read */
#define ST16C_SPR   0x0e   /* H  */

#define ISR_TX            0x02       /* 送信割り込み発生 */
#define IER_TX            0x02       /* 送信割り込み許可 */
#define ISR_RX            0x01       /* 受信割り込み発生 */
#define IER_RX            0x01       /* 受信割り込み許可 */

#define LCR_DL_MODE       0x80       /* Divisor Enable */
#define LCR_VAL           0x03       /* 8bit,1stop,Noparity,No break */
#define FCR_FIFO_DISABLE  0x00

#define LSR_RX_DATA_READY 0x01
#define LSR_TX_EMPTY      0x20

#define MCR_INT_ENABLE    0x08

/*
 *  115200bpsに設定
 *  4 = (7372800Hz)/(115200*16)
 */
#define DLM_VAL  0x00
#define DLL_VAL  0x04




/*
 * シリアルI/Oポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
    {ST16C_CHB, LCR_VAL, DLM_VAL, DLL_VAL, PINTER_PINT7E},
#if TNUM_SIOP > 2
    {ST16C_CHA, LCR_VAL, DLM_VAL, DLL_VAL, PINTER_PINT6E},
#endif /* TNUM_SIOP > 2 */    
};

/*
 *  シリアルI/Oポート初期化ブロックの取出し
 */
#define INDEX_SIOPINIB(siopid)  ((UINT)((siopid) - 1))
#define get_siopinib(siopid)  (&(siopinib_table[INDEX_SIOPINIB(siopid)]))


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
st16c_write(UW addr, UW offset, UB val)
{
    sil_wrh_mem((VP)(addr + offset),val);
}

Inline UB
st16c_read(UW addr, UW offset)
{
    return(sil_reh_mem((VP)(addr + offset)));
}


/*
 * 文字を受信したか?
 */ 
Inline BOOL
st16c_getready(SIOPCB *siopcb)
{
    UH status;

    status = st16c_read(siopcb->siopinib->reg_base, ST16C_LSR);
    
    return((status & LSR_RX_DATA_READY));
}

/*
 * 文字を送信できるか?
 */
Inline BOOL
st16c_putready(SIOPCB *siopcb)
{
    UH status;

    status = st16c_read(siopcb->siopinib->reg_base, ST16C_LSR);
    
    return (status & LSR_TX_EMPTY);
}

/*
 *  受信した文字の取り出し
 */
Inline UB
st16c_getchar(SIOPCB *siopcb)
{
    return(st16c_read(siopcb->siopinib->reg_base,ST16C_RHR));
}

/*
 *  送信する文字の書き込み
 */
Inline void
st16c_putchar(SIOPCB *siopcb, UB c)
{
    st16c_write(siopcb->siopinib->reg_base, ST16C_THR, c);
}

/*
 *  送信割込み許可
 */
Inline void
st16c_enable_send(SIOPCB *siopcb)
{
    st16c_write(siopcb->siopinib->reg_base, ST16C_IER,
                (st16c_read(siopcb->siopinib->reg_base,ST16C_IER) | IER_TX));
}

/*
 *  送信割込み禁止
 */
Inline void
st16c_disable_send(SIOPCB *siopcb)
{
    st16c_write(siopcb->siopinib->reg_base, ST16C_IER,
                (st16c_read(siopcb->siopinib->reg_base, ST16C_IER) & ~IER_TX));
                
}

/*
 *  受信割込み許可
 */
Inline void
st16c_enable_rcv(SIOPCB *siopcb)
{
    st16c_write(siopcb->siopinib->reg_base, ST16C_IER,
                (st16c_read(siopcb->siopinib->reg_base,ST16C_IER) | IER_RX));
}

/*
 *  受信割込み禁止
 */
Inline void
st16c_disable_rcv(SIOPCB *siopcb)
{
    st16c_write(siopcb->siopinib->reg_base, ST16C_IER,
                (st16c_read(siopcb->siopinib->reg_base, ST16C_IER) & ~IER_RX));
                
}


/*
 *  SIOドライバの初期化ルーチン
 */
void
st16c2550_initialize()
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
 *
 */
void
st16c2550_init_siopinib(const SIOPINIB *siopinib)
{
    /*
     *  分周比の設定
     */
    /* Divisor Enable */
    st16c_write(siopinib->reg_base, ST16C_LCR,
                (st16c_read(siopinib->reg_base, ST16C_LCR) | LCR_DL_MODE));
    st16c_write(siopinib->reg_base, ST16C_DLL, siopinib->dll_val);
    st16c_write(siopinib->reg_base, ST16C_DLM, siopinib->dlm_val);
    /* Divisor Disable */
    st16c_write(siopinib->reg_base, ST16C_LCR,
                (st16c_read(siopinib->reg_base, ST16C_LCR) & ~LCR_DL_MODE));

    /* モード設定 */
    st16c_write(siopinib->reg_base, ST16C_LCR, siopinib->lcr_val);

    /* FIFO Disable */
    st16c_write(siopinib->reg_base, ST16C_FCR, FCR_FIFO_DISABLE);

    /* 割込み禁止 */
    st16c_write(siopinib->reg_base, ST16C_IER, 0x00);
}


/*
 *  カーネル起動時のバーナー出力用の初期化
 */
void
st16c2550_init(void)
{
    
    st16c2550_init_siopinib(get_siopinib(1));
#if TNUM_SIOP > 2
    st16c2550_init_siopinib(get_siopinib(2));    
#endif /* TNUM_SIOP > 2 */        
    
}

/*
 * オープンしているポートがあるか
 */
BOOL
st16c2550_openflag(void)
{
#if TNUM_SIOP < 2
    return(siopcb_table[0].openflag);
#else /* TNUM_SIOP < 2 */
    return(siopcb_table[0].openflag || siopcb_table[1].openflag);
#endif /* TNUM_SIOP < 2 */
}


/*
 * シリアルI/Oポートのオープン
 */
SIOPCB *
st16c2550_opn_por(ID siopid, VP_INT exinf)
{
    SIOPCB      *siopcb;
    const SIOPINIB  *siopinib;

    siopcb = get_siopcb(siopid);
    siopinib = siopcb->siopinib;
    
    /*
     * 初期化
     */
    st16c2550_init_siopinib(siopcb->siopinib);
    
    /* 受信割込み許可 */
    st16c_write(siopcb->siopinib->reg_base, ST16C_IER, IER_RX);
    
    /* 割込み線をイネーブル */
    st16c_write(siopcb->siopinib->reg_base, ST16C_MCR, MCR_INT_ENABLE);

    siopcb->exinf = exinf;
    siopcb->getready = siopcb->putready = FALSE;
    siopcb->openflag = TRUE;

    return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
st16c2550_cls_por(SIOPCB *siopcb)
{
    /* 割込み禁止 */
    st16c_write(siopcb->siopinib->reg_base, ST16C_IER, 0x00);   
    siopcb->openflag = FALSE;
}


/*
 *  シリアルI/Oポートへのポーリングでの出力
 */
void
st16c2550_pol_putc(char c, ID siopid)
{
    const SIOPINIB *siopinib;
    
    siopinib = get_siopinib(siopid);
    
    while((st16c_read(siopinib->reg_base, ST16C_LSR) & LSR_TX_EMPTY)
          != LSR_TX_EMPTY)
        ;
    st16c_write(siopinib->reg_base, ST16C_THR, c);
}


/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
st16c2550_snd_chr(SIOPCB *siopcb, char c)
{
    if (st16c_putready(siopcb)){
        st16c_putchar(siopcb, c);
        return(TRUE);
    }
    return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
st16c2550_rcv_chr(SIOPCB *siopcb)
{
    if (st16c_getready(siopcb)) {
        return((INT)(UB) st16c_getchar(siopcb));
    }
    return(-1);
}





/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
st16c2550_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{

    switch (cbrtn) {
        case SIO_ERDY_SND:
            st16c_enable_send(siopcb);
            break;
        case SIO_ERDY_RCV:
            st16c_enable_rcv(siopcb);
            break;
    }
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
st16c2550_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
    switch (cbrtn) {
        case SIO_ERDY_SND:
            st16c_disable_send(siopcb);
            break;
        case SIO_ERDY_RCV:
            st16c_disable_rcv(siopcb);
            break;
    }
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
static void
st16c2550_isr_siop(SIOPCB *siopcb)
{
    if (st16c_getready(siopcb)) {
        /*
         *  受信通知コールバックルーチンを呼び出す．
         */
        st16c2550_ierdy_rcv(siopcb->exinf);
    }
    if (st16c_putready(siopcb)) {
        /*
         *  送信可能コールバックルーチンを呼び出す．
         */
        st16c2550_ierdy_snd(siopcb->exinf);
    }
}

/*
 *  SIOの割込みサービスルーチン
 */
void
st16c2550_isr()
{
    st16c2550_isr_siop(&(siopcb_table[0]));
}
