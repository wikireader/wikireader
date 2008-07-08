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
 *  @(#) $Id: mpc860_sil.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

#include <sil.h>

/*
 *  MPC860内蔵の制御レジスタへのアクセス・ユーティリティ
 *  
 *  powerpc.hに記述するのがエレガントだが、sil.hとのインクルードの
 *  順番の関係で独立したファイルとする。
 *  t_config.hを直接インクルードしたときにsil.hより先にpowerpc.hが
 *  インクルードされる可能性がある。
 *  
 *  コンパイラの型チェック機能を有効にするため、ポインタ型は
 *  VB *、VH *、VW *を用いている。
 *  
 */

#ifndef _MPC860_SIL_H_
#define _MPC860_SIL_H_

#ifndef _MACRO_ONLY

/*
 *  SILと同等のアクセス関数（型チェック機能付き）
 */

#ifndef SIL_DEBUG

/*
 *  8ビット単位の読出し／書込み
 */
Inline VB
mpc860_reb_mem(VB *mem)
{
	VB reg = sil_reb_mem((VP)mem);
	return(reg);
}

Inline void
mpc860_wrb_mem(VB *mem, VB data)
{
	sil_wrb_mem((VP)mem, data);
}

/*
 *  16ビット単位の読出し／書込み
 */
Inline VH
mpc860_reh_mem(VH *mem)
{
	VH reg = sil_reh_mem((VP)mem);
	return(reg);
}

Inline void
mpc860_wrh_mem(VH *mem, VH data)
{
	sil_wrh_mem((VP)mem, data);
}

/*
 *  32ビット単位の読出し／書込み
 */
Inline VW
mpc860_rew_mem(VW *mem)
{
	VW reg = sil_rew_mem((VP)mem);
	return(reg);
}

Inline void
mpc860_wrw_mem(VW *mem, VW data)
{
	sil_wrw_mem((VP)mem, data);
}



/*
 *  ビット演算
 */

/*
 *  8ビットレジスタのAND演算
 */
Inline void
mpc860_andb_mem(VB *mem, VB data)
{
	VB reg = mpc860_reb_mem((VP)mem);
	reg &= data;
	mpc860_wrb_mem(mem, reg);
}

/*
 *  8ビットレジスタのOR演算
 */
Inline void
mpc860_orb_mem(VB *mem, VB data)
{
	VB reg = mpc860_reb_mem(mem);
	reg |= data;
	mpc860_wrb_mem(mem, reg);
}


/*
 *  16ビットレジスタのAND演算
 */
Inline void
mpc860_andh_mem(VH *mem, VH data)
{
	VH reg = mpc860_reh_mem(mem);
	reg &= data;
	mpc860_wrh_mem(mem, reg);
}

/*
 *  16ビットレジスタのOR演算
 */
Inline void
mpc860_orh_mem(VH *mem, VH data)
{
	VH reg = mpc860_reh_mem(mem);
	reg |= data;
	mpc860_wrh_mem(mem, reg);
}

/*
 *  32ビットレジスタのAND演算
 */
Inline void
mpc860_andw_mem(VW *mem, VW data)
{
	VW reg = mpc860_rew_mem(mem);
	reg &= data;
	mpc860_wrw_mem(mem, reg);
}

/*
 *  32ビットレジスタのOR演算
 */
Inline void
mpc860_orw_mem(VW *mem, VW data)
{
	VW reg = mpc860_rew_mem(mem);
	reg |= data;
	mpc860_wrw_mem(mem, reg);
}

#else	/*  SIL_DEBUG  */

extern UW sil_log_id;
extern BOOL sil_debug_on;

/*
 *  SILのログ出力を割り当てるログ重要度の定義
 *　（user.txtの「4.4 ログ情報の重要度」参照）
 */
#ifndef LOG_SIL
#define LOG_SIL	LOG_EMERG
#endif	/*  LOG_SIL  */


/*
 *  8ビット単位の読出し
 */
#define mpc860_reb_mem(mem)	mpc860_reb_mem_deb(#mem, mem)

Inline VB
mpc860_reb_mem_deb(char *str, VB *mem)
{
	BOOL sil_debug_tmp, is_end_of_line;
	SYSUTIM sysutim;
	VB reg;
	ER err;
	SIL_PRE_LOC;
	
	sil_debug_tmp = sil_debug_on;
	if (sil_debug_on) {
		SIL_LOC_INT();
		sil_debug_on = FALSE;
		++sil_log_id;
		SIL_UNL_INT();
		err = vxget_tim(&sysutim);
		if (err != E_OK) {
			sysutim = 0;
		}
		SIL_LOC_INT();
	}
	
	reg = sil_reb_mem((VP)mem);

	if (sil_debug_tmp) {
		sil_debug_on = TRUE;
		SIL_UNL_INT();
		is_end_of_line = (reg == '\n') || (reg == '\r');
		if (!is_end_of_line) {
			syslog(LOG_SIL, 
			"%d time:%08d read VB %s(0x%08x) data=0x%02x(%c)",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)(UB)reg, (UW)(UB)reg);
		} else {
			syslog(LOG_SIL, 
			"%d time:%08d read VB %s(0x%08x) data=0x%02x(%s)",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)(UB)reg, 
			(reg == '\n') ? "\\n" : "\\r");
		}
	}
	
	return(reg);
}


