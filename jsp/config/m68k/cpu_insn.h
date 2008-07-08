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
 *  @(#) $Id: cpu_insn.h,v 1.9 2003/06/04 01:51:39 hiro Exp $
 */

/*
 *	プロセッサの特殊命令のインライン関数定義（M68040用）
 */

#ifndef	_CPU_INSN_H_
#define	_CPU_INSN_H_

/*
 *  ステータスレジスタ（SR）の現在値の読出し
 */
Inline UH
current_sr()
{
	UH	sr;

	Asm("move.w %%sr, %0" : "=g"(sr));
	return(sr);
}

/*
 *  ステータスレジスタ（SR）の現在値の変更
 */
Inline void
set_sr(UH sr)
{
	Asm("move.w %0, %%sr" : : "g"(sr));
}

/*
 *  NMIを除くすべての割込みを禁止
 */
Inline void
disint()
{
	Asm("or.w #0x0700, %sr");
}

/*
 *  すべての割込みを許可
 */
Inline void
enaint()
{
	Asm("and.w #~0x0700, %sr");
}

/*
 *  ベクタベースレジスタ（VBR）の現在値の読出し
 */
Inline VP
current_vbr()
{
	VP	vbr;

	Asm("movec.l %%vbr, %0" : "=r"(vbr));
	return(vbr);
}

/*
 *  ベクタベースレジスタ（VBR）の設定
 */
Inline void
set_vbr(VP vbr)
{
	Asm("movec.l %0, %%vbr" : : "r"(vbr));
}

/*
 *  レディキューサーチのためのビットマップサーチ関数
 *
 *  ビットマップの下位16ビットを使用し，最下位ビットを最低優先度に対応
 *  させる．
 */
#define	CPU_BITMAP_SEARCH
#define	PRIMAP_BIT(pri)		(0x8000u >> (pri))

Inline UINT
bitmap_search(UINT bitmap)
{
	UINT	offset;

	/*
	 *  このコードは，bitmap（UINT型）が32ビットであることを仮定し
	 *  ている．
	 */
	Asm("bfffo %1{16,16}, %0" : "=d"(offset) : "g"(bitmap));
	return(offset - 16);
}

#endif /* _CPU_INSN_H_ */
