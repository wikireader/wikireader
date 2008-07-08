/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

/*
 *  プロセッサに依存する定義（MIPS3用）
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

#define MIPS3

/* カーネル起動時のメッセージ */
#define COPYRIGHT_CPU \
"Copyright (C) 2001-2003 by Industrial Technology Institute,\n" \
"                            Miyagi Prefectural Government, JAPAN\n"

#ifndef _MACRO_ONLY

/*
 *  割込み／CPU例外ハンドラ番号の型
 */
typedef	UINT	INHNO;			/* 割込みハンドラ番号 */
typedef	UINT	EXCNO;			/* CPU例外ハンドラ番号 */

/*
 *  割込みマスクの型と割込みマスクの変更／参照
 */

/* MIPS3コアの割込みマスクの型定義 (初期値はシステム依存部に記述) */
typedef	UW	CORE_IPM;

/* 割込みマスクの型 */
typedef	struct {
	CORE_IPM	core;	/*  MIPS3コアの割込みマスク  */
	ICU_IPM		icu;	/*  割込みコントローラの割込みマスク  */
} IPM;

/* 割込みマスク操作用の関数 */
extern ER	chg_ipm(IPM p_ipm) throw();	/* 割込みマスクの変更 */
extern ER	get_ipm(IPM *p_ipm) throw();	/* 割込みマスクの取得 */

#endif /* _MACRO_ONLY */

/*
 *  割込みロック状態の制御
 *    MIPS3では、全ての割り込みがカーネル管理内なので、
 *      割込みロック状態 = CPUロック状態
 *    となり、このマクロの内容は、標準に提供されているものを使う。
 *    (もし、個別に定義をしようとするならば、下記のようになる。)
 */
/*
#define	SIL_PRE_LOC
#define	SIL_LOC_INT()	t_lock_cpu()
#define	SIL_UNL_INT()	t_unlock_cpu()
*/
/*============================================================================*/
/*  共通ドキュメントに無い定義  */

/*  全体の割込みの本数  */
#define TMAX_ALL_INTNO	(TMAX_CORE_INTNO + TMAX_ICU_INTNO)

#ifndef _MACRO_ONLY

/*  割込みハンドラ／割込みマスク擬似テーブル用の型  */
typedef struct {
	FP		inthdr;		/*  割込みハンドラの先頭アドレス  */
	CORE_IPM	intmask;	/*  MIPS3コアの割込みマスク  */
} INT_TABLE;

#endif /* _MACRO_ONLY */

#endif /* _CPU_DEFS_H_ */
