/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
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
 */

#ifndef	_CPU_INSN_H_
#define	_CPU_INSN_H_

/*
 *  制御レジスタの操作関数
 */

/*
 *  ステータスレジスタ(PSR)の現在値の読出し
 */
Inline UW
get_psr(void)
{
	UW psr;

	Asm("ld.w %0, %%psr": "=r"(psr));

	return psr;
}

/*
 *  ステータスレジスタ(PSR)の現在値の変更
 */
Inline void
set_psr(register UW psr)
{
	Asm("ld.w %%psr, %0": : "r"(psr));
}

/*
 *  スタックポインタ(SP)の現在値の読出し
 */
Inline VP
get_sp(void)
{
	VP sp;

	Asm("ld.w %0, %%sp": "=r"(sp));

	return sp;
}

/*
 *  スタックポインタ(SP)の現在値の変更
 */
Inline void
set_sp(VP sp)
{
	Asm("ld.w %%sp, %0": : "r"(sp));
}

/*
 *  プログラムカウンタ(PC)の現在値の変更
 */
Inline void
set_pc(VP pc)
{
	Asm("jp %0": "=r"(pc) : "0"(pc));
}

/*
 *  トラップベースレジスタ(TTBR)の現在値の読出し
 */
Inline VP
get_ttbr(void)
{
#ifdef __c33std
	return (VP) ((volatile s1c33Bcu_t *) S1C33_BCU_BASE)->ulTtbr;
#else
	VP ttbr;

	Asm("ld.w %0, %%ttbr": "=r"(ttbr));

	return ttbr;
#endif /* __c33std */
}

/*
 *  レディキューサーチのためのビットマップサーチ関数
 *  ビットマップの下位16ビットを使用し，最下位ビットを最低優先度に対応させる
 */
#ifdef CPU_BITMAP_SEARCH
Inline UINT
bitmap_search(UINT bitmap)
{
	INT offset;
	INT bit;

	Asm("swap %0, %1": "=r"(bitmap): "r"(bitmap));
	Asm("mirror %0, %1": "=r"(bitmap): "r"(bitmap));
	Asm("scan1 %0, %1": "=r"(bit): "r"(bitmap));
	if(bit != 8){
		return bit;
	}

	Asm("sll %0, %1": "=r"(bitmap): "r"(bit));
	offset = bit;
	Asm("scan1 %0, %1": "=r"(bit): "r"(bitmap));

	return offset + bit;
}
#endif	/* CPU_BITMAP_SEARCH */

#endif /* _CPU_INSN_H_ */
