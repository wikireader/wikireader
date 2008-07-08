/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Platform Development Center
 *                                          RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2003-2004 by Naoki Saito
 *             Nagoya Municipal Industrial Research Institute, JAPAN
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
 *  @(#) $Id: sfruart.c,v 1.4 2005/11/24 12:41:23 honda Exp $
 */

/*
 *	OAKS32 UART用シリアルI/Oモジュール
 */
#include <s_services.h>
#include "oaks32.h"
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
#define MR_DISABLE_DEF	0x00	/* シリアルモジュール無効 */
#define	C0_DEF		0x10	/* RTS/CTS未使用, カウントソースf1, 送信割込み要因は「送信バッファ空」 */
#define	C1_DEF		0x00	/* シリアル送受信禁止 */
#define	BRG1_DEF	48	/* 30000000/{(UxBRG+1)*16} = 38400 */
#define	BRG2_DEF	97	/* 30000000/{(UxBRG+1)*16} = 19200 */

#define	TB_LEVEL	4	/* 送信割込みレベル */
#define	RB_LEVEL	5	/* 受信割込みレベル */
#define DISABLE_LEVEL	0	/* 割込み禁止レベル */

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	UH  cntrl_addr;	/* 制御レジスタの番地 */
	UH  tic_addr;	/* 送信割込み制御レジスタの番地 */
	UH  ric_addr;	/* 受信割込み制御レジスタの番地 */

	UB  mr_def;	/* デフォルトの設定値 (MR) */
	UB  c0_def;	/* デフォルトの設定値 (C0) */
	UB  brg_def;	/* デフォルトの設定値 (BRG) */
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block {
	const SIOPINIB	*siopinib;	/* シリアルI/Oポート初期化ブロック */
	VP_INT exinf;	/* 拡張情報 */
	UB     uc1;	/* 送受信制御レジスタUiC1の設定値 */
};

/*
 *  シリアルI/Oポート初期化ブロック
 *
 *  ID = 1 をuart0，ID = 2 をuart1に対応させている．
 */
static const SIOPINIB siopinib_table[TNUM_SIOP] = {
	{ TADR_SFR_UART0_BASE, (TADR_SFR_INT_BASE+TADR_SFR_S0TIC_OFFSET),
	  (TADR_SFR_INT_BASE+TADR_SFR_S0RIC_OFFSET), MR_DEF, C0_DEF, BRG1_DEF },
#if TNUM_SIOP>1
	{ TADR_SFR_UART1_BASE, (TADR_SFR_INT_BASE+TADR_SFR_S1TIC_OFFSET),
	  (TADR_SFR_INT_BASE+TADR_SFR_S1RIC_OFFSET), MR_DEF, C0_DEF, BRG1_DEF }
#endif
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
		siopcb->uc1 = C1_DEF;
		/*
		 * 明示的にポートを不活性化しておく
		 */
		/* 割り込み禁止 */
		sil_wrb_mem((VP)(siopcb->siopinib->tic_addr), DISABLE_LEVEL);
		sil_wrb_mem((VP)(siopcb->siopinib->ric_addr), DISABLE_LEVEL);
		/* 送受信禁止 */
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);
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

	/* 送受信禁止 */
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET),
	   sil_reb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET))&~(TBIT_UiC1_TE | TBIT_UiC1_RE));

	/* ポート設定(動作モード、通信速度) */
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UMR_OFFSET), siopinib->mr_def);
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UC0_OFFSET), siopinib->c0_def);
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UBRG_OFFSET), siopinib->brg_def);

	/*
	 *  シリアル割込みの設定
	 */
	sil_wrb_mem((VP)(siopinib->tic_addr), TB_LEVEL);
	sil_wrb_mem((VP)(siopinib->ric_addr), RB_LEVEL);

	/* オープン時はコールバック禁止 */
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);

	/*
	 * ダミーデータ受信
	 */
	sil_reb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_URB_OFFSET));
	sil_reb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_URB_OFFSET));

	for(i = 0 ; i < SFRUART_COUNT ; i++){	/* オープン時送信READYまで待ち */
		if(sil_reb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_TI)
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

	/* 割り込み禁止 */
	sil_wrb_mem((VP)(siopinib->tic_addr), DISABLE_LEVEL);
	sil_wrb_mem((VP)(siopinib->ric_addr), DISABLE_LEVEL);

	/* 送受信禁止 */
	siopcb->uc1 = C1_DEF;
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);

	/* シリアルモジュールを無効にする */
	sil_wrb_mem((VP)(siopinib->cntrl_addr+TADR_SFR_UMR_OFFSET), MR_DISABLE_DEF);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
sfruart_snd_chr(SIOPCB *siopcb, char c)
{
	if(sil_reb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_TI){
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UTB_OFFSET), c);
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
	if(sil_reb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET)) & TBIT_UiC1_RI){
		return((INT)sil_reb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_URB_OFFSET)));
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
		siopcb->uc1 |= TBIT_UiC1_TE;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);
		break;
	case SIO_ERDY_RCV:
		siopcb->uc1 |= TBIT_UiC1_RE;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);
		break;
	default:
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
		siopcb->uc1 &= ~TBIT_UiC1_TE;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);
		break;
	case SIO_ERDY_RCV:
		siopcb->uc1 &= ~TBIT_UiC1_RE;
		sil_wrb_mem((VP)(siopcb->siopinib->cntrl_addr+TADR_SFR_UC1_OFFSET), siopcb->uc1);
		break;
	default:
		break;
	}
}

/*
 *  uart0からの受信割込み
 */
void serial_in_handler1(void)
{
	SIOPCB *siopcb = &siopcb_table[0];
	if((siopcb->uc1 & TBIT_UiC1_RE) != 0) {
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

	if((siopcb->uc1 & TBIT_UiC1_TE) != 0) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		sfruart_ierdy_snd(siopcb->exinf);
	}
}

#if TNUM_SIOP > 1
/*
 *  uart1からの受信割込み
 */
void serial_in_handler2(void)
{
	SIOPCB *siopcb = &siopcb_table[1];
	if((siopcb->uc1 & TBIT_UiC1_RE) != 0){
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
	if((siopcb->uc1 & TBIT_UiC1_TE) != 0) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		sfruart_ierdy_snd(siopcb->exinf);
	}
}
#endif
