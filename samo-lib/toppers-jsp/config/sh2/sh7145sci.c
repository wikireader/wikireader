/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2003 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2002-2004 by Hokkaido Industrial Research Institute, JAPAN
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
 *  @(#) $Id: sh7145sci.c,v 1.3 2005/07/06 00:45:07 honda Exp $
 */

/*
 *   SH2内蔵シリアルコミュニケーションインタフェースSCI用 簡易ドライバ
 */

#include <s_services.h>
#include "sh7145sci.h"

/*
 * シリアルI/Oポート初期化ブロック
 */
#ifndef GDB_STUB
const SIOPINIB siopinib_table[TNUM_PORT] = {
	{0xffff81b0, BRR9600, 0x0, 6}, /* SCI1 */
#if TNUM_PORT >= 2
	{0xffff81a0, BRR9600, 0x0, 6}, /* SCI0 */
#endif /* TNUM_PORT >= 2 */
};
#else /* GDB_STUB */
const SIOPINIB siopinib_table[TNUM_PORT] = {
	{0xffff81a0, BRR9600, 0x0, 6}, /* SCI0 */
};
#endif /* GDB_STUB */


/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
static SIOPCB siopcb_table[TNUM_PORT];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
				/*  ポートIDからデバイス番号を求めるマクロ  */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  文字を受信できるか？
 */
Inline BOOL
sh2sci_getready (SIOPCB * siopcb)
{
	/*  レシーブデータレジスタフル・フラグのチェック  */
	return (sil_reb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SSR)) &
			SSR_RDRF);
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
sh2sci_putready (SIOPCB * siopcb)
{
	/*  トランスミットデータレジスタエンプティ・フラグのチェック */
	return (sil_reb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SSR)) &
			SSR_TDRE);
}

/*
 *  受信した文字の取出し
 */
Inline char
sh2sci_getchar (SIOPCB * siopcb)
{
	char data;

	data = sil_reb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_RDR));
	/*  レシーブデータレジスタフル・フラグのクリア  */
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SSR),
				 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
									  SCI_SSR)) & ~SSR_RDRF);
	return data;
}

/*
 *  送信する文字の書込み
 */
Inline void
sh2sci_putchar (SIOPCB * siopcb, char c)
{
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_TDR), c);
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SSR),
				 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
									  SCI_SSR)) & ~SSR_TDRE);
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
sh2sci_initialize ()
{
	SIOPCB *siopcb;
	UINT i;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (siopcb = siopcb_table, i = 0; i < TNUM_PORT; siopcb++, i++) {
		siopcb->openflag = FALSE;
		siopcb->siopinib = (&siopinib_table[i]);
	}
}

/*
 *  オープンしているポートがあるか？
 */
BOOL
sh2sci_openflag (ID siopid)
{
	return (siopcb_table[siopid - 1].openflag);
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
sh2sci_opn_por (ID siopid, VP_INT exinf)
{
	SIOPCB *siopcb = get_siopcb (siopid);

	/*  送受信停止  */
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR), 0x00);

	/*  SCIデータ入出力ポートの設定  */
	/*  ピンアサイン */
	/* sys_initializeで設定 */

	/*  送受信フォーマット  */
	/*  調歩同期式  */
	/*  8ビット、パリティなし  */
	/*  ストップビットレングス：1   */
	/*  クロックセレクト */

	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SMR),
				 siopcb->siopinib->smr);
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_BRR),
				 (VB) siopcb->siopinib->brr);

	/*
	 * ボーレートの設定後、1カウント分待たなければならない。
	 */
	sil_dly_nse (sh2sci_DELAY);	/* 値はｓｈ１と同じ */

	/* エラーフラグをクリア */
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SSR),
				 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
									  SCI_SSR)) & ~(SSR_ORER | SSR_FER |
													SSR_PER));
	/*送受信許可、受信割り込み許可 */
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR),
				 (VB) (SCI_RIE | SCI_TE | SCI_RE));

	siopcb->exinf = exinf;
	siopcb->openflag = TRUE;
	return (siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
sh2sci_cls_por (SIOPCB * siopcb)
{
	/*  送受信停止、割込み禁止  */
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR),
				 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
									  SCI_SCR)) & (VB) ~ (SCI_TIE | SCI_RIE |
														  SCI_TE | SCI_RE));
	siopcb->openflag = FALSE;
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
sh2sci_snd_chr (SIOPCB * siopcb, char c)
{
	if (sh2sci_putready (siopcb)) {
		sh2sci_putchar (siopcb, c);
		return (TRUE);
	}
	return (FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
sh2sci_rcv_chr (SIOPCB * siopcb)
{
	if (sh2sci_getready (siopcb)) {
		return ((INT) (UB) sh2sci_getchar (siopcb));
		/*  (UB)でキャストするのはゼロ拡張にするため  */
	}
	return (-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
sh2sci_ena_cbr (SIOPCB * siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:			/* 送信割り込み要求を許可 */
		sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR),
					 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
										  SCI_SCR)) | SCI_TIE);
		break;
	case SIO_ERDY_RCV:			/* 受信割り込み要求を許可 */
		sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR),
					 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
										  SCI_SCR)) | SCI_RIE);
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
sh2sci_dis_cbr (SIOPCB * siopcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:			/* 送信割り込み要求を禁止 */
		sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR),
					 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
										  SCI_SCR)) & ~SCI_TIE);
		break;
	case SIO_ERDY_RCV:			/* 受信割り込み要求を禁止 */
		sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR),
					 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
										  SCI_SCR)) & ~SCI_RIE);
		break;
	}
}

