/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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
 *  @(#) $Id: sys_config.c,v 1.5 2005/07/06 00:45:07 honda Exp $
 */

#include "jsp_kernel.h"
#include <sil.h>
#include <s_services.h>
#include "sh7145sci.h"

/*
 *  ターゲットシステム依存の初期化
 */
/*
 *  シリアルI/Oポートの初期化 banner出力のためカーネルの初期化と無関係に行う
 */
void
sys_initialize ()
{
	SIOPCB *siopcb;

	/* 使用する周辺機器はここで有効にしておく。 */
	/*  SCIデータ入出力ポートの設定  */

#ifndef GDB_STUB

	/* SCI1 */
	sil_wrh_mem (MSTCR1, (VH) ((VH) sil_reh_mem (MSTCR1) & ~0x0002));
	sil_wrh_mem (PACRL2, sil_reh_mem (PACRL2) | (PFC_TXD1 | PFC_RXD1));
#if TNUM_PORT >=2
	/* SCI0 */
	sil_wrh_mem (MSTCR1, (VH) ((VH) sil_reh_mem (MSTCR1) & ~0x0001));
	sil_wrh_mem (PACRL2, sil_reh_mem (PACRL2) | (PFC_TXD0 | PFC_RXD0));
#endif /*TNUM_PORT >=2 */

#else /* GDB_STUB */

	/* SCI0 */
	sil_wrh_mem (MSTCR1, (VH) ((VH) sil_reh_mem (MSTCR1) & ~0x0001));
	sil_wrh_mem (PACRL2, sil_reh_mem (PACRL2) | (PFC_TXD0 | PFC_RXD0));

#endif /* GDB_STUB */

	/* CMT0 */
	sil_wrh_mem (MSTCR2, (VH) ((VH) sil_reh_mem (MSTCR2) & ~0x1000));
	/*
	 *  デバイス依存のオープン処理．
	 */
	/*バナー出力するため */
	sh2sci_initialize();
	siopcb = sh2sci_opn_por (LOGTASK_PORTID, 0);
}

/*
 *  ターゲットシステムの終了
 */
void
sys_exit ()
{
	sh2_exit ();
}

/*
 *  ターゲットシステムの文字出力
 */
void
sys_putc (char c)
{
	if (c == '\n') {
		sh2_putc (LOGTASK_PORTID, '\r');
	}
	sh2_putc (LOGTASK_PORTID, c);
}

/* ハードウェアの設定 */
/* ここではバスの設定のみ行う。周辺機器の設定はsys_initializeで行う。 */
#ifndef GDB_STUB
void
hardware_init_hook (void)
{
	sil_wrh_mem (BCR1, 0x202f);
	sil_wrh_mem (BCR2, 0x5500);
	sil_wrh_mem (WCR1, 0x0010);
	sil_wrh_mem (PACRH, 0x5000);
	sil_wrh_mem (PACRL1, 0x1540);
	sil_wrh_mem (PACRL2, 0x0140);
	sil_wrh_mem (PBCR1, 0x0002);
	sil_wrh_mem (PBCR2, 0xa005);
	sil_wrh_mem (PCCR, 0xfffc);
	sil_wrh_mem (PDCRH1, 0x5555);
	sil_wrh_mem (PDCRH2, 0x5555);
	sil_wrh_mem (PDCRL1, 0xffff);
	sil_wrh_mem (PDCRL2, 0x0000);
	sil_wrh_mem (PECRL1, 0x0000);
	sil_wrh_mem (PECRL2, 0x0000);
	sil_wrh_mem (PAIORL, 0x87e7);
	sil_wrh_mem (PBIOR, 0x023c);
}
#else /*  GDB_STUB  */
void
hardware_init_hook (void)
{
}
#endif /*  GDB_STUB  */
