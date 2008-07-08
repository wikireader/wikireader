/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
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
 *  @(#) $Id: sfruart.c,v 1.4 2007/01/05 02:33:59 honda Exp $
 */

/*
 *	OAKS16 SFR-UART0,1用シリアルI/Oモジュール
 */
#include <s_services.h>
#include "oaks16.h"
#include <sfruart.h>

/*
 *  デバイスオープンのリトライカウント
 *
 *  500回リトライすれば、十分に終了する．
 */
#define	SFRUART_COUNT	500

/*
 * シリアルコントロールレジスタの設定値 
 */
#define	MR_DEF		0x05	/* 内部クロック、非同期、8ビット、パリティなし、スリープなし */
#define	C0_DEF		0x10	/* RTS/CTS未使用, カウントソースf1 */
#define	C1_DEF		0x00	/* シリアル送受信不可 */
#define	BRG1_DEF	51		/* 19200bps = 16000000/{(UxBRG+1)*16 */
#define	BRG2_DEF	25		/* 38400bps = 16000000/{(UxBRG+1)*16 */
#define	TB_LEVEL	4		/* 送信割込みレベル */
#define	RB_LEVEL	5		/* 受信割込みレベル */

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	UH  cntrl;		/* コントロールレジスタの番地 */
	UH  hint;		/* 割込みレジスタの番地 */

	UB  mr_def;		/* デフォールトの設定値 (MR) */
	UB  c0_def;		/* デフォールトの設定値 (C0) */
	UB  brg_def;	/* デフォールトの設定値 (BRG) */
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block {
	const SIOPINIB	*siopinib;	/* シリアルI/Oポート初期化ブロック */
	VP_INT exinf;	/* 拡張情報 */
	UB     cr1;		/* CR1の設定値（割込み許可）*/
	UB     tic;		/* 送信割込みレベル */
} SIOPCB;

/*
 *  シリアルI/Oポート初期化ブロック
 *
 *  ID = 1 をuart0，ID = 2 をuart1に対応させている．
 */
static const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ TADR_SFR_UART0_BASE, TADR_SFR_INT_BASE, MR_DEF, C0_DEF, BRG1_DEF },
	{ TADR_SFR_UART1_BASE, TADR_SFR_INT_BASE+2, MR_DEF, C0_DEF, BRG1_DEF }
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
static SIOPCB siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  SIOドライバの初期化ルーチン
 */
void
sfruart_initialize(void)
{
	SIOPCB	*siopcb;
	UINT	i;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (siopcb = siopcb_table, i = 0; i < TNUM_SIOP; siopcb++, i++) {
		siopcb->siopinib = &(siopinib_table[i]);
		siopcb->cr1 = C1_DEF;
		siopcb->tic = 0;
	}
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
sfruart_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB		*siopcb;
	const SIOPINIB	*siopinib;
	int i;

	siopcb = get_siopcb(siopid);
	siopinib = siopcb->siopinib;

	sil_wrb_mem((VP)(siopinib->cntrl+TADR_SFR_UMR_OFFSET), siopinib->mr_def);
	sil_wrb_mem((VP)(siopinib->cntrl+TADR_SFR_UC0_OFFSET), siopinib->c0_def);
	sil_wrb_mem((VP)(siopinib->cntrl+TADR_SFR_UBRG_OFFSET), siopinib->brg_def);

	/*
	 *  シリアル割込みの設定
	 */
	set_ic_ilvl((VP)(siopinib->hint+TADR_SFR_S0TIC_OFFSET), siopcb->tic);
	set_ic_ilvl((VP)(siopinib->hint+TADR_SFR_S0RIC_OFFSET), RB_LEVEL);
	siopcb->cr1 = TBIT_UiC1_TE;
	sil_wrb_mem((VP)(siopinib->cntrl+TADR_SFR_UC1_OFFSET), siopcb->cr1);

	/*
	 * ダミーデータ受信
	 */
	sil_reb_mem((VP)(siopinib->cntrl+TADR_SFR_URB_OFFSET));
	sil_reb_mem((VP)(siopinib->cntrl+TADR_SFR_URB_OFFSET));

	for(i = 0 ; i < SFRUART_COUNT ; i++){	/* オープン時送信READYまで待ち */
		if(sil_reb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_TI)
			break;
	}

	siopcb->exinf = exinf;
	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
sfruart_cls_por(SIOPCB *siopcb)
{
	const SIOPINIB	*siopinib;

	siopinib = siopcb->siopinib;
	siopcb->tic = 0;
	set_ic_ilvl((VP)(siopinib->hint+TADR_SFR_S0TIC_OFFSET), siopcb->tic);
	sil_wrb_mem((VP)(siopinib->hint+TADR_SFR_S0RIC_OFFSET), 0);
	siopcb->cr1 = C1_DEF;
	sil_wrb_mem((VP)(siopinib->cntrl+TADR_SFR_UC1_OFFSET), siopcb->cr1);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
sfruart_snd_chr(SIOPCB *siopcb, char c)
{
	if(sil_reb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_TI){
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UTB_OFFSET), c);
		return(TRUE);
	}
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
sfruart_rcv_chr(SIOPCB *siopcb)
{
	if(sil_reb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_RI){
		return((INT)sil_reb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_URB_OFFSET)));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
sfruart_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:
		siopcb->tic = TB_LEVEL;
		set_ic_ilvl((VP)(siopcb->siopinib->hint+TADR_SFR_S0TIC_OFFSET), siopcb->tic);
		break;
	case SIO_ERDY_RCV:
		siopcb->cr1 |= TBIT_UiC1_RE;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET), siopcb->cr1);
		break;
	deafult:
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
sfruart_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:
		siopcb->tic = 0;
		set_ic_ilvl((VP)(siopcb->siopinib->hint+TADR_SFR_S0TIC_OFFSET), siopcb->tic);
		break;
	case SIO_ERDY_RCV:
		siopcb->cr1 &= ~TBIT_UiC1_RE;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET), siopcb->cr1);
		break;
	deafult:
		break;
	}
}

/*
 *  uart0からの受信割込み
 */
void serial_in_handler1(void)
{
	SIOPCB *siopcb = &siopcb_table[0];
	if((siopcb->cr1 & TBIT_UiC1_RE) != 0) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		sfruart_ierdy_rcv(siopcb->exinf);
	}
}

/*
 *  uart0からの送信割込み
 */
void serial_out_handler1(void)
{
	SIOPCB *siopcb = &siopcb_table[0];
	if(siopcb->tic > 0 && (sil_reb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_TI)){
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		sfruart_ierdy_snd(siopcb->exinf);
	}
}

/*
 *  uart1からの受信割込み
 */
void serial_in_handler2(void)
{
	SIOPCB *siopcb = &siopcb_table[1];
	if((siopcb->cr1 & TBIT_UiC1_RE) != 0){
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		sfruart_ierdy_rcv(siopcb->exinf);
	}
}

/*
 *  uart1からの送信割込み
 */
void serial_out_handler2(void)
{
	SIOPCB *siopcb = &siopcb_table[1];
	if(siopcb->tic > 0 && (sil_reb_mem((VP)(siopcb->siopinib->cntrl+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_TI)){
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		sfruart_ierdy_snd(siopcb->exinf);
	}
}

