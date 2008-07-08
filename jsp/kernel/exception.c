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
 *  @(#) $Id: exception.c,v 1.9 2003/06/04 01:46:16 hiro Exp $
 */

/*
 *	CPU例外管理機能
 */

#include "jsp_kernel.h"
#include "task.h"
#include "exception.h"

/*
 *  CPU例外ハンドラ番号の数（kernel_cfg.c）
 */
extern const UINT	tnum_excno;

/*
 *  CPU例外ハンドラ初期化ブロックのエリア（kernel_cfg.c）
 */
extern const EXCINIB	excinib_table[];

/* 
 *  CPU例外ハンドラ管理機能の初期化
 */
#ifdef __excini

void
exception_initialize()
{
	UINT		i;
	const EXCINIB	*excinib;

	for (excinib = excinib_table, i = 0; i < tnum_excno; excinib++, i++) {
		define_exc(excinib->excno, excinib->exchdr);
	}
}

#endif /* __excini */

/*
 *  CPU例外の発生したコンテキストの参照
 */
#ifdef __vxsns_ctx

SYSCALL BOOL
vxsns_ctx(VP p_excinf)
{
	BOOL	state;

	LOG_VXSNS_CTX_ENTER(p_excinf);
	state = exc_sense_context(p_excinf) ? TRUE : FALSE;
	LOG_VXSNS_CTX_LEAVE(state);
	return(state);
}

#endif /* __vxsns_ctx */

/*
 *  CPU例外の発生した時のCPUロック状態の参照
 */
#ifdef __vxsns_loc

SYSCALL BOOL
vxsns_loc(VP p_excinf)
{
	BOOL	state;

	LOG_VXSNS_LOC_ENTER(p_excinf);
	state = exc_sense_lock(p_excinf) ? TRUE : FALSE;
	LOG_VXSNS_LOC_LEAVE(state);
	return(state);
}

#endif /* __vxsns_loc */

/*
 *  CPU例外の発生した時のディスパッチ禁止状態の参照
 *
 *  sns_dsp と同一の処理内容となる．
 */
#ifdef __vxsns_dsp

SYSCALL BOOL
vxsns_dsp(VP p_excinf)
{
	BOOL	state;

	LOG_VXSNS_DSP_ENTER(p_excinf);
	state = !(enadsp) ? TRUE : FALSE;
	LOG_VXSNS_DSP_LEAVE(state);
	return(state);
}

#endif /* __vxsns_dsp */

/*
 *  CPU例外の発生した時のディスパッチ保留状態の参照
 */
#ifdef __vxsns_dpn

SYSCALL BOOL
vxsns_dpn(VP p_excinf)
{
	BOOL	state;

	LOG_VXSNS_DPN_ENTER(p_excinf);
	state = (exc_sense_context(p_excinf) || exc_sense_lock(p_excinf)
					|| !(enadsp)) ? TRUE : FALSE;
	LOG_VXSNS_DPN_LEAVE(state);
	return(state);
}

#endif /* __vxsns_dpn */

/*
 *  CPU例外の発生した時のタスク例外処理禁止状態の参照
 *
 *  sns_tex と同一の処理内容となる．
 */
#ifdef __vxsns_tex

SYSCALL BOOL
vxsns_tex(VP p_excinf)
{
	BOOL	state;

	LOG_VXSNS_TEX_ENTER(p_excinf);
	state = (runtsk != NULL && runtsk->enatex) ? FALSE : TRUE;
	LOG_VXSNS_TEX_LEAVE(state);
	return(state);
}

#endif /* __vxsns_tex */