/*
 *  8ビット単位の書き込み
 */
#define mpc860_wrb_mem(mem, data)	mpc860_wrb_mem_deb(#mem, mem, data)

Inline void
mpc860_wrb_mem_deb(char *str, VB *mem, VB data)
{
	BOOL sil_debug_tmp, is_end_of_line;
	SYSUTIM sysutim;
	ER err;
	SIL_PRE_LOC;
	
	sil_debug_tmp = sil_debug_on;
	if (sil_debug_on) {
		SIL_LOC_INT();
		sil_debug_on = FALSE;
		++sil_log_id;
		SIL_UNL_INT();
		err = vxget_tim(&sysutim);
		if (err != E_OK) {
			sysutim = 0;
		}
		SIL_LOC_INT();
	}
	
	sil_wrb_mem((VP)mem, data);

	if (sil_debug_tmp) {
		sil_debug_on = TRUE;
		SIL_UNL_INT();
		is_end_of_line = (data == '\n') || (data == '\r');
		if (!is_end_of_line) {
			syslog(LOG_SIL, 
			"%d time:%08d write VB %s(0x%08x) data=0x%02x(%c)",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)(UB)data, (UW)(UB)data);
		} else {
			syslog(LOG_SIL, 
			"%d time:%08d write VB %s(0x%08x) data=0x%02x(%s)",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)(UB)data, 
			(data == '\n') ? "\\n" : "\\r");
		}
	}
}

/*
 *  16ビット単位の読出し
 */
#define mpc860_reh_mem(mem)	mpc860_reh_mem_deb(#mem, mem)

Inline VH
mpc860_reh_mem_deb(char *str, VH *mem)
{
	BOOL sil_debug_tmp;
	SYSUTIM sysutim;
	VH reg;
	ER err;
	SIL_PRE_LOC;
	
	sil_debug_tmp = sil_debug_on;
	if (sil_debug_on) {
		SIL_LOC_INT();
		sil_debug_on = FALSE;
		++sil_log_id;
		SIL_UNL_INT();
		err = vxget_tim(&sysutim);
		if (err != E_OK) {
			sysutim = 0;
		}
		SIL_LOC_INT();
	}
	
	reg = sil_reh_mem((VP)mem);

	if (sil_debug_tmp) {
		sil_debug_on = TRUE;
		SIL_UNL_INT();
		syslog(LOG_SIL, 
			"%d time:%08d read VH %s(0x%08x) data=0x%04x",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)(UH)reg);
	}
	
	return(reg);
}

