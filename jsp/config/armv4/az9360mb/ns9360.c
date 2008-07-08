/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005-2007 by Y.D.K.Co.,LTD Technologies company
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
 *  @(#) $Id: ns9360.c,v 1.2 2007/05/21 01:33:50 honda Exp $
 */

/*
 *    内部機能用ドライバ
 */
#include <s_services.h>
#include <ns9360.h>

/*
 * 内部UART用 簡易SIOドライバ
 */

/*
 * シリアルI/Oポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{
		/* UART Channel A */
		 (VP)SC1SRA_REG , (VP)SC1FIFO_REG , (VP)SC1CRA_REG , (VP)SC1CRB_REG
		,(VP)SC1RCT_REG , (VP)SC1RBT_REG , (VP)SC1BRG_REG
		,(SCCRA_CE | SCCRA_WLS | SCCRA_DTR | SCCRA_RTS)
		,0x08000000
		,0x80000008
		,0x80000008
		,(0xC0140000 | 7)			/* 29.4912MHz /2/8 = 115200*16 */
		,BBBI_S1TX , BBBI_S1RX
	 }
#if TNUM_SIOP >= 2
    ,
    {
		/* UART Channel C */
		 (VP)SC3SRA_REG , (VP)SC3FIFO_REG , (VP)SC3CRA_REG , (VP)SC3CRB_REG
		,(VP)SC3RCT_REG ,(VP)SC3RBT_REG ,  (VP)SC3BRG_REG
		,(SCCRA_CE | SCCRA_WLS | SCCRA_DTR | SCCRA_RTS)
		,0x08000000
		,0x80000008
		,0x80000008
		,(0xC0140000 | 7)			/* 29.4912MHz /2/8 = 115200*16 */
		,BBBI_S3TX , BBBI_S3RX
	}
#endif
 };

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
SIOPCB  siopcb_table[TNUM_SIOP];

/*
 * システム出力用シリアルポート情報
 */
SIOPCB      *sys_siopcb;


/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)  ((UINT)((siopid) - 1))
#define get_siopcb(siopid)  (&(siopcb_table[INDEX_SIOP(siopid)]))


/*
 * 文字を受信したか?
 */ 
Inline BOOL
uart_getready(SIOPCB *siopcb)
{
	VW	dt;
	
	if(0 != siopcb->rxfdb)
		return TRUE;
	
	dt = sil_rew_mem((VP)siopcb->siopinib->pSraReg);
	return(dt &(SCSRA_RFS | SCSRA_RRDY));
}

/*
 * 文字を送信できるか?
 */
Inline BOOL
uart_putready(SIOPCB *siopcb)
{
	return(sil_rew_mem((VP)siopcb->siopinib->pSraReg)&(SCSRA_TRDY | SCSRA_THALF | SCSRA_TEMPTY));
}

/*
 *  受信した文字の取り出し
 */
Inline UB
uart_getchar(SIOPCB *siopcb)
{
	UW	dt;
	
	if(0 == siopcb->rxfdb)
	{
		siopcb->rxfdb = (sil_rew_mem((VP)siopcb->siopinib->pSraReg) >> 20) & 0x03;
		if(0 == siopcb->rxfdb)
			siopcb->rxfdb = 4;
		
		siopcb->rxbuf = sil_rew_mem((VP)siopcb->siopinib->pFifoReg);
	}
	--(siopcb->rxfdb);
	dt = (siopcb->rxbuf >> 24) & 0xff;
	siopcb->rxbuf <<= 8;
	
	return (UB)dt;

}

/*
 *  送信する文字の書き込み
 */
Inline void
uart_putchar(SIOPCB *siopcb, UB c)
{
    sil_wrb_mem((VP)siopcb->siopinib->pFifoReg, (VB)c);
}

/*
 *  送信割込み許可
 */
Inline void
uart_enable_send(SIOPCB *siopcb)
{
		sil_wrw_mem((VP)BBBIE_REG,sil_rew_mem((VP)BBBIE_REG) | siopcb->siopinib->BbbiTx);
}

/*
 *  送信割込み禁止
 */
Inline void
uart_disable_send(SIOPCB *siopcb)
{
	sil_wrw_mem((VP)BBBIE_REG,sil_rew_mem((VP)BBBIE_REG) & ~siopcb->siopinib->BbbiTx);
}

/*
 *  受信割込み許可
 */
Inline void
uart_enable_rcv(SIOPCB *siopcb)
{
	sil_wrw_mem((VP)BBBIE_REG,sil_rew_mem((VP)BBBIE_REG) | siopcb->siopinib->BbbiRx);
}

/*
 *  受信割込み禁止
 */
Inline void
uart_disable_rcv(SIOPCB *siopcb)
{
	sil_wrw_mem((VP)BBBIE_REG,sil_rew_mem((VP)BBBIE_REG) & ~siopcb->siopinib->BbbiRx);
}



/*
 * カーネル起動時のログ出力用の初期化
 */
void
uart_init(ID siopid){
    SIOPCB      *siopcb;

	uart_initialize();

    siopcb = get_siopcb(siopid);

	sys_siopcb = siopcb;

    /* UART Initialize */
    sil_wrw_mem((VP)siopcb->siopinib->pCraReg,(VW)siopcb->siopinib->CraData);
	sil_wrw_mem((VP)siopcb->siopinib->pCrbReg,(VW)siopcb->siopinib->CrbData);
	sil_wrw_mem((VP)siopcb->siopinib->pRctReg,(VW)0);
	
    /* Set bound rate */
	sil_wrw_mem((VP)siopcb->siopinib->pBrgReg,siopcb->siopinib->BrgData);
}


