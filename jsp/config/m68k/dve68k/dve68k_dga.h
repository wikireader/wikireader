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
 *  @(#) $Id: dve68k_dga.h,v 1.2 2003/06/04 01:52:23 hiro Exp $
 */

/*
 *	DGAのアクセスユーティリティ
 */

#ifndef _DVE68K_DGA_H_
#define _DVE68K_DGA_H_

/*
 *  DGAの割込み制御ビットの定義
 */
#define TBIT_ABT	0x40000000u	/* アボート割込みビット */
#define TBIT_SQR	0x04000000u	/* SQR 割込みビット */
#define	TBIT_TT0	0x00100000u	/* タイマ0 割込みビット */
#define	TBIT_GP0	0x00010000u	/* シリアルI/O 割込みビット */

/*
 *  DGAの割込みレベル設定のための定義
 */
#define	TIRQ_NMI	0x7u		/* ノンマスカブル割込み */
#define	TIRQ_LEVEL6	0x6u		/* 割込みレベル6 */
#define	TIRQ_LEVEL5	0x5u		/* 割込みレベル5 */
#define	TIRQ_LEVEL4	0x4u		/* 割込みレベル4 */
#define	TIRQ_LEVEL3	0x3u		/* 割込みレベル3 */
#define	TIRQ_LEVEL2	0x2u		/* 割込みレベル2 */
#define	TIRQ_LEVEL1	0x1u		/* 割込みレベル1 */

#define TBIT_ABTIL	24		/* アボート割込み */
#define TBIT_SQRIL	8		/* SRQ 割込み */

#define TBIT_TT0IL	16		/* タイマ0 割込み */
#define TBIT_GP0IL	0		/* シリアルI/O 割込み */

#ifndef _MACRO_ONLY

/*
 *  DGAのレジスタへのアクセス関数
 */

Inline UW
dga_read(VP addr)
{
	return((UW) dga_rew_reg(addr));
}

Inline void
dga_write(VP addr, UW val)
{
	dga_wrw_reg(addr, (VW) val);
}

Inline void
dga_bit_or(VP addr, UW bitpat)
{
	dga_write(addr, dga_read(addr) | bitpat);
}

Inline void
dga_bit_and(VP addr, UW bitpat)
{
	dga_write(addr, dga_read(addr) & bitpat);
}

Inline void
dga_set_ilv(VP addr, UINT shift, UINT level)
{
	dga_write(addr, (dga_read(addr) & ~(0x07 << shift))
						| (level << shift));
} 

#endif /* _MACRO_ONLY */
#endif /* _DVE68K_DGA_H_ */