/*
 *  16ビット単位の書き込み
 */
#define mpc860_wrh_mem(mem, data)	mpc860_wrh_mem_deb(#mem, mem, data)

Inline void
mpc860_wrh_mem_deb(char *str, VH *mem, VH data)
{
	BOOL sil_debug_tmp;
	SYSUTIM sysutim;
	ER err;
	SIL_PRE_LOC;
	
	sil_debug_tmp = sil_debug_on;
	if (sil_debug_on) {
		SIL_LOC_INT();
		sil_debug_on = FALSE;
		++sil_log_id;
		SIL_UNL_INT();
		err = vxget_tim(&sysutim);
		if (err != E_OK) {
			sysutim = 0;
		}
		SIL_LOC_INT();
	}
	
	sil_wrh_mem((VP)mem, data);

	if (sil_debug_tmp) {
		sil_debug_on = TRUE;
		SIL_UNL_INT();
		syslog(LOG_SIL, 
			"%d time:%08d write VH %s(0x%08x) data=0x%04x",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)(UH)data);
	}
}


/*
 *  32ビット単位の読出し
 */
#define mpc860_rew_mem(mem)	mpc860_rew_mem_deb(#mem, mem)

Inline VW
mpc860_rew_mem_deb(char *str, VW *mem)
{
	BOOL sil_debug_tmp;
	SYSUTIM sysutim;
	VW reg;
	ER err;
	SIL_PRE_LOC;
	
	sil_debug_tmp = sil_debug_on;
	if (sil_debug_on) {
		SIL_LOC_INT();
		sil_debug_on = FALSE;
		++sil_log_id;
		SIL_UNL_INT();
		err = vxget_tim(&sysutim);
		if (err != E_OK) {
			sysutim = 0;
		}
		SIL_LOC_INT();
	}
	
	reg = sil_rew_mem((VP)mem);

	if (sil_debug_tmp) {
		sil_debug_on = TRUE;
		SIL_UNL_INT();
		syslog(LOG_SIL, 
			"%d time:%08d read VW %s(0x%08x) data=0x%08x",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)reg);
	}
	
	return(reg);
}


/*
 *  32ビット単位の書き込み
 */
#define mpc860_wrw_mem(mem, data)	mpc860_wrw_mem_deb(#mem, mem, data)

Inline void
mpc860_wrw_mem_deb(char *str, VW *mem, VW data)
{
	BOOL sil_debug_tmp;
	SYSUTIM sysutim;
	ER err;
	SIL_PRE_LOC;
	
	sil_debug_tmp = sil_debug_on;
	if (sil_debug_on) {
		SIL_LOC_INT();
		sil_debug_on = FALSE;
		++sil_log_id;
		SIL_UNL_INT();
		err = vxget_tim(&sysutim);
		if (err != E_OK) {
			sysutim = 0;
		}
		SIL_LOC_INT();
	}
	
	sil_wrw_mem((VP)mem, data);

	if (sil_debug_tmp) {
		sil_debug_on = TRUE;
		SIL_UNL_INT();
		syslog(LOG_SIL, 
			"%d time:%08d write VW %s(0x%08x) data=0x%08x",
			sil_log_id, (UW)sysutim, str, 
			(UW)mem, (UW)data);
	}
}


/*
 *  ビット演算
 */

/*
 *  8ビットレジスタのAND演算
 */
