/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: hw_serial.c,v 1.12 2007/03/23 07:22:15 honda Exp $
 */

/*
 *  ターゲット依存シリアルI/Oモジュール（H8用）
 *    ・ポートが 1本の場合は HWPORT1_ADDRに指定されたポートを使用し、
 *      2本の場合は、HWPORT1_ADDRをユーザ用、HWPORT2_ADDRをコンソール
 *      出力として使用する。
 *      HWPORTx_ADDRは ターゲット依存の sys_config.h中で指定する。
 *    ・共通部のモジュールによって、XON/XOFFフロー制御が行わ
 *      れているので、信号線はTxDとRxDの2本でよい
 */

#include "jsp_kernel.h"
#include <hw_serial.h>
#include <h8_sil.h>

/*
 *  ボーレートを1ビット分の時間[nsec]に変換するマクロ
 *  　演算の途中でオーバーフローしないようlong型定数を用いている
 */
#define BAUD_TO_NSEC(b)       (UINT)(((1000000ul / (b)) + 1ul) * 1000ul)

/*
 *  ボーレートをBRRの値に変換するマクロ
 */
#define H8BRR_RATE(b)	(((b)>38400)?((UB)(((CPU_CLOCK+(16*(b)))/(32*(b)))-1))\
			          :((UB)((CPU_CLOCK/(32*(b)))-1)))

/*
 *  シリアルポートの初期化ブロック
 */

static const SIOPINIB siopinib_table[TNUM_PORT] = {
	{
		SYSTEM_SCI,
		SYSTEM_BAUD_RATE,
                {
                        (UB*)SYSTEM_SCI_IPR,
                        SYSTEM_SCI_IP_BIT,
                        SYSTEM_SCI_IPM
                },
		SYSTEM_SCI_SMR
	}

#if TNUM_PORT >= 2
	,{
		USER_SCI,
		USER_BAUD_RATE,
                {
                        (UB*)USER_SCI_IPR,
                        USER_SCI_IP_BIT,
                        USER_SCI_IPM
                },
		USER_SCI_SMR
	}
#endif	/* of #if TNUM_PORT >= 2 */

#if TNUM_PORT >= 3
	,{
		USER2_SCI,
		USER2_BAUD_RATE,
                {
                        (UB*)USER2_SCI_IPR,
                        USER2_SCI_IP_BIT,
                        USER2_SCI_IPM
                },
		USER2_SCI_SMR
	}
#endif	/* of #if TNUM_PORT >= 3 */

};


/*
 *  シリアルポートの制御ブロック
 */

SIOPCB siopcb_table[TNUM_PORT];

/*
 *  SIO ID から管理ブロックへの変換マクロ
 */

#define get_siopinib(sioid)	(&(siopinib_table[INDEX_SIO(sioid)]))

/*
 *  SCI の初期化
 */

void
SCI_initialize (ID sioid)
{
	const SIOPINIB	*inib;

	inib = get_siopinib(sioid);
	SCI_cls_por(inib->base);
			/*
			 *　モード設定
			 *　　・調歩同期式
			 *　　・キャラクタ長
			 *　　・パリティ
			 *　　・ストップビット長
			 *　　・クロック選択
			 */
	sil_wrb_mem((VP)(inib->base + H8SMR), inib->smr_init);

					/*  ボーレート設定  	*/
	sil_wrb_mem((VP)(inib->base + H8BRR), H8BRR_RATE(inib->baudrate));

			/* 割込み禁止とクロックソースの選択	*/
	h8_anb_reg((UB *)(inib->base + H8SCR),
            	    (UB)~(H8SCR_TIE  | H8SCR_RIE  | H8SCR_MPIE |
            	          H8SCR_TEIE | H8SCR_CKE1 | H8SCR_CKE0 ));

	/* ボーレートの設定後、1ビット分待たなければならない。*/
	sil_dly_nse(BAUD_TO_NSEC(inib->baudrate));

					/* エラーフラグをクリア		*/
	h8_anb_reg((UB *)(inib->base + H8SSR),
	            ~(H8SSR_ORER | H8SSR_FER | H8SSR_PER));

        /*
         *  プライオリティレベルの設定
         *      本当は割込みコントローラ依存部分を分離すべき
         */
        define_int_plevel(&(inib->irc));

	/* 受信割り込みと送信割込みの許可はシリアル I/O で行う */
	/* 送受信許可 */
	h8_orb_reg((VP)(inib->base + H8SCR), (H8SCR_TE | H8SCR_RE));
}

/*
 *  SCI のクローズ
 */

void
SCI_cls_por (UW base)
{
	/* TDRE が 1 になるまで待つ */
	while ((sil_reb_mem((VP)(base + H8SSR)) & H8SSR_TDRE) == 0)
		;

	/* 11ビット送信分待つ。*/
	sil_dly_nse(11*BAUD_TO_NSEC(H8_MIN_BAUD_RATE));
					/* 送受信停止		*/
	h8_anb_reg((UB *)(base + H8SCR),
	           (UB)~(H8SCR_TIE | H8SCR_RIE | H8SCR_TE | H8SCR_RE));
}