/*
 *  シリアルI/Oポートに対する送信割込み処理
 */
Inline void
sh2sci_isr_siop_out (SIOPCB * siopcb)
{
	VB scr0 = sil_reb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR));

	if ((scr0 & SCI_TIE) != 0 && sh2sci_putready (siopcb)) {
		/*
		 *  送信通知コールバックルーチンを呼び出す．
		 */
		sh2sci_ierdy_snd (siopcb->exinf);
	}
}

/*
 *  シリアルI/Oポートに対する受信割込み処理
 */
Inline void
sh2sci_isr_siop_in (SIOPCB * siopcb)
{
	VB scr0 = sil_reb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SCR));

	if ((scr0 & SCI_RIE) != 0 && sh2sci_getready (siopcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		sh2sci_ierdy_rcv (siopcb->exinf);
	}
}

/*
 *  シリアルI/Oポートに対する受信エラー割込み処理
 */
Inline void
sh2sci_isr_siop_err (SIOPCB * siopcb)
{
	sil_wrb_mem ((VB *) (siopcb->siopinib->reg_base + SCI_SSR),
				 sil_reb_mem ((VB *) (siopcb->siopinib->reg_base +
									  SCI_SSR)) & ~(SSR_ORER | SSR_FER |
													SSR_PER));
}

/*
 *  SCI送信割込みサービスルーチン
 *  
 */
void
sh2sci_isr_out (void)
{
	if (siopcb_table[0].openflag) {
		sh2sci_isr_siop_out (get_siopcb (1));
	}
}

/*
 *  SIO受信割込みサービスルーチン
 *  
 */
void
sh2sci_isr_in (void)
{
	if (siopcb_table[0].openflag) {
		sh2sci_isr_siop_in (get_siopcb (1));
	}
}

/*
 *  SIO受信エラー割込みサービスルーチン
 */
void
sh2sci_isr_error (void)
{
	if (siopcb_table[0].openflag) {
		sh2sci_isr_siop_err (get_siopcb (1));
	}
}

/*
 * ポーリングによる文字の送信
 */
void
sh2sci_putc_pol (ID portid, char c)
{
	while (!sh2sci_putready (&siopcb_table[portid - 1]));
	sh2sci_putchar (&siopcb_table[portid - 1], c);
}


#if TNUM_PORT >= 2
/*
 *  SIO受信割込みサービスルーチン
 *  
 */
void
sh2sci_isr2_in (void)
{
	if (siopcb_table[1].openflag) {
		sh2sci_isr_siop_in (get_siopcb (2));
	}
}

/*
 *  SCI送信割込みサービスルーチン
 *  
 */
void
sh2sci_isr2_out (void)
{
	if (siopcb_table[1].openflag) {
		sh2sci_isr_siop_out (get_siopcb (2));
	}
}

/*
 *  SIO受信エラー割込みサービスルーチン
 */
void
sh2sci_isr2_error (void)
{
	if (siopcb_table[1].openflag) {
		sh2sci_isr_siop_err (get_siopcb (2));
	}
}
#endif /* of #if TNUM_PORT >= 2 */
