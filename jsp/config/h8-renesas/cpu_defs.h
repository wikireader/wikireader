/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 *  @(#) $Id: cpu_defs.h,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

/*
 *      プロセッサに依存する定義（H8用）
 *　　　　　アプリケーションが必要とする定義
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

/*
 *  ユーザー定義情報
 */
#include <user_config.h>        /*  SUPPORT_VXGET_TIMマクロの定義  */

#include <h8.h>

#define H8

/* カーネル起動時のメッセージ */
#define COPYRIGHT_CPU \
"Copyright (C) 2001-2007 by Industrial Technology Institute,\n" \
"                            Miyagi Prefectural Government, JAPAN\n" \
"Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering,\n" \
"                 Tomakomai National College of Technology, JAPAN\n"

/*
 *  CPU のバイト順に関する定義
 */

#define SIL_ENDIAN              SIL_ENDIAN_BIG

#ifndef _MACRO_ONLY

typedef UINT    INHNO;                  /* 割込みハンドラ番号 */
typedef UINT    EXCNO;                  /* CPU例外ハンドラ番号 */

/*
 *  ターゲットシステム依存のサービスコール
 */

/*
 *  割込みマスクの型と割込みマスクの変更／参照
 */
typedef UB              IPM;            /* 割込みマスク */

#endif /* _MACRO_ONLY */
/*
 *  レベル０　すべての割込みを受け付ける
 */
#define IPM_LEVEL0      0u

/*
 *  レベル１　NMIおよびプライオリティレベル１の割込みのみを受け付ける
 */
#define IPM_LEVEL1      CCR_I

/*
 *  レベル２　NMI以外の割込みを受け付けない
 */
#define IPM_LEVEL2      (CCR_I | CCR_UI)

#ifndef _MACRO_ONLY

#ifdef SUPPORT_CHG_IPM
extern ER       chg_ipm(IPM ipm) throw();
extern ER       get_ipm(IPM *p_ipm) throw();
#endif  /*  SUPPORT_CHG_IPM  */


/*
 *  プライオリティレベル設定用のデータ構造
 */
typedef struct {
        UB *ipr;        /* 設定するIPRレジスタの番地 */
        UB bit;         /* IPRレジスタの該当するビット番号 */
        IPM ipm;        /* 設定する割込みレベル */
                        /* IPM_LEVEL0,IPM_LEVEL1のいずれか */
} IRC;                  /* IRC:Interrupt Request Controller */


/*
 *  性能評価用システム時刻計測機能
 */
#ifdef SUPPORT_VXGET_TIM

typedef UW  SYSUTIM;    /* 性能評価用システム時刻 */

extern ER   vxget_tim(SYSUTIM *pk_sysutim) throw();

#endif  /*  SUPPORT_VXGET_TIM  */

/*
 *  システムの中断処理
 */
Inline void
kernel_abort(void)
{
        while (1) {}
}

/*
 * 微少時間待ち
 * 　　注意事項：
 * 　　　標準ではdlytimはUINT型だが16ビットしかないので、
 * 　　　UW型に変更している。
 * 　　　sil_dly_nse()は内部でsil_dly_nse_long()を呼び出す。
 */
extern void sil_dly_nse_long(UW dlytim) throw();


/*
 *  エンディアンの反転
 */
#define	SIL_REV_ENDIAN_H(data) \
	((VH)((((UH)(data) & 0xff) << 8) | (((UH)(data) >> 8) & 0xff)))

#define	SIL_REV_ENDIAN_W(data) \
	((VW)((((UW)(data) & 0xff) << 24) | (((UW)(data) & 0xff00) << 8) \
		| (((UW)(data)>> 8) & 0xff00) | (((UW)(data) >> 24) & 0xff)))

/*
 *  メモリ空間アクセス関数
 *  
 *  コンパイラの警告を回避するため、機種依存部でアクセス関数を用意する。
 *  処理内容はjsp/include/sil.hとまったく同じ。
 *  定義する順番の都合上、上記の「エンディアンの反転」もダブって定義している。
 */
#define OMIT_SIL_ACCESS

/*
 *  8ビット単位の読出し／書込み
 */
Inline VB
sil_reb_mem(VP mem)
{
	VB data = *((volatile VB *) mem);
	return(data);
}

Inline void
sil_wrb_mem(VP mem, VB data)
{
	*((volatile VB *) mem) = data;
}

/*
 *  16ビット単位の読出し／書込み
 */
Inline VH
sil_reh_mem(VP mem)
{
	VH data = *((volatile VH *) mem);
	return(data);
}

Inline void
sil_wrh_mem(VP mem, VH data)
{
	*((volatile VH *) mem) = data;
}

#define	sil_reh_bem(mem)	sil_reh_mem(mem)
#define	sil_wrh_bem(mem, data)	sil_wrh_mem(mem, data)

Inline VH
sil_reh_lem(VP mem)
{
	VH data = *((volatile VH *) mem);
	return(SIL_REV_ENDIAN_H(data));
}

Inline void
sil_wrh_lem(VP mem, VH data)
{
	*((volatile VH *) mem) = SIL_REV_ENDIAN_H(data);
}

/*
 *  32ビット単位の読出し／書込み
 */
Inline VW
sil_rew_mem(VP mem)
{
	VW data = *((volatile VW *) mem);
	return(data);
}

Inline void
sil_wrw_mem(VP mem, VW data)
{
	*((volatile VW *) mem) = data;
}

#define	sil_rew_bem(mem)	sil_rew_mem(mem)
#define	sil_wrw_bem(mem, data)	sil_wrw_mem(mem, data)

Inline VW
sil_rew_lem(VP mem)
{
	VW data= *((volatile VW *) mem);
	return(SIL_REV_ENDIAN_W(data));
}

Inline void
sil_wrw_lem(VP mem, VW data)
{
	*((volatile VW *) mem) = SIL_REV_ENDIAN_W(data);
}

#endif /* _MACRO_ONLY */

#endif /* _CPU_DEFS_H_ */
