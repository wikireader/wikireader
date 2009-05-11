/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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
 *  @(#) $Id: sh7145sci.h,v 1.1 2005/07/06 00:45:07 honda Exp $
 */

/*
 *   SH2内蔵シリアルコミュニケーションインタフェースSCI用 簡易ドライバ
 */

#ifndef _sh7145sci_H_
#define _sh7145sci_H_

#include <t_config.h>

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_initialization_block
{
	UW reg_base;				/* レジスタのベースアドレス */
	UB brr;						/* ボーレートの設定値   */
	UB smr;						/* モードレジスタの設定値   */
	UB int_level;				/* 割り込みレベルの設定値   */
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロック
 */
typedef struct sio_port_control_block
{
	const SIOPINIB *siopinib;	/* シリアルI/Oポート初期化ブロック */
	VP_INT exinf;				/* 拡張情報 */
	BOOL openflag;				/* オープン済みフラグ */
	BOOL sendflag;				/* 送信割込みイネーブルフラグ */
	BOOL getready;				/* 文字を受信した状態 */
	BOOL putready;				/* 文字を送信できる状態 */
} SIOPCB;

/*
 *  コントロールレジスタのアドレスのオフセット 
 */
#define SCI_SCR	0x2
#define SCI_SSR	0x4
#define SCI_BRR	0x1
#define SCI_SMR	0x0
#define SCI_TDR	0x3
#define SCI_RDR	0x5
#define SCI_SDCR 0x6


/*
 *  コントロールレジスタの設定値
 */
#define SCI_TIE		0x80		/* トランスミットインタラプトイネーブル */
#define SCI_RIE		0x40		/* レシーブインタラプトイネーブル   */
#define SCI_TE		0x20		/* トランスミットイネーブル         */
#define SCI_RE		0x10		/* レシーブイネーブル           */
#define SSR_ORER	0x20		/* オーバーランエラー           */
#define SSR_FER		0x10		/* フレーミングエラー           */
#define SSR_PER		0x08		/* パリティエラー           */
#define SSR_TDRE	0x80		/* トランスミットデータレジスタエンプティ */
#define SSR_RDRF	0x40		/* レシーブデータレジスタフル       */

#define sh2sci_DELAY 	105000

#define PFC_TXD0	0x0004
#define PFC_RXD0	0x0001
#define PFC_TXD1	0x0100
#define PFC_RXD1	0x0040
#define PFC_TXD2	0x0020
#define PFC_RXD2	0x8000

/*
 *  SCIの設定
 */
#define SMR_CKS 	0x0			/*  分周比          */
#define BRR9600 	79			/*  9600 ビットレート    */
#define BRR19200	39			/*  19200 ビットレート    */
#define BRR38400	19			/*  38400 ビットレート    */

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND	1u		/* 送信可能コールバック */
#define SIO_ERDY_RCV	2u		/* 受信通知コールバック */

/*
 *  SIOドライバの初期化ルーチン
 */
extern void sh2sci_initialize (void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL sh2sci_openflag (ID siopid);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB *sh2sci_opn_por (ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void sh2sci_cls_por (SIOPCB * siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL sh2sci_snd_chr (SIOPCB * siopcb, char c);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT sh2sci_rcv_chr (SIOPCB * siopcb);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void sh2sci_ena_cbr (SIOPCB * siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void sh2sci_dis_cbr (SIOPCB * siopcb, UINT cbrtn);

/*
 *  SIOの割込みサービスルーチン
 */
extern void sh2sci_isr (void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void sh2sci_ierdy_snd (VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void sh2sci_ierdy_rcv (VP_INT exinf);

#endif /* _sh7145sci_H_ */
