/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2003 by Industrial Technology Institute,
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
 * 
 *  @(#) $Id: smc.c,v 1.1 2004/09/03 16:39:56 honda Exp $
 */

/*
 *   シリアルドライバのサンプル
 *
 *　　　　smc.{c,h}とhw_serial.hの分割の基準
 *　　　　　　・smc.{c,h}：シリアルデバイスに依存する部分のみ記述
 *　　　　　　・hw_serial.h：割込みコントローラ依存
 */

#include <s_services.h>
#include <smc.h>

/*
 *  シリアルI/Oポート管理ブロックの定義
 *  　2chサポートに拡張する場合は初期値用のデータも含める
 */
struct sio_port_control_block {
	VP_INT		exinf;		/* 拡張情報 */
	BOOL		openflag;	/* オープン済みフラグ */
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 *  　　ID = 1 をSMC1に対応させている．
 */
static SIOPCB	siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	((UINT)((siopid) - 1))
#define get_siopcb(siopid)	(&(siopcb_table[INDEX_SIOP(siopid)]))


/*
 *  文字を受信できるか？
 */
Inline BOOL
smc_getready(SIOPCB *siopcb)
{
	return(TRUE);
}

/*
 *  文字を送信できるか？
 */
Inline BOOL
smc_putready(SIOPCB *siopcb)
{
	return(TRUE);
}

/*
 *  受信した文字の取出し
 */
Inline char
smc_getchar(SIOPCB *siopcb)
{
    	return((char)0);
}

/*
 *  送信する文字の書込み
 */
Inline void
smc_putchar(SIOPCB *siopcb, char c)
{
}

/*
 *  SIOドライバの初期化ルーチン
 */
void
smc_initialize()
{
}


/*
 *  オープンしているポートがあるか？
 */
BOOL
smc_openflag(void)
{
	return(TRUE);
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
smc_opn_por(ID siopid, VP_INT exinf)
{
	return((SIOPCB *)0);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
smc_cls_por(SIOPCB *siopcb)
{
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL
smc_snd_chr(SIOPCB *siopcb, char c)
{
	return(FALSE);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT
smc_rcv_chr(SIOPCB *siopcb)
{
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
smc_ena_cbr(SIOPCB *siopcb, UINT cbrtn)
{
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
smc_dis_cbr(SIOPCB *siopcb, UINT cbrtn)
{
}

/*
 *  シリアルI/Oポートに対する送信割込み処理
 */
Inline void
smc_isr_siop_out(SIOPCB *siopcb)
{
}

/*
 *  シリアルI/Oポートに対する受信割込み処理
 */
Inline void
smc_isr_siop_in(SIOPCB *siopcb)
{
}

/*
 *  SIO送信割込みサービスルーチン
 *  
 *  　送受信共通
 *  
 */
void
smc_isr()
{
}

/*
 * ポーリングによる文字の送信
 */
void
smc_putc_pol(char c)
{
}

/*  end of file  */
