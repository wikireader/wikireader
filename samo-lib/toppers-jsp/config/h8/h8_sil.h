/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
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
 *
 *  @(#) $Id: h8_sil.h,v 1.7 2007/03/23 07:22:15 honda Exp $
 */

/*
 *  H8向けSILの拡張（ビット演算）
 *
 *  h8.hに記述するのがエレガントだが、sil.hとのインクルードの
 *  順番の関係で独立したファイルとする
 *  t_config.hを直接インクルードしたときにsil.hより先にこの
 *  ファイルがインクルードされる可能性がある。
 *
 *  コンパイラの型チェック機能を有効にするため、ポインタ型は
 *  UB *、UH *、UW *を用いている。
 *
 */

#ifndef _SIL_H8_H_
#define _SIL_H8_H_

#ifndef _MACRO_ONLY

#include <sil.h>
#include <cpu_insn.h>	/*  bitset(), bitclr()  */

/*
 *  8ビットレジスタのAND演算
 */
Inline void
h8_anb_reg(UB *mem, UB data)
{
        UB reg = sil_reb_mem((VP)mem);
        reg &= data;
        sil_wrb_mem((VP)mem, (VB)reg);
}

/*
 *  8ビットレジスタのOR演算
 */
Inline void
h8_orb_reg(UB *mem, UB data)
{
        UB reg = sil_reb_mem((VP)mem);
        reg |= data;
        sil_wrb_mem((VP)mem, (VB)reg);
}


/*
 *  16ビットレジスタのAND演算
 */
Inline void
h8_anh_reg(UH *mem, UH data)
{
        UH reg = sil_reh_mem((VP)mem);
        reg &= data;
        sil_wrh_mem((VP)mem, (VH)reg);
}

/*
 *  16ビットレジスタのOR演算
 */
Inline void
h8_orh_reg(UH *mem, UH data)
{
        UH reg = sil_reh_mem((VP)mem);
        reg |= data;
        sil_wrh_mem((VP)mem, (VH)reg);
}

/*
 *  32ビットレジスタのAND演算
 */
Inline void
h8_anw_reg(UW *mem, UW data)
{
        UW reg = sil_rew_mem((VP)mem);
        reg &= data;
        sil_wrw_mem((VP)mem, (VW)reg);
}

/*
 *  32ビットレジスタのOR演算
 */
Inline void
h8_orw_reg(UW *mem, UW data)
{
        UW reg = sil_rew_mem((VP)mem);
        reg |= data;
        sil_wrw_mem((VP)mem, (VW)reg);
}

/*
 *  割込みレベルの設定
 *
 *  　irc  ：IRCデータの先頭アドレス
 *
 *  　　UB *ipr：設定するIPRレジスタの番地
 *  　　UB bit ：IPRレジスタの該当するビット番号
 *  　　IPM ipm：設定する割込みレベル
 *  　　　　　　　IPM_LEVEL0,IPM_LEVEL1のいずれか
 */
Inline void
define_int_plevel(const IRC *irc)
{
        UB *ipr = irc->ipr;
        UB bit = irc->bit;
        IPM ipm = irc->ipm;

        switch(ipm) {
                case IPM_LEVEL0:        /*  プライオリティレベル0に設定  */
                        bitclr(ipr, bit);
                        break;
                case IPM_LEVEL1:        /*  プライオリティレベル1に設定  */
                        bitset(ipr, bit);
                        break;
                default:
                        assert(FALSE);
        }
}


/*
 *　I/Oポートのデータ・ディレクション・レジスタDDRへのアクセス
 *　
 *　　H8のDDRは書き込み専用であり、そのままでは所望のビットだけを
 *　　変更することができない。（bset,bclr命令でも回避不可）
 *　　そのため、本実装では、メモリ上にテンポラリを用意して、DDRの
 *　　現在値を保持する方法を採っている。
 */
extern UB sil_reb_ddr(UINT port) throw();
extern void sil_wrb_ddr(UINT port, UB data) throw();
extern void sil_anb_ddr(UINT port, UB data) throw();
extern void sil_orb_ddr(UINT port, UB data) throw();

/*
 *　DDRの番号定義
 *　
 *　　配列のインデックスに用いる。
 *　　なお、ポート7は入力専用でDDRレジスタがないため、省略している。
 */
enum IO_PORT {
	IO_PORT1,
	IO_PORT2,
	IO_PORT3,
	IO_PORT4,
	IO_PORT5,
	IO_PORT6,
	IO_PORT8,
	IO_PORT9,
	IO_PORTA,
	IO_PORTB
};

#endif /* _MACRO_ONLY */
#endif  /*  _SIL_H8_H_  */
