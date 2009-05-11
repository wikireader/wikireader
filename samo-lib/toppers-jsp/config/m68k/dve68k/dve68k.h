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
 *  @(#) $Id: dve68k.h,v 1.8 2003/06/18 12:40:08 hiro Exp $
 */

/*
 *	DVE68K/40 CPUボードのハードウェア資源の定義
 */

#ifndef _DVE68K_H_
#define _DVE68K_H_

/*
 *  割込みベクトルの定義
 */
#define	TVEC_G0I	0x40u		/* グループ0 割込みベクトル */
#define TVEC_SQR	0x42u		/* SQR 割込みベクトル */
#define TVEC_ABT	0x46u		/* アボート割込みベクトル */

#define	TVEC_G1I	0x48u		/* グループ1 割込みベクトル */
#define	TVEC_GP0	0x48u		/* シリアルI/O 割込みベクトル */
#define	TVEC_TT0	0x4cu		/* タイマ0 割込みベクトル */

#define	TVEC_SWI	0X50u		/* ソフトウェア割込みベクトル */
#define	TVEC_SPRI	0x40u		/* スプリアス割込みベクトル */

/*
 *  CPUボード上のレジスタ
 */
#define TADR_BOARD_REG0		0xfff48000
#define TADR_BOARD_REG1		0xfff48004
#define TADR_BOARD_REG2		0xfff48008

/*
 *  DGA-001のレジスタのアドレス
 */
#define TADR_DGA_CSR0		0xfff44000
#define TADR_DGA_CSR1		0xfff44004
#define TADR_DGA_CSR3		0xfff4400c
#define TADR_DGA_CSR4		0xfff44010
#define TADR_DGA_CSR5		0xfff44014
#define TADR_DGA_CSR12		0xfff44030
#define TADR_DGA_CSR13		0xfff44034
#define	TADR_DGA_CSR19		0xfff4404c
#define	TADR_DGA_CSR20		0xfff44050
#define	TADR_DGA_CSR21		0xfff44054
#define TADR_DGA_CSR23		0xfff4405c
#define TADR_DGA_CSR24		0xfff44060
#define	TADR_DGA_CSR25		0xfff44064
#define	TADR_DGA_IFR0		0xfff44070
#define TADR_DGA_IFR3		0xfff4407c

/*
 *  DGAへのアクセス関数
 */
#define	dga_rew_reg(addr)		sil_rew_mem(addr)
#define dga_wrw_reg(addr, val)		sil_wrw_mem(addr, val)

/*
 *  μPD72001（MPSC）のレジスタのアドレス
 */
#define	TADR_UPD72001_DATAA	0xfff45003
#define	TADR_UPD72001_CTRLA	0xfff45007
#define	TADR_UPD72001_DATAB	0xfff4500b
#define	TADR_UPD72001_CTRLB	0xfff4500f

/*
 *  μPD72001へのアクセス関数
 */
#define	upd72001_reb_reg(addr)		sil_reb_mem(addr)
#define upd72001_wrb_reg(addr, val)	sil_wrb_mem(addr, val)

/*
 *  GDB STUB呼出しルーチン
 */
#ifndef _MACRO_ONLY
#ifdef GDB_STUB

Inline void
dve68k_exit()
{
	Asm("trap #2");
}

Inline void
dve68k_putc(char c)
{
	Asm("move.l %0, %%d1; trap #3"
	  : /* no output */
	  : "g"((INT) c)
	  : "d0", "d1", "d2", "d6", "d7");
}

/*
 *  モニタ呼出しルーチン
 */
#else /* GDB_STUB */

Inline void
dve68k_exit()
{
	Asm("clr.l %%d0; trap #3"
	  : /* no output */
	  : /* no input */
	  : "d0", "d1", "d2", "d6", "d7");
}

Inline char
dve68k_getc()
{
	INT	c;

	Asm("moveq.l #3, %%d0; trap #3; move.l %%d0, %0"
	  : "=g"(c)
	  : /* no input */
	  : "d0", "d1", "d2", "d6", "d7");
	  return((char) c);
}

Inline void
dve68k_putc(char c)
{
	Asm("moveq.l #5, %%d0; move.l %0, %%d1; trap #3"
	  : /* no output */
	  : "g"((INT) c)
	  : "d0", "d1", "d2", "d6", "d7");
}

#endif /* GDB_STUB */
#endif /* _MACRO_ONLY */
#endif /* _DVE68K_H_ */
