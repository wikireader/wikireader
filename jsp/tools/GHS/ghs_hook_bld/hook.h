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
 *  @(#) $Id: hook.h,v 1.1 2003/12/19 11:28:13 honda Exp $
 */

#include <kernel_debug.h>

/************************************/
/* Hook-routine definitions         */
/************************************/

#define THF_TSK     0x0001
#define THF_TSKS    0x0002
#define THF_TEX     0x0004
#define THF_SEM     0x0008
#define THF_FLG     0x0010
#define THF_DTQ     0x0020
#define THF_MBX     0x0040
#define THF_MPF     0x0080
#define THF_TIM     0x1000
#define THF_SYS     0x2000
#define THF_INT     0x4000
#define THF_OTH     0x8000

#define THG_SYS     0x0001
#define THG_RET     0x0002
#define THG_DSP     0x0004
#define THG_STS     0x0008
#define THG_INT     0x0010


/************************************/
/* Hook-routine definitions         */
/************************************/
#ifdef GHS_HOOK
void vinit_hook(void);
void vinit_hook_data(void);
void sys_hook0(int grp, int code);
void sys_hook1(int grp, int code, int p1);
void sys_hook2(int grp, int code, int p1, int p2);
void sys_hook3(int grp, int code, int p1, int p2, int p3);
void sys_hook4(int grp, int code, int p1, int p2, int p3, int p4);

void sys_hook0_i(int grp, int code);
void sys_hook1_i(int grp, int code, int p1);
void sys_hook2_i(int grp, int code, int p1, int p2);

void trc_rtn_hook(int grp, int code, int ercd);

#define trc_rtn_hook_ok(a,b)	trc_rtn_hook(a,b,E_OK)

void sts_hook(ID tskid);


#endif