/*
 *  オンチップのUARTからのポーリング出力
 */
void
uart_putc(char c)
{
    UW buf_level;
    
    do{
        buf_level = sil_rew_mem((VP)sys_siopcb->siopinib->pSraReg) & SCSRA_TRDY;
    }while(buf_level == 0);
    
    sil_wrb_mem((VP)sys_siopcb->siopinib->pFifoReg,(VW)c);
}

/*
 *  SIOドライバの初期化ルーチン
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
        siopcb->rxfdb = 0;
        siopcb->rxbuf = 0;
        siopcb->siopid = i;
    }
}

/*
 * オープンしているポートがあるか
 */
BOOL
uart_openflag(ID siopid)
{
    return(siopcb_table[siopid].openflag);
}


/*
 * シリアルI/Oポートのオープン
 */
SIOPCB *
uart_opn_por(ID siopid, VP_INT exinf)
{
    SIOPCB      *siopcb;
    const SIOPINIB  *siopinib;
	UW		n;
	
    siopcb = get_siopcb(siopid);
    siopinib = siopcb->siopinib;

    /* UART Initialize */
    sil_wrw_mem((VP)siopinib->pCraReg,(VW)siopinib->CraData);
	sil_wrw_mem((VP)siopinib->pCrbReg,(VW)siopinib->CrbData);
	sil_wrw_mem((VP)siopinib->pRctReg,(VW)siopinib->RctData);
	sil_wrw_mem((VP)siopinib->pRbtReg,(VW)siopinib->RbtData);
	
    /* Set bound rate */
	sil_wrw_mem((VP)siopinib->pBrgReg,siopinib->BrgData);

    /* 受信RcvRdy,FIFO=Half、送信TxEmpty,FIFO-Half割込み許可 */
	n = sil_rew_mem((VP)siopinib->pCraReg);
	n &=  (~(SCCRA_RIE | SCCRA_TIC));
	n |= (0xe00 | 0x0c);
    sil_wrw_mem((VP)siopinib->pCraReg,(VW)n);
	
    /* BBUS割込み許可 */
	sil_wrw_mem((VP)BBBIE_REG,sil_rew_mem((VP)BBBIE_REG) | siopinib->BbbiRx);
	sil_wrw_mem((VP)INT_CFG8_11_REG,sil_rew_mem((VP)INT_CFG8_11_REG)|0x80000000);
	
    siopcb->exinf = exinf;
    siopcb->getready = siopcb->putready = FALSE;
    siopcb->openflag = TRUE;
    siopcb->rxfdb = 0;
    siopcb->rxbuf = 0;

    return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
uart_cls_por(SIOPCB *siopcb)
{
    siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
uart_snd_chr(SIOPCB *siopcb, char c)
{
    if (uart_putready(siopcb)){
        uart_putchar(siopcb, c);
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
 *  シリアルI/Oポートに対する送信割込み処理
 */
static void
uart_Txisr_siop(SIOPCB *siopcb)
{
    /*
     *  送信可能コールバックルーチンを呼び出す．
     */
    uart_ierdy_snd(siopcb->exinf);
}

/*
 *  シリアルI/Oポートに対する受信割込み処理
 */
static void
uart_Rxisr_siop(SIOPCB *siopcb)
{
    /*
     *  受信通知コールバックルーチンを呼び出す．
     *  最大4文字(32bit)受信しているのでここで全受信文字を処理する
     */
    while(1)
    {
        uart_ierdy_rcv(siopcb->exinf);
        if(0 == siopcb->rxfdb)
            break;                           /* 受信済みキャラクタなし */
	}
}


/*
 *  SIOのTx割込みサービスルーチン
 */
void
uart_Tx_isr()
{
	uart_Txisr_siop(&(siopcb_table[0]));
}

/*
 *  SIOのRx割込みサービスルーチン
 */
void
uart_Rx_isr()
{
	uart_Rxisr_siop(&(siopcb_table[0]));
	sil_wrw_mem((VP)siopcb_table[0].siopinib->pSraReg , 
					sil_rew_mem((VP)siopcb_table[0].siopinib->pSraReg) );
}

/*
 *  SIO2のTx割込みサービスルーチン
 */
void
uart2_Tx_isr()
{
	uart_Txisr_siop(&(siopcb_table[1]));
}

/*
 *  SIO2のRx割込みサービスルーチン
 */
void
uart2_Rx_isr()
{
	uart_Rxisr_siop(&(siopcb_table[1]));
	sil_wrw_mem((VP)siopcb_table[1].siopinib->pSraReg , 
					sil_rew_mem((VP)siopcb_table[1].siopinib->pSraReg) );
}


/*
 *  SIOの割込み許可ルーチン
 */
void
uart_TxRx_Enable(ID siopid)
{
	sil_wrw_mem((VP)BBBIE_REG,
				sil_rew_mem((VP)BBBIE_REG) | siopcb_table[siopid].siopinib->BbbiTx 
		                                   | siopcb_table[siopid].siopinib->BbbiRx);
}

/*
 *  SIOの割込み禁止ルーチン
 */
void
uart_TxRx_Disable(ID siopid)
{
		sil_wrw_mem((VP)BBBIE_REG,
				sil_rew_mem((VP)BBBIE_REG) & ~(siopcb_table[siopid].siopinib->BbbiTx
												 | siopcb_table[siopid].siopinib->BbbiRx) );
}

/*
 * 現在のシリアルポートの取得
 */
ID
uart_get_id(SIOPCB *siopcb)
{
	return(siopcb->siopid);
}

