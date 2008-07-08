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
 *  @(#) $Id: cpu_defs.h,v 1.10 2005/07/27 10:38:55 honda Exp $
 */

/*
 *  プロセッサに依存する定義（Microblaze用）
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

//#include "microblaze.h"

#ifndef MICROBLAZE
#define MICROBLAZE   /* プロセッサ略称 */
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
typedef	UW  SYSUTIM;	/* 性能評価用システム時刻 */
extern  ER   vxget_tim(SYSUTIM *pk_sysutim) throw();


/*
 *  MSRのビット
 */
#define MSR_IE 0x02
#define MSR_BE 0x01


/*
 *  すべての割込みを禁止
 */
Inline void
_disint_()
{
    UW msr;
  
    Asm("mfs %0,rmsr": "=r"(msr));
    msr &= ~MSR_IE;
    Asm("mts rmsr,%0" : : "r"(msr));
}


/*
 *  割込みを許可
 */
Inline void
_enaint_()
{
    UW msr;
    
    Asm("mfs %0,rmsr": "=r"(msr));
    msr |= MSR_IE;
    Asm("mts rmsr,%0" : : "r"(msr));    
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
    Asm("brki r16 0x20");
}

/*
 *  微少時間待ち
 */
Inline void
sil_dly_nse(UINT dlytim)
{
    Asm("add r5, r0, %0; \
         brlid r15, _sil_dly_nse; \
         nop\n"
        : 
        : "r"(dlytim)
        : "r15", "r5");
    
}


#endif /* _MACRO_ONLY */

/*
 *  プロセッサのエンディアン
 */
#define SIL_ENDIAN  SIL_ENDIAN_BIG  /* ビックエンディアン */


#endif /* _CPU_DEFS_H_ */
