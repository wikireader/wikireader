/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *                2003      by Advanced Data Controls, Corp
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
 *  @(#) $Id: hook_data.c,v 1.1 2003/12/19 11:28:13 honda Exp $
 */

#include "jsp_kernel.h"
#include "kernel.h"
#include "ghs_hook.h"



typedef struct v4_t_rcfg{
    ID tskid_max;
    ID semid_max;
    ID flgid_max;
    ID mbxid_max;
    ID dtqid_max;
    ID mpfid_max;
    ID cycno_max;
} V4_T_RCFG;

extern int _kernel_tmax_tskid;
extern int _kernel_tmax_semid;
extern int _kernel_tmax_flgid;
extern int _kernel_tmax_mbxid;
extern int _kernel_tmax_dtqid;
extern int _kernel_tmax_mpfid;
extern int _kernel_tmax_cycid;

V4_T_RCFG CFG;


void tool_initialize()
{
#ifdef GHS_HOOK
    /*トレスマクロのためのバッファを初期化*/
    vinit_hook();
#endif    

    /*カーネル資源を表示するための構造体を初期化*/
    CFG.tskid_max=_kernel_tmax_tskid;
    CFG.semid_max=_kernel_tmax_semid;
    CFG.flgid_max=_kernel_tmax_flgid;
    CFG.mbxid_max=_kernel_tmax_mbxid;
    CFG.dtqid_max=_kernel_tmax_dtqid;
    CFG.mpfid_max=_kernel_tmax_mpfid;
    CFG.cycno_max=_kernel_tmax_cycid;
}


