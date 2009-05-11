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
 *  @(#) $Id: hw_serial.h,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

/*
 *  ターゲット依存シリアルI/Oモジュール（H8用）
 *    ・ポートが 1本の場合は HWPORT1_ADDRに指定されたポートを使用し、
 *      2本の場合は、HWPORT1_ADDRをユーザ用、HWPORT2_ADDRをコンソール
 *      出力として使用する。
 *      HWPORTx_ADDRは ターゲット依存の sys_config.h中で指定する。
 *    ・共通部のモジュールによって、XON/XOFFフロー制御が行わ
 *      れているので、信号線はTxDとRxDの2本でよい
 */

#include <s_services.h>
#include <h8_sil.h>

#ifndef _MACRO_ONLY

/*
 *  シリアルポートの初期化ブロック
 */
typedef struct sio_port_initialization_block {
        UB      *base;          /* SCI のベースアドレス */
        UW      baudrate;       /* ボーレイト           */
        IRC     irc;            /* 割込みレベル設定情報 */
        UB      smr_init;       /* SMR の設定値         */
} SIOPINIB;

/*
 *  シリアルポートの制御ブロック
 */

typedef struct sio_port_control_block {
        const SIOPINIB  *inib;          /* 初期化ブロック       */
        VP_INT          exinf;          /* 拡張情報             */
        BOOL            openflag;       /* オープン済みフラグ   */
} SIOPCB;

extern SIOPCB siopcb_table[TNUM_PORT];

/*
 *  SIO ID から管理ブロックへの変換
 */
#define INDEX_SIO(sioid)        ((UINT)(sioid) - 1u)

Inline SIOPCB*
get_siopcb(ID sioid)
{
	UINT index = INDEX_SIO(sioid);
	
	assert(index < TNUM_PORT);
	
    return(&(siopcb_table[index]));
}

/*
 *  コールバックルーチンの識別番号
 */
#define SIO_ERDY_SND            1u              /* 送信可能コールバック                 */
#define SIO_ERDY_RCV            2u              /* 受信通知コールバック                 */

/*
 *  SCI 関数の参照
 */
extern void     SCI_initialize (ID sioid);      /* SCI の初期化                         */
extern void     SCI_cls_por(UB *base);          /* SCI のクローズ                       */
extern void     SCI_in_handler(ID sioid);       /* SCI 入力割り込みハンドラ             */
extern void     SCI_out_handler(ID sioid);      /* SCI 出力割り込みハンドラ             */
extern void     SCI_err_handler(ID sioid);      /* SCI 入力エラー割り込みハンドラ       */
extern void     SCI_ierdy_snd(VP_INT exinf);    /* シリアル I/O からの送信可能コールバック */
extern void     SCI_ierdy_rcv(VP_INT exinf);    /* シリアル I/O からの受信通知コールバック */

/*
 *  SCI レベルの関数
 */

/*
 *  SCI のオープン
 */
Inline SIOPCB *
SCI_opn_por (ID sioid)
{
        SCI_initialize(sioid);
        return get_siopcb(sioid);
}

/*
 *  SCI_putchar -- 送信する文字の書き込み
 */
Inline void
SCI_putchar(const SIOPCB *p, UB c)
{
        UB *base_addr = p->inib->base;

        sil_wrb_mem((VP)(base_addr + H8TDR), (VB)c);
        bitclr(base_addr + H8SSR, (UB)H8SSR_TDRE_BIT);
}


/*
 *  低レベル出力に用いるシリアルポートを
 *  制御するレジスタ群の先頭アドレス
 */
#if SCI_LOW_PORTID == SCI_PORTID1
#define SCI_LOW_BASE			SCI_PORT1_BASE
#else /* SCI_LOW_PORTID == SCI_PORTID1 */
#define SCI_LOW_BASE			SCI_PORT2_BASE
#endif

/*
 *  SCI_putchar_pol -- ポーリング方式による低レベル出力
 */
Inline void
SCI_putchar_pol(UB c)
{
        UB *base = (UB*)SCI_LOW_BASE;
        UB *addr = base + H8SSR;
        UB tdre = 0;

        /* TDREがセットされるまで待つ */
        while (tdre == 0) {
			/*  コンパイラの警告を抑制するため、キャストしている  */
			tdre = (UB)((UB)sil_reb_mem((VP)addr) & H8SSR_TDRE);
		}
        sil_wrb_mem((VP)(base + H8TDR), (VB)c);
        bitclr(addr, (UB)H8SSR_TDRE_BIT);
}

/*
 *  SCI_getchar -- 受信した文字の読み出し
 */
Inline INT
SCI_getchar(const SIOPCB *p)
{
        UB      *base_addr = p->inib->base;
        INT     ch = (UB)sil_reb_mem((VP)(base_addr + H8RDR));
                /* UB のキャストは、符号拡張を防止するため */

        bitclr(base_addr + H8SSR, (UB)H8SSR_RDRF_BIT);
        return ch;
}

/*
 *  SCI_putready -- 送信可能か
 */
Inline BOOL
SCI_putready(const SIOPCB *pcb)
{
        BOOL ret = TRUE;
        UB ssr = sil_reb_mem((VP)(pcb->inib->base + H8SSR));
        
        ssr &= (UB)H8SSR_TDRE;
        if (ssr == 0) {
			ret = FALSE;
		}
        return(ret);
}

/*
 *  SCI_getready -- 受信可能か
 */
Inline BOOL
SCI_getready(const SIOPCB *pcb)
{
        BOOL ret = TRUE;
        UB ssr = sil_reb_mem((VP)(pcb->inib->base + H8SSR));

        ssr &= (UB)H8SSR_RDRF;
        if (ssr == 0) {
			ret = FALSE;
		}
        return(ret);
}

/*
 *  送信割り込み制御関数
 */
Inline void
SCI_enable_send(const SIOPCB *p)
{
        UB *addr = p->inib->base + H8SCR;
        bitset(addr, (UB)H8SCR_TIE_BIT);
}

Inline void
SCI_disable_send(const SIOPCB *p)
{
        UB *addr = p->inib->base + H8SCR;
        bitclr(addr, (UB)H8SCR_TIE_BIT);
}

/*
 *  受信割り込み制御関数
 */
Inline void
SCI_enable_recv(const SIOPCB *p)
{
        UB *addr = p->inib->base + H8SCR;
        bitset(addr, (UB)H8SCR_RIE_BIT);
}

Inline void
SCI_disable_recv(const SIOPCB *p)
{
        UB *addr = p->inib->base + H8SCR;
        bitclr(addr, (UB)H8SCR_RIE_BIT);
}

/*
 *  SIO 関数の参照
 */
                                /* SCI の初期化                         */
extern void     sio_initialize(void);
                                /* シリアル I/O からのコールバック許可  */
extern void     sio_ena_cbr(SIOPCB *pcb, UINT cbrtn);
                                /* シリアル I/O からのコールバック禁止  */
extern void     sio_dis_cbr(SIOPCB *pcb, UINT cbrtn);

/*
 *  関数シミュレーションマクロ
 */
                        /* シリアル I/O からの送信可能コールバック      */
#define sio_ierdy_snd(e)        SCI_ierdy_snd(e)
                        /* シリアル I/O からの受信通知コールバック      */
#define sio_ierdy_rcv(e)        SCI_ierdy_rcv(e)

/*
 *  SIO レベルの関数
 */

extern void sio_in_handler (void);
extern void sio_out_handler (void);

#ifdef H8_CFG_SCI_ERR_HANDLER
extern void sio_err_handler (void);
#endif  /* of #ifdef H8_CFG_SCI_ERR_HANDLER */

#if TNUM_PORT >= 2u
extern void sio_in2_handler (void);
extern void sio_out2_handler (void);

#ifdef H8_CFG_SCI_ERR_HANDLER
extern void sio_err2_handler (void);
#endif  /* of #ifdef H8_CFG_SCI_ERR_HANDLER */

#endif  /* of #if TNUM_PORT >= 2u */

/*
 *  sio_opn_por -- ポートのオープン
 */
Inline SIOPCB *
sio_opn_por(ID sioid, VP_INT exinf)
{
        SIOPCB  *pcb = SCI_opn_por(sioid);

        pcb->exinf    = exinf;
        pcb->openflag = TRUE;
        return pcb;
}

/*
 *  sio_cls_por -- ポートのクローズ
 */
Inline void
sio_cls_por(SIOPCB *pcb)
{
        SCI_cls_por(pcb->inib->base);
        pcb->openflag = FALSE;
}

/*
 *  sio_snd_chr -- 文字送信
 */
Inline BOOL
sio_snd_chr(const SIOPCB *pcb, char chr)
{
        BOOL ret = FALSE;
        if (SCI_putready(pcb) == TRUE) {
                SCI_putchar(pcb, (UB)chr);
                ret = TRUE;
        }
        return ret;
}

/*
 *  sio_rcv_chr -- 文字受信
 */
Inline INT
sio_rcv_chr(const SIOPCB *pcb)
{
        INT ret = 0;
        if (SCI_getready(pcb) == TRUE) {
                ret = SCI_getchar(pcb);
        } else {
                ret = -1;
        }
        return(ret);
}

#endif  /* of #ifndef _MACRO_ONLY */

#endif /* _HW_SERIAL_H_ */
