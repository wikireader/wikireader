/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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
 *  プロセッサに依存する定義（H8S用）
 *
 *  このインクルードファイルは，kernel.h と sil.h でインクルードされる．
 *  他のファイルから直接インクルードすることはない．このファイルをイン
 *  クルードする前に，t_stddef.h と itron.h がインクルードされるので，
 *  それらに依存してもよい．
 */

#define H8S

/* カーネル起動時のメッセージ */
#define COPYRIGHT_CPU \
"Copyright (C) 2001-2007 by Industrial Technology Institute,\n" \
"                            Miyagi Prefectural Government, JAPAN\n" \
"Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering,\n" \
"                 Tomakomai National College of Technology, JAPAN\n"

#ifndef _MACRO_ONLY

/*
 *  割込み／CPU例外ハンドラ番号の型
 */
typedef	UINT	INHNO;			/* 割込みハンドラ番号 */
typedef	UINT	EXCNO;			/* CPU例外ハンドラ番号 */

/*
 *  割込みマスクの型と割込みマスクの変更／参照
 */

/* 割込みマスクの型 */
typedef	UB	IPM;			/* 割込みマスク */

/* 割込みマスク操作用の関数 */
extern ER	chg_ipm(IPM ipm) throw();	/* 割込みマスクの変更 */
extern ER	get_ipm(IPM *p_ipm) throw();	/* 割込みマスクの取得 */

#if 0		/*  現時点ではサポートしていない  */
/*
 *  INTNO型と割込みの禁止/許可
 */
typedef	UINT	INTNO;			/* 割込み番号 */

extern ER	dis_int(INTNO intno) throw();
extern ER	ena_int(INTNO intno) throw();
#endif


/*
 *　システムインタフェースレイヤ
 */

/*
 *  割込みロック状態の制御
 */
#define	SIL_PRE_LOC	IPM _intmask_ = current_intmask()
#define	SIL_LOC_INT()	_disint_()
#define	SIL_UNL_INT()	set_intmask(_intmask_)

/*
 * 微少時間待ち
 * 　　注意事項：
 * 　　　標準ではdlytimはUINT型だが16ビットしかないので、
 * 　　　sil_dly_nse_long()では引数をUW型に変更している。
 * 　　　sil_dly_nse()は内部でsil_dly_nse_long()を呼び出す。
 * 　　　
 * 　　　sil_dly_nse()のプロトタイプ宣言はjsp/include/sil.hにある。
 */
extern void sil_dly_nse_long(UW dlytim) throw();

/*
 *  プロセッサのエンディアン
 */
#define	SIL_ENDIAN	SIL_ENDIAN_BIG		/* ビッグエンディアン */


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

/*
 *　I/Oポートのデータ・ディレクション・レジスタDDRへのアクセス
 */

/*  ポート番号の定義  */
typedef enum  {
	IO_PORT1,
	IO_PORT2,
	IO_PORT3,
	IO_PORT5,	/*  ポート４にDDRはない  */
	IO_PORT6,
	IO_PORT7,
	IO_PORT8,
	IO_PORT9,
	IO_PORTA,
	IO_PORTB,
	IO_PORTC,
	IO_PORTD,
	IO_PORTE,
	IO_PORTF,
	IO_PORTG
} IO_PORT_ID;

extern UB sil_reb_ddr(IO_PORT_ID port) throw();
extern void sil_wrb_ddr(IO_PORT_ID port, UB data) throw();
extern void sil_anb_ddr(IO_PORT_ID port, UB data) throw();
extern void sil_orb_ddr(IO_PORT_ID port, UB data) throw();


/*============================================================================*/
/*  デバッグ用コード  */

#ifdef TEST_CPU_INSN
extern void test_cpu_insn(void);
#endif /* TEST_CPU_INSN */

#ifdef TEST_CPU_CONFIG
extern void test_cpu_config(void);
#endif /* TEST_CPU_CONFIG */

#ifdef TEST_H8S_SIL
extern void test_h8s_sil(void);
#endif /* TEST_H8S_SIL */

/*============================================================================*/



#endif /* _MACRO_ONLY */

#endif /* _CPU_DEFS_H_ */
