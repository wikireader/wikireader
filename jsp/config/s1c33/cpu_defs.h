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

/*
 *  プロセッサに依存する定義(S1C33用)
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

#define S1C33

/*
 *  CPU のバイト順に関する定義
 */

#define SIL_ENDIAN		SIL_ENDIAN_LITTLE

#ifndef _MACRO_ONLY

typedef UINT	INHNO;		/* 割込みハンドラ番号	*/
typedef UINT	EXCNO;		/* CPU例外ハンドラ番号	*/
typedef UINT	ILEVNO;		/* 割込みレベル		*/

/*
 *  割込みレベル変更／取得処理
 */
extern	ER	chg_ilv(ILEVNO ilevno);
extern	ER	get_ilv(ILEVNO *p_ilevno);

/*
 *  割込み許可／禁止処理
 */
typedef UINT	INTNO;		/* 割込み番号		*/

/*
 *  依存関係によりここでリネーム
 */
#define ena_int		_kernel_ena_int
#define dis_int		_kernel_dis_int

#ifdef LABEL_ASM
#define _ena_int	__kernel_ena_int
#define _dis_int	__kernel_dis_int
#endif /* LABEL_ASM */

extern	ER	ena_int(INTNO intno);
extern	ER	dis_int(INTNO intno);

/*
 *  現在の割込み許可フラグの読出し
 */
Inline UW
_current_intflag_(void)
{
	UW	psr;

	Asm("ld.w %0, %%psr" : "=r"(psr));
	return(psr & 0x00000010);
}

/*
 *  NMIを除くすべての割込みを禁止
 */
Inline void
_disint_(void)
{
	UW	psr;

	Asm("ld.w %0, %%psr" : "=r"(psr));
	psr &= 0xffffffef;
	Asm("ld.w %%psr, %0" : : "r"(psr));
}

/*
 *  割込み許可フラグの復元
 */
Inline void
_enaint_(UW intmask)
{
	UW	psr;

	Asm("ld.w %0, %%psr" : "=r"(psr));
	psr |= intmask;
	Asm("ld.w %%psr, %0" : : "r"(psr));
}

/*
 *  割込みロック状態の制御
 */
#define SIL_PRE_LOC	UW _intmask_ = _current_intflag_()
#define SIL_LOC_INT()	_disint_()
#define SIL_UNL_INT()	_enaint_(_intmask_)

#define OMIT_VGET_TIM

typedef	UW  SYSUTIM;		/* 性能評価用システム時刻 */

extern ER	vxget_tim(SYSUTIM *p_sysutim) throw();

/*
 *  システムの中断処理
 */

Inline void
kernel_abort (void)
{
	while (1){
		;
	}
}

#endif /* _MACRO_ONLY */
#endif /* _CPU_DEFS_H_ */
