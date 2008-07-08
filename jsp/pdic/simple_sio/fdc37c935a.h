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
 *  @(#) $Id: fdc37c935a.h,v 1.1 2003/12/11 06:47:39 honda Exp $
 */

/*
 * スーパI/O FDC37C935A 用ドライバ
 */
#ifndef _FDC37C935A_H_
#define _FDC37C935A_H_

#include <t_config.h>

/*
 * ベースアドレス 
 */
//#define SMSC_BASE_ADDR 0xB0400000

#define SMSC_CONFIG_PORT (SMSC_BASE_ADDR + 0x7E0)  /* H */
#define SMSC_INDEX_PORT  (SMSC_BASE_ADDR + 0x7E0)  /* H */
#define SMSC_DATA_PORT   (SMSC_BASE_ADDR + 0x7E2)  /* H */


#ifndef _MACRO_ONLY

Inline void
smsc_config_write(UH index, UH data)
{
    sil_wrh_mem((VP)SMSC_INDEX_PORT, (index << 8));
    sil_wrh_mem((VP)SMSC_DATA_PORT, (data << 8));
}

Inline UH
smsc_config_read(UH index)
{
    sil_wrh_mem((VP)SMSC_INDEX_PORT, (index << 8));
    return(sil_reh_mem((VP)SMSC_DATA_PORT) >> 8);
}

/*
 *  初期化
 */
extern void smsc_init(void);



#endif /*  _MACRO_ONLY */


/*
 * SCI0
 */
#define SMSC_SCI0_BASE_ADDR  0x03f8

#define SMSC_SCI0_RBR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x0)  /* H */
#define SMSC_SCI0_THR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x0)  /* H */
#define SMSC_SCI0_DLL (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x0)  /* H */
#define SMSC_SCI0_IER (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x2)  /* H */
#define SMSC_SCI0_DLM (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x2)  /* H */
#define SMSC_SCI0_IIR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x4)  /* H */
#define SMSC_SCI0_FCR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x4)  /* H */
#define SMSC_SCI0_LCR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x6)  /* H */
#define SMSC_SCI0_MCR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0x8)  /* H */
#define SMSC_SCI0_LSR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0xA)  /* H */
#define SMSC_SCI0_MSR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0xC)  /* H */
#define SMSC_SCI0_SCR (SMSC_BASE_ADDR + SMSC_SCI0_BASE_ADDR * 2 + 0xE)  /* H */

/*
 *  スーパーI/O(FDC37C935A)用 簡易SIOドライバ
 */

#ifndef _MACRO_ONLY

/*
 *  割込み以外の初期化
 *  カーネル起動時用の初期化(sys_putcを使用するため)
 */
extern void fdc37c935a_init(void);


/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block   SIOPCB;

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND    1u      /* 送信可能コールバック */
#define SIO_ERDY_RCV    2u      /* 受信通知コールバック */


/*
 *  SIOドライバの初期化ルーチン
 */
extern void smsc_sci0_initialize(void);


/*
 *  オープンしているポートがあるか？
 */
extern BOOL smsc_sci0_openflag(void);


/*
 *  シリアルI/Oポートのオープン
 */
extern SIOPCB   *smsc_sci0_opn_por(ID siopid, VP_INT exinf);


/*
 *  シリアルI/Oポートのクローズ
 */
extern void smsc_sci0_cls_por(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートへの文字送信
 */
extern BOOL smsc_sci0_snd_chr(SIOPCB *siopcb, char c);


/*
 *  シリアルI/Oポートからの文字受信
 */
extern INT  smsc_sci0_rcv_chr(SIOPCB *siopcb);


/*
 *  シリアルI/Oポートからのコールバックの許可
 */
extern void smsc_sci0_ena_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
extern void smsc_sci0_dis_cbr(SIOPCB *siopcb, UINT cbrtn);


/*
 *  SIOの割込みサービスルーチン
 */
extern void smsc_sci0_isr(void);


/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
extern void smsc_sci0_ierdy_snd(VP_INT exinf);


/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
extern void smsc_sci0_ierdy_rcv(VP_INT exinf);




#endif /* _MACRO_ONLY */


#endif /* _FDC37C935A_H_ */
