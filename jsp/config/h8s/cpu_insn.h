/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
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
 */

#ifndef	_CPU_INSN_H_
#define	_CPU_INSN_H_

/* 下記にて利用している、MAX_IPM は cpu_config.h で定義 */

#ifndef _MACRO_ONLY

/*
 *  制御レジスタの操作関数
 */

/* コンデションコードレジスタ（CCR）の現在値の読出し */
Inline UB current_ccr(void)
{
	UB	ccr;

	Asm( "stc.b	ccr, %0l" : "=r"(ccr) );

	return(ccr);
}

/* コンディションコードレジスタ（CCR）の現在値の変更 */
Inline void set_ccr(UB ccr)
{
	Asm( "ldc.b	%0l, ccr" ::"r"(ccr) );
}

/* エクステンドレジスタ（EXR）の現在値の読出し */
Inline UB current_exr(void)
{
	UB	exr;

	Asm( "stc.b	exr, %0l" : "=r"(exr) );

	return(exr);
}

/* コンデションコードレジスタ（EXR）の現在値の変更 */
Inline void set_exr(UB exr)
{
	Asm( "ldc.b	%0l, exr" :: "r"(exr) );
}

/*
 *  割込みマスク操作ライブラリ (割込みモード２用)
 */

/* 現在の割込みマスクの読出し */
Inline IPM current_intmask(void)
{
	return(  (IPM)  (current_exr() &  EXR_I_MASK) );
}

/* 割込みマスクの設定 */
Inline void set_intmask(IPM intmask)
{
	set_exr( (IPM) ((current_exr() & ~EXR_I_MASK) | intmask ) );
}

/* カーネル管理下のすべての割込み ＝ 割込みレベルがMAX_IPM 以下の割込み */

/* カーネル管理下のすべての割込みを禁止 (NMIを除く)  */
Inline void disint(void)
{
	/* set_intmask( (IPM) MAX_IPM ); */
	set_exr( (IPM) ((current_exr() & ~EXR_I_MASK) | MAX_IPM ) );
}

/* カーネル管理下のすべての割込みを許可 (NMIを除く) */
/*   この処理は、事実上、カーネル管理下の有無を問わず、すべての割込みを
     許可することになっている。 */
Inline void enaint()
{
	/* set_intmask( (IPM) 0 ); */
	set_exr( (IPM)  (current_exr() & ~EXR_I_MASK) );
}

/*
 *  割込みロック状態の制御用関数
 */

/* すべての割込みを禁止 (NMIを除く) */
Inline void _disint_()
{
	/* set_intmask( (IPM) 7 ); */
	set_exr( (IPM) (current_exr() | EXR_I_MASK) );
}

/*
 *  ビット操作用関数
 */

/* ビットクリア */
Inline void bitclr(UW addr, UB bit)
{
	Asm( "bclr %1l, @%0" :: "r"(addr), "r"(bit) );
}

/* ビットセット */
Inline void bitset(UW addr, UB bit)
{
	Asm( "bset %1l, @%0" :: "r"(addr), "r"(bit) );
}

#endif /* _MACRO_ONLY */

#endif /* _CPU_INSN_H_ */