/*
 *  SCI_in_handler -- SCI 入力割込みハンドラ
 */

void
SCI_in_handler(ID sioid)
{
	SIOPCB	*pcb;
	UB	status;
	
	pcb = get_siopcb(sioid);
	status = sil_reb_mem((VP)(pcb->inib->base + H8SSR));
	
	if (status & (H8SSR_ORER | H8SSR_FER | H8SSR_PER)) {

		/* エラー処理		*/

	    	/* エラーフラグをクリア	*/
		sil_wrb_mem((VP)(pcb->inib->base + H8SSR),
		            status & ~(H8SSR_ORER | H8SSR_FER | H8SSR_PER));
	}

	if (status & H8SSR_RDRF) {
		if (pcb->openflag) {
			/* 受信可能コールバックルーチンを呼出す。*/
			SCI_ierdy_rcv(pcb->exinf);
		} else {
			sil_wrb_mem((VP)(pcb->inib->base + H8SSR), 
			             status & ~H8SSR_RDRF);
		}
	}
}

/*
 *  SCI_out_handler -- SCI 出力割込みハンドラ
 */

void
SCI_out_handler(ID sioid)
{
	SIOPCB	*pcb;

	pcb = get_siopcb(sioid);
	if (pcb->openflag) {
		/* 送信可能コールバックルーチンを呼出す。*/
		SCI_ierdy_snd(pcb->exinf);
	}
}

#ifdef H8_CFG_SCI_ERR_HANDLER

/*
 *  SCI_err_handler -- SCI エラー入力割込みハンドラ
 */

void
SCI_err_handler(ID sioid)
{
	SIOPCB	*pcb;
	UB	status;

	pcb = get_siopcb(sioid);
	status = sil_reb_mem((VP)(pcb->inib->base + H8SSR));
	if (status & (H8SSR_ORER | H8SSR_FER | H8SSR_PER)) {

		/* エラー処理		*/

	    	/* エラーフラグをクリア	*/
		sil_wrb_mem((VP)(pcb->inib->base + H8SSR),
		            status & ~(H8SSR_ORER | H8SSR_FER | H8SSR_PER));
	}
}

#endif	/* of #ifdef H8_CFG_SCI_ERR_HANDLER */

/*
 *  sio_initialize -- シリアルポートドライバの初期化
 */

void
sio_initialize (void)
{
	int ix;

	/* SIO 管理ブロックの初期化 */
	for (ix = TNUM_PORT; ix -- > 0; ) {
		siopcb_table[ix].inib     = &siopinib_table[ix];
		siopcb_table[ix].openflag = FALSE;
	}
}

/*
 *  sio_ena_cbr -- シリアル I/O からのコールバックの許可
 */

void
sio_ena_cbr(SIOPCB *pcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:
		SCI_enable_send(pcb);
		break;
	case SIO_ERDY_RCV:
		SCI_enable_recv(pcb);
		break;
	}
}

/*
 *  sio_dis_cbr -- シリアル I/O からのコールバックの禁止
 */

void
sio_dis_cbr(SIOPCB *pcb, UINT cbrtn)
{
	switch (cbrtn) {
	case SIO_ERDY_SND:
		SCI_disable_send(pcb);
		break;
	case SIO_ERDY_RCV:
		SCI_disable_recv(pcb);
		break;
	}
}

/*
 *  SCI 割り込み
 */

void
sio_in_handler (void)
{
	SCI_in_handler(1);
}

void
sio_out_handler (void)
{
	SCI_out_handler(1);
}

#ifdef H8_CFG_SCI_ERR_HANDLER

void
sio_err_handler (void)
{
	SCI_err_handler(1);
}

#endif	/* of #ifdef H8_CFG_SCI_ERR_HANDLER */

#if TNUM_PORT >= 2

void
sio_in2_handler (void)
{
	SCI_in_handler(2);
}

void
sio_out2_handler (void)
{
	SCI_out_handler(2);
}

#ifdef H8_CFG_SCI_ERR_HANDLER

void
sio_err2_handler (void)
{
	SCI_err_handler(2);
}

#endif	/* of #ifdef H8_CFG_SCI_ERR_HANDLER */
#endif	/* of #if TNUM_PORT >= 2 */

#if TNUM_PORT >= 3

void
sio_in3_handler (void)
{
	SCI_in_handler(3);
}

void
sio_out3_handler (void)
{
	SCI_out_handler(3);
}

#ifdef H8_CFG_SCI_ERR_HANDLER

void
sio_err3_handler (void)
{
	SCI_err_handler(3);
}

#endif	/* of #ifdef H8_CFG_SCI_ERR_HANDLER */

#endif	/* of #if TNUM_PORT >= 3 */

