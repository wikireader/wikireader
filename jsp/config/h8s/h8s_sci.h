/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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
 */

#ifndef _H8S_SCI_H_
#define _H8S_SCI_H_

#include <s_services.h>		/* デバイスドライバ用標準インクルードファイル */
#include <h8s_sil.h>

/*
 *  H8S 内蔵シリアルコミュニケーションインタフェース(SCI)関連の定義
 */

/* SCIレジスタのアドレス */
/* ベースアドレス */
#define SCI0_BASE_ADDR	0xff78	/* チャネル０ */
#define SCI1_BASE_ADDR	0xff80	/* チャネル１ */
/* レジスタオフセット */
#define SMR		0	/* シリアルモードレジスタタ */
#define BRR		1	/* ビットレートレジスタ */
#define SCR		2	/* シリアルコントロールレジスタ */
#define TDR		3	/* トランスミットデータレジスタ */
#define SSR		4	/* シリアルステータスレジスタ */
#define RDR		5	/* レシーブデータレジスタ */
#define SCMR		6	/* スマートカードモードレジスタ */

/* 各レジスタのビットパターン */

/* RSR, RDR, TSR, TDR */
/* 定義すべきものは、特に無い */

/* SMR */
#define CA	BIT7
#define	CHR	BIT6	/* 8bit = 0 / 7bit = 1 */
#define	PE	BIT5	/* Parity OFF = 0 / Parity ON = 1 */
#define	OE	BIT4	/* EVEN Parity = 0 / ODD Parity = 1 */
#define	STOP	BIT3	/* 1 STOP BIT = 0 / 2 STOP BIT = 1 */
#define MP	BIT2
#define	CSK1	BIT1
#define	CSK0	BIT0

/* SCR */
#define TIE	BIT7
#define	RIE	BIT6
#define	TE	BIT5
#define	RE	BIT4
#define	MPIE	BIT3
#define TEIE	BIT2
#define	CKE1	BIT1
#define	CKE0	BIT0

/* SSR */
#define TDRE	BIT7
#define	RDRF	BIT6
#define	ORER	BIT5
#define	FER	BIT4
#define	PER	BIT3
#define TEND	BIT2
#define	MPB	BIT1
#define	MPBT	BIT0

/*
 *  ボーレート関係の定義
 *    動作周波数：20 [MHz] なので、マニュアルp567より決まる。
 */
#if BAUD_RATE == 9600		/* ビットレート： 9600 [bit/s] */
	#define BRR_RATE	64	/* N = 64 */
#elif BAUD_RATE == 38400	/* ビットレート：38400 [bit/s] */
	#define BRR_RATE	15	/* N = 15 */
#elif BAUD_RATE == 57600	/* ビットレート：57600 [bit/s] */
	#define BRR_RATE	10	/* N = 10 */
#endif

#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_control_block	SIOPCB;

#endif /* _MACRO_ONLY */

/*
 *  SCI用システムインタフェースレイヤー
 */
/* x : ポートのベースアドレス(UW 型)
   y : レジスタオフセット(UW 型)
   z : レジスタ値 */
#define h8s_sci_wrb( x, y, z )	h8s_wrb_mem( (VP)(x + y), z )
#define h8s_sci_reb( x, y )	h8s_reb_mem( (VP)(x + y) )

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND	1u		/* 送信可能コールバック */
#define SIO_ERDY_RCV	2u		/* 受信通知コールバック */

#ifndef _MACRO_ONLY

/*
 *  SIOドライバの初期化ルーチン
 */
extern void	h8s_sci_initialize(void);

/*
 *  オープンしているポートがあるか？
 */
extern BOOL	h8s_sci_openflag(void);

/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB	*h8s_sci_opn_por(ID siopid, VP_INT exinf);

/*
 *  シリアルI/Oポートのクローズ
 */
extern void	h8s_sci_cls_por(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL	h8s_sci_snd_chr(SIOPCB *siopcb, char c);

/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT	h8s_sci_rcv_chr(SIOPCB *siopcb);

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void	h8s_sci_ena_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void	h8s_sci_dis_cbr(SIOPCB *siopcb, UINT cbrtn);

/*
 *  SIOの割込みサービスルーチン
 */
extern void	h8s_sci_isr_in(void);
extern void	h8s_sci_isr_out(void);

/*
 *  SIOの受信エラー割込みサービスルーチン
 */
extern void	h8s_sci_isr_error(void);

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void	h8s_sci_ierdy_snd(VP_INT exinf);

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void	h8s_sci_ierdy_rcv(VP_INT exinf);

/*
 *  カーネル起動時用の初期化 (sys_putcで利用)
 */
extern void	h8s_sci_init(void);

/*
 *  シリアルI/Oポートへの文字送信（ポーリング）
 */
extern void	h8s_sci_putchar_pol(char c);

#endif /* _MACRO_ONLY */

#endif /* _H8S_sci_H_ */
