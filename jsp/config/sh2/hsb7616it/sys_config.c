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
#include "sh7615scif.h"
/*
 *  ターゲットシステム依存の初期化
 */
/* 入出力ポートの設定はsys_config.c */
/* 割込みベクタ番号の設定はhw_serial.h */
/* 管理ブロックの設定はsh7615scif.c */
void
sys_initialize ()
{
	SIOPCB *siopcb;

	/* 使用する周辺機器はここで有効にしておく。 */
	/*  SCIFデータ入出力ポートの設定  */
#ifndef GDB_STUB

	/* SCIF1 */
	sil_wrh_mem (PBCR, sil_reh_mem (PBCR) | (0x0800 | 0x2000));
#if TNUM_PORT >= 2
	/* SCIF2 */
	sil_wrh_mem (PBCR2, sil_reh_mem (PBCR2) | (0x0200 | 0x0800));
#endif /* TNUM_PORT >= 2 */

#else	/*  GDB_STUB  */

	/* SCIF2 */
	sil_wrh_mem (PBCR2, sil_reh_mem (PBCR2) | (0x0200 | 0x0800));

#endif	/*  GDB_STUB  */
	/*
	 *  デバイス依存のオープン処理．
	 */
	/*バナー出力するため */
	sh2scif_initialize();
	siopcb = sh2scif_opn_por (LOGTASK_PORTID, 0);

#ifndef GDB_STUB
	/* ベクタ番号の初期化 */

	sil_wrw_mem (DMA_VCRDMA0, 72);
	sil_wrw_mem (DMA_VCRDMA1, 73);

	sil_wrh_mem (VCRWDT, 74 * 0x100 + 75);
	sil_wrh_mem (VCRA, 76 * 0x100);
	sil_wrh_mem (VCRC, 77 * 0x100 + 78);
	sil_wrh_mem (VCRD, 79 * 0x100);
	sil_wrh_mem (VCRE, 80 * 0x100 + 81);
	sil_wrh_mem (VCRF, 82 * 0x100 + 83);
	sil_wrh_mem (VCRG, 84 * 0x100);
	sil_wrh_mem (VCRH, 85 * 0x100 + 86);
	sil_wrh_mem (VCRI, 87 * 0x100 + 88);
	sil_wrh_mem (VCRJ, 89 * 0x100 + 90);
	sil_wrh_mem (VCRK, 91 * 0x100 + 92);
	sil_wrh_mem (VCRL, 93 * 0x100 + 94);
	sil_wrh_mem (VCRM, 95 * 0x100 + 96);
	sil_wrh_mem (VCRN, 97 * 0x100 + 98);
	sil_wrh_mem (VCRO, 99 * 0x100 + 100);
	sil_wrh_mem (VCRP, 101 * 0x100 + 102);
	sil_wrh_mem (VCRQ, 103 * 0x100 + 104);
	sil_wrh_mem (VCRR, 105 * 0x100 + 106);
	sil_wrh_mem (VCRS, 107 * 0x100 + 108);
	sil_wrh_mem (VCRT, 109 * 0x100 + 110);
	sil_wrh_mem (VCRU, 111 * 0x100 + 112);
#endif	/*  GDB_STUB  */

}

/*
 *  ターゲットシステムの終了
 */
void
sys_exit()
{
	sh2_exit();
}

/*
 *  ターゲットシステムの文字出力
 */
void
sys_putc(char c)
{
	if (c == '\n') {
		sh2_putc (LOGTASK_PORTID, '\r');
	}
	sh2_putc (LOGTASK_PORTID, c);
}

/* ハードウェアの設定 */
void
hardware_init_hook (void)
{
}
