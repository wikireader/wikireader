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
 *  @(#) $Id: sh1_sil.h,v 1.4 2004/09/22 08:47:52 honda Exp $
 */

/*
 *  SH1内蔵の制御レジスタへのアクセス・ユーティリティ（ビット演算）
 *  
 *  sh1.hに記述するのがエレガントだが、sil.hとのインクルードの
 *  順番の関係で独立したファイルとする
 *  t_config.hを直接インクルードしたときにsil.hより先にこの
 *  ファイルがインクルードされる可能性がある。
 *  
 *  コンパイラの型チェック機能を有効にするため、ポインタ型は
 *  VB *、VH *、VW *を用いている。
 *  
 */

#ifndef _SIL_SH1_H_
#define _SIL_SH1_H_

#ifndef _MACRO_ONLY

/*
 *  8ビットレジスタのAND演算
 */
Inline void
sh1_anb_reg(VB *mem, VB data)
{
	VB reg = sil_reb_mem((VP)mem);
	reg &= data;
	sil_wrb_mem((VP)mem, reg);
}

/*
 *  8ビットレジスタのOR演算
 */
Inline void
sh1_orb_reg(VB *mem, VB data)
{
	VB reg = sil_reb_mem((VP)mem);
	reg |= data;
	sil_wrb_mem((VP)mem, reg);
}


/*
 *  16ビットレジスタのAND演算
 */
Inline void
sh1_anh_reg(VH *mem, VH data)
{
	VH reg = sil_reh_mem((VP)mem);
	reg &= data;
	sil_wrh_mem((VP)mem, reg);
}

/*
 *  16ビットレジスタのOR演算
 */
Inline void
sh1_orh_reg(VH *mem, VH data)
{
	VH reg = sil_reh_mem((VP)mem);
	reg |= data;
	sil_wrh_mem((VP)mem, reg);
}

/*
 *  32ビットレジスタのAND演算
 */
Inline void
sh1_anw_reg(VW *mem, VW data)
{
	VW reg = sil_rew_mem((VP)mem);
	reg &= data;
	sil_wrw_mem((VP)mem, reg);
}

/*
 *  32ビットレジスタのOR演算
 */
Inline void
sh1_orw_reg(VW *mem, VW data)
{
	VW reg = sil_rew_mem((VP)mem);
	reg |= data;
	sil_wrw_mem((VP)mem, reg);
}


/*
 *  割り込みレベルの設定
 *  
 *  　mem  ：変更するレジスタの番地
 *  　level：設定する割込みレベル
 *  　shift：何ビット左シフトしたところが該当する個所か
 *  
 */
Inline void
define_int_plevel(VH *mem, UW level, UINT shift)
{
	VH reg = sil_reh_mem((VP)mem);
	reg = (VH)((reg & ~(0xfu << shift)) | (level << shift));
	sil_wrh_mem((VP)mem, reg);
}



#endif /* _MACRO_ONLY */
#endif	/*  _SIL_SH1_H_  */
