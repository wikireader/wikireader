/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: sil.h,v 1.9 2006/02/12 05:27:25 hiro Exp $
 */

/*
 *	システムインタフェースレイヤ（ターゲット共通部）
 *
 *  アセンブリ言語のソースファイルやシステムコンフィギュレーションファ
 *  イルからこのファイルをインクルードする時は，_MACRO_ONLY を定義して
 *  おくことで，マクロ定義以外の記述を除くことができる．
 *
 *  このインクルードファイルは，標準インクルードファイル（s_services.h）
 *  でインクルードされる．また，カーネルから呼ばれるデバイスドライバの
 *  インクルードファイルで，インライン関数などでシステムインタフェース
 *  レイヤを用いている場合にも，このファイルがインクルードされる．この
 *  例外を除いて，他のファイルから直接インクルードされることはない．
 *
 *  この中でインクルードしているファイルを除いて，他のインクルードファ
 *  イルに依存していない．
 */

#ifndef _SIL_H_
#define _SIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  カーネル・アプリケーション 共通インクルードファイル
 */
#include <t_stddef.h>

/*
 *  ITRON仕様共通規定のデータ型・定数・マクロ
 */
#include <itron.h>

/*
 *  システムやプロセッサに依存する定義
 */
#include <sys_defs.h>
#include <cpu_defs.h>

/*
 *  システムログサービスのための定義
 */
#include <t_syslog.h>

/*
 *  エンディアン定数の定義
 */
#define	SIL_ENDIAN_LITTLE	0	/* リトルエンディアン */
#define	SIL_ENDIAN_BIG		1	/* ビッグエンディアン */

#ifndef _MACRO_ONLY

/*
 *  割込みロック状態の制御
 */
#ifndef SIL_PRE_LOC
#include <kernel.h>
#define	SIL_PRE_LOC	BOOL _sil_loc_ = sns_loc()
#define	SIL_LOC_INT()	((void)(!(_sil_loc_) \
				&& (sns_ctx() ? iloc_cpu() : loc_cpu())))
#define	SIL_UNL_INT()	((void)(!(_sil_loc_) \
				&& (sns_ctx() ? iunl_cpu() : unl_cpu())))
#endif /* SIL_PRE_LOC */

/*
 *  微少時間待ち
 */
extern void	sil_dly_nse(UINT dlytim) throw();

/*
 *  エンディアンの反転
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

/*
 *  メモリ空間アクセス関数
 */
#ifndef OMIT_SIL_ACCESS

/*
 *  8ビット単位の読出し／書込み
 */
#ifdef _int8_

Inline VB
sil_reb_mem(VP mem)
{
	VB	data;

	data = *((volatile VB *) mem);
	return(data);
}

Inline void
sil_wrb_mem(VP mem, VB data)
{
	*((volatile VB *) mem) = data;
}

#endif /* _int8_ */

/*
 *  16ビット単位の読出し／書込み
 */
#ifdef _int16_

Inline VH
sil_reh_mem(VP mem)
{
	VH	data;

	data = *((volatile VH *) mem);
	return(data);
}

Inline void
sil_wrh_mem(VP mem, VH data)
{
	*((volatile VH *) mem) = data;
}

#if SIL_ENDIAN == SIL_ENDIAN_BIG	/* ビッグエンディアンプロセッサ */

#define	sil_reh_bem(mem)	sil_reh_mem(mem)
#define	sil_wrh_bem(mem, data)	sil_wrh_mem(mem, data)

#ifndef OMIT_SIL_REH_LEM

Inline VH
sil_reh_lem(VP mem)
{
	VH	data;

	data = *((volatile VH *) mem);
	return(SIL_REV_ENDIAN_H(data));
}

#endif /* OMIT_SIL_REH_LEM */
#ifndef OMIT_SIL_WRH_LEM

Inline void
sil_wrh_lem(VP mem, VH data)
{
	*((volatile VH *) mem) = SIL_REV_ENDIAN_H(data);
}

#endif /* OMIT_SIL_WRH_LEM */
#else /* SIL_ENDIAN == SIL_ENDIAN_BIG *//* リトルエンディアンプロセッサ */

#define	sil_reh_lem(mem)	sil_reh_mem(mem)
#define	sil_wrh_lem(mem, data)	sil_wrh_mem(mem, data)

#ifndef OMIT_SIL_REH_BEM

Inline VH
sil_reh_bem(VP mem)
{
	VH	data;

	data = *((volatile VH *) mem);
	return(SIL_REV_ENDIAN_H(data));
}

#endif /* OMIT_SIL_REH_BEM */
#ifndef OMIT_SIL_WRH_BEM

Inline void
sil_wrh_bem(VP mem, VH data)
{
	*((volatile VH *) mem) = SIL_REV_ENDIAN_H(data);
}

#endif /* OMIT_SIL_WRH_BEM */
#endif /* SIL_ENDIAN == SIL_ENDIAN_BIG */
#endif /* _int16_ */

/*
 *  32ビット単位の読出し／書込み
 */

Inline VW
sil_rew_mem(VP mem)
{
	VW	data;

	data = *((volatile VW *) mem);
	return(data);
}

Inline void
sil_wrw_mem(VP mem, VW data)
{
	*((volatile VW *) mem) = data;
}

#if SIL_ENDIAN == SIL_ENDIAN_BIG	/* ビッグエンディアンプロセッサ */

#define	sil_rew_bem(mem)	sil_rew_mem(mem)
#define	sil_wrw_bem(mem, data)	sil_wrw_mem(mem, data)

#ifndef OMIT_SIL_REW_LEM

Inline VW
sil_rew_lem(VP mem)
{
	VW	data;

	data = *((volatile VW *) mem);
	return(SIL_REV_ENDIAN_W(data));
}

#endif /* OMIT_SIL_REW_LEM */
#ifndef OMIT_SIL_WRW_LEM

Inline void
sil_wrw_lem(VP mem, VW data)
{
	*((volatile VW *) mem) = SIL_REV_ENDIAN_W(data);
}

#endif /* OMIT_SIL_WRW_LEM */
#else /* SIL_ENDIAN == SIL_ENDIAN_BIG *//* リトルエンディアンプロセッサ */

#define	sil_rew_lem(mem)	sil_rew_mem(mem)
#define	sil_wrw_lem(mem, data)	sil_wrw_mem(mem, data)

#ifndef OMIT_SIL_REW_BEM

Inline VW
sil_rew_bem(VP mem)
{
	VW	data;

	data = *((volatile VW *) mem);
	return(SIL_REV_ENDIAN_W(data));
}

#endif /* OMIT_SIL_REW_BEM */
#ifndef OMIT_SIL_WRW_BEM

Inline void
sil_wrw_bem(VP mem, VW data)
{
	*((volatile VW *) mem) = SIL_REV_ENDIAN_W(data);
}

#endif /* OMIT_SIL_WRW_BEM */
#endif /* SIL_ENDIAN == SIL_ENDIAN_BIG */
#endif /* OMIT_SIL_ACCESS */

#endif /* _MACRO_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SIL_H_ */