#define mpc860_andb_mem(mem, data)				\
	if (sil_debug_on) {					\
		syslog(LOG_SIL, "%s & 0x%02x",	 		\
			#mem, (UW)(UB)data);			\
	}							\
	mpc860_andb_mem_deb(mem, data)

Inline void
mpc860_andb_mem_deb(VB *mem, VB data)
{
	VB reg = mpc860_reb_mem((VP)mem);
	reg &= data;
	mpc860_wrb_mem(mem, reg);
}

/*
 *  8ビットレジスタのOR演算
 */
#define mpc860_orb_mem(mem, data)				\
	if (sil_debug_on) {					\
		syslog(LOG_SIL, "%s | 0x%02x",	 		\
			#mem, (UW)(UB)data);			\
	}							\
	mpc860_orb_mem_deb(mem, data)

Inline void
mpc860_orb_mem_deb(VB *mem, VB data)
{
	VB reg = mpc860_reb_mem(mem);
	reg |= data;
	mpc860_wrb_mem(mem, reg);
}


/*
 *  16ビットレジスタのAND演算
 */
#define mpc860_andh_mem(mem, data)				\
	if (sil_debug_on) {					\
		syslog(LOG_SIL, "%s & 0x%04x",	 		\
			#mem, (UW)(UH)data);			\
	}							\
	mpc860_andh_mem_deb(mem, data)

Inline void
mpc860_andh_mem_deb(VH *mem, VH data)
{
	VH reg = mpc860_reh_mem(mem);
	reg &= data;
	mpc860_wrh_mem(mem, reg);
}

/*
 *  16ビットレジスタのOR演算
 */
#define mpc860_orh_mem(mem, data)				\
	if (sil_debug_on) {					\
		syslog(LOG_SIL, "%s | 0x%04x",	 		\
			#mem, (UW)(UH)data);			\
	}							\
	mpc860_orh_mem_deb(mem, data)

Inline void
mpc860_orh_mem_deb(VH *mem, VH data)
{
	VH reg = mpc860_reh_mem(mem);
	reg |= data;
	mpc860_wrh_mem(mem, reg);
}

/*
 *  32ビットレジスタのAND演算
 */
#define mpc860_andw_mem(mem, data)				\
	if (sil_debug_on) {					\
		syslog(LOG_SIL, "%s & 0x%08x",	 		\
			#mem, (UW)data);			\
	}							\
	mpc860_andw_mem_deb(mem, data)

Inline void
mpc860_andw_mem_deb(VW *mem, VW data)
{
	VW reg = mpc860_rew_mem(mem);
	reg &= data;
	mpc860_wrw_mem(mem, reg);
}

/*
 *  32ビットレジスタのOR演算
 */
#define mpc860_orw_mem(mem, data)				\
	if (sil_debug_on) {					\
		syslog(LOG_SIL, "%s | 0x%08x",	 		\
			#mem, (UW)data);			\
	}							\
	mpc860_orw_mem_deb(mem, data)

Inline void
mpc860_orw_mem_deb(VW *mem, VW data)
{
	VW reg = mpc860_rew_mem(mem);
	reg |= data;
	mpc860_wrw_mem(mem, reg);
}


#endif	/*  SIL_DEBUG  */

/*
 *  CPコマンドが発行可能になるまで待つ
 */
Inline void
mpc860_wait_ready_CP_command(void)
{
	UH cpcr;
    	do {
		cpcr = mpc860_reh_mem(CPCR);
    	} while(cpcr & CPCR_FLG); 
}	


/*
 *  CPコマンドを発行する
 *	引数
 *	　UW opcode：コマンド種別
 *	　UW ch_num：チャネル番号（デバイス番号）
 */
Inline void
mpc860_CP_command(UW opcode, UW ch_num)
{
	UH cpcr;
	
	/*  他のCPコマンド終了待ち  */
	mpc860_wait_ready_CP_command();
	
	/*  CPコマンド発行  */
	cpcr = (UH)((opcode << 8) | (ch_num << 4) | CPCR_FLG);
    	mpc860_wrh_mem(CPCR, cpcr);

	/*  CPコマンド終了待ち  */
	mpc860_wait_ready_CP_command();
    	
}



#endif /* _MACRO_ONLY */
#endif	/*  _MPC860_SIL_H_  */
/*  end of file  */
