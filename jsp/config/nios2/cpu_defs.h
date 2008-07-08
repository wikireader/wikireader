/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2004 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN  
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
 *  @(#) $Id: cpu_defs.h,v 1.3 2007/01/15 06:05:22 honda Exp $
 */

/*
 *  プロセッサに依存する定義（Nios2用）
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

#define OMIT_SIL_ACCESS

/*
 *  エンディアンの反転
 *  sil.hでも定義しているが，OMIT_SIL_ACCESSによりこのファイルで
 *  アクセス関数を定義してるため，ここで定義する 
 */
#ifndef SIL_REV_ENDIAN_H
#define	SIL_REV_ENDIAN_H(data) \
	((VH)((((UH)(data) & 0xff) << 8) | (((UH)(data) >> 8) & 0xff)))
#endif /* SIL_REV_ENDIAN_H */

#ifndef SIL_REV_ENDIAN_W
#define	SIL_REV_ENDIAN_W(data) \
	((VW)((((UW)(data) & 0xff) << 24) | (((UW)(data) & 0xff00) << 8) \
		| (((UW)(data)>> 8) & 0xff00) | (((UW)(data) >> 24) & 0xff)))
#endif /* SIL_REV_ENDIAN_H */


#ifndef NIOS2
#define NIOS2   /* プロセッサ略称 */
#endif 
/*
 *  プロセッサに依存する定義
 */
#ifndef _MACRO_ONLY

typedef	UINT		INHNO;		/* 割込みハンドラ番号 */
typedef	UINT		IPM;		/* 割込みマスク */
typedef	UINT		EXCNO;		/* CPU例外ハンドラ番号 */


/*
 *  INTNO型と割込みの禁止/許可
 */

typedef	UINT		INTNO;		/* 割込み番号 */

extern ER	dis_int(INTNO intno) throw(); 
extern ER	ena_int(INTNO intno) throw();


/*
 *  ターゲットシステム依存のサービスコール
 */
typedef	UD  SYSUTIM;	/* 性能評価用システム時刻 */
extern  ER  vxget_tim(SYSUTIM *pk_sysutim) throw();


/*
 *  割込み許可ビット
 */
#define STATUS_PIE 0x01

/*
 *  すべての割込みを禁止
 */
Inline void
_disint_()
{
    UW status;
    
    Asm("rdctl %0, status" : "=r" (status));
    status &= ~STATUS_PIE;
    Asm("wrctl status, %0" :: "r" (status));
}


/*
 *  割込みを許可
 */
Inline void
_enaint_()
{
    UW status;

    Asm("rdctl %0, status" : "=r" (status));
    status |= STATUS_PIE;
    Asm("wrctl status, %0" :: "r" (status));
}


/*
 *  割込みロック状態の制御
 */
#define SIL_PRE_LOC 
#define SIL_LOC_INT()   _disint_()
#define SIL_UNL_INT()   _enaint_()

/*
 * システムの中断処理
 */
Inline void
kernel_abort()
{

}


#endif /* _MACRO_ONLY */

/*
 *  プロセッサのエンディアン
 */
#define SIL_ENDIAN  SIL_ENDIAN_LITTLE 

#ifndef _MACRO_ONLY

/*
 * メモリアクセス関数
 */

/*
 *  8ビット単位の読出し／書込み
 */
Inline VB
sil_reb_mem(VP mem)
{
  return(__builtin_ldbuio(mem));
}

Inline void
sil_wrb_mem(VP mem, VB data)
{
    __builtin_stbio(mem, data);
}


/*
 *  16ビット単位の読出し／書込み
 */
Inline VH
sil_reh_mem(VP mem)
{
	return(__builtin_ldhuio(mem));
}

Inline void
sil_wrh_mem(VP mem, VH data)
{
    __builtin_sthio(mem, data);
}


#define	sil_reh_lem(mem)	sil_reh_mem(mem)
#define	sil_wrh_lem(mem, data)	sil_wrh_mem(mem, data)

Inline VH
sil_reh_bem(VP mem)
{
	VH	data;

	data = __builtin_ldhuio(mem);
	return(SIL_REV_ENDIAN_H(data));
}

Inline void
sil_wrh_bem(VP mem, VH data)
{
    __builtin_sthio(mem, SIL_REV_ENDIAN_H(data));
}


/*
 *  32ビット単位の読出し／書込み
 */
Inline VW
sil_rew_mem(VP mem)
{
    return(__builtin_ldwio(mem));
}

Inline void
sil_wrw_mem(VP mem, VW data)
{
    __builtin_stwio(mem, data);
}


#define	sil_rew_lem(mem)	sil_rew_mem(mem)
#define	sil_wrw_lem(mem, data)	sil_wrw_mem(mem, data)

Inline VW
sil_rew_bem(VP mem)
{
	VW	data;

	data = __builtin_ldwio(mem);
	return(SIL_REV_ENDIAN_W(data));
}

Inline void
sil_wrw_bem(VP mem, VW data)
{
    __builtin_stwio(mem, SIL_REV_ENDIAN_W(data));
}

/*
 *  微少時間待ち
 */
Inline void
sil_dly_nse(UINT dlytim)
{
    
}

#endif /* _MACRO_ONLY */


#endif /* _CPU_DEFS_H_ */
