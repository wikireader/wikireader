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
 *  @(#) $Id: st16c2550.h,v 1.1 2003/12/11 06:47:39 honda Exp $
 */

/*
 *   ST16C2550 用 簡易SIOドライバ
 */
#ifndef _ST16C2550_H_
#define _ST16C2550_H_

#include <t_config.h>

/*
 *  シリアルI/Oポート初期化ブロック
 */
typedef struct sio_port_initialization_block {
    UW reg_base;    /* レジスタのベースアドレス */
    UB lcr_val;     /* モードレジスタの設定値   */
    UB dlm_val;     /* ボーレート上位の設定値   */
    UB dll_val;     /* ボーレート下位の設定値   */
    UW pinter_val;  /* 割込み許可ビット   */    
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロック
 */
typedef struct sio_port_control_block {
    const SIOPINIB  *siopinib; /* シリアルI/Oポート初期化ブロック */
    VP_INT          exinf;     /* 拡張情報 */
    BOOL    openflag;          /* オープン済みフラグ */
    BOOL    sendflag;          /* 送信割込みイネーブルフラグ */
    BOOL    getready;          /* 文字を受信した状態 */
    BOOL    putready;          /* 文字を送信できる状態 */
}SIOPCB;


/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND    1u      /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u      /* 受信通知コールバック */


/*
 * カーネル起動時用の初期化(sys_putcを使用するため)
 */
extern void st16c2550_init(void);

/*
 *  シリアルI/Oポートへのポーリングでの出力
 */
extern void st16c2550_pol_putc(char c, ID siopid);


/*
 *  SIOドライバの初期化ルーチン
 */
extern void st16c2550_initialize(void);


/*
 *  オープンしているポートがあるか？
 */
extern BOOL st16c2550_openflag(void);


/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB   *st16c2550_opn_por(ID siopid, VP_INT exinf);


/*
 *  シリアルI/Oポートのクローズ
 */
extern void st16c2550_cls_por(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL st16c2550_snd_chr(SIOPCB *siopcb, char c);


/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT  st16c2550_rcv_chr(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void st16c2550_ena_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void st16c2550_dis_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  SIOの割込みサービスルーチン
 */
extern void st16c2550_isr(void);


/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void st16c2550_ierdy_snd(VP_INT exinf);


/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void st16c2550_ierdy_rcv(VP_INT exinf);


#endif /* _FDC37C935A_H_ */
