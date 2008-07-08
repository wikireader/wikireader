/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 * 
 *  @(#) $Id: cpu_insn.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	プロセッサの特殊命令のインライン関数定義（PowerPC用）
 */

#ifndef	_CPU_INSN_H_
#define	_CPU_INSN_H_

/*
 *  制御レジスタの操作関数
 */

/*
 *  マシンステータスレジスタ（MSR）の現在値の読出し
 */
Inline UW
current_msr(void)
{
	UW msr;
	Asm("mfmsr %0" : "=r"(msr));
	return(msr);
}


/*
 *  マシンステータスレジスタ（MSR）の現在値の変更
 */
Inline void
set_msr(UW msr)
{
	Asm("mtmsr %0" : : "r"(msr) );
}


/*
 *  NMIを除くすべての割込みを禁止
 */
Inline void
disint(void)
{
#if !defined(IBM_PPC_EMB_ENV)
	/*  
	 *   オリジナルのPowerPCアーキテクチャの場合
	 *   　　モトローラMPCシリーズ、IPM PowerPC6xx/7xxシリーズは
	 *   　　こちらに該当する。
	 */   
	UW msr;
	
	msr = current_msr();
	msr |= MSR_EE;
	msr ^= MSR_EE;
	set_msr(msr);

#else	/*  IBM_PPC_EMB_ENV  */
	/*  
	 *   The IBM PowerPC Embedded Environmentの場合
	 *   　IBM系PowerPC40xシリーズ対応
	 *   　　wrteei命令で直接EEビットをクリアできる。
	 */   
	Asm("wrteei 0" : );	/*  MSR.EE←0  */

#endif	/*  IBM_PPC_EMB_ENV  */
}


/*
 *  割込みを許可
 */
Inline void
enaint()
{
#if !defined(IBM_PPC_EMB_ENV)
	/*  
	 *   オリジナルのPowerPCアーキテクチャの場合
	 *   　　モトローラMPCシリーズ、IPM PowerPC6xx/7xxシリーズは
	 *   　　こちらに該当する。
	 */   
	UW msr;
	
	msr = current_msr();
	msr |= MSR_EE;
	set_msr(msr);

#else	/*  IBM_PPC_EMB_ENV  */
	/*  
	 *   The IBM PowerPC Embedded Environmentの場合
	 *   　IBM系PowerPC40xシリーズ対応
	 *   　　wrteei命令で直接EEビットをセットできる。
	 */   
	Asm("wrteei 1" : );	/*  MSR.EE←1  */

#endif	/*  IBM_PPC_EMB_ENV  */
}


/*
 *  スペシャル・パーパス・スレジスタ（SPR）の現在値の読出し
 *     no :レジスタ番号
 *     dst:読み出した値を格納する変数
 */

#define _current_spr(no, dst)	Asm("mfspr %0," #no : "=r"(dst))
#define current_spr(no, dst)	_current_spr(no, dst)


/*
 *  スペシャル・パーパス・スレジスタ（SPR）の現在値の変更
 *     no :レジスタ番号
 *     src:設定する値
 */
#define set_spr(no, src)	_set_spr(no, src)
#define _set_spr(no, src)	Asm("mtspr "#no", %1" : : "r"(src) )

/*
 *  SPRG0の現在値の読出し
 */
Inline UW
current_sprg0()
{
    UW sprg0;
    
    current_spr(SPRG0, sprg0);	/*  sprg0 ← SPRG0  */
    return(sprg0);
}



/*
 *  レディキューサーチのためのビットマップサーチ関数
 *
 *　　機種非依存部で用意する関数を利用するので
 *　　CPU_BITMAP_SEARCHマクロは定義しない
 */


#endif /* _CPU_INSN_H_ */
/*  end of file  */
